/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <vector>

using namespace Gdiplus;

class CGraphPlotter
{
// protected data
protected:
	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	COLORREF            m_rgbLineColor;
	Gdiplus::DashStyle  m_dsLineStyle;
	double              m_fLineThicknessInches;

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	BOOL                m_bTrendLine;
	COLORREF            m_rgbTrendLineColor;
	Gdiplus::DashStyle  m_dsTrendLineStyle;
	double              m_fTrendLineThicknessInches;

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	COLORREF            m_rgbGridColor;
	Gdiplus::DashStyle  m_dsGridLineStyle;
	double              m_fGridLineThicknessInches;

	// -------------------------------------------------------------
	// Text sizes (points)
	// -------------------------------------------------------------
	double              m_fTitleFontSizePoints;
	double              m_fAxisLabelFontSizePoints;
	double              m_fTickLabelFontSizePoints;

	// -------------------------------------------------------------
	// Padding (inches)
	// -------------------------------------------------------------
	double              m_fLeftPaddingInches;
	double              m_fRightPaddingInches;
	double              m_fTopPaddingInches;
	double              m_fBottomPaddingInches;

	// -------------------------------------------------------------
	// Tick mark length (inches)
	// -------------------------------------------------------------
	double           m_fTickLengthInches;

	std::vector<double> m_arrYears;
	std::vector<double> m_arrValues;

// public properties
public:
	std::vector<double>& GetYears()
	{
		return m_arrYears;
	}

	void SetYears(std::vector<double> value)
	{
		m_arrYears = value;
	}

	__declspec(property(get = GetYears, put = SetYears))
		std::vector<double> Years;

	std::vector<double>& GetValues()
	{
		return m_arrValues;
	}

	void SetValues(std::vector<double> value)
	{
		m_arrValues = value;
	}

	__declspec(property(get = GetValues, put = SetValues))
		std::vector<double> Values;

	/////////////////////////////////////////////////////////////////////////////
	// Graphing Appearance Properties
	/////////////////////////////////////////////////////////////////////////////

	// -------------------------------------------------------------
	// LineColor
	// -------------------------------------------------------------
	COLORREF GetLineColor()
	{
		return m_rgbLineColor;
	}

	void SetLineColor(COLORREF value)
	{
		m_rgbLineColor = value;
	}

	__declspec(property(get = GetLineColor, put = SetLineColor))
		COLORREF LineColor;

	// -------------------------------------------------------------
	// LineStyle
	// -------------------------------------------------------------
	Gdiplus::DashStyle GetLineStyle()
	{
		return m_dsLineStyle;
	}

	void SetLineStyle(Gdiplus::DashStyle value)
	{
		m_dsLineStyle = value;
	}

	__declspec(property(get = GetLineStyle, put = SetLineStyle))
		Gdiplus::DashStyle LineStyle;

	// -------------------------------------------------------------
	// LineThicknessInches
	// -------------------------------------------------------------
	double GetLineThicknessInches()
	{
		return m_fLineThicknessInches;
	}

	void SetLineThicknessInches(double value)
	{
		m_fLineThicknessInches = value;
	}

	__declspec(property(get = GetLineThicknessInches, put = SetLineThicknessInches))
		double LineThicknessInches;

	// -------------------------------------------------------------
	// TrendLine
	// -------------------------------------------------------------
	BOOL GetTrendLine()
	{
		return m_bTrendLine;
	}

	void SetTrendLine(BOOL value)
	{
		m_bTrendLine = value;
	}

	__declspec(property(get = GetTrendLine, put = SetTrendLine))
		BOOL TrendLine;

	// -------------------------------------------------------------
	// TrendLineColor
	// -------------------------------------------------------------
	COLORREF GetTrendLineColor()
	{
		return m_rgbTrendLineColor;
	}

	void SetTrendLineColor(COLORREF value)
	{
		m_rgbTrendLineColor = value;
	}

	__declspec(property(get = GetTrendLineColor, put = SetTrendLineColor))
		COLORREF TrendLineColor;

	// -------------------------------------------------------------
	// TrendLineStyle
	// -------------------------------------------------------------
	Gdiplus::DashStyle GetTrendLineStyle()
	{
		return m_dsTrendLineStyle;
	}

