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
// DrawImage
//
// Draws an image at the current position, scaled to fit content width.
// No spacing is applied here; spacing is handled by OnImage().
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawImage
(
	const CString& csPath
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return;
	}

	// load image
	Image image(csPath);
	if (image.GetLastStatus() != Ok)
	{
		delete pGraphics;
		return;
	}

	//
	// natural size in pixels
	//
	const int nNaturalWidth = image.GetWidth();
	const int nNaturalHeight = image.GetHeight();

	//
	// content width in inches (page width minus left margin)
	//
	const double fContentWidthInches =
		double(MarginInches.Width) - MarginInches.X;

	//
	// convert natural width to inches
	//
	const double fNaturalWidthInches =
		(double)nNaturalWidth / Dpi;

	double fScale = 1.0;

	//
	// scale down if too wide
	//
	if (fNaturalWidthInches > fContentWidthInches)
	{
		fScale = fContentWidthInches / fNaturalWidthInches;
	}

	//
	// final size in inches
	//
	const double fDrawWidthInches = fNaturalWidthInches * fScale;
	const double fDrawHeightInches =
		((double)nNaturalHeight / Dpi) * fScale;

	//
	// convert to pixels
	//
	const int nDrawWidth = ToPixelsX(fDrawWidthInches);
	const int nDrawHeight = ToPixelsY(fDrawHeightInches);

	//
	// compute position with indentation
	//
	const double fIndentInches = ComputeIndentInches();
	const int nX = ToPixelsX(XInches + fIndentInches);
	const int nY = ToPixelsY(YInches);

	//
	// draw
	//
	pGraphics->DrawImage
	(
		&image,
		nX,
		nY,
		nDrawWidth,
		nDrawHeight
	);

	delete pGraphics;

	//
	// advance vertical position
	//
	YInches += fDrawHeightInches;

	//
	// update pixel coordinates
	//
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);
} // DrawImage

/////////////////////////////////////////////////////////////////////////////
// DrawInlineCodeBackground
//
// Draws a light background behind inline code text.
// Called from DrawWrappedText() before drawing the text.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawInlineCodeBackground
(
	const CString& text,
	const Font& font,
	Color colorFG,
	Color colorBG
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return;
	}

	// measure the text
	double fWidthInches = 0.0;
	double fHeightInches = 0.0;

	if (!MeasureString(text, font, fWidthInches, fHeightInches))
	{
		delete pGraphics;
		return;
	}

	// padding around inline code
	const double fPadX = 0.05;
	const double fPadY = 0.02;

	double fLeftInches = XInches /*+ fIndentInches*/ - fPadX;
	double fTopInches = YInches - fPadY;

	double fWidth = fWidthInches + (2 * fPadX);
	double fHeight = fHeightInches + (2 * fPadY);

	Gdiplus::RectF rcInches
	(
		(REAL)fLeftInches,
		(REAL)fTopInches,
		(REAL)fWidth,
		(REAL)fHeight
	);

	Gdiplus::RectF rcPixels = ToPixelRect(rcInches);

	// background
	SolidBrush brush(colorBG); // GitHub inline code bg
	pGraphics->FillRectangle(&brush, rcPixels);

	// border
	Pen pen(colorFG, 1.0f); // GitHub inline code border
	pGraphics->DrawRectangle(&pen, rcPixels);

	delete pGraphics;
} // DrawInlineCodeBackground

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
// DrawWrappedText
//
// Draws text with automatic line wrapping based on remaining width.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawWrappedText
(
	const CString& csText,
	const Gdiplus::Font& font,
	double fIndentInches,
	bool bAdvanceLine/* = true*/
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return;
	}

	CString csRemaining = csText;

	while (!csRemaining.IsEmpty())
	{
		// compute remaining width on this line based on XInches
		double fRemainingWidthInches = 
			RemainingLineWidthInches() - fIndentInches;
		if (fRemainingWidthInches < 0.0)
		{
			fRemainingWidthInches = 0.0;
		}

		CString csLine;
		CString csNext;

		// split text into line + remainder
		bool bFits = SplitTextToFit
		(
			csRemaining,
			font,
			fRemainingWidthInches,
			csLine,
			csNext
		);

		if (!bFits)
		{
			//
			// Nothing fits on this line.
			// Draw nothing, advance to next line, and continue.
			//
			NewLine();
			csRemaining = csNext;   // usually the full text
			continue;
		}

		// draw the line
		{
			// compute pixel position
			const int nX = ToPixelsX(XInches + fIndentInches);
			const int nY = ToPixelsY(YInches);

			// origin point in pixels
			Gdiplus::PointF ptOrigin
			(
				(REAL)nX,
				(REAL)nY
			);

			Color textColor =
				(InLink && !InlineCode) ?
				Color::Blue :
				Color::Black;
			SolidBrush brush(textColor);

			// *** NEW ORDER ***
			// 1. Inline code background first
			//if (InlineCode)
			//{
			//	DrawInlineCodeBackground(csLine, font, fIndentInches);
			//}

			// 2. Blockquote bar second
			if (InBlockQuote)
			{
				DrawBlockQuoteBar(font, fIndentInches);
			}

			// 3. Text last
			pGraphics->DrawString
			(
				csLine,
				csLine.GetLength(),
				&font,
				ptOrigin,
				&brush
			);
		}

		// advance to next line
		if (CHelper::NearlyEqual(fRemainingWidthInches, 0.0))
		{
			NewLine();
		}

		// continue with remainder
		csRemaining = csNext;
	}

	delete pGraphics;
} // DrawWrappedText

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
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::NewLine
(
)
{
	const double fLineHeightInches = ComputeLineHeightInches(*CurrentFont);

	// advance Y by line height
	YInches += fLineHeightInches;
	Y = ToPixelsY(YInches);

	// reset X to left margin
	XInches = MarginInches.X;
	X = ToPixelsX(XInches);
} // NewLine

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawRightAlignedMarker
(
	Graphics* pGraphics,
	const CString& csMarker,
	const Gdiplus::Font& font,
	double fMarkerColumnWidthInches,
	double fColumnLeftInches,
	double fYInches
)
{
	// Measure marker width
	double fMarkerWidthInches = 0.0;
	double fMarkerHeightInches = 0.0;
	MeasureString(csMarker, font, fMarkerWidthInches, fMarkerHeightInches);

	// Compute left origin so marker is right-aligned
	double fLeftInches = 
		fColumnLeftInches + (fMarkerColumnWidthInches - fMarkerWidthInches);

	// Convert to pixels
	int nX = ToPixelsX(fLeftInches);
	int nY = ToPixelsY(fYInches);

	PointF ptOrigin((REAL)nX, (REAL)nY);
	SolidBrush brush(Color::Black);

	pGraphics->DrawString
	(
		csMarker,
		csMarker.GetLength(),
		&font,
		ptOrigin,
		&brush
	);
} // DrawRightAlignedMarker

