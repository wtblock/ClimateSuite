/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "TableMD.h"

/////////////////////////////////////////////////////////////////////////////
double CTableMD::ComputeLineHeightInches
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
