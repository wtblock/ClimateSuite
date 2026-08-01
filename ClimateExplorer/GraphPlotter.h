/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <vector>

class CGraphPlotter
{
// protected data
protected:
	COLORREF m_clrLineColor;
	INT      m_nLineStyle;     // Gdiplus::DashStyle
	INT      m_nLineWeight;    // pixels

	BOOL     m_bTrendLine;
	COLORREF m_clrTrendLineColor;
	INT      m_nTrendLineStyle;   // Gdiplus::DashStyle
	INT      m_nTrendLineWeight;  // pixels

	COLORREF m_clrGridColor;
	INT      m_nGridLineStyle;    // Gdiplus::DashStyle
	INT      m_nGridLineWeight;   // pixels

	std::vector<int> m_arrYears;
	std::vector<int> m_arrCounts;

// public properties
public:
	std::vector<int>* GetYears()
	{
		return &m_arrYears;
	}

	void SetYears(std::vector<int>* value)
	{
		m_arrYears = *value;
	}

	__declspec(property(get = GetYears, put = SetYears))
		std::vector<int>* Years;

	std::vector<int>* GetCounts()
	{
		return &m_arrCounts;
	}

	void SetCounts(std::vector<int>* value)
	{
		m_arrCounts = *value;
	}

	__declspec(property(get = GetCounts, put = SetCounts))
		std::vector<int>* Counts;

	/////////////////////////////////////////////////////////////////////////////
	// Graphing Appearance Properties
	/////////////////////////////////////////////////////////////////////////////


	// -------------------------------------------------------------
	// LineColor
	// -------------------------------------------------------------
	COLORREF GetLineColor() const
	{
		return m_clrLineColor;
	}

	void SetLineColor(COLORREF value)
	{
		m_clrLineColor = value;
	}

	__declspec(property(get = GetLineColor, put = SetLineColor))
		COLORREF LineColor;

	// -------------------------------------------------------------
	// LineStyle
	// -------------------------------------------------------------
	INT GetLineStyle() const
	{
		return m_nLineStyle;
	}

	void SetLineStyle(INT value)
	{
		m_nLineStyle = value;
	}

	__declspec(property(get = GetLineStyle, put = SetLineStyle))
		INT LineStyle;

	// -------------------------------------------------------------
	// LineWeight
	// -------------------------------------------------------------
	INT GetLineWeight() const
	{
		return m_nLineWeight;
	}

	void SetLineWeight(INT value)
	{
		m_nLineWeight = value;
	}

	__declspec(property(get = GetLineWeight, put = SetLineWeight))
		INT LineWeight;

	// -------------------------------------------------------------
	// TrendLine
	// -------------------------------------------------------------
	BOOL GetTrendLine() const
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
	COLORREF GetTrendLineColor() const
	{
		return m_clrTrendLineColor;
	}

	void SetTrendLineColor(COLORREF value)
	{
		m_clrTrendLineColor = value;
	}

	__declspec(property(get = GetTrendLineColor, put = SetTrendLineColor))
		COLORREF TrendLineColor;

	// -------------------------------------------------------------
	// TrendLineStyle
	// -------------------------------------------------------------
	INT GetTrendLineStyle() const
	{
		return m_nTrendLineStyle;
	}

	void SetTrendLineStyle(INT value)
	{
		m_nTrendLineStyle = value;
	}

	__declspec(property(get = GetTrendLineStyle, put = SetTrendLineStyle))
		INT TrendLineStyle;

	// -------------------------------------------------------------
	// TrendLineWeight
	// -------------------------------------------------------------
	INT GetTrendLineWeight() const
	{
		return m_nTrendLineWeight;
	}

	void SetTrendLineWeight(INT value)
	{
		m_nTrendLineWeight = value;
	}

	__declspec(property(get = GetTrendLineWeight, put = SetTrendLineWeight))
		INT TrendLineWeight;