/////////////////////////////////////////////////////////////////////////////
// DrawSegmetText
//
// Draws segments of paragraph text.
//
/////////////////////////////////////////////////////////////////////////////
double CMarkdownBitmapRenderer::DrawSegmentText
(
	shared_ptr<CParagraphToken> pSeg
)
{
	double value = RemainingLineWidthInches();
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		return value;
	}

	CString csText = pSeg->Text;
	shared_ptr<Gdiplus::Font> pFont = pSeg->Font;
	bool bBackground = pSeg->Background;
	Color colorFG = pSeg->ColorFG;
	Color colorBG = pSeg->ColorBG;
	bool bBlockQuote = pSeg->BlockQuote;
	int nBlockQuoteDepth = pSeg->BlockQuoteDepth;
	int nParagraphLine = pSeg->ParagraphLine;
	bool bListMarker = pSeg->ListMarker;

	if (bBlockQuote &&nParagraphLine > 1)
	{
		NewLine();
	}

	double fX = XInches;
	double fY = YInches;

	if (bBlockQuote)
	{
		if (CHelper::NearlyEqual(fX, 0.0))
		{
			for (int n = 1; n <= nBlockQuoteDepth; n++)
			{
				double dOffset = fX;
				dOffset += 0.4 * n;
				XInches = dOffset;
				DrawBlockQuoteBar(*CurrentFont, dOffset);
			}
			fX = XInches;
		}
	}

	int nToken = 1;
	int nStart = 0;
	CString csToken = csText.Tokenize(L" ", nStart);
	bool bMarker = false;
	while (!csToken.IsEmpty())
	{
		double fWidth = 0, fHeight = 0;
		if (bListMarker && nToken++ == 1)
		{
			fWidth = m_paragraph.MarkerLength;
			DrawRightAlignedMarker
			(
				pGraphics,
				csToken,
				*CurrentFont.get(),
				fWidth,
				fX,
				fY
			);
			bMarker = true;
		}
		else
		{
			MeasureString(csToken + L" ", *pFont.get(), fWidth, fHeight);
		}

		// start a new line?
		if (fWidth > value)
		{
			NewLine();
			fX = ComputeIndentInches();
			XInches = fX;
			value = RemainingLineWidthInches();
		}

		if (bBackground)
		{
			DrawInlineCodeBackground(csToken, *pFont.get(), colorFG, colorBG);
		}

		// compute pixel position
		const int nX = ToPixelsX(fX);
		const int nY = ToPixelsY(YInches);

		// origin point in pixels
		Gdiplus::PointF ptOrigin((REAL)nX, (REAL)nY);

		SolidBrush brush(Color::Black);

		if (!bMarker)
		{
			pGraphics->DrawString
			(
				csToken,
				csToken.GetLength(),
				pFont.get(),
				ptOrigin,
				&brush
			);
		}

		bMarker = false;

		fX += fWidth;
		XInches = fX;
		value = RemainingLineWidthInches();

		csToken = csText.Tokenize(L" ", nStart);
	}

	delete pGraphics;
	return value;
} // DrawSegmentText

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

	// base indentation in inches
	double fIndentInches = ComputeIndentInches();

	CString csText(text);

	// links use underline + blue color
	if (InLink)
	{
		int style = CurrentFont->GetStyle() | FontStyleUnderline;
		Gdiplus::FontFamily ff;
		CurrentFont->GetFamily(&ff);
		REAL fSize = CurrentFont->GetSize();
		Gdiplus::Font linkFont
		(
			&ff, fSize, style, UnitPoint
		);

		DrawWrappedText(csText, linkFont, fIndentInches);
		delete pGraphics;
		return;
	}

	if (InParagraph)
	{
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
		pToken->ParagraphLine = nParagraphLine++;
		pToken->ListMarker = !csListMarker.IsEmpty();
		pToken->ListItem = InListItem;
		pToken->InlineCode = InlineCode;
		pToken->Emphasis = Emphasis;
		pToken->Strong = Strong;
		ParagraphLine = nParagraphLine;
		m_paragraph.Append(pToken);

		delete pGraphics;
		return;
	}

	// normal text uses CurrentFont
	DrawWrappedText(csText, *CurrentFont, fIndentInches);

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
	// switch to paragraph font
	CurrentFont = FontParagraph;

	// ensure X is at left margin
	XInches = MarginInches.X;
	X = ToPixelsX(XInches);

	InParagraph = true;
	ParagraphLine = 1;
	m_paragraph.Clear();

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

	//// base indentation in inches
	//double fIndentInches = ComputeIndentInches();
	//CString csSpace(L" ");
	//double fWidth = 0, fHeight = 0;
	//MeasureString(csSpace, *CurrentFont, fWidth, fHeight);

	//long lSegments = m_paragraph.Count;
	//for (auto& node : m_paragraph.Items)
	//{
	//	CString csSegment = node->Text;
	//	double dX = XInches;

	//	// remove the previous space is the segment is a lone comma
	//	if (csSegment.Left(1) == L",")
	//	{
	//		dX -= fWidth;
	//		XInches = dX;
	//	}
	//	shared_ptr<Gdiplus::Font> pFont = node->Font;
	//	double fLen = node->Length;
	//	double dRemaining = DrawSegmentText(node);
	//}

	Gdiplus::Graphics* pGraphics = GetGraphics();

	double dLeft = MarginInches.X;
	double dRight = dLeft + MarginInches.Width;
	double dY = YInches;

	bool bDrawLine = false;
	double fLineHeight =0;
	double fParagraphSpacing = 0;

	do
	{
		bDrawLine = m_paragraph.DrawLine(pGraphics, dLeft, dRight, dY);
		fLineHeight = ComputeLineHeightInches(*FontParagraph);
		fParagraphSpacing = fLineHeight * 0.60;

		YInches += fParagraphSpacing;
		dY = YInches;

	} while (bDrawLine == true);

	delete pGraphics;

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
	YInches += 0.06;
	NewLine();      // ensure clean block start

	//
	// Draw the image (no spacing inside DrawImage)
	//
	DrawImage(csPath);

	//
	// GitHub-style spacing after image
	//
	YInches += 0.06;
	NewLine();
} // OnImage

