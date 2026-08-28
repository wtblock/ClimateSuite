/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Page.h"
#include "PageGraph.h"
#include "PageImage.h"
#include "PageMD.h"
#include "PageHTML.h"
#include "PageMap.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"

/////////////////////////////////////////////////////////////////////////////
CPage::CPage
(
	UINT nPage, CString csLayout, 
	CClimateExplorerDoc* pDoc, 
	CPage::PAGE_TYPE eType
)
{
	Page = nPage;
	Layout = csLayout;
	PageType = eType;

	m_pDoc = pDoc;

	HeightOfPage = pDoc->HeightOfPage;
	WidthOfPage = pDoc->WidthOfPage;
	Map = pDoc->Map;
	TopMargin = pDoc->TopMargin;
	BottomMargin = pDoc->BottomMargin;
	InsideMargin = pDoc->InsideMargin;
	OutsideMargin = pDoc->OutsideMargin;
	Gutter = pDoc->Gutter;

	Rect = MarginRectangle;
}

/////////////////////////////////////////////////////////////////////////////
// add an image to the page
bool CPage::AddAnImage(shared_ptr<CGraphPlotter> pPlot)
{
	CString csImage = pPlot->GraphTitle;
	bool value = false;
	if (PageIsFull)
	{
		return value;
	}
	value = true;

	shared_ptr<CPageGraph> pGraph = make_shared<CPageGraph>(pPlot, m_pDoc);

	// replaces an image if it exists
	//m_arrPlots.add(csImage, pPlot, true);
	m_arrContent.add(csImage, pGraph, true);

	return value;
} // AddAnImage

/////////////////////////////////////////////////////////////////////////////
// after getting a new page number, rectangles
// may need adjusting
vector<CRect> CPage::GetRectangles()
{
	vector<CRect> value;

	Rect = MarginRectangle;
	CRect rect = Rect;
	int nHeight = rect.Height();
	int nWidth = rect.Width();
	CString csLayout = Layout;

	int nRect = 0;
	for (auto& node : m_arrContent.Items)
	{
		// initialize the image rectangle to the margin rectangle which 
		// also applies to full page layout
		CRect rectPlot = rect;

		// based on the position in the collection and the layout
		// of the page, calculate the image rectangle's size
		if (csLayout == L"Half")
		{
			if (nRect == 0)
			{
				rectPlot.bottom = rectPlot.top + nHeight / 2;
			}
			else
			{
				rectPlot.top = rectPlot.bottom - nHeight / 2;
			}
		}
		else if (csLayout == L"Quarter")
		{
			switch (nRect)
			{
			case 0: // upper left quadrant
				rectPlot.right = rectPlot.left + nWidth / 2;
				rectPlot.bottom = rectPlot.top + nHeight / 2;
				break;
			case 1: // upper right quadrant
				rectPlot.left = rectPlot.right - nWidth / 2;
				rectPlot.bottom = rectPlot.top + nHeight / 2;
				break;
			case 2: // lower left quadrant
				rectPlot.right = rectPlot.left + nWidth / 2;
				rectPlot.top = rectPlot.bottom - nHeight / 2;
				break;
			case 3: // lower right quadrant
				rectPlot.left = rectPlot.right - nWidth / 2;
				rectPlot.top = rectPlot.bottom - nHeight / 2;
				break;
			}
		}

		// store the plot rectangle in the output vector
		value.push_back(rectPlot);

		// update the position in the collection
		nRect++;
	}

	return value;

} // ComputeRectangles

