/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MarkdownBitmapRenderer.h"
#include "CHelper.h"
#include "ColorPlus.h"
#include <numeric>

/////////////////////////////////////////////////////////////////////////////
// Initialize heading fonts (smart pointers)
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::InitHeadingFonts()
{
	FontFamily ff(L"Segoe UI");

	Heading1Font = 
		std::make_shared<Font>(&ff, 32.0f, FontStyleBold, UnitPoint);
	Heading2Font = 
		std::make_shared<Font>(&ff, 26.0f, FontStyleBold, UnitPoint);
	Heading3Font = 
		std::make_shared<Font>(&ff, 22.0f, FontStyleBold, UnitPoint);
	Heading4Font = 
		std::make_shared<Font>(&ff, 18.0f, FontStyleBold, UnitPoint);
	Heading5Font = 
		std::make_shared<Font>(&ff, 16.0f, FontStyleBold, UnitPoint);
	Heading6Font = 
		std::make_shared<Font>(&ff, 14.0f, FontStyleBold, UnitPoint);
} // InitHeadingFonts

/////////////////////////////////////////////////////////////////////////////
double CMarkdownBitmapRenderer::ComputeIndentInches()
{
	double fIndent = 0.0;

	// list indentation (per depth)
	if (ListDepth > 0)
	{
		fIndent += (ListDepth * 0.30);
	}

	// block quote indentation (per depth)
	if (BlockQuoteDepth > 0)
	{
		fIndent += (BlockQuoteDepth * 0.40);
	}

	// list-item indentation (bullet/number area)
	if (InListItem)
	{
		fIndent += 0.25;   // marker width
	}

	return fIndent;
} // ComputeIndentInches

/////////////////////////////////////////////////////////////////////////////
// DrawCodeBlockBackground
//
// Draws a light gray background behind a code block.
// Called once per code block, not per line.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawCodeBlockBackground
(
	double fTopInches,
	double fBottomInches
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return;
	}

	// padding inside the code block
	const double fPadInches = 0.08;

	double fLeftInches = MarginInches.X + fPadInches;
	double fRightInches = MarginInches.X + MarginInches.Width - fPadInches;

	double fWidthInches = fRightInches - fLeftInches;
	double fHeightInches = (fBottomInches - fTopInches);

	Gdiplus::RectF rcInches
	(
		(REAL)fLeftInches,
		(REAL)fTopInches,
		(REAL)fWidthInches,
		(REAL)fHeightInches
	);

	Gdiplus::RectF rcPixels = ToPixelRect(rcInches);

	SolidBrush brush(Color(240, 240, 240));   // light gray
	pGraphics->FillRectangle(&brush, rcPixels);

	delete pGraphics;
} // DrawCodeBlockBackground

/////////////////////////////////////////////////////////////////////////////
// DrawBlockQuoteBar
//
// Draws the vertical bar for blockquotes.
// Normalized GitHub-style alignment.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawBlockQuoteBar
(
	const Gdiplus::Font& font, double fIndentInches
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return;
	}

	//
	// GitHub-style bar:
	//   - left edge = margin + 0.10"
	//   - width     = 0.04"
	//
	const double fBarLeftInches = MarginInches.X + 0.10;
	const double fBarWidthInches = 0.04;

	if (CHelper::NearlyEqual(fIndentInches, 0.0))
	{
		fIndentInches = fBarLeftInches;
	}

	//
	// Height of the bar = height of the current line.
	// We measure a capital "A" using the *actual* font for this line.
	//
	double fDummyWidthInches = 0.0;
	double fLineHeightInches = 0.0;

	MeasureString(L"A", font, fDummyWidthInches, fLineHeightInches);

	//
	// Convert inches to pixel rectangle
	//
	Gdiplus::RectF rcPixels = ToPixelRect
	(
		Gdiplus::RectF
		(
			(REAL)fIndentInches - fBarWidthInches,
			(REAL)YInches,
			(REAL)fBarWidthInches,
			(REAL)fLineHeightInches
		)
	);

	//
	// GitHub-style color
	//
	SolidBrush brush(Color::Silver);

	pGraphics->FillRectangle(&brush, rcPixels);

	delete pGraphics;
} // DrawBlockQuoteBar