/////////////////////////////////////////////////////////////////////////////
// OnHorizontalRule
//
// Draws a horizontal rule with normalized spacing and thickness.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHorizontalRule()
{
	NewLine();

	const double fLeftInches = MarginInches.X;
	const double fRightInches = MarginInches.X + MarginInches.Width;

	const int x1 = ToPixelsX(fLeftInches);
	const int x2 = ToPixelsX(fRightInches);
	const int y = ToPixelsY(YInches);

	Graphics* pGraphics = GetGraphics();
	if (pGraphics != nullptr)
	{
		int nThickness = ToPixelsY(0.02);
		Gdiplus::REAL fThick = (Gdiplus::REAL)nThickness;
		Gdiplus::Pen pen(Color::Silver, fThick);
		pGraphics->DrawLine(&pen, x1, y, x2, y);
		delete pGraphics;
	}

	NewLine();
} // OnHorizontalRule

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
	default: fBeforeInches = fLineHeight * 0.20; break;
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
	// compute heading line height
	double fLineHeight = ComputeLineHeightInches(*CurrentFont);

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
		int y = ToPixelsY(YInches + fLineHeight);

		Graphics* pGraphics = GetGraphics();
		if (pGraphics)
		{
			Gdiplus::Pen pen(Color::Silver, (Gdiplus::REAL)nThickness);
			pGraphics->DrawLine(&pen, x1, y, x2, y);
			delete pGraphics;
		}
	}

	NewLine();

	//// proportional spacing after heading
	//double fAfterInches = 0.0;

	//switch (HeadingLevel)
	//{
	//case 1: fAfterInches = fLineHeight * 0.20; break;
	//case 2: fAfterInches = fLineHeight * 0.15; break;
	//default: fAfterInches = fLineHeight * 0.10; break;
	//}

	//YInches += fAfterInches;
	//Y = ToPixelsY(YInches);

	HeadingLevel = 0;
	CurrentFont = FontParagraph;
} // OnHeadingEnd

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnUnorderedListStart()
{
	InUnorderedList = true;
	ListDepth = ListDepth + 1;
}

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnUnorderedListEnd()
{
	InUnorderedList = false;
	ListDepth = ListDepth - 1;
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
	XInches = MarginInches.X;
	X = ToPixelsX(XInches);

	CurrentFont = FontParagraph;

	double fLineHeight = ComputeLineHeightInches(*CurrentFont);

	// spacing between list items
	YInches += fLineHeight / 2;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);

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
	OnParagraphStart();

	// draw the bullet or number
	//DrawListMarker(csActualMarker);

	// *** IMPORTANT ***
	// No indentation logic here.
	// Indentation is now computed exclusively by ComputeIndentInches().
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
	OnParagraphEnd();

	// end list-item indentation
	InListItem = false;
	ListMarker = L"";


	double fLineHeight = ComputeLineHeightInches(*CurrentFont);

	// spacing between list items
	//YInches += fLineHeight / 4;

	// update pixel coordinates
	X = ToPixelsX(XInches);
	Y = ToPixelsY(YInches);

	// *** IMPORTANT ***
	// No indentation logic here.
	// No XInches resets.
	// Indentation is now computed exclusively by ComputeIndentInches().
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

	// bars are drawn per line in DrawWrappedText()
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
// OnHeading1
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeading1(const CString& text)
{
	YInches += 0.15;
	Y = ToPixelsY(YInches);

	DrawWrappedText(text, *Heading1Font, ComputeIndentInches(), false);

	YInches += 0.20;
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// OnHeading2
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeading2(const CString& text)
{
	YInches += 0.12;
	Y = ToPixelsY(YInches);

	DrawWrappedText(text, *Heading2Font, ComputeIndentInches(), false);

	YInches += 0.15;
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// OnHeading3
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeading3(const CString& text)
{
	YInches += 0.10;
	Y = ToPixelsY(YInches);

	DrawWrappedText(text, *Heading3Font, ComputeIndentInches(), false);

	YInches += 0.12;
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// OnHeading4
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeading4(const CString& text)
{
	YInches += 0.08;
	Y = ToPixelsY(YInches);

	DrawWrappedText(text, *Heading4Font, ComputeIndentInches(), false);

	YInches += 0.10;
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// OnHeading5
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeading5(const CString& text)
{
	YInches += 0.06;
	Y = ToPixelsY(YInches);

	DrawWrappedText(text, *Heading5Font, ComputeIndentInches(), false);

	YInches += 0.08;
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// OnHeading6
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnHeading6(const CString& text)
{
	YInches += 0.06;
	Y = ToPixelsY(YInches);

	DrawWrappedText(text, *Heading6Font, ComputeIndentInches(), false);

	YInches += 0.06;
	Y = ToPixelsY(YInches);
}

/////////////////////////////////////////////////////////////////////////////
// TABLE RENDERING
/////////////////////////////////////////////////////////////////////////////

//
// Called when a table begins
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableStart()
{
	InTable = true;
	InTableHeader = false;
	InTableRow = false;

	m_vecCurrentRow.clear();
	m_vecAllRows.clear();
	m_vecColumnWidthsInches.clear();
	m_vecColumnAlignment.clear();
	m_vecColumnBackColor.clear();
	m_mapCellBackColor.clear();
	m_mapCellTextColor.clear();
	m_mapCellFontStyle.clear();
	m_mapCellPadding.clear();
	m_mapCellBorder.clear();
	m_mapCellCornerRadius.clear();
	m_mapCellShadow.clear();
	m_mapCellGradient.clear();
	m_mapCellImageBackground.clear();
	m_mapCellOverlay.clear();
	m_mapCellBlend.clear();

	TableColumnCount = 0;

	YInches += TableSpacingBeforeInches;
	NewLine();
} // OnTableStart


//
// Called when a table ends
//
void CMarkdownBitmapRenderer::OnTableEnd()
{
	InTable = false;

	RenderTable();

	// spacing after table
	YInches += TableSpacingAfterInches;
	NewLine();
}

//
// Called when header row begins
//
void CMarkdownBitmapRenderer::OnTableHeaderStart()
{
	InTableHeader = true;
	InTableRow = true;
	m_vecCurrentRow.clear();
}

//
// Called when header row ends
//
void CMarkdownBitmapRenderer::OnTableHeaderEnd()
{
	InTableHeader = false;
	InTableRow = false;

	// store header row
	m_vecAllRows.push_back(m_vecCurrentRow);

	// determine column count
	TableColumnCount = (int)m_vecCurrentRow.size();
}

//
// Called when a normal row begins
//
void CMarkdownBitmapRenderer::OnTableRowStart()
{
	InTableRow = true;
	m_vecCurrentRow.clear();
}

//
// Called when a normal row ends
//
void CMarkdownBitmapRenderer::OnTableRowEnd()
{
	InTableRow = false;

	// store row
	m_vecAllRows.push_back(m_vecCurrentRow);
}

//
// Called once per cell
//
void CMarkdownBitmapRenderer::OnTableCell(const CString& csText, int nColumnIndex)
{
	// ensure vector is large enough
	if ((int)m_vecCurrentRow.size() <= nColumnIndex)
	{
		m_vecCurrentRow.resize(nColumnIndex + 1);
	}

	m_vecCurrentRow[nColumnIndex] = csText;
}

/////////////////////////////////////////////////////////////////////////////
// MeasureTableColumns
//
// Computes column widths in inches based on all rows.
// Uses MeasureString() and cell padding.
//
/////////////////////////////////////////////////////////////////////////////
bool CMarkdownBitmapRenderer::MeasureTableColumns
(
	const std::vector<std::vector<CString>>& vecRows,
	std::vector<double>& vecColumnWidthsInches
)
{
	vecColumnWidthsInches.clear();

	if (vecRows.empty())
		return false;

	const int nCols = (int)vecRows[0].size();
	vecColumnWidthsInches.resize(nCols, 0.0);

	FontFamily ff(L"Segoe UI");
	Font font(&ff, 12.0f, FontStyleRegular, UnitPoint);

	// --- Step 1: measure natural widths ---
	for (int col = 0; col < nCols; ++col)
	{
		double fMaxWidth = 0.0;

		for (const auto& row : vecRows)
		{
			if (col >= (int)row.size())
				continue;

			const CString& csText = row[col];

			double fWidthInches = 0.0;
			double fHeightInches = 0.0;

			MeasureString(csText, font, fWidthInches, fHeightInches);

			fWidthInches += (TableCellPadLeftInches + TableCellPadRightInches);

			if (fWidthInches > fMaxWidth)
				fMaxWidth = fWidthInches;
		}

		vecColumnWidthsInches[col] = fMaxWidth;
	}

	// --- Step 2: apply minimum width ---
	const double fMinColWidthInches = 0.50; // half-inch minimum
	for (double& w : vecColumnWidthsInches)
	{
		if (w < fMinColWidthInches)
			w = fMinColWidthInches;
	}

	// --- Step 3: apply maximum width ---
	const double fMaxColWidthInches = 3.00; // 3-inch cap
	for (double& w : vecColumnWidthsInches)
	{
		if (w > fMaxColWidthInches)
			w = fMaxColWidthInches;
	}

	// --- Step 4: compute total width ---
	double fTotalWidthInches = 0.0;
	for (double w : vecColumnWidthsInches)
		fTotalWidthInches += w;

	// --- Step 5: compute available width ---
	double fAvailableWidthInches =
		MarginInches.Width - ComputeIndentInches();

	// --- Step 6: shrink if overflowing ---
	if (fTotalWidthInches > fAvailableWidthInches && fTotalWidthInches > 0.0)
	{
		double fScale = fAvailableWidthInches / fTotalWidthInches;

		for (double& w : vecColumnWidthsInches)
			w *= fScale;

		return true;
	}

	// --- Step 7: expand proportionally if underfull ---
	if (fTotalWidthInches < fAvailableWidthInches && fTotalWidthInches > 0.0)
	{
		double fExtra = fAvailableWidthInches - fTotalWidthInches;

		// proportional distribution
		for (double& w : vecColumnWidthsInches)
		{
			double fShare = (w / fTotalWidthInches) * fExtra;
			w += fShare;
		}
	}

	return true;
} // MeasureTableColumns

/////////////////////////////////////////////////////////////////////////////
bool CMarkdownBitmapRenderer::WrapTextToLines
(
	const CString& csText,
	const Gdiplus::Font& font,
	double fMaxWidthInches,
	std::vector<CString>& vecLines
)
{
	vecLines.clear();

	CString csRemaining = csText;

	while (!csRemaining.IsEmpty())
	{
		CString csLine;
		CString csNext;

		// Use your existing splitter
		SplitTextToFit
		(
			csRemaining,
			font,
			fMaxWidthInches,
			csLine,
			csNext
		);

		vecLines.push_back(csLine);
		csRemaining = csNext;
	}

	return true;
} // WrapTextToLines

/////////////////////////////////////////////////////////////////////////////
// DrawTableCell
//
// Draws a single cell at the given position.
// Handles padding, header shading, wrapping, alignment, and borders.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawTableCell
(
	const CString& csText,
	double fLeftInches,
	double fWidthInches,
	bool bHeader,
	int nAlignment,
	int nVAlign,
	int nColumnIndex,
	double& fCellHeightOut
)
{
	Graphics* pGraphics = GetGraphics();
	if (!pGraphics)
	{
		fCellHeightOut = 0.0;
		return;
	}

	// default font families
	FontFamily ffNormal(L"Segoe UI");
	FontFamily ffMono(L"Consolas");

	// pointer to the chosen family
	FontFamily* pFamily = &ffNormal;

	// default style (header bolding still applies)
	FontStyle style = bHeader ? FontStyleBold : FontStyleRegular;

	// per-cell font style override
	auto itFont = m_mapCellFontStyle.find({ CurrentRowIndex, nColumnIndex });
	if (itFont != m_mapCellFontStyle.end())
	{
		int nStyle = itFont->second;

		// monospace?
		if (nStyle & CellFontStyle_Monospace)
		{
			pFamily = &ffMono;   // switch family safely
		}

		// bold?
		if (nStyle & CellFontStyle_Bold)
			style = (FontStyle)(style | FontStyleBold);

		// italic?
		if (nStyle & CellFontStyle_Italic)
			style = (FontStyle)(style | FontStyleItalic);
	}

	// final font
	Font font(pFamily, 12.0f, style, UnitPoint);

	// default padding
	double padLeft = TableCellPadLeftInches;
	double padRight = TableCellPadRightInches;
	double padTop = TableCellPadTopInches;
	double padBottom = TableCellPadBottomInches;

	// per-cell padding override
	auto itPad = m_mapCellPadding.find({ CurrentRowIndex, nColumnIndex });
	if (itPad != m_mapCellPadding.end())
	{
		padLeft = itPad->second.Left;
		padRight = itPad->second.Right;
		padTop = itPad->second.Top;
		padBottom = itPad->second.Bottom;
	}

	// inner width for wrapping
	double fInnerWidthInches =
		fWidthInches - (padLeft + padRight);

	//
	// --- WRAP TEXT ---
	//
	std::vector<CString> vecLines;
	WrapTextToLines(csText, font, fInnerWidthInches, vecLines);

	//
	// --- MEASURE LINE HEIGHT ---
	//
	double fLineHeightInches = 0.0;
	{
		double fW = 0.0;
		double fH = 0.0;
		MeasureString(L"A", font, fW, fH);
		fLineHeightInches = fH;
	}

	//
	// --- COMPUTE CELL HEIGHT ---
	//
	double fTextBlockHeight = vecLines.size() * fLineHeightInches;

	double fCellHeightInches =
		padTop +
		fTextBlockHeight +
		padBottom;

	fCellHeightOut = fCellHeightInches;

	//
	// --- CELL RECTANGLE ---
	//
	Gdiplus::RectF rcCellInches
	(
		(REAL)fLeftInches,
		(REAL)YInches,
		(REAL)fWidthInches,
		(REAL)fCellHeightInches
	);

	Gdiplus::RectF rcCellPixels = ToPixelRect(rcCellInches);

	//
	// --- PER-CELL SHADOW ---
	//
	auto itShadow = m_mapCellShadow.find({ CurrentRowIndex, nColumnIndex });
	if (itShadow != m_mapCellShadow.end())
	{
		const CellShadow& shadow = itShadow->second;

		if (shadow.Enabled)
		{
			// shadow rectangle (offset)
			Gdiplus::RectF rcShadow = rcCellPixels;
			rcShadow.X += shadow.OffsetX;
			rcShadow.Y += shadow.OffsetY;

			// simulate blur by drawing multiple translucent rectangles
			for (int i = 0; i < (int)shadow.Blur; ++i)
			{
				float alphaScale = 1.0f - (float)i / shadow.Blur;
				Gdiplus::Color c
				(
					(BYTE)(shadow.Color.GetAlpha() * alphaScale),
					shadow.Color.GetRed(),
					shadow.Color.GetGreen(),
					shadow.Color.GetBlue()
				);

				SolidBrush blurBrush(c);
				pGraphics->FillRectangle(&blurBrush, rcShadow);

				// expand slightly each iteration
				rcShadow.X -= 0.5f;
				rcShadow.Y -= 0.5f;
				rcShadow.Width += 1.0f;
				rcShadow.Height += 1.0f;
			}
		}
	}

	// --- PER-CELL GRADIENT ---
	auto itGrad = m_mapCellGradient.find({ CurrentRowIndex, nColumnIndex });
	if (itGrad != m_mapCellGradient.end())
	{
		const CellGradient& grad = itGrad->second;

		if (grad.Type != Gradient_None)
		{
			if (grad.Type == Gradient_Linear)
			{
				// linear gradient brush
				LinearGradientBrush brush(
					rcCellPixels,
					grad.StartColor,
					grad.EndColor,
					grad.Angle,
					false
				);

				pGraphics->FillRectangle(&brush, rcCellPixels);
			}
			else if (grad.Type == Gradient_Radial)
			{
				// radial gradient simulation using PathGradientBrush
				GraphicsPath path;
				path.AddRectangle(rcCellPixels);

				PathGradientBrush brush(&path);

				brush.SetCenterColor(grad.StartColor);

				int count = 1;
				Gdiplus::Color surroundColors[1] = { grad.EndColor };
				brush.SetSurroundColors(surroundColors, &count);

				// radius multiplier (1.0 = full cell)
				REAL focusX = rcCellPixels.X + rcCellPixels.Width / 2.0f;
				REAL focusY = rcCellPixels.Y + rcCellPixels.Height / 2.0f;

				brush.SetCenterPoint(Gdiplus::PointF(focusX, focusY));

				pGraphics->FillRectangle(&brush, rcCellPixels);
			}
		}
	}

	//
	// --- PER-CELL IMAGE BACKGROUND ---
	//
	auto itImg = m_mapCellImageBackground.find({ CurrentRowIndex, nColumnIndex });
	if (itImg != m_mapCellImageBackground.end())
	{
		const CellImageBackground& bg = itImg->second;

		if (bg.Mode != Image_None && bg.pImage)
		{
			// set opacity
			ImageAttributes attrs;
			float matrix[5][5] =
			{
				{1, 0, 0, 0, 0},
				{0, 1, 0, 0, 0},
				{0, 0, 1, 0, 0},
				{0, 0, 0, bg.Opacity, 0},
				{0, 0, 0, 0, 1}
			};
			attrs.SetColorMatrix((ColorMatrix*)matrix);

			if (bg.Mode == Image_Stretch)
			{
				pGraphics->DrawImage
				(
					bg.pImage,
					rcCellPixels,
					0, 0,
					(REAL)bg.pImage->GetWidth(),
					(REAL)bg.pImage->GetHeight(),
					UnitPixel,
					&attrs
				);
			}
			else if (bg.Mode == Image_Center)
			{
				float imgW = (float)bg.pImage->GetWidth();
				float imgH = (float)bg.pImage->GetHeight();

				float x = rcCellPixels.X + (rcCellPixels.Width - imgW) / 2.0f;
				float y = rcCellPixels.Y + (rcCellPixels.Height - imgH) / 2.0f;

				pGraphics->DrawImage
				(
					bg.pImage,
					x, y,
					imgW,
					imgH
				);
			}
			else if (bg.Mode == Image_Tile)
			{
				float imgW = (float)bg.pImage->GetWidth();
				float imgH = (float)bg.pImage->GetHeight();

				for (float y = rcCellPixels.Y; y < rcCellPixels.Y + rcCellPixels.Height; y += imgH)
				{
					for (float x = rcCellPixels.X; x < rcCellPixels.X + rcCellPixels.Width; x += imgW)
					{
						pGraphics->DrawImage
						(
							bg.pImage,
							x, y,
							imgW,
							imgH
						);
					}
				}
			}
		}
	}

	// --- PER-CELL OVERLAY ---
	auto itOv = m_mapCellOverlay.find({ CurrentRowIndex, nColumnIndex });
	if (itOv != m_mapCellOverlay.end())
	{
		const CellOverlay& ov = itOv->second;

		if (ov.Mode == Overlay_Color)
		{
			// solid color overlay with opacity
			Gdiplus::Color c(
				(BYTE)(ov.Color.GetAlpha() * ov.Opacity),
				ov.Color.GetRed(),
				ov.Color.GetGreen(),
				ov.Color.GetBlue()
			);

			SolidBrush brush(c);
			pGraphics->FillRectangle(&brush, rcCellPixels);
		}
		else if (ov.Mode == Overlay_Image && ov.pImage)
		{
			// image overlay with opacity
			ImageAttributes attrs;
			float matrix[5][5] =
			{
				{1, 0, 0, 0, 0},
				{0, 1, 0, 0, 0},
				{0, 0, 1, 0, 0},
				{0, 0, 0, ov.ImageOpacity, 0},
				{0, 0, 0, 0, 1}
			};
			attrs.SetColorMatrix((ColorMatrix*)matrix);

			pGraphics->DrawImage(
				ov.pImage,
				rcCellPixels,
				0, 0,
				(REAL)ov.pImage->GetWidth(),
				(REAL)ov.pImage->GetHeight(),
				UnitPixel,
				&attrs
			);
		}
		else if (ov.Mode == Overlay_Glass)
		{
			// frosted-glass simulation: translucent white overlay
			Gdiplus::Color glassColor(
				(BYTE)(255 * ov.GlassOpacity),
				255, 255, 255
			);

			SolidBrush brush(glassColor);
			pGraphics->FillRectangle(&brush, rcCellPixels);
		}
	}

	// --- PER-CELL BLEND MODE ---
	auto itBlend = m_mapCellBlend.find({ CurrentRowIndex, nColumnIndex });
	if (itBlend != m_mapCellBlend.end())
	{
		const CellBlend& blend = itBlend->second;

		if (blend.Mode != Blend_None)
		{
			// convert blend color to normalized floats
			float r = blend.Color.GetRed() / 255.0f;
			float g = blend.Color.GetGreen() / 255.0f;
			float b = blend.Color.GetBlue() / 255.0f;
			float a = blend.Opacity;

			ColorMatrix matrix = {};

			switch (blend.Mode)
			{
			case Blend_Multiply:
				// multiply darkening
				matrix = 
				{
					r, 0, 0, 0, 0,
					0, g, 0, 0, 0,
					0, 0, b, 0, 0,
					0, 0, 0, a, 0,
					0, 0, 0, 0, 1
				};
				break;

			case Blend_Screen:
				// screen brightening
				matrix = 
				{
					1 - r, 0,     0,     0, 0,
					0,     1 - g, 0,     0, 0,
					0,     0,     1 - b, 0, 0,
					0,     0,     0,     a, 0,
					0,     0,     0,     0, 1
				};
				break;

			case Blend_Overlay:
				// overlay (contrast boost)
				matrix = 
				{
					r * 2, 0,     0,     0, 0,
					0,     g * 2, 0,     0, 0,
					0,     0,     b * 2, 0, 0,
					0,     0,     0,     a, 0,
					0,     0,     0,     0, 1
				};
				break;

			case Blend_Lighten:
				matrix = 
				{
					1, 0, 0, 0, r * a,
					0, 1, 0, 0, g * a,
					0, 0, 1, 0, b * a,
					0, 0, 0, a, 0,
					0, 0, 0, 0, 1
				};
				break;

			case Blend_Darken:
				matrix = 
				{
					r, 0, 0, 0, 0,
					0, g, 0, 0, 0,
					0, 0, b, 0, 0,
					0, 0, 0, a, 0,
					0, 0, 0, 0, 1
				};
				break;
			}

			ImageAttributes attrs;
			attrs.SetColorMatrix(&matrix);

			// apply blend by drawing a transparent rectangle using the matrix
			pGraphics->DrawImage
			(
				WhitePixel.get(), // a 1x1 white pixel image you already have
				rcCellPixels,
				0, 0,
				1, 1,
				UnitPixel,
				&attrs
			);
		}
	}

	//
	// --- HEADER SHADING ---
	//
	if (bHeader)
	{
		SolidBrush headerBrush(Color(0xF0, 0xF0, 0xF0));
		pGraphics->FillRectangle(&headerBrush, rcCellPixels);
	}

	// per-cell background color
	auto itCellBack = 
		m_mapCellBackColor.find({ CurrentRowIndex, nColumnIndex });
	if (itCellBack != m_mapCellBackColor.end())
	{
		Gdiplus::Color backColor = itCellBack->second;
		if (backColor.GetAlpha() != 0) // not transparent
		{
			SolidBrush backBrush(backColor);
			pGraphics->FillRectangle(&backBrush, rcCellPixels);
		}
	}

	// per-column background color
	if (nColumnIndex < (int)m_vecColumnBackColor.size())
	{
		Gdiplus::Color backColor = m_vecColumnBackColor[nColumnIndex];
		if (backColor.GetAlpha() != 0) // not transparent
		{
			SolidBrush backBrush(backColor);
			pGraphics->FillRectangle(&backBrush, rcCellPixels);
		}
	}

	//
	// --- PER-CELL CORNER RADIUS ---
	//
	CellCornerRadius radius;

	auto itRad = m_mapCellCornerRadius.find({ CurrentRowIndex, nColumnIndex });
	if (itRad != m_mapCellCornerRadius.end())
	{
		radius = itRad->second;
	}

	// If any radius is non-zero, draw rounded background shape
	bool bRounded =
		(radius.TL > 0.0f) ||
		(radius.TR > 0.0f) ||
		(radius.BR > 0.0f) ||
		(radius.BL > 0.0f);

	if (bRounded)
	{
		GraphicsPath path;

		float x = rcCellPixels.X;
		float y = rcCellPixels.Y;
		float w = rcCellPixels.Width;
		float h = rcCellPixels.Height;

		// top-left
		if (radius.TL > 0.0f)
			path.AddArc
			(
				x, y, radius.TL * 2, radius.TL * 2, 180, 90
			);
		else
			path.AddLine(x, y, x, y);

		// top-right
		if (radius.TR > 0.0f)
			path.AddArc
			(
				x + w - radius.TR * 2, y, 
				radius.TR * 2, 
				radius.TR * 2, 270, 90
			);
		else
			path.AddLine(x + w, y, x + w, y);

		// bottom-right
		if (radius.BR > 0.0f)
			path.AddArc
			(
				x + w - radius.BR * 2, 
				y + h - radius.BR * 2, 
				radius.BR * 2, 
				radius.BR * 2, 0, 90
			);
		else
			path.AddLine(x + w, y + h, x + w, y + h);

		// bottom-left
		if (radius.BL > 0.0f)
			path.AddArc
			(
				x, y + h - radius.BL * 2, 
				radius.BL * 2, 
				radius.BL * 2, 90, 90
			);
		else
			path.AddLine(x, y + h, x, y + h);

		path.CloseFigure();

		// fill rounded background (respecting per-cell background color)
		Gdiplus::Color fillColor(0xF0, 0xF0, 0xF0); // default header fallback

		auto itCellBack = 
			m_mapCellBackColor.find({ CurrentRowIndex, nColumnIndex });
		if (itCellBack != m_mapCellBackColor.end())
			fillColor = itCellBack->second;
		else if (nColumnIndex < (int)m_vecColumnBackColor.size())
			fillColor = m_vecColumnBackColor[nColumnIndex];

		SolidBrush roundedBrush(fillColor);
		pGraphics->FillPath(&roundedBrush, &path);
	}

	//
	// --- PER-CELL BORDER OVERRIDES ---
	//
	CellBorder border;

	// default GitHub-style horizontal borders
	border.DrawTop = true;
	border.DrawBottom = true;
	border.DrawLeft = false;
	border.DrawRight = false;
	border.Color = Gdiplus::Color(0xD0, 0xD7, 0xDE);
	border.Thickness = 1.0f;

	// override?
	auto itBorder = m_mapCellBorder.find({ CurrentRowIndex, nColumnIndex });
	if (itBorder != m_mapCellBorder.end())
	{
		border = itBorder->second;
	}

	Pen pen(border.Color, border.Thickness);

	// top
	if (border.DrawTop)
	{
		pGraphics->DrawLine
		(
			&pen,
			rcCellPixels.X,
			rcCellPixels.Y,
			rcCellPixels.X + rcCellPixels.Width,
			rcCellPixels.Y
		);
	}

	// bottom
	if (border.DrawBottom)
	{
		pGraphics->DrawLine
		(
			&pen,
			rcCellPixels.X,
			rcCellPixels.Y + rcCellPixels.Height,
			rcCellPixels.X + rcCellPixels.Width,
			rcCellPixels.Y + rcCellPixels.Height
		);
	}

	// left
	if (border.DrawLeft)
	{
		pGraphics->DrawLine
		(
			&pen,
			rcCellPixels.X,
			rcCellPixels.Y,
			rcCellPixels.X,
			rcCellPixels.Y + rcCellPixels.Height
		);
	}

	// right
	if (border.DrawRight)
	{
		pGraphics->DrawLine
		(
			&pen,
			rcCellPixels.X + rcCellPixels.Width,
			rcCellPixels.Y,
			rcCellPixels.X + rcCellPixels.Width,
			rcCellPixels.Y + rcCellPixels.Height
		);
	}


	//
	// --- VERTICAL ALIGNMENT ---
	//
	double fBaseYInches = YInches + padTop;

	double fAvailableHeight =
		fCellHeightInches - (padTop + padBottom);

	if (nVAlign == 1) // middle
	{
		fBaseYInches =
			YInches +
			padTop +
			(fAvailableHeight - fTextBlockHeight) / 2.0;
	}
	else if (nVAlign == 2) // bottom
	{
		fBaseYInches =
			YInches +
			fCellHeightInches -
			padBottom -
			fTextBlockHeight;
	}

	//
	// --- DRAW EACH WRAPPED LINE ---
	//
	for (size_t i = 0; i < vecLines.size(); ++i)
	{
		const CString& csLine = vecLines[i];

		// measure line width
		double fLineWidthInches = 0.0;
		double fTmpHeight = 0.0;
		MeasureString(csLine, font, fLineWidthInches, fTmpHeight);

		// horizontal alignment
		double fTextXInches = fLeftInches + padLeft;

		if (nAlignment == 1) // center
		{
			fTextXInches =
				fLeftInches +
				padLeft +
				(fInnerWidthInches - fLineWidthInches) / 2.0;
		}
		else if (nAlignment == 2) // right
		{
			fTextXInches =
				fLeftInches +
				fWidthInches -
				padRight -
				fLineWidthInches;
		}

		double fTextYInches =
			fBaseYInches + (i * fLineHeightInches);

		int nX = ToPixelsX(fTextXInches);
		int nY = ToPixelsY(fTextYInches);

		Gdiplus::PointF ptOrigin((REAL)nX, (REAL)nY);

		// default text color
		Gdiplus::Color textColor(0, 0, 0);

		// per-cell text color override
		auto itText =
			m_mapCellTextColor.find({ CurrentRowIndex, nColumnIndex });

		if (itText != m_mapCellTextColor.end())
		{
			Gdiplus::Color overrideColor = itText->second;
			if (overrideColor.GetAlpha() != 0) // not transparent
				textColor = overrideColor;
		}

		SolidBrush brush(textColor);

		pGraphics->DrawString
		(
			csLine,
			csLine.GetLength(),
			&font,
			ptOrigin,
			&brush
		);
	}

	delete pGraphics;
} // DrawTableCell

  /////////////////////////////////////////////////////////////////////////////
// DrawTableRow
//
// Draws a full row using measured column widths.
// Handles striping, vertical alignment, wrapped text, and row height.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::DrawTableRow
(
	const std::vector<CString>& vecCells,
	const std::vector<double>& vecColumnWidthsInches,
	bool bHeader,
	int nRowIndex
)
{
	CurrentRowIndex = nRowIndex;

	double fLeftInches = MarginInches.X + ComputeIndentInches();

	// --- FIRST PASS: measure each cell height ---
	std::vector<double> vecHeights(vecCells.size(), 0.0);
	double fMaxHeightInches = 0.0;

	for (int col = 0; col < (int)vecCells.size(); ++col)
	{
		int nAlignment = 0;
		if (col < (int)m_vecColumnAlignment.size())
			nAlignment = m_vecColumnAlignment[col];

		int nVAlign = 0;
		if (col < (int)m_vecColumnVAlign.size())
			nVAlign = m_vecColumnVAlign[col];

		double fCellHeight = 0.0;

		// measure only — DrawTableCell computes height but does NOT advance Y
		DrawTableCell
		(
			vecCells[col],
			fLeftInches,
			vecColumnWidthsInches[col],
			bHeader,
			nAlignment,
			nVAlign,
			col,
			fCellHeight
		);

		vecHeights[col] = fCellHeight;

		if (fCellHeight > fMaxHeightInches)
			fMaxHeightInches = fCellHeight;

		fLeftInches += vecColumnWidthsInches[col];
	}

	// --- STRIPE BACKGROUND (GitHub-style) ---
	bool bStripe = (!bHeader && ((nRowIndex % 2) == 1));

	if (bStripe)
	{
		Graphics* pGraphics = GetGraphics();
		if (pGraphics)
		{
			SolidBrush stripeBrush(Color(0xFA, 0xFA, 0xFA));

			double fRowWidthInches = 0.0;
			for (double w : vecColumnWidthsInches)
				fRowWidthInches += w;

			Gdiplus::RectF rcStripeInches
			(
				(REAL)(MarginInches.X + ComputeIndentInches()),
				(REAL)YInches,
				(REAL)fRowWidthInches,
				(REAL)fMaxHeightInches
			);

			pGraphics->FillRectangle(&stripeBrush, ToPixelRect(rcStripeInches));
			delete pGraphics;
		}
	}

	// --- SECOND PASS: draw each cell at correct Y ---
	fLeftInches = MarginInches.X + ComputeIndentInches();

	for (int col = 0; col < (int)vecCells.size(); ++col)
	{
		int nAlignment = 0;
		if (col < (int)m_vecColumnAlignment.size())
			nAlignment = m_vecColumnAlignment[col];

		int nVAlign = 0;
		if (col < (int)m_vecColumnVAlign.size())
			nVAlign = m_vecColumnVAlign[col];

		double fCellHeight = 0.0;

		DrawTableCell
		(
			vecCells[col],
			fLeftInches,
			vecColumnWidthsInches[col],
			bHeader,
			nAlignment,
			nVAlign,
			col,
			fCellHeight
		);

		fLeftInches += vecColumnWidthsInches[col];
	}

	// --- advance Y ---
	YInches += fMaxHeightInches;
	NewLine();
} // DrawTableRow

/////////////////////////////////////////////////////////////////////////////
// Final table rendering pass
//
// Called after OnTableEnd() — draws all rows.
//
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::RenderTable()
{
	if (m_vecAllRows.empty())
		return;

	// measure columns
	MeasureTableColumns(m_vecAllRows, m_vecColumnWidthsInches);

	// draw header row (index 0)
	DrawTableRow(m_vecAllRows[0], m_vecColumnWidthsInches, true, 0);

	// draw body rows
	for (size_t i = 1; i < m_vecAllRows.size(); ++i)
	{
		DrawTableRow(m_vecAllRows[i], m_vecColumnWidthsInches, false, (int)i);
	}
} // RenderTable

/////////////////////////////////////////////////////////////////////////////
// OnTableColumnAlignment
//
// Records alignment for a given column.
// alignment: 0 = left, 1 = center, 2 = right
/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableColumnAlignment
(
	int nColumnIndex,
	int nAlignment
)
{
	if (nColumnIndex < 0)
	{
		return;
	}

	if ((int)m_vecColumnAlignment.size() <= nColumnIndex)
	{
		m_vecColumnAlignment.resize(nColumnIndex + 1, 0); // default left
	}

	if (nAlignment < 0) nAlignment = 0;
	if (nAlignment > 2) nAlignment = 2;

	m_vecColumnAlignment[nColumnIndex] = nAlignment;
} // OnTableColumnAlignment

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableColumnVerticalAlignment
(
	int nColumnIndex,
	int nVAlign
)
{
	if (nColumnIndex < 0)
		return;

	if ((int)m_vecColumnVAlign.size() <= nColumnIndex)
		m_vecColumnVAlign.resize(nColumnIndex + 1, 0); // default top

	if (nVAlign < 0) nVAlign = 0;
	if (nVAlign > 2) nVAlign = 2;

	m_vecColumnVAlign[nColumnIndex] = nVAlign;
} // OnTableColumnVerticalAlignment

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableColumnBackColor
(
	int nColumnIndex,
	Gdiplus::Color color
)
{
	if (nColumnIndex < 0)
		return;

	if ((int)m_vecColumnBackColor.size() <= nColumnIndex)
		m_vecColumnBackColor.resize(nColumnIndex + 1, Gdiplus::Color::Transparent);

	m_vecColumnBackColor[nColumnIndex] = color;
} // OnTableColumnBackColor

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellBackColor
(
	int nRowIndex,
	int nColumnIndex,
	Gdiplus::Color color
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	m_mapCellBackColor[{nRowIndex, nColumnIndex}] = color;
} // OnTableCellBackColor

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellTextColor
(
	int nRowIndex,
	int nColumnIndex,
	Gdiplus::Color color
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	m_mapCellTextColor[{nRowIndex, nColumnIndex}] = color;
} // OnTableCellTextColor

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellFontStyle
(
	int nRowIndex,
	int nColumnIndex,
	int nStyle
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	m_mapCellFontStyle[{nRowIndex, nColumnIndex}] = nStyle;
} // OnTableCellFontStyle

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellPadding
(
	int nRowIndex,
	int nColumnIndex,
	double left,
	double right,
	double top,
	double bottom
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellPadding pad;
	pad.Left = left;
	pad.Right = right;
	pad.Top = top;
	pad.Bottom = bottom;

	m_mapCellPadding[{nRowIndex, nColumnIndex}] = pad;
} // OnTableCellPadding

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellBorder
(
	int nRowIndex,
	int nColumnIndex,
	bool drawTop,
	bool drawBottom,
	bool drawLeft,
	bool drawRight,
	Gdiplus::Color color,
	float thickness
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellBorder b;
	b.DrawTop = drawTop;
	b.DrawBottom = drawBottom;
	b.DrawLeft = drawLeft;
	b.DrawRight = drawRight;
	b.Color = color;
	b.Thickness = thickness;

	m_mapCellBorder[{nRowIndex, nColumnIndex}] = b;
} // OnTableCellBorder

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellCornerRadius
(
	int nRowIndex,
	int nColumnIndex,
	float tl,
	float tr,
	float br,
	float bl
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellCornerRadius r;
	r.TL = tl;
	r.TR = tr;
	r.BR = br;
	r.BL = bl;

	m_mapCellCornerRadius[{nRowIndex, nColumnIndex}] = r;
} // OnTableCellCornerRadius

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellShadow
(
	int nRowIndex,
	int nColumnIndex,
	bool enabled,
	float offsetX,
	float offsetY,
	float blur,
	Gdiplus::Color color
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellShadow s;
	s.Enabled = enabled;
	s.OffsetX = offsetX;
	s.OffsetY = offsetY;
	s.Blur = blur;
	s.Color = color;

	m_mapCellShadow[{nRowIndex, nColumnIndex}] = s;
} // OnTableCellShadow

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellGradient
(
	int nRowIndex,
	int nColumnIndex,
	CellGradientType type,
	Gdiplus::Color startColor,
	Gdiplus::Color endColor,
	float angle,
	float radius
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellGradient g;
	g.Type = type;
	g.StartColor = startColor;
	g.EndColor = endColor;
	g.Angle = angle;
	g.Radius = radius;

	m_mapCellGradient[{nRowIndex, nColumnIndex}] = g;
} // OnTableCellGradient

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellImageBackground
(
	int nRowIndex,
	int nColumnIndex,
	CellImageMode mode,
	Gdiplus::Image* pImage,
	float opacity
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellImageBackground bg;
	bg.Mode = mode;
	bg.pImage = pImage;
	bg.Opacity = opacity;

	m_mapCellImageBackground[{nRowIndex, nColumnIndex}] = bg;
} // OnTableCellImageBackground

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellOverlay
(
	int nRowIndex,
	int nColumnIndex,
	CellOverlayMode mode,
	Gdiplus::Color color,
	float opacity,
	Gdiplus::Image* pImage,
	float imageOpacity,
	float glassOpacity
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellOverlay ov;
	ov.Mode = mode;
	ov.Color = color;
	ov.Opacity = opacity;
	ov.pImage = pImage;
	ov.ImageOpacity = imageOpacity;
	ov.GlassOpacity = glassOpacity;

	m_mapCellOverlay[{nRowIndex, nColumnIndex}] = ov;
} // OnTableCellOverlay

/////////////////////////////////////////////////////////////////////////////
void CMarkdownBitmapRenderer::OnTableCellBlend
(
	int nRowIndex,
	int nColumnIndex,
	CellBlendMode mode,
	Gdiplus::Color color,
	float opacity
)
{
	if (nRowIndex < 0 || nColumnIndex < 0)
		return;

	CellBlend b;
	b.Mode = mode;
	b.Color = color;
	b.Opacity = opacity;

	m_mapCellBlend[{nRowIndex, nColumnIndex}] = b;
} // OnTableCellBlend

/////////////////////////////////////////////////////////////////////////////
