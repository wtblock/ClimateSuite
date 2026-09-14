/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TableCell.h"
#include "SmartArray.h"

/////////////////////////////////////////////////////////////////////////////
// CTableRow
// 
// A class to contain the CTableCell objects making up rows of CTableMD
/////////////////////////////////////////////////////////////////////////////
class CTableRow
{
// public types
public:

// protected data
protected:
	// collection of table cells that make up a row of the table
	CSmartArray<CTableCell> m_arrCells;

	// font used for this cell
	shared_ptr<Gdiplus::Font> m_pFont;

	// the graphic to be rendered on
	Graphics* m_pGraphics;

	// maximum cell width in inches
	double m_dMaxWidth;

	// maximum cell height in inches
	double m_dMaxHeight;

	// left position of the row in inches
	double m_dLeft;

	// top position of the row in inches
	double m_dTop;

	// width of the row in inches
	double m_dWidth;

	// height of the row in inches
	double m_dHeight;

	// heading cells are the first row
	bool m_bHeading;

	// foreground color
	Color m_colorFG; 

	// background color (inline code)
	Color m_colorBG; 

	// background color for heading row
	Color m_colorHeadingBG; 

// public properties
public:
	// number of cells in the row
	long GetCount()
	{
		long value = m_arrCells.Count;
		return value;
	}
	// number of cells in the row
	__declspec(property(get = GetCount))
		long Count;

	// get a cell by index
	shared_ptr<CTableCell> GetCell(long lCell)
	{
		long lCells = Count;

		shared_ptr<CTableCell> value;
		if (0 <= lCell && lCell < lCells)
		{
			value = m_arrCells.get(lCell);
		}
		return value;
	}
	// get a cell by index
	__declspec(property(get = GetCell))
		shared_ptr<CTableCell> Cell[];

	// the maximum cell text length in inches
	double GetMaxLength()
	{
		double value = 0;
		for (auto& cell : m_arrCells.Items)
		{
			if (cell->Length > value)
			{
				value = cell->Length;
			}
		}
		return value;
	}
	// the maximum cell text length in inches
	__declspec(property(get = GetMaxLength))
		double MaxLength;

	// the maximum cell width in inches
	double GetMaxWidth()
	{
		if (CHelper::NearlyEqual(m_dMaxWidth, 0.0))
		{
			double value = 0;
			for (auto& cell : m_arrCells.Items)
			{
				if (cell->Width > value)
				{
					value = cell->Width;
				}
			}
			MaxWidth = value;
		}
		return m_dMaxWidth;
	}
	// the maximum cell width in inches
	void SetMaxWidth(double value)
	{
		m_dMaxWidth = value;
	}
	// the maximum cell width in inches
	__declspec(property(get = GetMaxWidth, put = SetMaxWidth))
		double MaxWidth;

	// the maximum cell text height in inches
	double GetMaxHeight()
	{
		if (CHelper::NearlyEqual(m_dMaxHeight, 0.0))
		{
			double value = 0;
			for (auto& cell : m_arrCells.Items)
			{
				if (cell->Height > value)
				{
					value = cell->Height;
				}
			}
			MaxHeight = value;
		}
		return m_dMaxHeight;
	}
	// the maximum cell text height in inches
	void SetMaxHeight(double value)
	{
		m_dMaxHeight = value;
	}
	// the maximum cell text height in inches
	__declspec(property(get = GetMaxHeight, put = SetMaxHeight))
		double MaxHeight;

	// font used for this cell
	void SetRowFont(shared_ptr<Gdiplus::Font> value)
	{
		m_pFont = value;
		for (auto& cell : m_arrCells.Items)
		{
			cell->CellFont = value;
		}
	}

	// font used for this cell
	__declspec(property(put = SetRowFont))
		shared_ptr<Gdiplus::Font> RowFont;

	// the graphic to render on
	void SetRowGraphics(Graphics* value)
	{
		m_pGraphics = value;
		for (auto& cell : m_arrCells.Items)
		{
			cell->CellGraphics = value;
		}
	}

	// the graphic to render on
	__declspec(property(put = SetRowGraphics))
		Graphics* RowGraphics;

	// left position of the row in inches
	double GetLeft()
	{
		return m_dLeft;
	}

	// left position of the row in inches
	void SetLeft(double value)
	{
		m_dLeft = value;
		double dMax = MaxWidth;
		double dCell = value;
		for (auto& pCell : m_arrCells.Items)
		{
			pCell->Left = dCell;
			pCell->Width = dMax;
			dCell += dMax;
		}
	}

	// left position of the row in inches
	__declspec(property(get = GetLeft, put = SetLeft))
		double Left;

	// top position of the row in inches
	double GetTop()
	{
		return m_dTop;
	}