/////////////////////////////////////////////////////////////////////////////
// DrawListMarker
//
// Draws the bullet or ordered-list number for a list item.
// Normalized GitHub-style alignment.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawListMarker(const CString& csMarker)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return;
	}

	//
	// GitHub-style marker column:
	//   - left edge = margin + 0.15"
	//   - width     = 0.30"
	//
	const double fColumnLeftInches = MarginInches.X + 0.15;
	const double fColumnWidthInches = 0.30;

	//
	// Measure marker width in inches
	//
	double fMarkerWidthInches = 0.0;
	double fMarkerHeightInches = 0.0;

	if (!MeasureString(csMarker, *CurrentFont.get(), fMarkerWidthInches, fMarkerHeightInches))
	{
		delete pGraphics;
		return;
	}

	//
	// Right-align marker inside the column
	//
	double fMarkerXInches =
		fColumnLeftInches + (fColumnWidthInches - fMarkerWidthInches);

	//
	// Baseline alignment (slight downward shift)
	//
	double fMarkerYInches = YInches + 0.02;

	//
	// Convert to pixels
	//
	int nX = ToPixelsX(fMarkerXInches);
	int nY = ToPixelsY(fMarkerYInches);

	Gdiplus::PointF ptOrigin((REAL)nX, (REAL)nY);

	SolidBrush brush(Color(0, 0, 0));

	//
	// Draw the marker
	//
	pGraphics->DrawString
	(
		csMarker,
		csMarker.GetLength(),
		CurrentFont.get(),
		ptOrigin,
		&brush
	);

	delete pGraphics;

	//
	// IMPORTANT:
	// Do NOT advance XInches here.
	// Wrapped text begins at ComputeIndentInches().
	//
} // DrawListMarker

/////////////////////////////////////////////////////////////////////////////
// SplitTextToFit
//
// Splits text into a line that fits the remaining width and a remainder.
// Returns true if any text fits on the current line.
//
/////////////////////////////////////////////////////////////////////////////
bool CMarkdownBitmapRenderer::SplitTextToFit
(
	const CString& csText,
	const Gdiplus::Font& font,
	double fRemainingWidthInches,
	CString& csLine, // the portion of the text that fits
	CString& csRemainder // the portion of the text that did not fit
)
{
	csLine.Empty();
	csRemainder.Empty();

	// nothing fits if remaining width is zero
	if (fRemainingWidthInches <= 0.0)
	{
		csRemainder = csText;
		return false;
	}

	int nLenText = csText.GetLength();
	int nStart = 0;
	CString csToken = csText.Tokenize(L" ", nStart);
	while (!csToken.IsEmpty())
	{
		// test against a temporary string in case it gets to big
		CString csTemp = csLine.IsEmpty() ? 
			csToken : csLine + L" " + csToken;
		double fWidthInches = 0.0;
		double fHeightInches = 0.0;

		if (!MeasureString(csTemp, font, fWidthInches, fHeightInches))
		{
			break;
		}

		// if this substring exceeds the remaining width, stop
		if (fWidthInches > fRemainingWidthInches)
		{
			int nLenLine = csLine.GetLength();
			int nDiff = nLenText - nLenLine;
			if (nDiff > 0)
			{
				csRemainder = csText.Right(nDiff);
				csRemainder.TrimLeft(L" ");
			}
			return true;
		}

		// assign the output line the temporary value
		csLine = csTemp;

		// next word in the text
		csToken = csText.Tokenize(L" ", nStart);
	}

	// entire text fits
	csLine = csText;
	csRemainder.Empty();
	return true;
} // SplitTextToFit

