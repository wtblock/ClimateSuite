/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Paragraph.h"
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MeasureString
//
// Measures the width and height of the given text using inch‑based layout.
// Returns true if measurement succeeded.
//
/////////////////////////////////////////////////////////////////////////////
void CParagraph::MeasureString
(
	const CString& csText,
	shared_ptr<Gdiplus::Font> pFont,
	double& outWidthInches,
	double& outHeightInches
)
{
	ASSERT(m_pGraphics != nullptr);

	// get DPI for conversion
	const REAL dpiX = m_pGraphics->GetDpiX();
	const REAL dpiY = m_pGraphics->GetDpiY();

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

	m_pGraphics->MeasureString
	(
		csText,
		csText.GetLength(),
		pFont.get(),
		rcLayout,
		&rcBounds
	);

	// convert measured bounds from pixels back to inches
	outWidthInches = rcBounds.Width / dpiX;
	outHeightInches = rcBounds.Height / dpiY;

} // MeasureString

/////////////////////////////////////////////////////////////////////////////
// ToPixelRect
//
// Converts an inch‑based rectangle into a pixel‑based rectangle.
//
/////////////////////////////////////////////////////////////////////////////
Gdiplus::RectF CParagraph::ToPixelRect
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
// DrawBlockQuoteBar
//
// Draws the vertical bar for blockquotes.
// Normalized GitHub-style alignment.
//
/////////////////////////////////////////////////////////////////////////////
void CParagraph::DrawBlockQuoteBar
(
	shared_ptr<Gdiplus::Font> pFont, 
	double fIndentInches, double dX, double dY
)
{
	ASSERT(m_pGraphics != nullptr);

	//
	// GitHub-style bar:
	//   - left edge = margin + 0.10"
	//   - width     = 0.04"
	//
	const double fBarLeftInches = dX + 0.10;
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

	MeasureString
	(
		L"A", pFont, fDummyWidthInches, fLineHeightInches
	);

	//
	// Convert inches to pixel rectangle
	//
	Gdiplus::RectF rcPixels = ToPixelRect
	(
		Gdiplus::RectF
		(
			(REAL)fIndentInches - fBarWidthInches,
			(REAL)dY,
			(REAL)fBarWidthInches,
			(REAL)fLineHeightInches * 1.1
		)
	);

	//
	// GitHub-style color
	//
	SolidBrush brush(Color::Silver);

	m_pGraphics->FillRectangle(&brush, rcPixels);

} // DrawBlockQuoteBar

/////////////////////////////////////////////////////////////////////////////
double CParagraph::ComputeLineHeightInches
(
	shared_ptr<Gdiplus::Font> pFont
)
{
	ASSERT(m_pGraphics != nullptr);

	// Physical pixel height of the font at this DPI
	REAL heightPixels = pFont->GetHeight(m_pGraphics);

	// Convert pixels → inches using the bitmap's DPI
	REAL dpiY = m_pGraphics->GetDpiY();

	return static_cast<double>(heightPixels) / static_cast<double>(dpiY);
} // ComputeLineHeightInches

/////////////////////////////////////////////////////////////////////////////
// by default, tokens can be phrases, but in order to draw the lines
// justified, the tokens need to be expanded into individual words
void CParagraph::ExpandTokens()
{
	// have we already done this?
	if (Expanded)
	{
		return;
	}

	// array of words
	CSmartArray<CParagraphToken> arrWords;

	long lToken = 0;
	// expand the phrase array into the word array
	for (auto& token : Items)
	{
		lToken++;
		// inline formatting means the previous token was not EOL
		if (token->InlineCode || token->Emphasis || token->Strong)
		{
			long lWords = arrWords.Count;
			if (lWords > 0)
			{
				shared_ptr<CParagraphToken> pToken = 
					arrWords.get(lWords - 1);
				pToken->EndOfLine = false;
			}
		}

		CString csPhrase = token->Text;
		int nStart = 0;
		CString csWord = csPhrase.Tokenize(L" ", nStart);
		while (!csWord.IsEmpty())
		{
			// inline formatting will create tokens of punchuation
			if 
			(
				csWord == L"," || csWord == L";" || 
				csWord == L":" || csWord == L"."
			)
			{
				// get the previous word and append the isolated
				// punchuation to that word
				long lWords = arrWords.Count;
				if (lWords > 0)
				{
					shared_ptr<CParagraphToken> pToken =
						arrWords.get(lWords - 1);
					CString csText = pToken->Text;
					csText += csWord;
					pToken->Text = csText;

					// get the next word if any
					csWord = csPhrase.Tokenize(L" ", nStart);
					continue;
				}
			}

			// create a copy of the current token
			shared_ptr<CParagraphToken> pNew = 
				make_shared<CParagraphToken>(token);

			// record the word and its size
			pNew->Text = csWord;

			// list marker tokens are a single word with a 
			// reserved length
			bool bListMarker = token->ListMarker;
			int nListDepth = token->ListDepth;
			double dMarkerLength = MarkerLength;

			// if it is not a list marker, calculate the length
			if (bListMarker)
			{
				if (nListDepth > 1)
				{
					dMarkerLength *= nListDepth;
				}
				pNew->Length = dMarkerLength;

				// only the first token is a list marker
				token->ListMarker = false;
			}
			else
			{
				pNew->Length = TextLength(csWord, token->Font);
			}

			// collect the word in our temporary array
			arrWords.append(pNew);

			// get the next word if any
			csWord = csPhrase.Tokenize(L" ", nStart);

			// mark the last token as the end of line
			if (csWord.IsEmpty())
			{
				bool bEOL = true;
				// inline formatting means the previous token was not EOL
				if (token->InlineCode || token->Emphasis || token->Strong)
				{
					bEOL = false;
				}
				pNew->EndOfLine = bEOL;
			}
		}
	}

	// copy the words to the token array
	m_arrTokens.clear();
	for (auto& token : arrWords.Items)
	{
		m_arrTokens.append(token);
	}

	// flag to indicate this has been done
	Expanded = true;

} // ExpandTokens

