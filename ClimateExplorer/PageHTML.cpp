/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageHTML.h"

/////////////////////////////////////////////////////////////////////////////
// CPageHTML::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPageHTML::WriteXml(IXmlWriter* pWriter, int /*nPage*/, int /*nItem*/)
{
	HRESULT hr = S_OK;

	// <HTML>
	hr = pWriter->WriteStartElement(nullptr, L"HTML", nullptr);
	if (FAILED(hr))
		return;

	// Placeholder — no content yet

	// </HTML>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageHTML::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageHTML::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	// Skip until </HTML>
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

			if (wcscmp(pwszLocalName, L"HTML") == 0)
				break;
		}
	}
} // ReadXml

/////////////////////////////////////////////////////////////////////////////
