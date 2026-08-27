/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageMap.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMap::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPageMap::WriteXml(IXmlWriter* pWriter, int /*nPage*/, int /*nItem*/)
{
	HRESULT hr = S_OK;

	// <Map>
	hr = pWriter->WriteStartElement(nullptr, L"Map", nullptr);
	if (FAILED(hr))
		return;

	// Placeholder — no content yet

	// </Map>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageMap::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageMap::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	// Skip until </Map>
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

			if (wcscmp(pwszLocalName, L"Map") == 0)
				break;
		}
	}
} // ReadXml

/////////////////////////////////////////////////////////////////////////////