/////////////////////////////////////////////////////////////////////////////
void CParagraph::DrawRightAlignedMarker
(
	const CString& csMarker,
	shared_ptr<Gdiplus::Font> pFont,
	double fMarkerColumnWidthInches,
	double fColumnLeftInches,
	double fYInches
)
{
	ASSERT(m_pGraphics != nullptr);

	// Measure marker width
	double fMarkerWidthInches = TextLength( csMarker, pFont);

	// Compute left origin so marker is right-aligned
	double fLeftInches =
		fColumnLeftInches + (fMarkerColumnWidthInches - fMarkerWidthInches);

	// Convert to pixels
	int nX = ToPixelsX(fLeftInches);
	int nY = ToPixelsY(fYInches);

	PointF ptOrigin((REAL)nX, (REAL)nY);
	SolidBrush brush(Color::Black);

	m_pGraphics->DrawString
	(
		csMarker,
		csMarker.GetLength(),
		pFont.get(),
		ptOrigin,
		&brush
	);
} // DrawRightAlignedMarker

/////////////////////////////////////////////////////////////////////////////
// DrawInlineCodeBackground
//
// Draws a light background behind inline code text.
// Called from DrawWrappedText() before drawing the text.
//
/////////////////////////////////////////////////////////////////////////////
void CParagraph::DrawInlineCodeBackground
(
	const CString& text,
	shared_ptr<Gdiplus::Font> pFont,
	Color colorFG,
	Color colorBG,
	double dX,
	double dY
)
{
	// measure the text
	double fWidthInches = 0.0;
	double fHeightInches = 0.0;

	MeasureString(text, pFont, fWidthInches, fHeightInches);

	// padding around inline code
	const double fPadX = 0.05;
	const double fPadY = 0.02;

	double fLeftInches = dX /*+ fIndentInches*/ - fPadX;
	double fTopInches = dY - fPadY;

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
	m_pGraphics->FillRectangle(&brush, rcPixels);

	// border
	Pen pen(colorFG, 10.0f); // GitHub inline code border
	m_pGraphics->DrawRectangle(&pen, rcPixels);

} // DrawInlineCodeBackground

/////////////////////////////////////////////////////////////////////////////
CSmartArray<CParagraphToken> CParagraph::GetLine
(
	double dStart, 
	double dEnd, 
	double& dSpace
)
{
	// a half inch of padding as a minimum amount of space to be divided
	// up between tokens in case a collection of tokens is exactly the 
	// same size a the space available leaving no room for gaps between
	// tokens
	double dPad = 0.5;

	CSmartArray<CParagraphToken> value;
	double dLine = dStart;
	double dStop = dEnd - dPad;
	bool bEOL = false;

	int nTokens = 0;
	for (auto& token : Items)
	{
		if (nTokens == 0)
		{
			if (token->BlockQuote)
			{
				dLine += (token->BlockQuoteDepth * 0.4);
			}
			else if (!token->ListMarker)
			{
				if (token->ListItem)
				{
					dLine = ListIndent;
				}
			}
		}
		shared_ptr<Gdiplus::Font> pFont = token->Font;

		// get the length of the current word
		CString csText = token->Text;
		double dLen = token->Length;

		// if the word's length overflows the line length, we are done
		// with this line
		if (dLen + dLine > dStop)
		{
			break;
		}

		dLine += dLen;
		value.append(token);
		nTokens++;

		// if the token is the end of line, exit the loop and set
		// the space value zero
		bEOL = token->EndOfLine;
		if (bEOL)
		{
			dSpace = 0.0;
			break;
		}
	}

	// if not the end of line, calculate the space between words
	if (!bEOL)
	{
		// available space
		double dAvailable = dEnd - dStart;

		// padding is extra space for the line to fit into
		double dPadding = dAvailable - dLine;

		// space between words
		dSpace = dPadding / nTokens;
	}

	// remove the words that made up this line
	while (nTokens > 0)
	{
		m_arrTokens.remove(0);
		nTokens--;
	}

	return value;
} // GetLine

