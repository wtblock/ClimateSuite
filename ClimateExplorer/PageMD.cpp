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
// CPageMD::WriteXml  (Unified CE + CEx)
// Writes <Markdown> with Text + TextPath.
// If ZipWriter->IsOpen == true → writes PNG + MD file to ZIP.
/////////////////////////////////////////////////////////////////////////////
void CPageMD::WriteXml(IXmlWriter* pWriter, int nPage, int nItem)
{
	HRESULT hr = S_OK;

	// <Markdown>
	hr = pWriter->WriteStartElement(nullptr, L"Markdown", nullptr);
	if (FAILED(hr)) return;

	// -------------------------------------------------------------
	// <Text>  (always written)
	// -------------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"Text", nullptr);
	if (FAILED(hr)) return;

	hr = pWriter->WriteString(Markdown);
	if (FAILED(hr)) return;

	hr = pWriter->WriteEndElement(); // </Text>
	if (FAILED(hr)) return;

	// -------------------------------------------------------------
	// <TextPath>  (always written — needed for CEx → CE Save As)
	// -------------------------------------------------------------
	if (!ContentPath.IsEmpty())
	{
		hr = pWriter->WriteStartElement(nullptr, L"TextPath", nullptr);
		if (FAILED(hr)) return;

		hr = pWriter->WriteString(ContentPath);
		if (FAILED(hr)) return;

		hr = pWriter->WriteEndElement(); // </TextPath>
		if (FAILED(hr)) return;
	}

	// -------------------------------------------------------------
	// Always write title (CEx + CE)
	// -------------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"Title", nullptr);
	if (FAILED(hr)) return;

	hr = pWriter->WriteString(ContentTitle);
	if (FAILED(hr)) return;

	hr = pWriter->WriteEndElement(); // </Title>

	// -------------------------------------------------------------
	//// CE-only behavior: write PNG + MD file to ZIP
	// -------------------------------------------------------------
	auto pZip = m_pDoc->ZipWriter;
	if (pZip && pZip->IsOpen)
	{
		// Compute CE filenames
		CString csImageFilename;
		csImageFilename.Format
		(
			L"Markdown/Page_%04u_Markdown_%02u.png",
			nPage,
			nItem
		);

		CString csTextFilename;
		csTextFilename.Format
		(
			L"Markdown/Page_%04u_Markdown_%02u.md",
			nPage,
			nItem
		);

		// <ImagePath>
		hr = pWriter->WriteStartElement(nullptr, L"ImagePath", nullptr);
		if (FAILED(hr)) return;
		hr = pWriter->WriteString(csImageFilename);
		if (FAILED(hr)) return;
		hr = pWriter->WriteEndElement();

		// <TextPath>
		hr = pWriter->WriteStartElement(nullptr, L"TextPath", nullptr);
		if (FAILED(hr)) return;
		hr = pWriter->WriteString(csTextFilename);
		if (FAILED(hr)) return;
		hr = pWriter->WriteEndElement();

		// -------------------------------------------------------------
		// Write PNG to ZIP
		// -------------------------------------------------------------
		if (ImageContent != nullptr)
		{
			std::vector<BYTE> pngBytes;
			bool bOK = CHelper::EncodeBitmapToMemory
			(
				static_cast<Gdiplus::Bitmap*>(ImageContent.get()),
				L"image/png",
				pngBytes
			);

			if (bOK)
			{
				pZip->AddFile(csImageFilename, pngBytes.data(), pngBytes.size());
			}
		}

		// -------------------------------------------------------------
		// Write .md text file to ZIP (UTF‑8)
		// -------------------------------------------------------------
		{
			std::string utf8 = CHelper::Utf16ToUtf8(Markdown);
			pZip->AddFile
			(
				csTextFilename,
				reinterpret_cast<const BYTE*>(utf8.data()),
				utf8.size()
			);
		}
	}

	// </Markdown>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageMD::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageMD::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	CString csText;        // raw markdown text
	CString csTextPath;    // external or CE internal .md path
	CString csImagePath;   // CE internal .png path
	CString csTitle;       // title of the page section

	while (pReader->Read(&nodeType) == S_OK)
	{
		// End of <Markdown>
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);
			if (name && wcscmp(name, L"Markdown") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		// ---------------------------------------------------------
		// <Text>raw markdown</Text>
		// ---------------------------------------------------------
		if (wcscmp(name, L"Text") == 0)
		{
			XmlNodeType ntText;
			hr = pReader->Read(&ntText);

			if (SUCCEEDED(hr) && ntText == XmlNodeType_Text)
			{
				const WCHAR* pwszText = nullptr;
				pReader->GetValue(&pwszText, nullptr);

				if (pwszText)
					csText = pwszText;
			}

			continue;
		}

		// ---------------------------------------------------------
		// <TextPath>external or CE internal .md path</TextPath>
		// ---------------------------------------------------------
		if (wcscmp(name, L"TextPath") == 0)
		{
			XmlNodeType ntText;
			hr = pReader->Read(&ntText);

			if (SUCCEEDED(hr) && ntText == XmlNodeType_Text)
			{
				const WCHAR* pwszText = nullptr;
				pReader->GetValue(&pwszText, nullptr);

				if (pwszText)
					csTextPath = pwszText;
			}

			continue;
		}

		// ---------------------------------------------------------
		// <Title>text</Title>
		// ---------------------------------------------------------
		if (wcscmp(name, L"Title") == 0)
		{
			XmlNodeType ntText;
			hr = pReader->Read(&ntText);

			if (SUCCEEDED(hr) && ntText == XmlNodeType_Text)
			{
				const WCHAR* pwszText = nullptr;
				pReader->GetValue(&pwszText, nullptr);

				if (pwszText)
					csTitle = pwszText;
			}

			continue;
		}

		// ---------------------------------------------------------
		// CE-only: <ImagePath>Markdown/Page_XXXX_Markdown_YY.png</ImagePath>
	// ---------------------------------------------------------
		if (wcscmp(name, L"ImagePath") == 0)
		{
			XmlNodeType ntText;
			hr = pReader->Read(&ntText);

			if (SUCCEEDED(hr) && ntText == XmlNodeType_Text)
			{
				const WCHAR* pwszText = nullptr;
				pReader->GetValue(&pwszText, nullptr);

				if (pwszText)
					csImagePath = pwszText;
			}

			continue;
		}
	}

	// ---------------------------------------------------------
	// Store title, text, and path
	// ---------------------------------------------------------
	ContentTitle = csTitle;
	Markdown = csText;
	ContentPath = csTextPath;

	// ---------------------------------------------------------
	// Load Markdown text from CE ZIP if present
	// ---------------------------------------------------------
	if (!csTextPath.IsEmpty() && m_pDoc && m_pDoc->ZipReader)
	{
		auto pZip = m_pDoc->ZipReader;

		if (pZip->IsOpen)
		{
			std::vector<uint8_t> bytes;

			if (pZip->ExtractFile(csTextPath, bytes))
			{
				// Convert UTF‑8 → UTF‑16 using your helper
				std::string utf8(bytes.begin(), bytes.end());
				CString csUtf16 = CHelper::Utf8ToUtf16(utf8.data(), utf8.size());

				Markdown = csUtf16;
			}
		}
	}
	else
	{
		// ---------------------------------------------------------
		// Load Markdown text from external filesystem (CEx)
		// ---------------------------------------------------------
		if (!csTextPath.IsEmpty())
		{
			if (::PathFileExists(csTextPath))
			{
				vector<CString> lines = CHelper::ReadTextAuto(csTextPath);

				CString csTextFile;
				for (const CString& line : lines)
				{
					csTextFile += line;
					csTextFile += L"\r\n";
				}

				Markdown = csTextFile;
			}
		}
	}

	// ---------------------------------------------------------
	// Load rendered PNG from CE ZIP if present
	// ---------------------------------------------------------
	if (!csImagePath.IsEmpty() && m_pDoc && m_pDoc->ZipReader)
	{
		auto pZip = m_pDoc->ZipReader;

		if (pZip->IsOpen)
		{
			std::vector<uint8_t> bytes;

			if (pZip->ExtractFile(csImagePath, bytes))
			{
				IStream* pStream = SHCreateMemStream(bytes.data(),
					(UINT)bytes.size());
				if (pStream)
				{
					Gdiplus::Image* pImg = Gdiplus::Image::FromStream(pStream);
					pStream->Release();

					if (pImg)
					{
						m_pImageContent = shared_ptr<Gdiplus::Image>(pImg);
						return;
					}
				}
			}
		}
	}

	// ---------------------------------------------------------
	// If CE image not present, Markdown will render lazily on demand
	//
	// (m_pImageContent stays null; GetImageContent() will render)
	// ---------------------------------------------------------

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
	ImageContent = pImagePlus->ImagePlus;

	return ImageContent;
} // GetImageContent

/////////////////////////////////////////////////////////////////////////////