/////////////////////////////////////////////////////////////////////////////
// RemainingLineWidthInches
//
// Returns the remaining horizontal space (in inches) on the current line.
//
/////////////////////////////////////////////////////////////////////////////
double CMarkdownBitmapRenderer::RemainingLineWidthInches()
{
	const double fLeftInches = MarginInches.X;
	const double fRightInches = MarginInches.X + MarginInches.Width;

	// remaining width = right boundary - current X
	double fRemaining = fRightInches - XInches;

	return (fRemaining > 0.0 ? fRemaining : 0.0);
} // RemainingLineWidthInches

/////////////////////////////////////////////////////////////////////////////
// HeadingBreak
//
// Adds spacing before and after a heading.
// Resets XInches to the left margin.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::HeadingBreak
(
	int nLevel
)
{
	// spacing amounts in inches
	double fBeforeInches = 0.10;   // default
	double fAfterInches = 0.08;   // default

	// larger spacing for top‑level headings
	if (nLevel == 1)
	{
		fBeforeInches = 0.20;
		fAfterInches = 0.15;
	}
	else if (nLevel == 2)
	{
		fBeforeInches = 0.15;
		fAfterInches = 0.12;
	}

	// apply spacing BEFORE heading
	YInches += fBeforeInches;

	// reset X to left margin
	XInches = MarginInches.X;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);

	// (the heading text will be drawn here)

	// apply spacing AFTER heading
	YInches += fAfterInches;

	// reset X again
	XInches = MarginInches.X;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);
} // HeadingBreak

/////////////////////////////////////////////////////////////////////////////
// ParagraphBreak
//
// Advances YInches by a paragraph spacing amount.
// Resets XInches to the left margin.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::ParagraphBreak
(
)
{
	const double fParagraphSpacingInches = 0.10;   // 0.10 inches

	// advance Y by paragraph spacing
	YInches += fParagraphSpacingInches;

	// reset X to left margin
	XInches = MarginInches.X;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);
} // ParagraphBreak

/////////////////////////////////////////////////////////////////////////////
// ToPixelRect
//
// Converts an inch‑based rectangle into a pixel‑based rectangle.
//
/////////////////////////////////////////////////////////////////////////////
Gdiplus::RectF CMarkdownBitmapRenderer::ToPixelRect
(
	const Gdiplus::RectF& rcInches
)
{
	return Gdiplus::RectF
	(
		(REAL)ToPixelsX(rcInches.X),
		(REAL)ToPixelsY(rcInches.Y),
		(REAL)ToPixelsX(rcInches.Width),
		(REAL)ToPixelsY(rcInches.Height)
	);
} // ToPixelRect

/////////////////////////////////////////////////////////////////////////////
// MeasureString
//
// Measures the width and height of the given text using inch‑based layout.
// Returns true if measurement succeeded.
//
/////////////////////////////////////////////////////////////////////////////
bool CMarkdownBitmapRenderer::MeasureString
(
	const CString& csText,
	const Gdiplus::Font& font,
	double& outWidthInches,
	double& outHeightInches
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return false;
	}

	// get DPI for conversion
	const REAL dpiX = pGraphics->GetDpiX();
	const REAL dpiY = pGraphics->GetDpiY();

	// *** IMPORTANT ***
	// Use a huge layout rectangle so MeasureString NEVER clips the text.
	// This forces GDI+ to return the TRUE width of the string.
	Gdiplus::RectF rcLayout
	(
		0.0f,
		0.0f,
		1000000.0f,   // effectively infinite width
		1000000.0f    // effectively infinite height
	);

	Gdiplus::RectF rcBounds;

	pGraphics->MeasureString
	(
		csText,
		csText.GetLength(),
		&font,
		rcLayout,
		&rcBounds
	);

	// convert measured bounds from pixels back to inches
	outWidthInches = rcBounds.Width / dpiX;
	outHeightInches = rcBounds.Height / dpiY;

	delete pGraphics;
	return true;
} // MeasureString

