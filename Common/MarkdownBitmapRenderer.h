/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "MarkdownRenderer.h"
#include "ImagePlus.h"
#include "SmartArray.h"
#include "Paragraph.h"
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

	// True while inside the header row
	bool m_bInTableHeader;

	// True while inside a normal row
	bool m_bInTableRow;

	// Accumulates cells for the current row
	std::vector<CString> m_vecCurrentRow;

	// Accumulates all rows (header + body) for column measurement
	std::vector<std::vector<CString>> m_vecAllRows;

	// Computed column widths (in inches)
	std::vector<double> m_vecColumnWidthsInches;

	// pixel X positions for column boundaries
	std::vector<int> m_vecColumnX;

	// Number of columns in the current table
	int m_nTableColumnCount;

	// current row index of the table
	int m_nCurrentRowIndex = 0;

	// top of table in inches
	double m_dTableTopYInches;

	// Spacing above and below the table (GitHub-style)
	double m_fTableSpacingBeforeInches;
	double m_fTableSpacingAfterInches;

	// Padding inside each cell (GitHub-style)
	double m_fTableCellPadLeftInches;
	double m_fTableCellPadRightInches;
	double m_fTableCellPadTopInches;
	double m_fTableCellPadBottomInches;

	//
	// TABLE ALIGNMENT
	//

	// 0 = left, 1 = center, 2 = right
	std::vector<int> m_vecColumnAlignment;

	// 0 = top, 1 = middle, 2 = bottom
	std::vector<int> m_vecColumnVAlign;

	// per-column background colors (ARGB)
	std::vector<Gdiplus::Color> m_vecColumnBackColor;

	// per-cell background colors: [row][column]
	std::map<std::pair<int, int>, Gdiplus::Color> m_mapCellBackColor;

	// per-cell text colors: [row][column]
	std::map<std::pair<int, int>, Gdiplus::Color> m_mapCellTextColor;

	// per-cell font styles: [row][column]
	std::map<std::pair<int, int>, int> m_mapCellFontStyle;

	// per-cell padding overrides: [row][column]
	std::map<std::pair<int, int>, CellPadding> m_mapCellPadding;

	std::map<std::pair<int, int>, CellBorder> m_mapCellBorder;

	std::map<std::pair<int, int>, CellCornerRadius> m_mapCellCornerRadius;

	std::map<std::pair<int, int>, CellShadow> m_mapCellShadow;

	std::map<std::pair<int, int>, CellGradient> m_mapCellGradient;

	std::map<std::pair<int, int>, CellImageBackground> m_mapCellImageBackground;

	std::map<std::pair<int, int>, CellOverlay> m_mapCellOverlay;

	std::map<std::pair<int, int>, CellBlend> m_mapCellBlend;


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


	// InTableHeader
	bool GetInTableHeader() const
	{
		return m_bInTableHeader;
	}

	void SetInTableHeader(bool value)
	{
		m_bInTableHeader = value;
	}

	__declspec(property(get = GetInTableHeader, put = SetInTableHeader))
		bool InTableHeader;


	// InTableRow
	bool GetInTableRow() const
	{
		return m_bInTableRow;
	}

	void SetInTableRow(bool value)
	{
		m_bInTableRow = value;
	}

	__declspec(property(get = GetInTableRow, put = SetInTableRow))
		bool InTableRow;


	// TableColumnCount
	int GetTableColumnCount() const
	{
		return m_nTableColumnCount;
	}

	void SetTableColumnCount(int value)
	{
		m_nTableColumnCount = value;
	}

	__declspec(property(get = GetTableColumnCount, put = SetTableColumnCount))
		int TableColumnCount;

	// CurrentRowIndex
	int GetCurrentRowIndex() const
	{
		return m_nCurrentRowIndex;
	}

	void SetCurrentRowIndex(int value)
	{
		m_nCurrentRowIndex = value;
	}

	__declspec(property(get = GetCurrentRowIndex, put = SetCurrentRowIndex))
		int CurrentRowIndex;


	// TableSpacingBeforeInches
	double GetTableSpacingBeforeInches() const
	{
		return m_fTableSpacingBeforeInches;
	}

	void SetTableSpacingBeforeInches(double value)
	{
		m_fTableSpacingBeforeInches = value;
	}

	__declspec(property(get = GetTableSpacingBeforeInches, put = SetTableSpacingBeforeInches))
		double TableSpacingBeforeInches;


	// TableSpacingAfterInches
	double GetTableSpacingAfterInches() const
	{
		return m_fTableSpacingAfterInches;
	}

	void SetTableSpacingAfterInches(double value)
	{
		m_fTableSpacingAfterInches = value;
	}

	__declspec(property(get = GetTableSpacingAfterInches, put = SetTableSpacingAfterInches))
		double TableSpacingAfterInches;


	// TableCellPadLeftInches
	double GetTableCellPadLeftInches() const
	{
		return m_fTableCellPadLeftInches;
	}

	void SetTableCellPadLeftInches(double value)
	{
		m_fTableCellPadLeftInches = value;
	}

	__declspec(property(get = GetTableCellPadLeftInches, put = SetTableCellPadLeftInches))
		double TableCellPadLeftInches;


	// TableCellPadRightInches
	double GetTableCellPadRightInches() const
	{
		return m_fTableCellPadRightInches;
	}

	void SetTableCellPadRightInches(double value)
	{
		m_fTableCellPadRightInches = value;
	}

	__declspec(property(get = GetTableCellPadRightInches, put = SetTableCellPadRightInches))
		double TableCellPadRightInches;


	// TableCellPadTopInches
	double GetTableCellPadTopInches() const
	{
		return m_fTableCellPadTopInches;
	}

	void SetTableCellPadTopInches(double value)
	{
		m_fTableCellPadTopInches = value;
	}

	__declspec(property(get = GetTableCellPadTopInches, put = SetTableCellPadTopInches))
		double TableCellPadTopInches;


	// TableCellPadBottomInches
	double GetTableCellPadBottomInches() const
	{
		return m_fTableCellPadBottomInches;
	}

	void SetTableCellPadBottomInches(double value)
	{
		m_fTableCellPadBottomInches = value;
	}

	__declspec(property(get = GetTableCellPadBottomInches, put = SetTableCellPadBottomInches))
		double TableCellPadBottomInches;

	// top of table in inches
	double GetTableTopYInches()
	{
		return m_dTableTopYInches;
	}

	// top of table in inches
	void SetTableTopYInches(double value)
	{
		m_dTableTopYInches = value;
	}

	// top of table in inches
	__declspec(property(get = GetTableTopYInches, put = SetTableTopYInches))
		double TableTopYInches;


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

	//
	// TABLE RENDERING (Step 64)
	//

	// Called when a table begins
	void OnTableStart();

	// Called when a table ends
	void OnTableEnd();

	// Called when a header row begins
	void OnTableHeaderStart();

	// Called when a header row ends
	void OnTableHeaderEnd();

	// Called when a normal row begins
	void OnTableRowStart();

	// Called when a normal row ends
	void OnTableRowEnd();

	// Called once per cell (header or body)
	void OnTableCell(const CString& csText, int nColumnIndex);

	// Draws a fully measured table row
	void DrawTableRow
	(
		const std::vector<CString>& vecCells,
		const std::vector<double>& vecColumnWidthsInches,
		bool bHeader,
		int nRowIndex
	);

	// Measures column widths for the entire table
	bool MeasureTableColumns
	(
		const std::vector<std::vector<CString>>& vecRows,
		std::vector<double>& vecColumnWidthsInches
	);

	// Draws a single table cell
	void DrawTableCell
	(
		const CString& csText,
		double fLeftInches,
		double fWidthInches,
		bool bHeader,
		int nAlignment,
		int nVAlign,
		int nColumnIndex,
		double& fCellHeightOut
	);

	void OnTableColumnAlignment
	(
		int nColumnIndex,
		int nAlignment
	);

	void OnTableColumnVerticalAlignment(int nColumnIndex, int nVAlign);

	void OnTableColumnBackColor(int nColumnIndex, Gdiplus::Color color);

	void OnTableCellBackColor
	(
		int nRowIndex, int nColumnIndex, Gdiplus::Color color
	);

	void OnTableCellTextColor
	(
		int nRowIndex, int nColumnIndex, Gdiplus::Color color
	);

	void OnTableCellFontStyle(int nRowIndex, int nColumnIndex, int nStyle);

	void OnTableCellPadding
	(
		int nRowIndex,
		int nColumnIndex,
		double left,
		double right,
		double top,
		double bottom
	);

	void OnTableCellBorder
	(
		int nRowIndex,
		int nColumnIndex,
		bool drawTop,
		bool drawBottom,
		bool drawLeft,
		bool drawRight,
		Gdiplus::Color color,
		float thickness
	);

	void OnTableCellCornerRadius
	(
		int nRowIndex,
		int nColumnIndex,
		float tl,
		float tr,
		float br,
		float bl
	);

	void OnTableCellShadow
	(
		int nRowIndex,
		int nColumnIndex,
		bool enabled,
		float offsetX,
		float offsetY,
		float blur,
		Gdiplus::Color color
	);

	void OnTableCellGradient
	(
		int nRowIndex,
		int nColumnIndex,
		CellGradientType type,
		Gdiplus::Color startColor,
		Gdiplus::Color endColor,
		float angle,
		float radius
	);

	void OnTableCellImageBackground
	(
		int nRowIndex,
		int nColumnIndex,
		CellImageMode mode,
		Gdiplus::Image* pImage,
		float opacity
	);

	void OnTableCellOverlay
	(
		int nRowIndex,
		int nColumnIndex,
		CellOverlayMode mode,
		Gdiplus::Color color,
		float opacity,
		Gdiplus::Image* pImage,
		float imageOpacity,
		float glassOpacity
	);

	void OnTableCellBlend
	(
		int nRowIndex,
		int nColumnIndex,
		CellBlendMode mode,
		Gdiplus::Color color,
		float opacity
	);

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
		InTableHeader = false;
		InTableRow = false;

		m_vecCurrentRow.clear();
		m_vecAllRows.clear();
		m_vecColumnWidthsInches.clear();

		TableColumnCount = 0;

		// GitHub-style table spacing
		TableSpacingBeforeInches = 0.10;   // spacing before table
		TableSpacingAfterInches = 0.10;   // spacing after table

		// GitHub-style cell padding
		TableCellPadLeftInches = 0.06;
		TableCellPadRightInches = 0.06;
		TableCellPadTopInches = 0.04;
		TableCellPadBottomInches = 0.04;

		// top of table in inches
		TableTopYInches = 0;

		//
		// TABLE ALIGNMENT (Step 65)
		//
		m_vecColumnAlignment.clear();

		m_vecColumnVAlign.clear();

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

