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
bool CTableMD::HeadingSeparator(CString csRow)
{
	// test to see if the heading separator has already been parsed
	if (m_arrJustify.size() != 0)
	{
		return false;
	}

	bool value = true;
	csRow.Trim(L"| ");

	int nStart = 0;
	CString csCell = csRow.Tokenize(L"|", nStart);
	while (!csCell.IsEmpty())
	{	
		CTableCell::JUSTIFY eJustify = CTableCell::eJustifyLeft;
		csCell.Trim(L" ");
		if (csCell.IsEmpty())
		{
			break;
		}
		CString csLeft = csCell.Left(1);
		CString csRight = csCell.Right(1);
		if (csLeft == L":")
		{
			if (csRight == L":")
			{
				eJustify = CTableCell::eJustifyCenter;
			}
			else if ( csRight == L"-")
			{
				eJustify = CTableCell::eJustifyLeft;
			}
			else
			{
				value = false;
				break;
			}
		}
		else if (csRight == L":")
		{
			if (csLeft == L"-")
			{
				eJustify = CTableCell::eJustifyRight;
			}
			else
			{
				value = false;
				break;
			}
		}
		else if (csLeft == L"-" && csRight == L"-")
		{
			eJustify = CTableCell::eJustifyLeft;
		}
		else 
		{
			value = false;
			break;
		}

		// adds a justification enumeration to the column array
		AddJustify(eJustify);

		// next column cell
		csCell = csRow.Tokenize(L"|", nStart);
	}

	// if any blocks fail the syntax, remove justification content
	if (value == false)
	{
		m_arrJustify.clear();
	}

	return value;
} // HeadingSeparator
/////////////////////////////////////////////////////////////////////////////