/////////////////////////////////////////////////////////////////////////////
// draws a line of the paragraph and returns true if there are more lines
// dStart is the beginning of the line in inches and dEnd is the end of
// the line in inches. dY is the Y coordinate in inches.
bool CParagraph::DrawLine
(
	Graphics* pGraphics, double dStart, double dEnd, double dY
)
{
	m_pGraphics = pGraphics;

	Dpi = pGraphics->GetDpiX();

	ExpandTokens();

	bool value = false;
	double dSpace = 0;
	CSmartArray<CParagraphToken> pLine = GetLine(dStart, dEnd, dSpace);

	// this is the last line if there are no more words
	value = Count > 0;

	double dX = dStart;
	int nToken = 0;
	bool bMarker = false;
	double dLength = 0;
	CString csText;
	for (auto& token : pLine.Items)
	{
		nToken++;
		dLength = token->Length;
		csText = token->Text;
		shared_ptr<Gdiplus::Font> pFont = token->Font;
		
		// last line?
		if (value == false && nToken == 1)
		{
			dSpace = TextLength(L" ", pFont) * 0.4;
		}

		if (token->Background)
		{
			DrawInlineCodeBackground
			(
				csText, pFont, token->ColorFG, token->ColorBG, dX, dY
			);
		}

		if (token->BlockQuote)
		{
			if (CHelper::NearlyEqual(dX, 0.0))
			{
				for (int n = 1; n <= token->BlockQuoteDepth; n++)
				{
					double dOffset = dX;
					dOffset += 0.4 * n;
					DrawBlockQuoteBar
					(
						token->Font, dOffset, dX, dY
					);
				}
			}
		}

		if (token->ListMarker && nToken == 1)
		{
			DrawRightAlignedMarker
			(
				csText,
				pFont,
				dLength,
				dX,
				dY
			);
			bMarker = true;
		}
		else if (token->ListItem && nToken == 1)
		{
			dX = ListIndent;
		}
		else if (token->BlockQuote && nToken == 1)
		{
			dX += (token->BlockQuoteDepth * 0.4);
		}

		const int nX = ToPixelsX(dX);
		const int nY = ToPixelsY(dY);

		// origin point in pixels
		Gdiplus::PointF ptOrigin((REAL)nX, (REAL)nY);

		SolidBrush brush(Color::Black);

		if (!bMarker)
		{
			pGraphics->DrawString
			(
				csText,
				csText.GetLength(),
				pFont.get(),
				ptOrigin,
				&brush
			);
		}

		bMarker = false;

		dX += dLength;
		dX += dSpace;

	}

	pLine.clear();

	if (value == false)
	{
		Expanded = false;
	}

	return value;
} // DrawLine

/////////////////////////////////////////////////////////////////////////////
// DrawImage
//
// Draws an image at the current position, scaled to fit content width.
// No spacing is applied here; spacing is handled by OnImage().
//
/////////////////////////////////////////////////////////////////////////////
void CParagraph::DrawImage
(
	Graphics* pGraphics, const CString& csPath
)
{
	m_pGraphics = pGraphics;

	// load image
	Image image(csPath);

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
	// Compute natural size in inches using image DPI
	//
	REAL imgDpiX = image.GetHorizontalResolution();
	REAL imgDpiY = image.GetVerticalResolution();

	double fNaturalWidthInches = (double)nNaturalWidth / imgDpiX;
	double fNaturalHeightInches = (double)nNaturalHeight / imgDpiY;

	//
	// Scale to fit content width
	//
	double fScaleWidth = 1.0;
	if (fNaturalWidthInches > fContentWidthInches)
	{
		fScaleWidth = fContentWidthInches / fNaturalWidthInches;
	}

	//
	// Scale to fit remaining page height
	//
	double pageBottomInches = MarginInches.Y + MarginInches.Height;
	double availableHeightInches = pageBottomInches - YInches;

	double fScaleHeight = 1.0;
	if (fNaturalHeightInches > availableHeightInches)
	{
		fScaleHeight = availableHeightInches / fNaturalHeightInches;
	}

	//
	// Final scale preserves aspect ratio
	//
	double fScale = min(fScaleWidth, fScaleHeight);

	//
	// Final draw size
	//
	double fDrawWidthInches = fNaturalWidthInches * fScale;
	double fDrawHeightInches = fNaturalHeightInches * fScale;

	//
	// convert to pixels
	//
	const int nDrawWidth = ToPixelsX(fDrawWidthInches);
	const int nDrawHeight = ToPixelsY(fDrawHeightInches);

	//
	// compute position with indentation
	//
	double fContentLeftInches = MarginInches.X;

	// compute centered X position
	double fCenteredXInches =
		fContentLeftInches + (fContentWidthInches - fDrawWidthInches) / 2;

	// apply indentation (blockquote, list depth)
	//fCenteredXInches += ComputeIndentInches();

	int nX = ToPixelsX(fCenteredXInches);
	int nY = ToPixelsY(YInches);

	//
	// draw
	//
	m_pGraphics->DrawImage
	(
		&image,
		nX,
		nY,
		nDrawWidth,
		nDrawHeight
	);

	//
	// advance vertical position
	//
	YInches += fDrawHeightInches;

} // DrawImage

/////////////////////////////////////////////////////////////////////////////