/////////////////////////////////////////////////////////////////////////////
// Finalize
//
// Converts the rendered GDI+ Bitmap into a CImagePlus wrapper.
// This allows Markdown pages to be drawn using StretchDIBits,
// honoring mapping mode and DPI exactly like image pages.
/////////////////////////////////////////////////////////////////////////////
shared_ptr<CImagePlus> CMarkdownBitmapRenderer::Finalize()
{
	if (!m_pBitmap)
	{
		return nullptr;
	}

	shared_ptr<CImagePlus> pImage =
		make_shared<CImagePlus>(m_pBitmap);

	return pImage;
} // Finalize

/////////////////////////////////////////////////////////////////////////////
// CreateBitmap
//
// Allocates a new GDI+ Bitmap for Markdown rendering.
// The caller specifies pixel dimensions.
/////////////////////////////////////////////////////////////////////////////
bool CMarkdownBitmapRenderer::CreateBitmap(int nWidth, int nHeight)
{
	RenderedImage = make_shared<Bitmap>
	(
		nWidth,
		nHeight,
		PixelFormat32bppARGB
	);

	if (!RenderedImage)
	{
		return false;
	}

	// apply logical DPI to the bitmap
	RenderedImage->SetResolution
	(
		(REAL)Dpi,
		(REAL)Dpi
	);

	return true;
} // CreateBitmap

/////////////////////////////////////////////////////////////////////////////
// GetGraphics
//
// Returns a Graphics object for drawing into the bitmap.
// Caller must delete the Graphics object.
/////////////////////////////////////////////////////////////////////////////
Graphics* CMarkdownBitmapRenderer::GetGraphics()
{
	if (!RenderedImage)
	{
		return nullptr;
	}

	Graphics* pGraphics = Graphics::FromImage(RenderedImage.get());
	return pGraphics;
} // GetGraphics

/////////////////////////////////////////////////////////////////////////////
double CMarkdownBitmapRenderer::ComputeLineHeightInches
(
	const Gdiplus::Font& font
)
{
	// Create a Graphics object tied to the bitmap (correct DPI)
	Graphics* g = GetGraphics();
	if (!g)
		return 0.0;

	// Physical pixel height of the font at this DPI
	REAL heightPixels = font.GetHeight(g);

	// Convert pixels → inches using the bitmap's DPI
	REAL dpiY = g->GetDpiY();

	delete g;

	return static_cast<double>(heightPixels) / static_cast<double>(dpiY);
} // ComputeLineHeightInches

/////////////////////////////////////////////////////////////////////////////
shared_ptr<CImagePlus> CMarkdownBitmapRenderer::ToImagePlus()
{
	return Finalize();

} // ToImagePlus

/////////////////////////////////////////////////////////////////////////////
// NewLine
//
// Advances to the next line.
//
// the multiplier determines the amount of the current line height
// to use for offsetting the Y coordinate
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::NewLine( float fMultiplier/* = 1.0f*/)
{
	const double fLineHeightInches = ComputeLineHeightInches(*CurrentFont);

	// advance Y by line height
	double dY = YInches;
	double dOffset = fLineHeightInches * fMultiplier;

	dY += dOffset;
	Y = ToPixelsY(dY);
	YInches = dY;

	// reset X to left margin
	XInches = MarginInches.X;
	X = ToPixelsX(XInches);
} // NewLine

/////////////////////////////////////////////////////////////////////////////
// DrawParagraph
//
// Draws segments of paragraph text.
//
/////////////////////////////////////////////////////////////////////////////
double CMarkdownBitmapRenderer::DrawParagraph()
{
	Gdiplus::Graphics* pGraphics = GetGraphics();

	double dLeft = MarginInches.X;
	double dRight = dLeft + MarginInches.Width;
	double dY = YInches;

	double fLineHeight = 0;
	double fParagraphSpacing = 0;

	if (InListItem || HeadingLevel > 0)
	{
		m_paragraph.Justify = false;
	}
	else
	{
		m_paragraph.Justify = true;
	}

	while (DrawLine)
	{
		DrawLine = m_paragraph.DrawLine(pGraphics, dLeft, dRight, dY);
		fLineHeight = ComputeLineHeightInches(*CurrentFont);
		fParagraphSpacing = fLineHeight * 0.60;

		YInches += fParagraphSpacing;
		dY = YInches;

	}

	delete pGraphics;

	return fParagraphSpacing;

} // DrawParagraph

