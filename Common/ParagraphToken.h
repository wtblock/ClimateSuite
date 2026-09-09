/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include "ImagePlus.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CParagraphToken
// 
// A class that represents the tokens that make up a paragraph which include
// colors, fonts, and flags associated with drawing the token.
/////////////////////////////////////////////////////////////////////////////
class CParagraphToken
{
// protected data
protected:
	CString         m_csText;            // raw token text
	double          m_fLength;           // length of text
	shared_ptr<Gdiplus::Font> m_pFont;   // font used for this token
	Color           m_colorFG;           // foreground color
	Color           m_colorBG;           // background color (inline code)
	bool            m_bBackground;       // draw background?
	bool            m_bStrong;           // bold
	bool            m_bEmphasis;         // italic
	bool            m_bInlineCode;       // inline code span
	bool            m_bBlockQuote;       // inside blockquote?
	bool            m_bListItem;         // inside a list?
	bool            m_bListMarker;       // is this the list marker?
	bool            m_bEndOfLine;        // last token in a line
	int             m_nBlockQuoteDepth;  // depth of blockquote nesting
	int             m_nParagraphLine;    // line number within paragraph

// public properties
public:
	///////////////////////////////////////////////////////////////////////////
	// Text
	///////////////////////////////////////////////////////////////////////////
	CString GetText(void) const
	{
		return m_csText;
	}

	void SetText(const CString& csText)
	{
		m_csText = csText;
	}
	__declspec(property(get = GetText, put = SetText))
		CString Text;

	///////////////////////////////////////////////////////////////////////////
	// length of text
	///////////////////////////////////////////////////////////////////////////
	double GetLength(void) const
	{
		return m_fLength;
	}

	void SetLength(double nLine)
	{
		m_fLength = nLine;
	}
	__declspec(property(get = GetLength, put = SetLength))
		double Length;

	///////////////////////////////////////////////////////////////////////////
	// Font
	///////////////////////////////////////////////////////////////////////////
	shared_ptr<Gdiplus::Font> GetFont(void) const
	{
		return m_pFont;
	}

	void SetFont(shared_ptr<Gdiplus::Font> pFont)
	{
		m_pFont = pFont;
	}
	__declspec(property(get = GetFont, put = SetFont))
		shared_ptr<Gdiplus::Font> Font;

	///////////////////////////////////////////////////////////////////////////
	// Foreground Color
	///////////////////////////////////////////////////////////////////////////
	Color GetColorFG(void) const
	{
		return m_colorFG;
	}

	void SetColorFG(const Color& color)
	{
		m_colorFG = color;
	}
	__declspec(property(get = GetColorFG, put = SetColorFG))
		Color ColorFG;

	///////////////////////////////////////////////////////////////////////////
	// Background Color
	///////////////////////////////////////////////////////////////////////////
	Color GetColorBG(void) const
	{
		return m_colorBG;
	}

	void SetColorBG(const Color& color)
	{
		m_colorBG = color;
	}
	__declspec(property(get = GetColorBG, put = SetColorBG))
		Color ColorBG;

	///////////////////////////////////////////////////////////////////////////
	// Background Flag
	///////////////////////////////////////////////////////////////////////////
	bool GetBackground(void) const
	{
		return m_bBackground;
	}

	void SetBackground(bool bBackground)
	{
		m_bBackground = bBackground;
	}
	__declspec(property(get = GetBackground, put = SetBackground))
		bool Background;

	///////////////////////////////////////////////////////////////////////////
	// Strong (Bold)
	///////////////////////////////////////////////////////////////////////////
	bool GetStrong(void) const
	{
		return m_bStrong;
	}

	void SetStrong(bool bStrong)
	{
		m_bStrong = bStrong;
	}
	__declspec(property(get = GetStrong, put = SetStrong))
		bool Strong;

	///////////////////////////////////////////////////////////////////////////
	// Emphasis (Italic)
	///////////////////////////////////////////////////////////////////////////
	bool GetEmphasis(void) const
	{
		return m_bEmphasis;
	}

	void SetEmphasis(bool bEmphasis)
	{
		m_bEmphasis = bEmphasis;
	}
	__declspec(property(get = GetEmphasis, put = SetEmphasis))
		bool Emphasis;

