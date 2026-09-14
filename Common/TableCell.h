/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include "ImagePlus.h"
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CTableCell
// 
// A class to populate the cells of a CTableRow
/////////////////////////////////////////////////////////////////////////////
class CTableCell
{
// public types
public:
	// tokens when calculating cell length
	typedef pair<CString, double> CELL_TOKEN;

	// justification of the cell
	typedef enum tagJustify
	{
		eJustifyLeft,
		eJustifyCenter,
		eJustifyRight
	} JUSTIFY;

// protected data
protected:
	// text content of the cell
	CString m_csText;

	// heading cells are the first row
	bool m_bHeading;

	// justification of the cell
	JUSTIFY m_eJustify;

	// left position of the cell in inches
	double m_dLeft;

	// top position of the cell in inches
	double m_dTop;

	// width of the cell in inches
	double m_dWidth;

	// height of the cell in inches
	double m_dHeight;

	// length of the text in inches
	double m_dLength;

	// horizontal padding in inches
	double m_dPad;

	// dots per inch
	double m_dDpi;

	// width of the cell border in inches
	double m_dBorderWidth;

	// number of lines when wrapped
	long m_lLines;

	// font used for this cell
	shared_ptr<Gdiplus::Font> m_pFont;

	// the graphic to be rendered on
	Graphics* m_pGraphics;

	// foreground color
	Color m_colorFG; 

	// background color (inline code)
	Color m_colorBG; 

	// background color for heading row
	Color m_colorHeadingBG; 

	// vector of cell tokens to aid with length and
	// wrapping
	vector<CELL_TOKEN> m_arrTokens;

	// vector of wrapped lines
	vector<CString> m_arrLines;

// public properties
public:
	// text content of the cell
	CString GetText()
	{
		return m_csText;
	}

	// text content of the cell
	void SetText(CString value)
	{
		m_csText = value;
	}

	// text content of the cell
	__declspec(property(get = GetText, put = SetText))
		CString Text;

	// heading cells are the first row
	bool GetHeading()
	{
		return m_bHeading;
	}

	// heading cells are the first row
	void SetHeading(bool value)
	{
		m_bHeading = value;
	}

	// heading cells are the first row
	__declspec(property(get = GetHeading, put = SetHeading))
		bool Heading;

	// justification of the cell
	CTableCell::JUSTIFY GetJustify()
	{
		return m_eJustify;
	}

	// justification of the cell
	void SetJustify(CTableCell::JUSTIFY value)
	{
		m_eJustify = value;
	}

	// justification of the cell
	__declspec(property(get = GetJustify, put = SetJustify))
		CTableCell::JUSTIFY Justify;

	// array lines after text has been wrapped
	vector<CString>* GetWrappedText();
	// array lines after text has been wrapped
	__declspec(property(get = GetWrappedText))
		vector<CString>* WrappedText;

	// left position of the cell in inches
	double GetLeft()
	{
		return m_dLeft;
	}

	// left position of the cell in inches
	void SetLeft(double value)
	{
		m_dLeft = value;
	}

	// left position of the cell in inches
	__declspec(property(get = GetLeft, put = SetLeft))
		double Left;

	// top position of the cell in inches
	double GetTop()
	{
		return m_dTop;
	}

	// top position of the cell in inches
	void SetTop(double value)
	{
		m_dTop = value;
	}

	// top position of the cell in inches
	__declspec(property(get = GetTop, put = SetTop))
		double Top;

	// width of the cell in inches
	double GetWidth()
	{
		// if the current width is zero, use the string length
		// plus padding
		if (CHelper::NearlyEqual(m_dWidth, 0.0))
		{
			double dPad = Pad;
			m_dWidth = TextLength();
			m_dWidth += dPad;
		}
		return m_dWidth;
	}

	// width of the cell in inches
	void SetWidth(double value)
	{
		m_dWidth = value;
	}

	// width of the cell in inches
	__declspec(property(get = GetWidth, put = SetWidth))
		double Width;

	// height of the cell in inches
	double GetHeight()
	{
		if (CHelper::NearlyEqual(m_dHeight, 0.0))
		{
			ASSERT(m_pGraphics != nullptr);

			// Physical pixel height of the font at this DPI
			REAL heightPixels = CellFont->GetHeight(m_pGraphics);

			// Convert pixels → inches using the bitmap's DPI
			REAL dpiY = m_pGraphics->GetDpiY();

			Height =
				static_cast<double>(heightPixels) /
				static_cast<double>(dpiY);

			long lLines = Lines;
			if (lLines > 0)
			{ 
				m_dHeight *= lLines;
			}
		}
		return m_dHeight;
	}