	void SetTrendLineStyle(Gdiplus::DashStyle value)
	{
		m_dsTrendLineStyle = value;
	}

	__declspec(property(get = GetTrendLineStyle, put = SetTrendLineStyle))
		Gdiplus::DashStyle TrendLineStyle;

	// -------------------------------------------------------------
	// TrendLineThicknessInches
	// -------------------------------------------------------------
	double GetTrendLineThicknessInches()
	{
		return m_fTrendLineThicknessInches;
	}

	void SetTrendLineThicknessInches(double value)
	{
		m_fTrendLineThicknessInches = value;
	}

	__declspec(property(get = GetTrendLineThicknessInches, put = SetTrendLineThicknessInches))
		double TrendLineThicknessInches;

	// -------------------------------------------------------------
	// GridColor
	// -------------------------------------------------------------
	COLORREF GetGridColor()
	{
		return m_rgbGridColor;
	}

	void SetGridColor(COLORREF value)
	{
		m_rgbGridColor = value;
	}

	__declspec(property(get = GetGridColor, put = SetGridColor))
		COLORREF GridColor;

	// -------------------------------------------------------------
	// GridLineStyle
	// -------------------------------------------------------------
	Gdiplus::DashStyle GetGridLineStyle()
	{
		return m_dsGridLineStyle;
	}

	void SetGridLineStyle(Gdiplus::DashStyle value)
	{
		m_dsGridLineStyle = value;
	}

	__declspec(property(get = GetGridLineStyle, put = SetGridLineStyle))
		Gdiplus::DashStyle GridLineStyle;

	// -------------------------------------------------------------
	// GridLineThicknessInches
	// -------------------------------------------------------------
	double GetGridLineThicknessInches()
	{
		return m_fGridLineThicknessInches;
	}

	void SetGridLineThicknessInches(double value)
	{
		m_fGridLineThicknessInches = value;
	}

	__declspec(property(get = GetGridLineThicknessInches, put = SetGridLineThicknessInches))
		double GridLineThicknessInches;

	// -------------------------------------------------------------
	// TitleFontSizePoints
	// -------------------------------------------------------------
	double GetTitleFontSizePoints()
	{
		return m_fTitleFontSizePoints;
	}

	void SetTitleFontSizePoints(double value)
	{
		m_fTitleFontSizePoints = value;
	}

	__declspec(property(get = GetTitleFontSizePoints, put = SetTitleFontSizePoints))
		double TitleFontSizePoints;

	// -------------------------------------------------------------
	// AxisLabelFontSizePoints
	// -------------------------------------------------------------
	double GetAxisLabelFontSizePoints()
	{
		return m_fAxisLabelFontSizePoints;
	}

	void SetAxisLabelFontSizePoints(double value)
	{
		m_fAxisLabelFontSizePoints = value;
	}

	__declspec(property(get = GetAxisLabelFontSizePoints, put = SetAxisLabelFontSizePoints))
		double AxisLabelFontSizePoints;

	// -------------------------------------------------------------
	// TickLabelFontSizePoints
	// -------------------------------------------------------------
	double GetTickLabelFontSizePoints()
	{
		return m_fTickLabelFontSizePoints;
	}

	void SetTickLabelFontSizePoints(double value)
	{
		m_fTickLabelFontSizePoints = value;
	}

	__declspec(property(get = GetTickLabelFontSizePoints, put = SetTickLabelFontSizePoints))
		double TickLabelFontSizePoints;

	// -------------------------------------------------------------
	// LeftPaddingInches
	// -------------------------------------------------------------
	double GetLeftPaddingInches()
	{
		return m_fLeftPaddingInches;
	}

	void SetLeftPaddingInches(double value)
	{
		m_fLeftPaddingInches = value;
	}

	__declspec(property(get = GetLeftPaddingInches, put = SetLeftPaddingInches))
		double LeftPaddingInches;

	// -------------------------------------------------------------
	// RightPaddingInches
	// -------------------------------------------------------------
	double GetRightPaddingInches()
	{
		return m_fRightPaddingInches;
	}

	void SetRightPaddingInches(double value)
	{
		m_fRightPaddingInches = value;
	}