	///////////////////////////////////////////////////////////////////////////
	// Inline Code
	///////////////////////////////////////////////////////////////////////////
	bool GetInlineCode(void) const
	{
		return m_bInlineCode;
	}

	void SetInlineCode(bool bInlineCode)
	{
		m_bInlineCode = bInlineCode;
	}
	__declspec(property(get = GetInlineCode, put = SetInlineCode))
		bool InlineCode;

	///////////////////////////////////////////////////////////////////////////
	// Block Quote
	///////////////////////////////////////////////////////////////////////////
	bool GetBlockQuote(void) const
	{
		return m_bBlockQuote;
	}

	void SetBlockQuote(bool bBlockQuote)
	{
		m_bBlockQuote = bBlockQuote;
	}
	__declspec(property(get = GetBlockQuote, put = SetBlockQuote))
		bool BlockQuote;

	///////////////////////////////////////////////////////////////////////////
	// List item
	///////////////////////////////////////////////////////////////////////////
	bool GetListItem(void) const
	{
		return m_bListItem;
	}

	void SetListItem(bool bListItem)
	{
		m_bListItem = bListItem;
	}
	__declspec(property(get = GetListItem, put = SetListItem))
		bool ListItem;

	///////////////////////////////////////////////////////////////////////////
	// Block Quote Depth
	///////////////////////////////////////////////////////////////////////////
	int GetBlockQuoteDepth(void) const
	{
		return m_nBlockQuoteDepth;
	}

	void SetBlockQuoteDepth(int nDepth)
	{
		m_nBlockQuoteDepth = nDepth;
	}
	__declspec(property(get = GetBlockQuoteDepth, put = SetBlockQuoteDepth))
		int BlockQuoteDepth;

	///////////////////////////////////////////////////////////////////////////
	// List Marker
	///////////////////////////////////////////////////////////////////////////
	bool GetListMarker(void) const
	{
		return m_bListMarker;
	}

	void SetListMarker(bool bListMarker)
	{
		m_bListMarker = bListMarker;
	}
	__declspec(property(get = GetListMarker, put = SetListMarker))
		bool ListMarker;

	///////////////////////////////////////////////////////////////////////////
	// End of Line
	///////////////////////////////////////////////////////////////////////////
	bool GetEndOfLine(void) const
	{
		return m_bEndOfLine;
	}
	void SetEndOfLine(bool bEndOfLine)
	{
		m_bEndOfLine = bEndOfLine;
	}
	__declspec(property(get = GetEndOfLine, put = SetEndOfLine))
		bool EndOfLine;

	///////////////////////////////////////////////////////////////////////////
	// Paragraph Line Number
	///////////////////////////////////////////////////////////////////////////
	int GetParagraphLine(void) const
	{
		return m_nParagraphLine;
	}

	void SetParagraphLine(int nLine)
	{
		m_nParagraphLine = nLine;
	}
	__declspec(property(get = GetParagraphLine, put = SetParagraphLine))
		int ParagraphLine;

// protected methods
protected:

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CParagraphToken()
	{
		m_csText.Empty();
		m_fLength = 0;
		m_pFont.reset();
		m_colorFG = Color::Black;
		m_colorBG = Color::White;
		m_bBackground = false;
		m_bStrong = false;
		m_bEmphasis = false;
		m_bInlineCode = false;
		m_bBlockQuote = false;
		m_bListItem = false;
		m_nBlockQuoteDepth = 0;
		m_bListMarker = false;
		m_bEndOfLine = false;
		m_nParagraphLine = 1;
	}

	CParagraphToken(shared_ptr<CParagraphToken> pToken)
	{
		Text = pToken->Text;
		Length = pToken->Length;
		Font = pToken->Font;
		ColorFG = pToken->ColorFG;
		ColorBG = pToken->ColorBG;
		Background = pToken->Background;
		Strong = pToken->Strong;
		Emphasis = pToken->Emphasis;
		InlineCode = pToken->InlineCode;
		BlockQuote = pToken->BlockQuote;
		ListItem = pToken->ListItem;
		BlockQuoteDepth = pToken->BlockQuoteDepth;
		ListMarker = pToken->ListMarker;
		EndOfLine = pToken->EndOfLine;
		ParagraphLine = pToken->ParagraphLine;

	}
	~CParagraphToken()
	{

	}
}; // class CParagraphToken

/////////////////////////////////////////////////////////////////////////////
