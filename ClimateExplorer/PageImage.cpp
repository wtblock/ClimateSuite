/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageImage.h"

/////////////////////////////////////////////////////////////////////////////
// CPageImage::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPageImage::WriteXml(IXmlWriter* pWriter)
{
	HRESULT hr = S_OK;

	// <Image>
	hr = pWriter->WriteStartElement(nullptr, L"Image", nullptr);
	if (FAILED(hr))
		return;

	// Placeholder — no content yet

	// </Image>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageImage::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageImage::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	// Skip until </Image>
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

			if (wcscmp(pwszLocalName, L"Image") == 0)
				break;
		}
	}
} // ReadXml

/////////////////////////////////////////////////////////////////////////////
