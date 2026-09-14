/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MarkdownRenderer.h"
#include "ImagePlus.h"
#include "SmartArray.h"
#include "Paragraph.h"
#include "TableMD.h"
#include <map>

/////////////////////////////////////////////////////////////////////////////
// CMarkdownBitmapRenderer
/////////////////////////////////////////////////////////////////////////////
class CMarkdownBitmapRenderer : public CMarkdownRenderer
{
// public data
public:

// protected data
protected:
	shared_ptr<Bitmap> m_pBitmap;

	CParagraph m_paragraph;
	CTableMD m_table;

	bool m_bDrawLine;

	// dots per inch
	double m_dDpi;

	double m_fXInches;
	double m_fYInches;

	int  m_nX;
	int  m_nY;

	bool m_bEmphasis;
	bool m_bStrong;
	bool m_bInlineCode;

	int  m_nHeadingLevel;

	bool m_bInUnorderedList;
	bool m_bInOrderedList;
	bool m_bInListItem;
	CString m_csListMarker;

	bool m_bInBlockQuote;
	bool m_bInCodeBlock;

	Gdiplus::RectF m_rcMarginInches;
	std::shared_ptr<Gdiplus::Bitmap> m_pWhitePixel;

	int m_nOrderedListCounter;

	int m_nListDepth;
	int m_nBlockQuoteDepth;

	bool m_bLooseList;

	double m_fCodeBlockTopInches;

	// multiline text that requires wrapping
	bool m_bInParagraph;
	int m_nParagraphLine;

	bool m_bInLink;

	// Heading fonts (smart pointers)
	std::shared_ptr<Gdiplus::Font> Heading1Font;
	std::shared_ptr<Gdiplus::Font> Heading2Font;
	std::shared_ptr<Gdiplus::Font> Heading3Font;
	std::shared_ptr<Gdiplus::Font> Heading4Font;
	std::shared_ptr<Gdiplus::Font> Heading5Font;
	std::shared_ptr<Gdiplus::Font> Heading6Font;

	// Paragraph font
	std::shared_ptr<Gdiplus::Font> FontParagraph;

	// The font currently in use for drawing
	std::shared_ptr<Gdiplus::Font> m_pCurrentFont;

	//
	// TABLE RENDERING
	//

	// True while inside a table
	bool m_bInTable;

	// True until the heading divider is encountered
	bool m_bInTableHeading;


// public properties
public:
	shared_ptr<Bitmap> GetRenderedImage()
	{
		return m_pBitmap;
	}

	void SetRenderedImage(shared_ptr<Bitmap> value)
	{
		m_pBitmap = value;
	}

	__declspec(property(get = GetRenderedImage, put = SetRenderedImage))
		shared_ptr<Bitmap> RenderedImage;

	shared_ptr<Gdiplus::Font> GetCurrentFont()
	{
		if (m_pCurrentFont == nullptr)
		{
			m_pCurrentFont = FontParagraph;
		}
		return m_pCurrentFont;
	}

	void SetCurrentFont(shared_ptr<Gdiplus::Font> value)
	{
		m_pCurrentFont = value;
	}

	__declspec(property(get = GetCurrentFont, put = SetCurrentFont))
		shared_ptr<Gdiplus::Font> CurrentFont;

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

	int GetX()
	{
		return m_nX;
	}

	void SetX(int value)
	{
		m_nX = value;
	}

	__declspec(property(get = GetX, put = SetX))
		int X;

	int GetY()
	{
		return m_nY;
	}

	void SetY(int value)
	{
		m_nY = value;
	}

	__declspec(property(get = GetY, put = SetY))
		int Y;

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

	bool GetDrawLine()
	{
		return m_bDrawLine;
	}

	void SetDrawLine(bool value)
	{
		m_bDrawLine = value;
	}

	__declspec(property(get = GetDrawLine, put = SetDrawLine))
		bool DrawLine;

	bool GetEmphasis()
	{
		return m_bEmphasis;
	}

	void SetEmphasis(bool value)
	{
		m_bEmphasis = value;
	}

	__declspec(property(get = GetEmphasis, put = SetEmphasis))
		bool Emphasis;

	bool GetStrong()
	{
		return m_bStrong;
	}

	void SetStrong(bool value)
	{
		m_bStrong = value;
	}

	__declspec(property(get = GetStrong, put = SetStrong))
		bool Strong;

	bool GetInlineCode()
	{
		return m_bInlineCode;
	}

	void SetInlineCode(bool value)
	{
		m_bInlineCode = value;
	}

	__declspec(property(get = GetInlineCode, put = SetInlineCode))
		bool InlineCode;

	int GetHeadingLevel()
	{
		return m_nHeadingLevel;
	}

	void SetHeadingLevel(int value)
	{
		m_nHeadingLevel = value;
	}

	__declspec(property(get = GetHeadingLevel, put = SetHeadingLevel))
		int HeadingLevel;

