/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "PageContent.h"
#include <afxstr.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
// CPageMD
//
// Shell wrapper for Markdown content. This class will eventually handle:
//
//     • XML serialization of Markdown text
//     • XML deserialization of Markdown text
//     • CE (.CE) minimal export (likely PNG only)
//     • CEx (.CEx) full‑fidelity export
//     • Integration with CPageContent polymorphism
//
// For now, it simply stores Markdown text and compiles cleanly.
//
/////////////////////////////////////////////////////////////////////////////
class CPageMD : public CPageContent
{
// protected data
protected:
	///////////////////////////////////////////////////////////////////////////
	// pointer back to the hosting document
	///////////////////////////////////////////////////////////////////////////
	CClimateExplorerDoc* m_pDoc;

	CString m_csMarkdown;   // raw markdown text

// public properties
public:
	// image of the content
	virtual shared_ptr<Gdiplus::Image> GetImageContent();

	CString GetMarkdown()
	{
		return m_csMarkdown;
	}

	void SetMarkdown(const CString& value)
	{
		m_csMarkdown = value;
		m_pImageContent.reset(); // force re-render
	}

	__declspec(property(get = GetMarkdown, put = SetMarkdown))
		CString Markdown;

	// get paths to images in the markdown
	std::vector<CString> GetImageReferences()
	{
		std::vector<CString> value;
		int nMD = 0;
		CString csLine = m_csMarkdown.Tokenize(L"\n\r", nMD);
		while (!csLine.IsEmpty())
		{
			csLine.TrimLeft(L" \t");
			if (csLine.Left(2) == L"![")
			{
				// if there is not text between the brackets
				bool bNoAlt = csLine.Left(4) == L"![](";

				// parse the pathname
				int nLine = 0;
				CString csAlt = csLine.Tokenize(L"![]()", nLine);
				if (!csAlt.IsEmpty())
				{
					CString csPath;
					if (bNoAlt)
					{
						csPath = csAlt;
					}
					else
					{
						csPath = csLine.Tokenize(L"![]()", nLine);
					}

					if (!csPath.IsEmpty())
					{
						value.push_back(csPath);
					}
				}
			}
			csLine = m_csMarkdown.Tokenize(L"\n\r", nMD);
		}
		return value;
	}
	// get paths to images in the markdown
	__declspec(property(get = GetImageReferences))
		std::vector<CString> ImageReferences;

// public override methods
public:
	///////////////////////////////////////////////////////////////////////////
	// WriteXml
	//
	// Shell only — does nothing yet.
	// Will be implemented during the serialization refactor.
	///////////////////////////////////////////////////////////////////////////
	virtual void WriteXml
	(
		IXmlWriter* pWriter, int nPage = 0, int nItem = 0
	) override;

	///////////////////////////////////////////////////////////////////////////
	// ReadXml
	//
	// Shell only — does nothing yet.
	// Will be implemented during the serialization refactor.
	///////////////////////////////////////////////////////////////////////////
	virtual void ReadXml(IXmlReader* pReader, int nPage = 0, int nItem = 0) override;

public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor / Destructor
	///////////////////////////////////////////////////////////////////////////
	CPageMD()
	{
		ContentType = ContentMD;
		m_pDoc = nullptr;
	}
	CPageMD(CClimateExplorerDoc* pDoc);

	virtual ~CPageMD()
	{
	}
};