	// top position of the row in inches
	void SetTop(double value)
	{
		m_dTop = value;
		double dMax = MaxHeight;
		double dCell = value;
		for (auto& pCell : m_arrCells.Items)
		{
			pCell->Top = dCell;
			pCell->Height = dMax;
		}
	}

	// top position of the row in inches
	__declspec(property(get = GetTop, put = SetTop))
		double Top;

	// width of the row in inches
	double GetWidth()
	{
		if (CHelper::NearlyEqual(m_dWidth, 0.0))
		{
			double dWidth = 0;
			for (auto& pCell : m_arrCells.Items)
			{
				dWidth += pCell->Width;
			}
			Width = dWidth;
		}
		return m_dWidth;
	}

	// width of the row in inches
	void SetWidth(double value)
	{
		m_dWidth = value;
	}

	// width of the row in inches
	__declspec(property(get = GetWidth, put = SetWidth))
		double Width;

	// height of the row in inches
	double GetHeight()
	{
		if (CHelper::NearlyEqual(m_dHeight, 0.0))
		{
			Height = MaxHeight;
		}
		return m_dHeight;
	}

	// height of the row in inches
	void SetHeight(double value)
	{
		m_dHeight = value;
	}

	// height of the row in inches
	__declspec(property(get = GetHeight, put = SetHeight))
		double Height;

	// heading cells are the first rows before heading separator
	bool GetHeading()
	{
		bool value = false;
		long lCount = Count;
		if (lCount > 0)
		{
			shared_ptr<CTableCell> pCell = Cell[0];
			value = pCell->Heading;
			Heading = value;
		}
		return m_bHeading;
	}

	// heading cells are the first rows before heading separator
	void SetHeading(bool value)
	{
		m_bHeading = value;
	}

	// heading cells are the first rows before heading separator
	__declspec(property(get = GetHeading, put = SetHeading))
		bool Heading;

	// Foreground Color
	Color GetColorFG(void) const
	{
		return m_colorFG;
	}

	// Foreground Color
	void SetColorFG(const Color& color)
	{
		m_colorFG = color;
		for (auto& pCell : m_arrCells.Items)
		{
			pCell->ColorFG = color;
		}
	}

	// Foreground Color
	__declspec(property(get = GetColorFG, put = SetColorFG))
		Color ColorFG;

	// Background Color
	Color GetColorBG(void) const
	{
		return m_colorBG;
	}

	// Background Color
	void SetColorBG(const Color& color)
	{
		m_colorBG = color;
		for (auto& pCell : m_arrCells.Items)
		{
			pCell->ColorBG = color;
		}
	}

	// Background Color
	__declspec(property(get = GetColorBG, put = SetColorBG))
		Color ColorBG;

	// background color for heading row
	Color GetColorHeadingBG(void) const
	{
		return m_colorHeadingBG;
	}

	// background color for heading row
	void SetColorHeadingBG(const Color& color)
	{
		m_colorHeadingBG = color;
		for (auto& pCell : m_arrCells.Items)
		{
			pCell->ColorHeadingBG = color;
		}
	}

	// background color for heading row
	__declspec(property(get = GetColorHeadingBG, put = SetColorHeadingBG))
		Color ColorHeadingBG;

	// justification of the cell
	CTableCell::JUSTIFY GetJustify(long lCell)
	{
		CTableCell::JUSTIFY value = CTableCell::eJustifyLeft;
		long lColumns = Count;
		if (0 <= lCell && lCell < lColumns)
		{
			value = Cell[lCell]->Justify;
		}
		return value;
	}

	// justification of the cell
	void SetJustify(long lCell, CTableCell::JUSTIFY value)
	{
		long lColumns = Count;
		if (0 <= lCell && lCell < lColumns)
		{
			Cell[lCell]->Justify = value;
		}
	}

	// justification of the cell
	__declspec(property(get = GetJustify, put = SetJustify))
		CTableCell::JUSTIFY Justify[];


// protected methods
protected:

// public methods
public:
	// add a cell to the row and return its index
	long Add()
	{
		long value = m_arrCells.add();
		bool bHeading = Heading;
		if (bHeading)
		{
			shared_ptr<CTableCell> pCell = Cell[value];
			pCell->Heading = true;
		}
		return value;
	}

	// draw the row
	void Draw()
	{
		for (auto& cell : m_arrCells.Items)
		{
			cell->Draw();
		}
	}

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CTableRow()
	{
		RowFont = nullptr;
		RowGraphics = nullptr;
		MaxHeight = 0;
		MaxWidth = 0;
		Left = 0;
		Top = 0;
		Width = 0;
		Height = 0;
		Heading = false;
		ColorBG = Color::White;
		ColorHeadingBG = Color::Silver;
		ColorFG = Color::Black;
	}
	~CTableRow()
	{

	}
}; // class CTableRow

/////////////////////////////////////////////////////////////////////////////

