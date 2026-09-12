/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "md4c.h"
#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Forward declaration
/////////////////////////////////////////////////////////////////////////////
class CMarkdownRenderer;

/////////////////////////////////////////////////////////////////////////////
// CMarkdownParser
//
// Wraps the md4c C library in a C++ class suitable for Climate Explorer.
// Provides:
//   • Parse() method
//   • Callback dispatch
//   • Properties for debugging
//   • Integration with Markdown → Bitmap renderer
/////////////////////////////////////////////////////////////////////////////
class CMarkdownParser
{
// public types
public:
	enum ConflictRule
	{
		ConflictRule_Allow = 0,
		ConflictRule_Downgrade = 1
	};

// protected data
protected:
	CString m_csMarkdown;                          // raw markdown text
	shared_ptr<CMarkdownRenderer> m_pRenderer;     // renderer target
	bool m_bInStrong = false;
	bool m_bInEmphasis = false;
	bool m_bInInlineCode = false;

	MD_BLOCKTYPE m_eCurrentBlock = MD_BLOCK_DOC; // or MD_BLOCK_DOC as default

// public properties
public:
	CString GetMarkdown()
	{
		return m_csMarkdown;
	}

	void SetMarkdown(CString value)
	{
		m_csMarkdown = value;
	}

	shared_ptr<CMarkdownRenderer> GetRenderer()
	{
		return m_pRenderer;
	}

	void SetRenderer(shared_ptr<CMarkdownRenderer> value)
	{
		m_pRenderer = value;
	}

	__declspec(property(get = GetMarkdown, put = SetMarkdown))
		CString Markdown;

	__declspec(property(get = GetRenderer, put = SetRenderer))
		shared_ptr<CMarkdownRenderer> Renderer;

	bool GetInStrong() const 
	{ 
		return m_bInStrong; 
	}
	void SetInStrong(bool value) 
	{ 
		m_bInStrong = value; 
	}

	bool GetInEmphasis() const 
	{ 
		return m_bInEmphasis; 
	}
	void SetInEmphasis(bool value) 
	{ 
		m_bInEmphasis = value; 
	}

	bool GetInInlineCode() const 
	{ 
		return m_bInInlineCode; 
	}
	void SetInInlineCode(bool value) 
	{ 
		m_bInInlineCode = value; 
	}

	__declspec(property(get = GetInStrong, put = SetInStrong))
		bool InStrong;

	__declspec(property(get = GetInEmphasis, put = SetInEmphasis))
		bool InEmphasis;

	__declspec(property(get = GetInInlineCode, put = SetInInlineCode))
		bool InInlineCode;

	MD_BLOCKTYPE GetCurrentBlock() const
	{
		return m_eCurrentBlock;
	}

	void SetCurrentBlock(MD_BLOCKTYPE eValue)
	{
		m_eCurrentBlock = eValue;
	}

	__declspec(property(get = GetCurrentBlock, put = SetCurrentBlock))
		MD_BLOCKTYPE CurrentBlock;

// protected methods
protected:
	ConflictRule ResolveConflict
	(
		MD_BLOCKTYPE eBlock,
		MD_SPANTYPE  eSpan
	);

	ConflictRule ResolveTextConflict
	(
		MD_BLOCKTYPE eBlock,
		MD_TEXTTYPE  eText
	);

	static int EnterBlockCallback
	(
		const MD_BLOCKTYPE type,
		void* detail,
		void* userdata
	);

	static int LeaveBlockCallback
	(
		const MD_BLOCKTYPE type,
		void* detail,
		void* userdata
	);

	static int EnterSpanCallback
	(
		const MD_SPANTYPE type,
		void* detail,
		void* userdata
	);

	static int LeaveSpanCallback
	(
		const MD_SPANTYPE type,
		void* detail,
		void* userdata
	);

	static int TextCallback
	(
		MD_TEXTTYPE       type,
		const MD_CHAR* text,
		MD_SIZE           size,
		void* userdata
	);

// public methods
public:
	bool Parse();

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CMarkdownParser()
	{
		m_pRenderer = nullptr;
	}

	~CMarkdownParser()
	{

	}
}; // class CMarkdownParser

/////////////////////////////////////////////////////////////////////////////