/////////////////////////////////////////////////////////////////////////////
// DrawText
//
// Draws a single Markdown text run, with style and wrapping.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawText(const CString& text)
{
	if (!RenderedImage)
		return;

	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
		return;

	// populate a table
	if (text.Left(1) == L"|")
	{
		CString csRow = text;

		// if first row, clear the table
		if (!InTable)
		{
			m_table.Clear();
			InTable = true;
			InTableHeading = true;
		}

		if (m_table.HeadingSeparator(csRow))
		{
			InTableHeading = false;
			return;
		}

		long lRow = m_table.Add();
		shared_ptr<CTableRow> pRow = m_table.Row[lRow];
		int nStart = 0;
		csRow.Trim(L"| ");
		CString csCell = csRow.Tokenize(L"|", nStart);
		while (!csCell.IsEmpty())
		{
			csCell.Trim(L" ");
			long lCell = pRow->Add();
			shared_ptr<CTableCell> pCell = pRow->Cell[lCell];
			pCell->Text = csCell;
			pCell->Heading = InTableHeading;
			csCell = csRow.Tokenize(L"|", nStart);
		}
		return;
	}

	// base indentation in inches
	double fIndentInches = ComputeIndentInches();

	CString csText(text);

	CString csListMarker = ListMarker;
	if (InListItem && !csListMarker.IsEmpty())
	{
		csText.Format(L"%s %s", csListMarker, text);
		ListMarker = L"";
	}
	int nParagraphLine = ParagraphLine;

	int nStyle = CurrentFont->GetStyle();
	Color colorFG = Color::Black;
	Color colorBG = Color::White;
	bool bBackground = false;
	Gdiplus::FontFamily ff;
	CurrentFont->GetFamily(&ff);
	REAL fSize = CurrentFont->GetSize();
	shared_ptr<Gdiplus::Font> pFont;

	if (Emphasis)
		nStyle |= FontStyleItalic;

	if (Strong)
		nStyle |= FontStyleBold;

	if (InlineCode)
	{
		bBackground = true;
		colorFG = Color::Silver;
		colorBG = Color::Silver;
		nStyle = FontStyleRegular;
		fSize = 12.0f;
		pFont = make_shared<Gdiplus::Font>
			(L"Consolas", fSize, nStyle, UnitPoint);
	}
	else
	{
		pFont = make_shared<Gdiplus::Font>
			(&ff, fSize, nStyle, UnitPoint);
	}

	int nBlockQuoteDepth = 0;
	bool bBlockQuote = false;
	if (InBlockQuote)
	{
		bBlockQuote = true;
		nBlockQuoteDepth = BlockQuoteDepth;
	}

	double fWidth = 0, fHeight = 0;
	MeasureString(csText, *pFont, fWidth, fHeight);
	shared_ptr<CParagraphToken> pToken =
		make_shared<CParagraphToken>();
	pToken->Text = csText;
	pToken->Font = pFont;
	pToken->Length = fWidth;
	pToken->Background = bBackground;
	pToken->ColorFG = colorFG;
	pToken->ColorBG = colorBG;
	pToken->BlockQuote = bBlockQuote;
	pToken->BlockQuoteDepth = nBlockQuoteDepth;
	pToken->ListDepth = ListDepth;
	pToken->ParagraphLine = nParagraphLine++;
	pToken->ListMarker = !csListMarker.IsEmpty();
	pToken->UnorderedList = InUnorderedList;
	pToken->OrderedListCounter = OrderedListCounter;
	pToken->ListItem = InListItem;
	pToken->InlineCode = InlineCode;
	pToken->Emphasis = Emphasis;
	pToken->Strong = Strong;
	ParagraphLine = nParagraphLine;
	m_paragraph.Append(pToken);
	DrawLine = true;

	delete pGraphics;

} // DrawText

