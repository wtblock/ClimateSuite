/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageMD.h"
#include "ClimateExplorerDoc.h"
#include "MarkdownBitmapRenderer.h"
#include "MarkdownParser.h"

/////////////////////////////////////////////////////////////////////////////
CPageMD::CPageMD(CClimateExplorerDoc* pDoc)
{
	ContentType = ContentMD;
	m_pDoc = pDoc;
} // CPageImage

/////////////////////////////////////////////////////////////////////////////
// CPageMD::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPageMD::WriteXml(IXmlWriter* pWriter, int /*nPage*/, int /*nItem*/)
{
	HRESULT hr = S_OK;

	// <MD>
	hr = pWriter->WriteStartElement(nullptr, L"MD", nullptr);
	if (FAILED(hr))
		return;

	// Placeholder — no content yet

	// </MD>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageMD::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageMD::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	// Skip until </MD>
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

			if (wcscmp(pwszLocalName, L"MD") == 0)
				break;
		}
	}
} // ReadXml

/////////////////////////////////////////////////////////////////////////////
// image of the content
shared_ptr<Gdiplus::Image> CPageMD::GetImageContent()
{
	shared_ptr<Gdiplus::Image> value = m_pImageContent;

	// already rendered → return cached image
	if (value != nullptr)
	{
		return value;
	}

	// ---------------------------------------------------------
	// Load markdown text if not already loaded
	// ---------------------------------------------------------
	if (m_csMarkdown.IsEmpty())
	{
		CString csPath = ContentPath;

		if (::PathFileExists(csPath))
		{
			vector<CString> lines = CHelper::ReadTextAuto(csPath);

			CString csText;
			for (const CString& line : lines)
			{
				csText += line;
				csText += L"\r\n";
			}

			m_csMarkdown = csText;
		}
	}

	// still empty → nothing to render
	if (m_csMarkdown.IsEmpty())
	{
		return nullptr;
	}

	// ---------------------------------------------------------
	// Render markdown into a bitmap
	// ---------------------------------------------------------
	shared_ptr<CMarkdownBitmapRenderer> pRenderer =
		make_shared<CMarkdownBitmapRenderer>();

	Gdiplus::RectF rcMargin = m_pDoc->RealMargin;
	rcMargin.Y -= m_pDoc->TopOfPage;
	rcMargin.Y -= m_pDoc->TopMargin;
	rcMargin.X -= m_pDoc->LeftMargin;

	pRenderer->MarginInches = rcMargin;
	pRenderer->XInches = rcMargin.X;
	pRenderer->YInches = rcMargin.Y;
	pRenderer->X = pRenderer->ToPixelsX(pRenderer->XInches);
	pRenderer->Y = pRenderer->ToPixelsY(pRenderer->YInches);

	// convert inches → pixels using renderer.Dpi
	int nWidth = (int)(rcMargin.Width * pRenderer->Dpi);
	int nHeight = (int)(rcMargin.Height * pRenderer->Dpi);

	if (!pRenderer->CreateBitmap(nWidth, nHeight))
	{
		return nullptr;
	}

	CMarkdownParser parser;
	parser.Renderer = pRenderer;
	parser.Markdown = m_csMarkdown;

	parser.Parse();

	// ---------------------------------------------------------
	// Wrap bitmap in CImagePlus
	// ---------------------------------------------------------
	shared_ptr<CImagePlus> pImagePlus = pRenderer->Finalize();
	if (!pImagePlus)
	{
		return nullptr;
	}

	// ---------------------------------------------------------
	// Cache and return
	// ---------------------------------------------------------
	m_pImageContent = pImagePlus->ImagePlus;

	return m_pImageContent;
} // GetImageContent

/////////////////////////////////////////////////////////////////////////////
