/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ParagraphToken.h"
#include "SmartArray.h"
#include <memory>
#include "ImagePlus.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CParagraph
// 
// A class that contains all of the tokens of a paragraph and determines
// how to layout the lines so they can be rendered justified in the
// horizontal space
/////////////////////////////////////////////////////////////////////////////
class CParagraph
{
// protected data
protected:
	// collection of tokens that make up a paragraph
	CSmartArray<CParagraphToken> m_arrTokens;

	double m_dDpi;

	double m_fXInches;
	double m_fYInches;

	Graphics* m_pGraphics;

	bool m_bExpanded;

	// right justify text?
	bool m_bJustify;

	double m_dMarkerLength;

	double m_dListIndent;

	Gdiplus::RectF m_rcMarginInches;

// public properties
public:
	// number of tokens in the paragraph
	long GetCount()
	{
		return m_arrTokens.Count;
	}
	// number of tokens in the paragraph
	__declspec(property(get = GetCount))
		long Count;

	// tokens of the paragraph
	vector<shared_ptr<CParagraphToken> > GetItems()
	{
		return m_arrTokens.Items;
	}
	// tokens of the paragraph
	__declspec(property(get = GetItems))
		vector<shared_ptr<CParagraphToken> > Items;

	shared_ptr<CParagraphToken> GetLastToken()
	{
		shared_ptr<CParagraphToken> value = nullptr;
		if (Count > 0)
		{
			value = m_arrTokens.get(Count - 1);
		}
		return value;
	}
	__declspec(property(get = GetLastToken))
		shared_ptr<CParagraphToken> LastToken;

	// dots per inch
	double GetDpi()
	{
		return m_dDpi;
	}

	// dots per inch
	void SetDpi(double value)
	{
		m_dDpi = value;
	}

	// dots per inch
	__declspec(property(get = GetDpi, put = SetDpi))
		double Dpi;

	double GetXInches()
	{
		return m_fXInches;
	}

	void SetXInches(double value)
	{
		m_fXInches = value;
	}

	__declspec(property(get = GetXInches, put = SetXInches))
		double XInches;

	double GetYInches()
	{
		return m_fYInches;
	}

	void SetYInches(double value)
	{
		m_fYInches = value;
	}

	__declspec(property(get = GetYInches, put = SetYInches))
		double YInches;

	// length of list marker in inches
	double GetMarkerLength()
	{
		m_dMarkerLength = 0.5;
		return m_dMarkerLength;
	}

	// length of list marker in inches
	__declspec(property(get = GetMarkerLength))
		double MarkerLength;

	// indentation of wrapped list lines
	double GetListIndent()
	{
		m_dListIndent = 0.55;
		return m_dListIndent;
	}

	// indentation of wrapped list lines
	__declspec(property(get = GetListIndent))
		double ListIndent;

	Gdiplus::RectF GetMarginInches()
	{
		return m_rcMarginInches;
	}

	void SetMarginInches(const Gdiplus::RectF& value)
	{
		m_rcMarginInches = value;
	}

	__declspec(property(get = GetMarginInches, put = SetMarginInches))
		Gdiplus::RectF MarginInches;

	// phrases are expanded into words
	bool GetExpanded()
	{
		return m_bExpanded;
	}

	// phrases are expanded into words
	void SetExpanded(bool value)
	{
		m_bExpanded = value;
	}

	// phrases are expanded into words
	__declspec(property(get = GetExpanded, put = SetExpanded))
		bool Expanded;

	// right justify text?
	bool GetJustify()
	{
		return m_bJustify;
	}

	// right justify text?
	void SetJustify(bool value)
	{
		m_bJustify = value;
	}

	// right justify text?
	__declspec(property(get = GetJustify, put = SetJustify))
		bool Justify;

// protected methods
protected:
	void ExpandTokens();

	void MeasureString
	(
		const CString& csText,
		shared_ptr<Gdiplus::Font> pFont,
		double& outWidthInches,
		double& outHeightInches
	);
	
	// get the length of text in inches
	double TextLength
	(
		const CString& csText,
		shared_ptr<Gdiplus::Font> pFont
	)
	{
		double value = 0;
		double dHeight = 0;
		MeasureString(csText, pFont, value, dHeight);
		return value;
	}

	void DrawRightAlignedMarker
	(
		const CString& csMarker,
		shared_ptr<Gdiplus::Font> pFont,
		double fMarkerColumnWidthInches,
		double fColumnLeftInches,
		double fYInches
	);

	void DrawInlineCodeBackground
	(
		const CString& text,
		shared_ptr<Gdiplus::Font> pFont,
		Color colorFG,
		Color colorBG,
		double dX,
		double dY
	);

	int ToPixelsX(double inches)
	{
		return (int)(inches * Dpi);
	}

	int ToPixelsY(double inches)
	{
		return (int)(inches * Dpi);
	}

	Gdiplus::RectF ToPixelRect
	(
		const Gdiplus::RectF& rcInches
	);

	void DrawBlockQuoteBar
	(
		shared_ptr<Gdiplus::Font> pFont,
		double fIndentInches, double dX, double dY
	);

	double ComputeLineHeightInches
	(
		
		shared_ptr<Gdiplus::Font> pFont
	);

	CSmartArray<CParagraphToken> GetLine
	(
		double dStart,
		double dEnd,
		double& dSpace
	);

// public methods
public:
	void DrawImage
	(
		Graphics* pGraphics, const CString& csPath
	);

	bool DrawLine
	(
		Graphics* pGraphics, double dStart, double dEnd, double dY
	);

	// append a token to the paragraph
	void Append(shared_ptr<CParagraphToken> pToken)
	{
		m_arrTokens.append(pToken);
	}

	// clear the paragraph tokens
	void Clear()
	{
		m_arrTokens.clear();
	}

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CParagraph()
	{
		Dpi = 1000;

		XInches = 0.0;
		YInches = 0.0;

		m_pGraphics = nullptr;
		Expanded = false;
		Justify = true;
	}
	~CParagraph()
	{

	}
}; // class CParagraph

/////////////////////////////////////////////////////////////////////////////