/////////////////////////////////////////////////////////////////////////////
// OnParagraphStart
//
// Begins a paragraph. Sets the paragraph font and establishes baseline.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnParagraphStart()
{
	if (!InListItem)
	{
		// switch to paragraph font
		CurrentFont = FontParagraph;

		// ensure X is at left margin
		XInches = MarginInches.X;
		X = ToPixelsX(XInches);
	}

	InParagraph = true;
	ParagraphLine = 1;
	m_paragraph.Clear();
	m_table.Clear();

	// do NOT move Y here
} // OnParagraphStart

/////////////////////////////////////////////////////////////////////////////
// OnParagraphEnd
//
// Ends the current paragraph and adds paragraph spacing.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnParagraphEnd()
{
	InParagraph = false;
	ParagraphLine = 0;

	if (InTable)
	{
		CurrentFont = FontParagraph;

		// pass in some common properties needed by the table
		Graphics* pGraphics = GetGraphics();
		shared_ptr<Gdiplus::Font> pFont = CurrentFont;
		m_table.TableGraphics = pGraphics;
		m_table.TableFont = pFont;
		m_table.Top = YInches;
		m_table.JustifyCells();

		// center the table on the page by default
		double dTableWidth = m_table.Width;
		double dPageWidth = MarginInches.Width;
		double dDelta = dPageWidth - dTableWidth;
		m_table.Left = XInches + dDelta / 2;

		// draw the table
		m_table.Draw();

		// clean up
		InTable = false;
		InTableHeading = false;
		delete pGraphics;
		return;
	}

	double fParagraphSpacing = DrawParagraph();

	if (InBlockQuote)
	{
		XInches = MarginInches.X;
		YInches += fParagraphSpacing;
	}
	else if (!InListItem)
	{
		// finish current line
		NewLine();

		// update pixel coordinates
		X = ToPixelsX(XInches);
		Y = ToPixelsY(YInches);
	}

} // OnParagraphEnd

/////////////////////////////////////////////////////////////////////////////
// OnImage
//
// Block-level image rendering with GitHub-style spacing.
// Spacing is handled here; DrawImage() performs pure drawing.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnImage
(
	const CString& csPath,
	const CString& csAlt
)
{
	//
	// GitHub-style spacing before image
	//
	//NewLine(0.1f);

	m_paragraph.MarginInches = MarginInches;
	m_paragraph.XInches = XInches;
	m_paragraph.YInches = YInches;

	//
	// Draw the image (no spacing inside DrawImage)
	//
	Graphics* pGraphics = GetGraphics();
	m_paragraph.DrawImage(pGraphics, csPath);
	delete pGraphics;

	//
	// GitHub-style spacing after image
	//
	YInches = m_paragraph.YInches;
	XInches = m_paragraph.XInches;

	//NewLine(0.1f);
} // OnImage

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHtml(const CString& csHtml)
{
	// Look for <img ...>
	int iStart = csHtml.Find(_T("<img"));
	if (iStart < 0)
		return;

	int iEnd = csHtml.Find(_T(">"), iStart);
	if (iEnd < 0)
		return;

	CString csTag = csHtml.Mid(iStart, iEnd - iStart + 1);

	// Extract src="..."
	CString csSrc = CHelper::ExtractHtmlAttribute(csTag, _T("src"));
	if (csSrc.IsEmpty())
		return;

	// Optional alt="..."
	CString csAlt = CHelper::ExtractHtmlAttribute(csTag, _T("alt"));

	// Render the image using your existing pipeline
	OnImage(csSrc, csAlt);
} // OnHtml