	bool GetInUnorderedList()
	{
		return m_bInUnorderedList;
	}

	void SetInUnorderedList(bool value)
	{
		m_bInUnorderedList = value;
	}

	__declspec(property(get = GetInUnorderedList, put = SetInUnorderedList))
		bool InUnorderedList;

	bool GetInOrderedList()
	{
		return m_bInOrderedList;
	}

	void SetInOrderedList(bool value)
	{
		m_bInOrderedList = value;
	}

	__declspec(property(get = GetInOrderedList, put = SetInOrderedList))
		bool InOrderedList;

	bool GetInListItem()
	{
		return m_bInListItem;
	}

	void SetInListItem(bool value)
	{
		m_bInListItem = value;
	}

	__declspec(property(get = GetInListItem, put = SetInListItem))
		bool InListItem;

	CString GetListMarker()
	{
		return m_csListMarker;
	}

	void SetListMarker(CString value)
	{
		m_csListMarker = value;
	}

	__declspec(property(get = GetListMarker, put = SetListMarker))
		CString ListMarker;

	bool GetInBlockQuote()
	{
		return m_bInBlockQuote;
	}

	void SetInBlockQuote(bool value)
	{
		m_bInBlockQuote = value;
	}

	__declspec(property(get = GetInBlockQuote, put = SetInBlockQuote))
		bool InBlockQuote;

	bool GetInCodeBlock()
	{
		return m_bInCodeBlock;
	}

	void SetInCodeBlock(bool value)
	{
		m_bInCodeBlock = value;
	}

	__declspec(property(get = GetInCodeBlock, put = SetInCodeBlock))
		bool InCodeBlock;

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

	std::shared_ptr<Gdiplus::Bitmap> GetWhitePixel()
	{
		return m_pWhitePixel;
	}

	void SetWhitePixel(const std::shared_ptr<Gdiplus::Bitmap> value)
	{
		m_pWhitePixel = value;
	}

	__declspec(property(get = GetWhitePixel, put = SetWhitePixel))
		std::shared_ptr<Gdiplus::Bitmap> WhitePixel;

	int GetOrderedListCounter()
	{
		return m_nOrderedListCounter;
	}

	void SetOrderedListCounter(int value)
	{
		m_nOrderedListCounter = value;
	}

	__declspec(property(get = GetOrderedListCounter, put = SetOrderedListCounter))
		int OrderedListCounter;

	int GetListDepth()
	{
		return m_nListDepth;
	}

	void SetListDepth(int value)
	{
		m_nListDepth = value;
	}

	__declspec(property(get = GetListDepth, put = SetListDepth))
		int ListDepth;

	int GetBlockQuoteDepth()
	{
		return m_nBlockQuoteDepth;
	}

	void SetBlockQuoteDepth(int value)
	{
		m_nBlockQuoteDepth = value;
	}

	__declspec(property(get = GetBlockQuoteDepth, put = SetBlockQuoteDepth))
		int BlockQuoteDepth;

	bool GetLooseList() const
	{
		return m_bLooseList;
	}

	void SetLooseList(bool value)
	{
		// breakpoint-friendly: you can stop here whenever LooseList changes
		m_bLooseList = value;
	}

	__declspec(property(get = GetLooseList, put = SetLooseList))
		bool LooseList;

	double GetCodeBlockTopInches()
	{
		return m_fCodeBlockTopInches;
	}

	void SetCodeBlockTopInches(double value)
	{
		m_fCodeBlockTopInches = value;
	}

	__declspec(property(get = GetCodeBlockTopInches, put = SetCodeBlockTopInches))
		double CodeBlockTopInches;

	bool GetInParagraph() const
	{
		return m_bInParagraph;
	}

	void SetInParagraph(bool value)
	{
		m_bInParagraph = value;
	}

	__declspec(property(get = GetInParagraph, put = SetInParagraph))
		bool InParagraph;

	int GetParagraphLine() const
	{
		return m_nParagraphLine;
	}

	void SetParagraphLine(int value)
	{
		m_nParagraphLine = value;
	}

	__declspec(property(get = GetParagraphLine, put = SetParagraphLine))
		int ParagraphLine;

	bool GetInLink() const
	{
		return m_bInLink;
	}

	void SetInLink(bool value)
	{
		m_bInLink = value;
	}

	__declspec(property(get = GetInLink, put = SetInLink))
		bool InLink;

	//
	// TABLE RENDERING (Step 64)
	//

	// InTable
	bool GetInTable() const
	{
		return m_bInTable;
	}

	void SetInTable(bool value)
	{
		m_bInTable = value;
	}

	__declspec(property(get = GetInTable, put = SetInTable))
		bool InTable;

	// InTableHeading
	bool GetInTableHeading() const
	{
		return m_bInTableHeading;
	}

	void SetInTableHeading(bool value)
	{
		m_bInTableHeading = value;
	}

