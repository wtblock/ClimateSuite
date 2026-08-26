///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
///////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageEx.h"
#include "PageGraph.h"
#include "PageImage.h"
#include "PageMD.h"
#include "PageHTML.h"
#include "PageMap.h"

/////////////////////////////////////////////////////////////////////////////
// CPageEx::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPageEx::WriteXml(IXmlWriter* pWriter)
{
	HRESULT hr = S_OK;

	// <Page>
	hr = pWriter->WriteStartElement(nullptr, L"Page", nullptr);
	if (FAILED(hr))
		return;

	// Type="Graph" / "Image" / "MD" / "HTML" / "Map" / "Cover" / "TOC"
	CString csType;

	switch (m_eType)
	{
	case pageCover:
		csType = L"Cover";
		break;

	case pageTOC:
		csType = L"TOC";
		break;

	case pageGraph:
		csType = L"Graph";
		break;

	case pageImage:
		csType = L"Image";
		break;

	case pageMD:
		csType = L"MD";
		break;

	case pageHTML:
		csType = L"HTML";
		break;

	case pageMap:
		csType = L"Map";
		break;

	default:
		csType = L"Unknown";
		break;
	}

	hr = pWriter->WriteAttributeString(nullptr, L"Type", nullptr, csType);
	if (FAILED(hr))
		return;

	// Number="1"
	CString csNumber;
	csNumber.Format(L"%u", m_nPage);

	hr = pWriter->WriteAttributeString(nullptr, L"Number", nullptr, csNumber);
	if (FAILED(hr))
		return;

	/////////////////////////////////////////////////////////////////////////////
	// Write content items
	/////////////////////////////////////////////////////////////////////////////
	for (auto& pContent : m_arrContent)
	{
		if (pContent != nullptr)
		{
			pContent->WriteXml(pWriter);
		}
	}

	// </Page>
	hr = pWriter->WriteEndElement();

} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageEx::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageEx::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	const WCHAR* pwszLocalName = nullptr;

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
				m_eType = pageCover;

			else if (wcscmp(pwszAttrValue, L"TOC") == 0)
				m_eType = pageTOC;

			else if (wcscmp(pwszAttrValue, L"Graph") == 0)
				m_eType = pageGraph;

			else if (wcscmp(pwszAttrValue, L"Image") == 0)
				m_eType = pageImage;

			else if (wcscmp(pwszAttrValue, L"MD") == 0)
				m_eType = pageMD;

			else if (wcscmp(pwszAttrValue, L"HTML") == 0)
				m_eType = pageHTML;

			else if (wcscmp(pwszAttrValue, L"Map") == 0)
				m_eType = pageMap;

			else
				m_eType = pageGraph; // safe fallback
		}
		else if (wcscmp(pwszAttrName, L"Number") == 0)
		{
			m_nPage = _wtoi(pwszAttrValue);
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
			pContent = std::make_shared<CPageGraph>();
		}
		else if (wcscmp(pwszLocalName, L"Image") == 0)
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
			m_arrContent.push_back(pContent);
		}
	}

} // ReadXML

///////////////////////////////////////////////////////////////////////////
