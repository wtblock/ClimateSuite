/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageImage.h"
#include "PlusGDI.h"
#include "ClimateExplorerDoc.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
CPageImage::CPageImage(CClimateExplorerDoc* pDoc)
{
	ContentType = ContentImage;
	m_pDoc = pDoc;
} // CPageImage

/////////////////////////////////////////////////////////////////////////////
// CPageImage::WriteXml  (Unified CE + CEx)
// Writes <Picture> with ImagePath + Title.
// If ZipWriter->IsOpen == true → embeds <Image> and writes PNG to ZIP.
/////////////////////////////////////////////////////////////////////////////
void CPageImage::WriteXml(IXmlWriter* pWriter, int nPage, int nItem)
{
	HRESULT hr = S_OK;
	if (m_pDoc == nullptr)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		m_pDoc = pFrame->ClimateExplorerDocument;
	}

	// <Picture>
	hr = pWriter->WriteStartElement(nullptr, L"Picture", nullptr);
	if (FAILED(hr)) return;

	// -------------------------------------------------------------
	// Always write external path (CEx + CE)
	// -------------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"ImagePath", nullptr);
	if (FAILED(hr)) return;

	hr = pWriter->WriteString(m_csContentPath);
	if (FAILED(hr)) return;

	hr = pWriter->WriteEndElement(); // </ImagePath>

	// -------------------------------------------------------------
	// Always write title (CEx + CE)
	// -------------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"Title", nullptr);
	if (FAILED(hr)) return;

	hr = pWriter->WriteString(m_csContentTitle);
	if (FAILED(hr)) return;

	hr = pWriter->WriteEndElement(); // </Title>

	// -------------------------------------------------------------
	// CE-only behavior: embed <Image> and write PNG to ZIP
	// -------------------------------------------------------------
	auto pZip = m_pDoc->ZipWriter;
	if (pZip && pZip->IsOpen)
	{
		// Compute CE filename
		CString csFilename;
		csFilename.Format
		(
			L"Images/Page_%04u_Picture_%02u.png",
			nPage,
			nItem
		);

		// <Image value="Images/Page_XXXX_Picture_YY.png"/>
		hr = pWriter->WriteStartElement(nullptr, L"Image", nullptr);
		if (FAILED(hr)) return;

		hr = pWriter->WriteAttributeString(nullptr, L"value", nullptr, csFilename);
		if (FAILED(hr)) return;

		hr = pWriter->WriteEndElement(); // </Image>

		// ---------------------------------------------------------
		// Convert m_pImageContent → PNG bytes
		// ---------------------------------------------------------
		if (m_pImageContent != nullptr)
		{
			std::vector<BYTE> pngBytes;
			bool bOK = CHelper::EncodeBitmapToMemory
			(
				static_cast<Gdiplus::Bitmap*>(m_pImageContent.get()),
				L"image/png",
				pngBytes
			);

			// Use your generalized encoder
			if (bOK)
			{
				// Add PNG to ZIP
				pZip->AddFile(csFilename, pngBytes.data(), pngBytes.size());
			}
		}
	}

	// </Picture>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageImage::ReadXml introduces a new Picture block
// Question: Is the page Type needed? In the following example
// // there is a graph and a picture.
//  <Page Number="3" Type="Graph" Layout="Half">
//    <Graph>
//    </Graph>
//    <Picture>
//      <ImagePath value=".\Images\COP Meetings vs. CO2.jpg" type="string" />
//      <Title value="COP Meetings vs. CO2" type="string" />
//    </Picture>
//  </Page>
/////////////////////////////////////////////////////////////////////////////
// CPageImage::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageImage::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	CString csExternalPath;
	CString csTitle;

	// Temporary: CE ZIP path
	CString csZipPath;

	while (pReader->Read(&nodeType) == S_OK)
	{
		// End of <Picture>
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);
			if (name && wcscmp(name, L"Picture") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		//
		// <ImagePath>external path</ImagePath>
		//
		if (wcscmp(name, L"ImagePath") == 0)
		{
			XmlNodeType ntText;
			hr = pReader->Read(&ntText);

			if (SUCCEEDED(hr) && ntText == XmlNodeType_Text)
			{
				const WCHAR* pwszText = nullptr;
				pReader->GetValue(&pwszText, nullptr);

				if (pwszText)
					csExternalPath = pwszText;
			}

			continue;
		}

		//
		// <Title>text</Title>
		//
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

		//
		// CE-only: <Image value="Images/Page_XXXX_Picture_YY.ext"/>
		//
		if (wcscmp(name, L"Image") == 0)
		{
			const WCHAR* attrName = nullptr;
			const WCHAR* attrValue = nullptr;

			while (pReader->MoveToNextAttribute() == S_OK)
			{
				pReader->GetLocalName(&attrName, nullptr);
				pReader->GetValue(&attrValue, nullptr);

				if (wcscmp(attrName, L"value") == 0)
					csZipPath = attrValue;
			}

			pReader->MoveToElement();
			continue;
		}
	}

	//
	// Store title and external path
	//
	ContentTitle = csTitle;
	ContentPath = csExternalPath;

	//
	// Load image immediately
	//

	// CE: load from ZIP if present
	if (!csZipPath.IsEmpty() && m_pDoc && m_pDoc->ZipReader)
	{
		auto pZip = m_pDoc->ZipReader;

		if (pZip->IsOpen)
		{
			std::vector<uint8_t> bytes;

			if (pZip->ExtractFile(csZipPath, bytes))
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

	// CEx: load from external filesystem path
	if (!csExternalPath.IsEmpty())
	{
		if (::PathFileExists(csExternalPath))
		{
			CPlusGDI gdi;
			if (gdi.Open(csExternalPath))
			{
				Gdiplus::Image* pImage = CHelper::CopyImage(gdi.GetImage());
				m_pImageContent = shared_ptr<Gdiplus::Image>(pImage);
				return;
			}
		}
	}

	// If neither source worked, leave m_pImageContent null
}

/////////////////////////////////////////////////////////////////////////////
// image of the content
shared_ptr<Gdiplus::Image> CPageImage::GetImageContent()
{
	shared_ptr<Gdiplus::Image> value = m_pImageContent;

	if (value == nullptr)
	{
		CPlusGDI gdi;

		CString csPath = ContentPath;
		if (::PathFileExists(csPath))
		{
			if (gdi.Open(csPath))
			{
				Gdiplus::Image* pImage = CHelper::CopyImage(gdi.GetImage());
				value = shared_ptr<Gdiplus::Image>(pImage);
				m_pImageContent = value;
			}
		}
	}
	
	int imgWidth = value->GetWidth();
	int imgHeight = value->GetHeight();
	return value;
} // GetImageContent

/////////////////////////////////////////////////////////////////////////////
