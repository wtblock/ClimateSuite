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
#include "MainFrm.h"
#include "ImagePlus.h"
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
	CString m_csContentTitle;

	// path of the content
	CString m_csContentPath;

	// image of the content
	shared_ptr<Gdiplus::Image> m_pImageContent;

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
	virtual CString GetContentTitle()
	{
		return m_csContentTitle;
	}
	// Title of the content
	void SetContentTitle(CString value)
	{
		m_csContentTitle = value;
	}
	// Title of the content
	__declspec(property(get = GetContentTitle, put = SetContentTitle))
		CString ContentTitle;

	// path of the content
	virtual CString GetContentPath()
	{
		return m_csContentPath;
	}
	// path of the content
	void SetContentPath(CString value)
	{
		m_csContentPath = value;
	}
	// path of the content
	__declspec(property(get = GetContentPath, put = SetContentPath))
		CString ContentPath;

	// image of the content
	virtual shared_ptr<Gdiplus::Image> GetImageContent()
	{
		return m_pImageContent;
	}
	// image of the content
	void SetImageContent(shared_ptr<Gdiplus::Image> value)
	{
		m_pImageContent = value;
	}
	// image of the content
	__declspec(property(get = GetImageContent, put = SetImageContent))
		shared_ptr<Gdiplus::Image> ImageContent;

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
