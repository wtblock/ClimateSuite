/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TableRow.h"

/////////////////////////////////////////////////////////////////////////////
// CTableMD
// 
// A class to support tables rendered by markdown.
/////////////////////////////////////////////////////////////////////////////
class CTableMD
{
// public types
public:

// protected data
protected:
	// collection of table rows that make up a table
	CSmartArray<CTableRow> m_arrRows;

	// font used for this cell
	shared_ptr<Gdiplus::Font> m_pFont;

	// the graphic to be rendered on
	Graphics* m_pGraphics;

	// left position of the row in inches
	double m_dLeft;

	// top position of the row in inches
	double m_dTop;

	// width of the row in inches
	double m_dWidth;

	// height of the row in inches
	double m_dHeight;

	// dots per inch
	double m_dDpi;

	// width of the cell border in inches
	double m_dBorderWidth;

	// foreground color
	Color m_colorFG; 

	// background color
	Color m_colorBG; 

	// background color for heading row
	Color m_colorHeadingBG; 

// public properties
public:
	// number of rows in the table
	long GetCount()
	{
		long value = m_arrRows.Count;
		return value;
	}
	// number of rows in the table
	__declspec(property(get = GetCount))
		long Count;

	// number of rows in the table
	long GetRows()
	{
		long value = Count;
		return value;
	}
	// number of rows in the table
	__declspec(property(get = GetRows))
		long Rows;

	// number of columns in the table
	long GetColumns()
	{
		long value = 0;
		long lRows = Rows;
		if (lRows > 0)
		{
			shared_ptr<CTableRow> pRow = Row[0];
			value = pRow->Count;
		}
		return value;
	}
	// number of columns in the table
	__declspec(property(get = GetColumns))
		long Columns;

	// get a row by index
	shared_ptr<CTableRow> GetRow(long lRow)
	{
		long lRows = Count;

		shared_ptr<CTableRow> value;
		if (0 <= lRow && lRow < lRows)
		{
			value = m_arrRows.get(lRow);
		}
		return value;
	}
	// get a cell by index
	__declspec(property(get = GetRow))
		shared_ptr<CTableRow> Row[];

	// font used for this table
	shared_ptr<Gdiplus::Font> GetTableFont()
	{
		return m_pFont;
	}

	// font used for this table
	void SetTableFont(shared_ptr<Gdiplus::Font> value)
	{
		m_pFont = value;
		for (auto& row : m_arrRows.Items)
		{
			row->RowFont = value;
		}
	}

	// font used for this table
	__declspec(property(put = SetTableFont))
		shared_ptr<Gdiplus::Font> TableFont;

	// the graphic to render on
	Graphics* GetTableGraphics()
	{
		return m_pGraphics;
	}

	// the graphic to render on
	void SetTableGraphics(Graphics* value)
	{
		m_pGraphics = value;
		for (auto& row : m_arrRows.Items)
		{
			row->RowGraphics = value;
		}
	}

	// the graphic to render on
	__declspec(property(get = GetTableGraphics, put = SetTableGraphics))
		Graphics* TableGraphics;

	// the maximum cell width in inches
	double GetMaxWidth()
	{
		double value = 0;
		for (auto& pRow : m_arrRows.Items)
		{
			double dMax = pRow->MaxWidth;
			if (dMax > value)
			{
				value = dMax;
			}
		}
		return value;
	}
	// the maximum cell width in inches
	__declspec(property(get = GetMaxWidth))
		double MaxWidth;

	// the maximum cell text height in inches
	double GetMaxHeight()
	{
		double value = 0;
		for (auto& pRow : m_arrRows.Items)
		{
			double dMax = pRow->MaxHeight;
			if (dMax > value)
			{
				value = dMax;
			}
		}
		return value;
	}
	// the maximum cell text height in inches
	__declspec(property(get = GetMaxHeight))
		double MaxHeight;

	// left position of the table in inches
	double GetLeft()
	{
		return m_dLeft;
	}

	// left position of the table in inches
	void SetLeft(double value)
	{
		m_dLeft = value;
		double dMax = MaxWidth;
		for (auto& pRow : m_arrRows.Items)
		{
			pRow->MaxWidth = dMax;
			pRow->Left = value;
		}
	}

	// left position of the table in inches
	__declspec(property(get = GetLeft, put = SetLeft))
		double Left;

	// top position of the table in inches
	double GetTop()
	{
		return m_dTop;
	}