	__declspec(property(get = GetRightPaddingInches, put = SetRightPaddingInches))
		double RightPaddingInches;

	// -------------------------------------------------------------
	// TopPaddingInches
	// -------------------------------------------------------------
	double GetTopPaddingInches()
	{
		return m_fTopPaddingInches;
	}

	void SetTopPaddingInches(double value)
	{
		m_fTopPaddingInches = value;
	}

	__declspec(property(get = GetTopPaddingInches, put = SetTopPaddingInches))
		double TopPaddingInches;

	// -------------------------------------------------------------
	// BottomPaddingInches
	// -------------------------------------------------------------
	double GetBottomPaddingInches()
	{
		return m_fBottomPaddingInches;
	}

	void SetBottomPaddingInches(double value)
	{
		m_fBottomPaddingInches = value;
	}

	__declspec(property(get = GetBottomPaddingInches, put = SetBottomPaddingInches))
		double BottomPaddingInches;

	// -------------------------------------------------------------
	// TickLengthInches
	// -------------------------------------------------------------
	double GetTickLengthInches()
	{
		return m_fTickLengthInches;
	}

	void SetTickLengthInches(double value)
	{
		m_fTickLengthInches = value;
	}

	__declspec(property(get = GetTickLengthInches, put = SetTickLengthInches))
		double TickLengthInches;

// protected methods
protected:
	// -------------------------------------------------------------
	// Initialize default properties
	// -------------------------------------------------------------
	void SetDefaults();

	// -------------------------------------------------------------
	// RenderPlot
	// -------------------------------------------------------------
	std::unique_ptr<Gdiplus::Bitmap> RenderPlot(const CRect& rcPixels);

	// -------------------------------------------------------------
	// GenerateTicks
	// -------------------------------------------------------------
	void GenerateTicks
	(
		double                 dataMin,
		double                 dataMax,
		int                    maxTicks,
		std::vector<double>& outTicks
	);

	// -------------------------------------------------------------
	// DrawGrid
	// -------------------------------------------------------------
	void DrawGrid
	(
		Graphics& g,
		double                      leftInches,
		double                      topInches,
		double                      rightInches,
		double                      bottomInches,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks
	);

	// -------------------------------------------------------------
	// DrawTicks
	// -------------------------------------------------------------
	void DrawTicks
	(
		Graphics& g,
		double                      leftInches,
		double                      topInches,
		double                      rightInches,
		double                      bottomInches,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks
	);

	// -------------------------------------------------------------
	// DrawAxisNumbers
	// -------------------------------------------------------------
	void DrawAxisNumbers
	(
		Graphics& g,
		double                      leftInches,
		double                      topInches,
		double                      rightInches,
		double                      bottomInches,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks
	);

	// -------------------------------------------------------------
	// DrawAxisLabels
	// -------------------------------------------------------------
	void DrawAxisLabels
	(
		Graphics& g,
		double      leftInches,
		double      topInches,
		double      rightInches,
		double      bottomInches
	);

	// -------------------------------------------------------------
	// DrawCurve
	// -------------------------------------------------------------
	void DrawCurve
	(
		Graphics& g,
		double leftInches,
		double topInches,
		double rightInches,
		double bottomInches,
		double xTickMin,
		double xTickMax,
		double yTickMin,
		double yTickMax
	);

	// -------------------------------------------------------------
	// DrawTrendLine
	// -------------------------------------------------------------
	void DrawRunningAverage10
	(
		Graphics& g,
		double leftInches,
		double topInches,
		double rightInches,
		double bottomInches,
		double xTickMin,
		double xTickMax,
		double yTickMin,
		double yTickMax
	);

	void DrawTitle
	(
		Graphics& g,
		double     leftInches,
		double     topInches,
		double     rightInches
	);

	std::vector<double> MakeNiceTicks
	(
		double minVal,
		double maxVal,
		int targetCount
	);

// public methods
public:
	std::shared_ptr<Bitmap> CreatePlot(const CRect& rcPixels);

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CGraphPlotter();
	CGraphPlotter(std::vector<double> years, std::vector<double>values);
	virtual ~CGraphPlotter();

}; // CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
