/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
// CPageContent
//
// Base class for all page‑content types in Climate Explorer.
// Derived classes will include:
//
//     • CPageGraph   (wraps CGraphPlotter)
//     • CPageImage   (PNG/JPEG images)
//     • CPageMD      (Markdown)
//     • CPageHTML    (HTML fragments)
//     • CPageMap     (Station Map)
//
// This class is intentionally minimal so the project continues
// to compile while we migrate functionality into the new design.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <afxstr.h>
#include <xmllite.h>

class CPageContent
{
public:
	///////////////////////////////////////////////////////////////////////////
	// CONTENT_TYPE
	///////////////////////////////////////////////////////////////////////////
	enum CONTENT_TYPE
	{
		ContentGraph,
		ContentImage,
		ContentMD,
		ContentHTML,
		ContentMap
	};

protected:
	// Content type property
	CONTENT_TYPE m_eContentType;

	// Title of the content
	CString m_csTitle;

public:
	// Content type property
	CONTENT_TYPE GetContentType()
	{
		return m_eContentType;
	}

	// Content type property
	void SetContentType(CONTENT_TYPE value)
	{
		m_eContentType = value;
	}

	// Content type property
	__declspec(property(get = GetContentType, put = SetContentType))
		CONTENT_TYPE ContentType;

	// Title of the content
	virtual CString GetTitle()
	{
		return m_csTitle;
	}
	// Title of the content
	void SetTitle(CString value)
	{
		m_csTitle = value;
	}
	// Title of the content
	__declspec(property(get = GetTitle, put = SetTitle))
		CString Title;


public:
	/////////////////////////////////////////////////////////////////////////////
	// ReadElementString
	//
	// Reads the text content of the current element.
	// Assumes the reader is positioned on <ElementName>.
	// Returns true if a text node was read successfully.
	//
	/////////////////////////////////////////////////////////////////////////////
	static bool ReadElementString(IXmlReader* pReader, CString& outValue)
	{
		HRESULT hr = S_OK;
		XmlNodeType nodeType = XmlNodeType_None;

		// Move to text node
		hr = pReader->Read(&nodeType);
		if (FAILED(hr))
			return false;

		if (nodeType == XmlNodeType_Text)
		{
			const WCHAR* pwszValue = nullptr;
			hr = pReader->GetValue(&pwszValue, nullptr);
			if (SUCCEEDED(hr) && pwszValue != nullptr)
			{
				outValue = pwszValue;
			}
		}

		// Move past </Element>
		hr = pReader->Read(&nodeType);
		if (FAILED(hr))
			return false;

		return true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Virtual XML methods (implemented by derived classes)
	///////////////////////////////////////////////////////////////////////////
	virtual void WriteXml(IXmlWriter* pWriter, int nPage=0, int nItem=0) = 0;
	virtual void ReadXml(IXmlReader* pReader) = 0;

public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor / Destructor
	///////////////////////////////////////////////////////////////////////////
	CPageContent()
		: m_eContentType(ContentGraph)   // default; derived classes override
	{
	}

	virtual ~CPageContent()
	{
	}
};
