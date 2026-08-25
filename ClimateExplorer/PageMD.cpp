/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageMD.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMD::WriteXml
/////////////////////////////////////////////////////////////////////////////
void CPageMD::WriteXml(IXmlWriter* pWriter)
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