	__declspec(property(get = GetInTableHeading, put = SetInTableHeading))
		bool InTableHeading;



// protected methods
protected:
	void InitHeadingFonts();
	double DrawParagraph();

	void DrawInlineCodeBackground
	(
		const CString& text,
		const Gdiplus::Font& font,
		Color colorFG,
		Color colorBG
	);


// public methods
public:
	int ToPixelsX(double inches)
	{
		return (int)(inches * Dpi);
	}

	int ToPixelsY(double inches)
	{
		return (int)(inches * Dpi);
	}

	void DrawText(const CString& text);
	void NewLine();

	void OnImage
	(
		const CString& csPath,
		const CString& csAlt
	);
	void OnParagraphStart();
	void OnParagraphEnd();

	void OnHeadingStart(int level);
	void OnHeadingEnd();

	void OnUnorderedListStart();
	void OnUnorderedListEnd();

	void OnOrderedListStart();
	void OnOrderedListEnd();

	void OnListItemStart();
	void OnListItemEnd();

	void OnBlockQuoteStart();
	void OnBlockQuoteEnd();

	void OnCodeBlockStart();
	void OnCodeBlockEnd();

	void OnEmphasisStart();
	void OnEmphasisEnd();

	void OnStrongStart();
	void OnStrongEnd();

	void OnInlineCodeStart();
	void OnInlineCodeEnd();

	void OnLinkStart(const CString& href);
	void OnLinkEnd();

	void OnText(const CString& text);
	void OnInlineCodeText(const CString& text);
	void OnHtmlText(const CString& text);
	void OnEntityText(const CString& text);

	/////////////////////////////////////////////////////////////////////////////
	// CreateBitmap
	/////////////////////////////////////////////////////////////////////////////
	bool CreateBitmap(int nWidth, int nHeight);

	/////////////////////////////////////////////////////////////////////////////
	// GetGraphics
	/////////////////////////////////////////////////////////////////////////////
	Graphics* GetGraphics();

	double ComputeLineHeightInches
	(
		const Gdiplus::Font& font
	);
	
	void RenderTable();

	void DrawCodeBlockBackground
	(
		double fTopInches,
		double fBottomInches
	);

	void DrawBlockQuoteBar(const Gdiplus::Font& font, double fInches = 0);

	double ComputeIndentInches();

	void DrawListMarker
	(
		const CString& csMarker
	);

	bool SplitTextToFit
	(
		const CString& csText,
		const Gdiplus::Font& font,
		double                fRemainingWidthInches,
		CString& csLine,
		CString& csRemainder
	);

	double RemainingLineWidthInches();

	void HeadingBreak
	(
		int nLevel
	);

	void ParagraphBreak();


	Gdiplus::RectF ToPixelRect
	(
		const Gdiplus::RectF& rcInches
	);

	bool MeasureString
	(
		const CString& csText,
		const Gdiplus::Font& font,
		double& outWidthInches,
		double& outHeightInches
	);

	bool WrapTextToLines
	(
		const CString& csText,
		const Gdiplus::Font& font,
		double fMaxWidthInches,
		std::vector<CString>& vecLines
	);

	/////////////////////////////////////////////////////////////////////////////
	// Finalize
	/////////////////////////////////////////////////////////////////////////////
	shared_ptr<CImagePlus> Finalize();

	/////////////////////////////////////////////////////////////////////////////
	// ToImagePlus
	/////////////////////////////////////////////////////////////////////////////
	shared_ptr<CImagePlus> ToImagePlus();

// public constructor/destructor
public:
	CMarkdownBitmapRenderer()
	{
		RenderedImage = NULL;
		DrawLine = false;

		Dpi = 1000.0;

		X = 0;
		Y = 0;

		XInches = 0.0;
		YInches = 0.0;

		Emphasis = false;
		Strong = false;
		InlineCode = false;

		HeadingLevel = 0;

		InUnorderedList = false;
		InOrderedList = false;
		InListItem = false;
		ListMarker = L"";

		InBlockQuote = false;
		InCodeBlock = false;

		OrderedListCounter = 0;
		ListDepth = 0;
		BlockQuoteDepth = 0;
		LooseList = false;

		InParagraph = false;
		ParagraphLine = 0;

		InLink = false;

		InitHeadingFonts();

		//
		// TABLE RENDERING (Step 64)
		//
		InTable = false;
		InTableHeading = false;

		// create 1x1 white pixel
		WhitePixel = 
			std::make_shared<Gdiplus::Bitmap>(1, 1, PixelFormat32bppARGB);

		Gdiplus::Color white(255, 255, 255, 255);
		WhitePixel->SetPixel(0, 0, white);

		FontParagraph = std::make_shared<Gdiplus::Font>(L"Segoe UI", 12.0f);

		CurrentFont = FontParagraph;
	}

	virtual ~CMarkdownBitmapRenderer()
	{
	}
}; // class CMarkdownBitmapRenderer

/////////////////////////////////////////////////////////////////////////////