	// -------------------------------------------------------------
	// GridColor
	// -------------------------------------------------------------
	COLORREF GetGridColor() const
	{
		return m_clrGridColor;
	}

	void SetGridColor(COLORREF value)
	{
		m_clrGridColor = value;
	}

	__declspec(property(get = GetGridColor, put = SetGridColor))
		COLORREF GridColor;

	// -------------------------------------------------------------
	// GridLineStyle
	// -------------------------------------------------------------
	INT GetGridLineStyle() const
	{
		return m_nGridLineStyle;
	}

	void SetGridLineStyle(INT value)
	{
		m_nGridLineStyle = value;
	}

	__declspec(property(get = GetGridLineStyle, put = SetGridLineStyle))
		INT GridLineStyle;

	// -------------------------------------------------------------
	// GridLineWeight
	// -------------------------------------------------------------
	INT GetGridLineWeight() const
	{
		return m_nGridLineWeight;
	}

	void SetGridLineWeight(INT value)
	{
		m_nGridLineWeight = value;
	}

	__declspec(property(get = GetGridLineWeight, put = SetGridLineWeight))
		INT GridLineWeight;

// protected methods
protected:
	std::shared_ptr<Gdiplus::Bitmap> RenderPlot
	(
		const CRect& rcPixels,
		const std::vector<double>& xValues,
		const std::vector<double>& yValues,
		const CString& csTitle
	);

	void DrawGrid
	(
		Gdiplus::Graphics& g,
		const CRect& rcInterior,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks,
		double xMin, double xMax,
		double yMin, double yMax
	);

	void DrawAxes(Gdiplus::Graphics& g, const CRect& rcInterior);

	void DrawTicks
	(
		Gdiplus::Graphics& g,
		const CRect& rcInterior,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks,
		double xMin, double xMax,
		double yMin, double yMax
	);

	void DrawCurve
	(
		Gdiplus::Graphics& g, const CRect& rcInterior,
		const std::vector<double>& xValues,
		const std::vector<double>& yValues,
		double xMin, double xMax, double yMin, double yMax
	);
	void DrawTrendLine
	(
		Gdiplus::Graphics& g, const CRect& rcInterior,
		const std::vector<double>& xValues,
		const std::vector<double>& yValues,
		double xMin, double xMax, double yMin, double yMax
	);
	void DrawTitle
	(
		Gdiplus::Graphics& g, const CRect& rcPixels,
		const CString& csTitle
	);

	void DrawAxisLabels
	(
		Gdiplus::Graphics& g,
		const CRect& rcPixels,
		const CRect& rcInterior,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks,
		double xMin, double xMax,
		double yMin, double yMax,
		const CString& csXLabel,
		const CString& csYLabel
	);

	void DrawAxisNumbers
	(
		Gdiplus::Graphics& g,
		const CRect& rcInterior,
		const std::vector<double>& xTicks,
		const std::vector<double>& yTicks,
		double xMin, double xMax,
		double yMin, double yMax,
		const CString& csXLabel,
		const CString& csYLabel
	);

	std::vector<double> MakeNiceTicks
	(
		double minVal,
		double maxVal,
		int targetCount
	);

// public methods
public:
	// create a high-resolution landscape bitmap for plotting
	std::shared_ptr<Gdiplus::Bitmap> CreatePlotBitmap(const CRect& rcPixels);

	CRect GetPlotInterior(const CRect& rcPixels);

	std::shared_ptr<Gdiplus::Bitmap> RenderStationCount
	(
		const CRect& rcPixels,
		const std::vector<int>& years,
		const std::vector<int>& counts
	);

	// test case using pregenerated test data
	std::shared_ptr<Gdiplus::Bitmap> RenderStationCount
	(
		const CRect& rcPixels
	)
	{
		return RenderStationCount(rcPixels, *Years, *Counts);
	}

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CGraphPlotter();
	virtual ~CGraphPlotter();

}; // CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
