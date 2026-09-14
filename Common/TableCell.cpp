/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "TableCell.h"

/////////////////////////////////////////////////////////////////////////////
// MeasureString
//
// Measures the width and height of the given text using inch‑based layout.
// If the given text is empty, the cell text is used instead.
//
/////////////////////////////////////////////////////////////////////////////
void CTableCell::MeasureString
(
	double& outWidthInches,
	double& outHeightInches,
	CString input/* = L""*/
)
{
	ASSERT(m_pGraphics != nullptr);

	CString csText;
	if (input.IsEmpty())
	{
		csText = Text;
	}
	else
	{
		csText = input;
	}

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
		CellFont.get(),
		rcLayout,
		&rcBounds
	);

	// convert measured bounds from pixels back to inches
	outWidthInches = rcBounds.Width / dpiX;
	outHeightInches = rcBounds.Height / dpiY;

} // MeasureString

/////////////////////////////////////////////////////////////////////////////
// array lines after text has been wrapped
vector<CString>* CTableCell::GetWrappedText()
{
	long lLines = Lines;
	if (lLines == 0)
	{
		// tokenized cell text
		vector<CELL_TOKEN>* pTokens = CellTokens;

		// width of the cell in inches
		double dWidth = Width;

		CString csLine;
		double dLine = 0;

		for (auto& token : m_arrTokens)
		{
			// if first token overlaps, we have to accept it
			// because are not splitting words
			if (csLine.IsEmpty())
			{
				csLine += token.first;
				csLine += L" ";
				dLine += token.second;
				if (dLine > dWidth)
				{
					csLine.TrimRight(L" ");
					m_arrLines.push_back(csLine);
					dLine = 0;
					csLine.Empty();
				}
			}
			else 
			{
				// if the current line length plust the token length
				// exceeds the space, add the line to our collection
				// and begin a new line
				if (dLine + token.second > dWidth)
				{
					csLine.TrimRight(L" ");
					m_arrLines.push_back(csLine);
					dLine = 0;
					csLine.Empty();
				}

				// increment the line length and add the token's text
				// to the current line
				dLine += token.second;
				csLine += token.first;
				csLine += L" ";
			}
		}

		// store the remaining line if any
		if (!csLine.IsEmpty())
		{
			csLine.TrimRight(L" ");
			m_arrLines.push_back(csLine);
		}
	}
	return &m_arrLines;
} // GetWrappedText

/////////////////////////////////////////////////////////////////////////////
double CTableCell::ComputeLineHeightInches
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
void CTableCell::Draw()
{
	Gdiplus::RectF rcInches(Left, Top, Width, Height);
	Gdiplus::RectF rcPixels = ToPixelRect(rcInches);

	// background
	bool bHeading = Heading;
	Color bg = bHeading ? ColorHeadingBG : ColorBG;
	SolidBrush brush(bg);
	m_pGraphics->FillRectangle(&brush, rcPixels);

	// body text of the table
	shared_ptr<Gdiplus::Font> pFont = CellFont;

	// create a heading font based on the body font
	int nStyle = pFont->GetStyle();
	Gdiplus::FontFamily ff;
	pFont->GetFamily(&ff);
	REAL fSize = pFont->GetSize();
	nStyle |= FontStyleBold;
	shared_ptr<Gdiplus::Font> pHeadingFont = make_shared<Gdiplus::Font>
		(&ff, fSize, nStyle, UnitPoint);

	double dLineHeight = ComputeLineHeightInches(pFont);

	long lLines = Lines;
	vector<CString>* pLines = WrappedText;

	double dPad = Pad / 2;

	// array of text lines
	double dTop = Top;
	for (auto& csLine : *pLines)
	{
		bool bHeading = Heading;

		// Convert to pixels
		int nX = ToPixelsX(Left + dPad);
		int nY = ToPixelsY(dTop);

		switch (Justify)
		{
		case eJustifyCenter:
		{
			nX = ToPixelsX(Left + (Width - Length) / 2);
			break;
		}
		case eJustifyRight:
		{
			// bold fonts for headings need to adjust starting point
			double dRightPad = bHeading ? dPad * 2 : dPad;
			nX = ToPixelsX(Left + Width - dRightPad - Length);
			break;
		}
		}

		PointF ptOrigin((REAL)nX, (REAL)nY);
		SolidBrush brush(Color::Black);

		m_pGraphics->DrawString
		(
			csLine,
			csLine.GetLength(),
			bHeading ? pHeadingFont.get() : pFont.get(),
			ptOrigin,
			&brush
		);

		dTop += dLineHeight;
	}

} // Draw

/////////////////////////////////////////////////////////////////////////////