	// top position of the table in inches
	void SetTop(double value)
	{
		m_dTop = value;
		double dRow = value;
		double dMax = MaxHeight;
		for (auto& pRow : m_arrRows.Items)
		{
			pRow->MaxHeight = MaxHeight;
			pRow->Top = dRow;
			dRow += dMax;
		}
	}

	// top position of the table in inches
	__declspec(property(get = GetTop, put = SetTop))
		double Top;

	// width of the table in inches
	double GetWidth()
	{
		if (CHelper::NearlyEqual(m_dWidth, 0.0))
		{
			long lColumns = Columns;
			double dMaxWidth = MaxWidth;
			Width = dMaxWidth * lColumns;
		}
		return m_dWidth;
	}

	// width of the table in inches
	void SetWidth(double value)
	{
		m_dWidth = value;
	}

	// width of the table in inches
	__declspec(property(get = GetWidth, put = SetWidth))
		double Width;

	// height of the table in inches
	double GetHeight()
	{
		if (CHelper::NearlyEqual(m_dHeight, 0.0))
		{
			long lRows = Rows;
			double dMaxHeight = MaxHeight;
			Height = dMaxHeight * lRows;
		}
		return m_dHeight;
	}

	// height of the table in inches
	void SetHeight(double value)
	{
		m_dHeight = value;
	}

	// height of the table in inches
	__declspec(property(get = GetHeight, put = SetHeight))
		double Height;

	// dots per inch
	double GetDpi()
	{
		Graphics* pGraphics = TableGraphics;
		if (m_dDpi == 0 && pGraphics != nullptr)
		{
			Dpi = pGraphics->GetDpiX();
		}
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

	// width of the cell border in inches
	double GetBorderWidth()
	{
		return m_dBorderWidth;
	}

	// width of the cell border in inches
	void SetBorderWidth(double value)
	{
		m_dBorderWidth = value;
	}

	// width of the cell border in inches
	__declspec(property(get = GetBorderWidth, put = SetBorderWidth))
		double BorderWidth;

	// Foreground Color
	Color GetColorFG(void) const
	{
		return m_colorFG;
	}

	// Foreground Color
	void SetColorFG(const Color& color)
	{
		m_colorFG = color;
		for (auto& pRow : m_arrRows.Items)
		{
			pRow->ColorFG = color;
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
		for (auto& pRow : m_arrRows.Items)
		{
			pRow->ColorBG = color;
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
		for (auto& pRow : m_arrRows.Items)
		{
			pRow->ColorHeadingBG = color;
		}
	}

	// background color for heading row
	__declspec(property(get = GetColorHeadingBG, put = SetColorHeadingBG))
		Color ColorHeadingBG;

// protected methods
protected:
	int ToPixelsX(double inches)
	{
		return (int)(inches * Dpi);
	}

	int ToPixelsY(double inches)
	{
		return (int)(inches * Dpi);
	}

	// rectangle in inches to rectangle in pixels
	Gdiplus::RectF ToPixelRect
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
	}

	double ComputeLineHeightInches(shared_ptr<Gdiplus::Font> pFont);

// public methods
public:
	// add a row to the table and return its index
	long Add()
	{
		long value = m_arrRows.add();
		if (value == 0)
		{
			shared_ptr<CTableRow> pRow = Row[value];
			pRow->Heading = true;
		}
		return value;
	}

	// draw the table
	void Draw()
	{
		for (auto& row : m_arrRows.Items)
		{
			row->Draw();
		}

		Gdiplus::RectF rcInches(Left, Top, Width, Height);
		Gdiplus::RectF rcPixels = ToPixelRect(rcInches);

		// border
		Pen pen(ColorFG, BorderWidth * 3);
		m_pGraphics->DrawRectangle(&pen, rcPixels);
	}

	// clear the table
	void Clear()
	{
		m_arrRows.clear();
	}

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CTableMD()
	{
		TableGraphics = nullptr;
		TableFont = nullptr;
		Left = 0;
		Top = 0;
		Width = 0;
		Height = 0;
		Dpi = 0;
		BorderWidth = 10.0; // pixels
		ColorBG = Color::White;
		ColorHeadingBG = Color::Silver;
		ColorFG = Color::Black;
	}
	~CTableMD()
	{

	}
}; // class CTableMD

/////////////////////////////////////////////////////////////////////////////