/////////////////////////////////////////////////////////////////////////////
// CPage::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPage::WriteXml
(	
	IXmlWriter* pWriter
)
{
	HRESULT hr = S_OK;

	// <Page>
	hr = pWriter->WriteStartElement(nullptr, L"Page", nullptr);
	if (FAILED(hr))
		return;

	// Type="Graph" / "Image" / "MD" / "HTML" / "Map" / "Cover" / "TOC"
	CString csType;
	switch (PageType)
	{
	case CPage::pageCover: csType = L"Cover";   break;
	case CPage::pageTOC:   csType = L"TOC";     break;
	case CPage::pageGraph: csType = L"Graph";   break;
	case CPage::pageImage: csType = L"Image";   break;
	case CPage::pageMD:    csType = L"MD";      break;
	case CPage::pageHTML:  csType = L"HTML";    break;
	case CPage::pageMap:   csType = L"Map";     break;
	default:               csType = L"Unknown"; break;
	}

	// Number="1"
	CString csNumber;
	csNumber.Format(L"%u", Page);

	hr = pWriter->WriteAttributeString(nullptr, L"Number", nullptr, csNumber);
	if (FAILED(hr))
		return;

	// page type
	hr = pWriter->WriteAttributeString(nullptr, L"Type", nullptr, csType);
	if (FAILED(hr))
		return;

	// layout
	pWriter->WriteAttributeString(nullptr, L"Layout", nullptr, Layout);

	/////////////////////////////////////////////////////////////////////////////
	// Write content items
	/////////////////////////////////////////////////////////////////////////////
	int nItem = 1;
	for (auto& Content : m_arrContent.Items)
	{
		CString csTitle = Content.first;
		shared_ptr<CPageContent> pContent = Content.second;
		if (pContent != nullptr)
		{
			pContent->WriteXml(pWriter, Page, nItem++);
		}
	}

	// </Page>
	hr = pWriter->WriteEndElement();

} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPage::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPage::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	const WCHAR* pwszLocalName = nullptr;
	std::shared_ptr<CPageContent> pContent;

	// We expect <Page ...>
	hr = pReader->GetLocalName(&pwszLocalName, nullptr);
	if (FAILED(hr) || pwszLocalName == nullptr)
		return;

	if (wcscmp(pwszLocalName, L"Page") != 0)
		return;

	// Read attributes: Type, Number
	UINT cAttributes = 0;
	hr = pReader->GetAttributeCount(&cAttributes);
	if (FAILED(hr))
		return;

	hr = pReader->MoveToFirstAttribute();
	if (FAILED(hr))
		return;

	for (UINT i = 0; i < cAttributes; i++)
	{
		const WCHAR* pwszAttrName = nullptr;
		const WCHAR* pwszAttrValue = nullptr;

		hr = pReader->GetLocalName(&pwszAttrName, nullptr);
		if (FAILED(hr))
			return;

		hr = pReader->GetValue(&pwszAttrValue, nullptr);
		if (FAILED(hr))
			return;

		if (wcscmp(pwszAttrName, L"Type") == 0)
		{
			if (wcscmp(pwszAttrValue, L"Cover") == 0)
				PageType = pageCover;

			else if (wcscmp(pwszAttrValue, L"TOC") == 0)
				PageType = pageTOC;

			if (wcscmp(pwszLocalName, L"Graph") == 0)
			{
				// Create a CPageGraph that owns a CGraphPlotter
				pContent = std::make_shared<CPageGraph>(m_pDoc);

				// Let the content read its XML subtree
				pContent->ReadXml(pReader);

				// Store it in the page’s content collection
				m_arrContent.add(pContent->Title, pContent, true);
				
				continue;
			}

			else if (wcscmp(pwszAttrValue, L"Picture") == 0)
				PageType = pageImage;

			else if (wcscmp(pwszAttrValue, L"MD") == 0)
				PageType = pageMD;

			else if (wcscmp(pwszAttrValue, L"HTML") == 0)
				PageType = pageHTML;

			else if (wcscmp(pwszAttrValue, L"Map") == 0)
				PageType = pageMap;
			else
				PageType = pageGraph; // safe fallback
		}
		else if (wcscmp(pwszAttrName, L"Number") == 0)
		{
			Page = _wtoi(pwszAttrValue);
		}

		hr = pReader->MoveToNextAttribute();
		if (hr == S_FALSE)
			break;
		if (FAILED(hr))
			return;
	}

	// Move back to element
	hr = pReader->MoveToElement();
	if (FAILED(hr))
		return;

	/////////////////////////////////////////////////////////////////////////////
	// Read content items
	/////////////////////////////////////////////////////////////////////////////
	XmlNodeType nodeType = XmlNodeType_None;

	while (true)
	{
		hr = pReader->Read(&nodeType);
		if (FAILED(hr))
			return;

		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* pwszLocalName = nullptr;
			hr = pReader->GetLocalName(&pwszLocalName, nullptr);
			if (FAILED(hr) || pwszLocalName == nullptr)
				return;

			if (wcscmp(pwszLocalName, L"Page") == 0)
				break; // finished reading this page
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* pwszLocalName = nullptr;
		hr = pReader->GetLocalName(&pwszLocalName, nullptr);
		if (FAILED(hr) || pwszLocalName == nullptr)
			return;

		std::shared_ptr<CPageContent> pContent;

		if (wcscmp(pwszLocalName, L"Graph") == 0)
		{
			// Correct constructor
			pContent = std::make_shared<CPageGraph>(m_pDoc);

			// Read picker + appearance + SQL
			pContent->ReadXml(pReader);

			// Add to new content system
			CString csTitle = pContent->Title;
			m_arrContent.add(csTitle, pContent, true);

			continue;
		}
		else if (wcscmp(pwszLocalName, L"Picture") == 0)
		{
			pContent = std::make_shared<CPageImage>();
		}
		else if (wcscmp(pwszLocalName, L"MD") == 0)
		{
			pContent = std::make_shared<CPageMD>();
		}
		else if (wcscmp(pwszLocalName, L"HTML") == 0)
		{
			pContent = std::make_shared<CPageHTML>();
		}
		else if (wcscmp(pwszLocalName, L"Map") == 0)
		{
			pContent = std::make_shared<CPageMap>();
		}
		else
		{
			// Unknown element — skip it safely
			continue;
		}

		if (pContent != nullptr)
		{
			pContent->ReadXml(pReader);
			CString csTitle = pContent->Title;
			m_arrContent.add(csTitle, pContent, true);
		}
	}

} // ReadXML

/////////////////////////////////////////////////////////////////////////////