	// height of the cell in inches
	void SetHeight(double value)
	{
		m_dHeight = value;
	}

	// height of the cell in inches
	__declspec(property(get = GetHeight, put = SetHeight))
		double Height;

	// length of the text in inches
	double GetLength()
	{
		if (CHelper::NearlyEqual(m_dLength, 0.0))
		{
			Length = TextLength();
		}
		return m_dLength;
	}

	// length of the text in inches
	void SetLength(double value)
	{
		m_dLength = value;
	}

	// length of the text in inches
	__declspec(property(get = GetLength, put = SetLength))
		double Length;

	// horizontal padding in inches
	double GetPad()
	{
		double value = m_dPad;
		if (CHelper::NearlyEqual(value, 0.0))
		{
			value = TextLength(L"A") / 2;
			Pad = value;
		}
		return m_dPad;
	}

	// horizontal padding in inches
	void SetPad(double value)
	{
		m_dPad = value;
	}

	// horizontal padding in inches
	__declspec(property(get = GetPad, put = SetPad))
		double Pad;

	// dots per inch
	double GetDpi()
	{
		Graphics* pGraphics = CellGraphics;
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
	}

	// background color for heading row
	__declspec(property(get = GetColorHeadingBG, put = SetColorHeadingBG))
		Color ColorHeadingBG;

	// number of lines when wrapped
	long GetLines()
	{
		Lines = (long)m_arrLines.size();
		return m_lLines;
	}

	// number of lines when wrapped
	void SetLines(long value)
	{
		m_lLines = value;
	}

	// number of lines when wrapped
	__declspec(property(get = GetLines, put = SetLines))
		long Lines;

	// font used for this cell
	shared_ptr<Gdiplus::Font> GetCellFont()
	{
		return m_pFont;
	}
	// font used for this cell
	void SetCellFont(shared_ptr<Gdiplus::Font> pFont)
	{
		m_pFont = pFont;
	}

	// font used for this cell
	__declspec(property(get = GetCellFont, put = SetCellFont))
		shared_ptr<Gdiplus::Font> CellFont;

	// the graphic to be rendered on
	Graphics* GetCellGraphics()
	{
		return m_pGraphics;
	}
	// the graphic to be rendered on
	void SetCellGraphics(Graphics* value)
	{
		m_pGraphics = value;
	}

	// the graphic to be rendered on
	__declspec(property(get = GetCellGraphics, put = SetCellGraphics))
		Graphics* CellGraphics;

	// vector of cell tokens to aid with length and
	// wrapping
	vector<CELL_TOKEN>* GetCellTokens()
	{
		int nTokens = (int)m_arrTokens.size();
		if (nTokens == 0)
		{
			CString csText = Text;
			int nStart = 0;
			CString csToken = csText.Tokenize(L" ", nStart);
			while (!csToken.IsEmpty())
			{
				// length of the token plus the space separator
				double dLen = TextLength(csToken + L" ");
				CELL_TOKEN ct;
				ct.first = csToken;
				ct.second = dLen;
				m_arrTokens.push_back(ct);
				csToken = csText.Tokenize(L" ", nStart);
			}
		}
		return &m_arrTokens;
	};
	// vector of cell tokens to aid with length and
	// wrapping
	__declspec(property(get = GetCellTokens))
		vector<CELL_TOKEN>* CellTokens;

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

	void MeasureString
	(
		double& outWidthInches,
		double& outHeightInches,
		CString input = L""
	);

	// get the length of cell or the given text in inches
	double TextLength( CString input = L"")
	{
		double value = 0;
		double dHeight = 0;
		MeasureString(value, dHeight,input);
		return value;
	}

	// get the line height of cell or the given text in inches
	double LineHeight( CString input = L"")
	{
		double value = 0;
		double dWidth = 0;
		MeasureString(dWidth, value, input);
		return value;
	}

	double ComputeLineHeightInches(shared_ptr<Gdiplus::Font> pFont);

// public methods
public:
	// draw the cell
	void Draw();

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CTableCell()
	{
		Justify = eJustifyLeft;
		Left = 0;
		Heading = false;
		Top = 0;
		Width = 0;
		Height = 0;
		Length = 0;
		BorderWidth = 10.0; // pixels
		Pad = 0;
		Dpi = 0;
		Lines = 0;
		CellFont = nullptr;
		CellGraphics = nullptr;
		ColorBG = Color::White;
		ColorHeadingBG = Color::Silver;
		ColorFG = Color::Black;
	}
	~CTableCell()
	{

	}
}; // class CTableCell

/////////////////////////////////////////////////////////////////////////////