/////////////////////////////////////////////////////////////////////////////
// OnHeadingStart
//
// Begins a heading. Adds spacing before the heading.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeadingStart(int level)
{
	HeadingLevel = level;

	switch (level)
	{
	case 1: CurrentFont = Heading1Font; break;
	case 2: CurrentFont = Heading2Font; break;
	case 3: CurrentFont = Heading3Font; break;
	case 4: CurrentFont = Heading4Font; break;
	case 5: CurrentFont = Heading5Font; break;
	case 6: CurrentFont = Heading6Font; break;
	}

	double fBeforeInches = 0.0;

	double fLineHeight = ComputeLineHeightInches(*CurrentFont);

	switch (level)
	{
	case 1: fBeforeInches = fLineHeight * 0.40; break;
	case 2: fBeforeInches = fLineHeight * 0.30; break;
	default: fBeforeInches = 0; break;
	}

	YInches += fBeforeInches;
	Y = ToPixelsY(YInches);
} // OnHeadingStart

/////////////////////////////////////////////////////////////////////////////
// OnHeadingEnd
//
// Ends a heading. Adds spacing after the heading.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeadingEnd()
{
	// compute heading line height and draw the heading
	double fLineHeight = DrawParagraph();

	// draw underline for H1/H2
	if (HeadingLevel == 1 || HeadingLevel == 2)
	{
		double fThicknessInches = (HeadingLevel == 1 ? 0.03 : 0.02);
		int nThickness = ToPixelsY(fThicknessInches);

		double fLeftInches = MarginInches.X;
		double fRightInches = MarginInches.X + MarginInches.Width;

		int x1 = ToPixelsX(fLeftInches);
		int x2 = ToPixelsX(fRightInches);

		// underline must use the *current baseline*
		int y = ToPixelsY(YInches + fLineHeight * 0.65);

		Graphics* pGraphics = GetGraphics();
		if (pGraphics)
		{
			Gdiplus::Pen pen(Color::Silver, (Gdiplus::REAL)nThickness);
			pGraphics->DrawLine(&pen, x1, y, x2, y);
			delete pGraphics;
		}
	}

	CurrentFont = FontParagraph;
	NewLine();

	HeadingLevel = 0;
} // OnHeadingEnd

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHorizontalRule
(
)
{
	NewLine(0.2f);
	double fThicknessInches = (HeadingLevel == 1 ? 0.03 : 0.02);
	int nThickness = ToPixelsY(fThicknessInches);

	double fLeftInches = MarginInches.X;
	double fRightInches = MarginInches.X + MarginInches.Width;

	int x1 = ToPixelsX(fLeftInches);
	int x2 = ToPixelsX(fRightInches);

	float fLineHeight = ComputeLineHeightInches(*CurrentFont);
	int y = ToPixelsY(YInches + fLineHeight * 0.4);

	Graphics* pGraphics = GetGraphics();
	if (pGraphics)
	{
		Gdiplus::Pen pen(Color::Silver, (Gdiplus::REAL)nThickness);
		pGraphics->DrawLine(&pen, x1, y, x2, y);
		delete pGraphics;
	}
	NewLine(0.2f);
} // OnHorizontalRule

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnUnorderedListStart()
{
	InUnorderedList = true;
	ListDepth = ListDepth + 1;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnUnorderedListEnd()
{
	int nListDepth = ListDepth;
	nListDepth--;
	InUnorderedList = nListDepth > 0;
	ListDepth = nListDepth;
	if (nListDepth == 0)
	{
		const double fLineHeightInches = ComputeLineHeightInches(*CurrentFont);

		// advance Y by line height
		YInches += fLineHeightInches / 2;
		Y = ToPixelsY(YInches);

		// reset X to left margin
		XInches = MarginInches.X;
		X = ToPixelsX(XInches);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnOrderedListStart()
{
	OrderedListCounter = 1;
	ListDepth = ListDepth + 1;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnOrderedListEnd()
{
	OrderedListCounter = 0;
	ListDepth = ListDepth - 1;
	NewLine();
}

/////////////////////////////////////////////////////////////////////////////
// OnListItemStart
//
// Begins a list item. Draws the bullet or number.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnListItemStart
(
)
{
	// reset X to left margin
	XInches = MarginInches.X + ListDepth * 0.4;
	X = ToPixelsX(XInches);

	CurrentFont = FontParagraph;

	// mark state
	InListItem = true;

	CString csActualMarker;

	if (OrderedListCounter > 0)
	{
		csActualMarker.Format(L"%d.", OrderedListCounter);
		OrderedListCounter = OrderedListCounter + 1;
	}
	else
	{
		csActualMarker = L"•";
	}

	ListMarker = csActualMarker;

} // OnListItemStart

/////////////////////////////////////////////////////////////////////////////
// OnListItemEnd
//
// Ends a list item. Resets list-item state.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnListItemEnd
(
)
{

	double fLineHeight = DrawParagraph();

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);

	// end list-item indentation
	InListItem = false;
	ListMarker = L"";
} // OnListItemEnd

/////////////////////////////////////////////////////////////////////////////
// OnBlockQuoteStart
//
// Begins a block quote. Increases depth.
// Adds a small spacing bump when entering depth 1.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnBlockQuoteStart()
{
	BlockQuoteDepth = BlockQuoteDepth + 1;

	if (BlockQuoteDepth == 1)
	{
		CurrentFont = FontParagraph;

		// start blockquote on a clean line
		//NewLine();
	}

	InBlockQuote = true;

} // OnBlockQuoteStart

/////////////////////////////////////////////////////////////////////////////
// OnBlockQuoteEnd
//
// Ends a block quote. Decreases depth.
// Only forces a new line when exiting depth 1.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnBlockQuoteEnd()
{
	// decrease depth
	BlockQuoteDepth = BlockQuoteDepth - 1;

	// exiting the outermost block quote?
	if (BlockQuoteDepth == 0)
	{
		InBlockQuote = false;
		//NewLine();   // only here
	}

	// clamp depth (safety)
	if (BlockQuoteDepth < 0)
	{
		BlockQuoteDepth = 0;
		InBlockQuote = false;
	}
} // OnBlockQuoteEnd

/////////////////////////////////////////////////////////////////////////////
// OnCodeBlockStart
//
// Begins a code block. Records top position and applies top spacing.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnCodeBlockStart()
{
	InCodeBlock = true;

	// BEFORE spacing (0.10")
	YInches += 0.10;

	// top padding (0.08")
	YInches += 0.08;

	// record top of code block
	CodeBlockTopInches = YInches;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// OnCodeBlockEnd
//
// Ends a code block. Draws background and applies bottom spacing.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnCodeBlockEnd()
{
	// bottom padding (0.08")
	YInches += 0.08;

	// record bottom
	double fBottomInches = YInches;

	// draw background rectangle
	DrawCodeBlockBackground(CodeBlockTopInches, fBottomInches);

	InCodeBlock = false;

	// AFTER spacing (0.10")
	YInches += 0.10;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnEmphasisStart()
{
	Emphasis = true;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnEmphasisEnd()
{
	Emphasis = false;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnStrongStart()
{
	Strong = true;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnStrongEnd()
{
	Strong = false;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnInlineCodeStart()
{
	InlineCode = true;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnInlineCodeEnd()
{
	InlineCode = false;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnText(const CString& text)
{
	DrawText(text);
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnInlineCodeText(const CString& text)
{
	DrawText(text);
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHtmlText(const CString& text)
{
	DrawText(text);
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnEntityText(const CString& text)
{
	DrawText(text);
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnLinkStart(const CString& href)
{
	// existing behavior — keep this
	Emphasis = false;
	Strong = false;

	// new Step 45 behavior
	InLink = true;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnLinkEnd()
{
	InLink = false;
}

/////////////////////////////////////////////////////////////////////////////
