/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "GraphPlotter.h"
#include <gdiplus.h>
#include <vector>
#include <cmath>

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
CGraphPlotter::CGraphPlotter()
{
	// -------------------------------------------------------------
	// Default line appearance
	// -------------------------------------------------------------
	LineColor = RGB(0, 0, 128);   // dark blue
	LineStyle = Gdiplus::DashStyleSolid;
	LineWeight = 10;              // pixels

	// -------------------------------------------------------------
	// Default trend line appearance
	// -------------------------------------------------------------
	TrendLine = FALSE;
	TrendLineColor = RGB(139, 0, 0);   // dark red
	TrendLineStyle = Gdiplus::DashStyleDash;
	TrendLineWeight = 10;                // pixels

	// -------------------------------------------------------------
	// Default grid appearance
	// -------------------------------------------------------------
	GridColor = RGB(220, 220, 220);   // light gray
	GridLineStyle = Gdiplus::DashStyleDot;
	GridLineWeight = 10;                    // pixels

	// test data using the output of station count
	std::vector< std::pair<int, int> > arrTest = 
	{
		{ 1868,    1 }, { 1869,    2 }, { 1870,    1 }, { 1871,    3 },
		{ 1872,    7 }, { 1873,    9 }, { 1874,   12 }, { 1875,   12 },
		{ 1876,   12 }, { 1877,   14 }, { 1878,   15 }, { 1879,   17 },
		{ 1880,   18 }, { 1881,   19 }, { 1882,   20 }, { 1883,   22 },
		{ 1884,   30 }, { 1885,   40 }, { 1886,   42 }, { 1887,   73 },
		{ 1888,  100 }, { 1889,  133 }, { 1890,  146 }, { 1891,  241 },
		{ 1892,  294 }, { 1893,  524 }, { 1894,  565 }, { 1895,  612 },
		{ 1896,  662 }, { 1897,  711 }, { 1898,  737 }, { 1899,  758 },
		{ 1900,  783 }, { 1901,  802 }, { 1902,  827 }, { 1903,  863 },
		{ 1904,  899 }, { 1905,  914 }, { 1906,  940 }, { 1907,  971 },
		{ 1908,  986 }, { 1909, 1012 }, { 1910, 1014 }, { 1911, 1041 },
		{ 1912, 1048 }, { 1913, 1070 }, { 1914, 1086 }, { 1915, 1094 },
		{ 1916, 1113 }, { 1917, 1119 }, { 1918, 1135 }, { 1919, 1140 },
		{ 1920, 1147 }, { 1921, 1153 }, { 1922, 1156 }, { 1923, 1159 },
		{ 1924, 1163 }, { 1925, 1170 }, { 1926, 1172 }, { 1927, 1176 },
		{ 1928, 1188 }, { 1929, 1191 }, { 1930, 1192 }, { 1931, 1199 },
		{ 1932, 1203 }, { 1933, 1203 }, { 1934, 1204 }, { 1935, 1203 },
		{ 1936, 1206 }, { 1937, 1207 }, { 1938, 1208 }, { 1939, 1207 },
		{ 1940, 1209 }, { 1941, 1211 }, { 1942, 1213 }, { 1943, 1213 },
		{ 1944, 1213 }, { 1945, 1210 }, { 1946, 1211 }, { 1947, 1211 },
		{ 1948, 1215 }, { 1949, 1216 }, { 1950, 1215 }, { 1951, 1217 },
		{ 1952, 1217 }, { 1953, 1216 }, { 1954, 1217 }, { 1955, 1217 },
		{ 1956, 1215 }, { 1957, 1218 }, { 1958, 1217 }, { 1959, 1217 },
		{ 1960, 1216 }, { 1961, 1216 }, { 1962, 1216 }, { 1963, 1217 },
		{ 1964, 1217 }, { 1965, 1216 }, { 1966, 1217 }, { 1967, 1216 },
		{ 1968, 1216 }, { 1969, 1215 }, { 1970, 1216 }, { 1971, 1216 },
		{ 1972, 1217 }, { 1973, 1217 }, { 1974, 1215 }, { 1975, 1214 },
		{ 1976, 1214 }, { 1977, 1213 }, { 1978, 1217 }, { 1979, 1215 },
		{ 1980, 1213 }, { 1981, 1213 }, { 1982, 1213 }, { 1983, 1214 },
		{ 1984, 1210 }, { 1985, 1213 }, { 1986, 1212 }, { 1987, 1216 },
		{ 1988, 1215 }, { 1989, 1215 }, { 1990, 1213 }, { 1991, 1209 },
		{ 1992, 1207 }, { 1993, 1206 }, { 1994, 1203 }, { 1995, 1198 },
		{ 1996, 1191 }, { 1997, 1181 }, { 1998, 1184 }, { 1999, 1178 },
		{ 2000, 1177 }, { 2001, 1167 }, { 2002, 1158 }, { 2003, 1149 },
		{ 2004, 1139 }, { 2005, 1129 }, { 2006, 1115 }, { 2007, 1097 },
		{ 2008, 1091 }, { 2009, 1071 }, { 2010, 1046 }, { 2011, 1026 },
		{ 2012, 1003 }, { 2013,  960 }, { 2014,  942 }, { 2015,  924 },
		{ 2016,  913 }, { 2017,  904 }, { 2018,  880 }, { 2019,  861 },
		{ 2020,  848 }, { 2021,  841 }, { 2022,  825 }, { 2023,  815 },
		{ 2024,  802 }, { 2025,  789 }, { 2026,  753 },
	};

	for (auto& node : arrTest)
	{
		Years->push_back((int)node.first);
		Counts->push_back((int)node.second);
	}
} // CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
CGraphPlotter::~CGraphPlotter()
{

} // ~CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
// CreatePlotBitmap
//
// Creates a high-resolution GDI+ Bitmap sized to the landscape pixel
// rectangle provided by the caller. This bitmap is the foundation for
// all graph rendering operations.
//
// Behavior:
//   • Allocates a 32bpp ARGB bitmap
//   • Applies high-quality rendering settings
//   • Returns the bitmap wrapped in shared_ptr
/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Gdiplus::Bitmap>
CGraphPlotter::CreatePlotBitmap(const CRect& rcPixels)
{
	// width and height in pixels
	const INT nWidth = rcPixels.Width();
	const INT nHeight = rcPixels.Height();

	// MFC DEBUG_NEW interferes with GDI+ operator new
#ifdef _DEBUG
#undef new
#endif

	std::shared_ptr<Gdiplus::Bitmap> pBitmap
	(
		new Gdiplus::Bitmap(nWidth, nHeight)
	);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	// create graphics object
	Gdiplus::Graphics graphics(pBitmap.get());

	// high-quality rendering settings
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

	// clear background to white
	graphics.Clear(Gdiplus::Color(255, 255, 255));

	return pBitmap;
} // CreatePlotBitmap

/////////////////////////////////////////////////////////////////////////////
// GetPlotInterior
//
// Defines the interior plotting rectangle inside the bitmap.
// Padding ensures axes, labels, and titles have room.
//
// Default padding:
//   • Left   = 200 px
//   • Right  = 200 px
//   • Top    = 150 px
//   • Bottom = 200 px
/////////////////////////////////////////////////////////////////////////////
CRect CGraphPlotter::GetPlotInterior(const CRect& rcPixels)
{
	const INT nLeftPadding = 200;
	const INT nRightPadding = 200;
	const INT nTopPadding = 150;
	const INT nBottomPadding = 200;

	CRect rcInterior
	(
		rcPixels.left + nLeftPadding,
		rcPixels.top + nTopPadding,
		rcPixels.right - nRightPadding,
		rcPixels.bottom - nBottomPadding
	);

	return rcInterior;
} // GetPlotInterior

/////////////////////////////////////////////////////////////////////////////
// RenderPlot
//
// Updated to use Excel-style “nice number” ticks.
// Generates tick arrays for both axes and passes them to:
//
//   • DrawGrid
//   • DrawAxes
//   • DrawTicks
//   • DrawCurve
//   • DrawTrendLine
//   • DrawTitle
//   • DrawAxisLabels
//   • DrawAxisNumbers
//
// All other rendering behavior remains unchanged.
/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Gdiplus::Bitmap> CGraphPlotter::RenderPlot
(
	const CRect& rcPixels,
	const std::vector<double>& xValues,
	const std::vector<double>& yValues,
	const CString& csTitle
)
{
	// width and height in pixels
	const INT nWidth = rcPixels.Width();
	const INT nHeight = rcPixels.Height();

#ifdef _DEBUG
#undef new
#endif

	std::shared_ptr<Gdiplus::Bitmap> pBitmap
	(
		new Gdiplus::Bitmap(nWidth, nHeight)
	);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	// graphics object
	Gdiplus::Graphics g(pBitmap.get());

	// high-quality rendering
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

	// clear background
	g.Clear(Gdiplus::Color(255, 255, 255));

	// -------------------------------------------------------------
	// Compute axis ranges
	// -------------------------------------------------------------
	double xMin = DBL_MAX;
	double xMax = -DBL_MAX;
	double yMin = DBL_MAX;
	double yMax = -DBL_MAX;

	for (size_t i = 0; i < xValues.size(); ++i)
	{
		if (xValues[i] < xMin) xMin = xValues[i];
		if (xValues[i] > xMax) xMax = xValues[i];
	}

	for (size_t i = 0; i < yValues.size(); ++i)
	{
		if (yValues[i] < yMin) yMin = yValues[i];
		if (yValues[i] > yMax) yMax = yValues[i];
	}

	// -------------------------------------------------------------
	// Fixed interior plot rectangle (padding)
	// -------------------------------------------------------------
	const INT nLeftPadding = 200;
	const INT nRightPadding = 200;
	const INT nTopPadding = 150;
	const INT nBottomPadding = 200;

	CRect rcInterior
	(
		rcPixels.left + nLeftPadding,
		rcPixels.top + nTopPadding,
		rcPixels.right - nRightPadding,
		rcPixels.bottom - nBottomPadding
	);

	// -------------------------------------------------------------
	// Generate Excel-style tick arrays
	// -------------------------------------------------------------
	const int targetTickCountX = 10;
	const int targetTickCountY = 10;

	std::vector<double> xTicks = MakeNiceTicks
	(
		xMin,
		xMax,
		targetTickCountX
	);

	std::vector<double> yTicks = MakeNiceTicks
	(
		yMin,
		yMax,
		targetTickCountY
	);

	// -------------------------------------------------------------
	// Draw grid lines (interior ticks only)
	// -------------------------------------------------------------
	DrawGrid
	(
		g,
		rcInterior,
		xTicks,
		yTicks,
		xMin,
		xMax,
		yMin,
		yMax
	);

	// -------------------------------------------------------------
	// Draw axes (unchanged)
	// -------------------------------------------------------------
	DrawAxes
	(
		g,
		rcInterior
	);

	// -------------------------------------------------------------
	// Draw tick marks (ticks at all tick values)
	// -------------------------------------------------------------
	DrawTicks
	(
		g,
		rcInterior,
		xTicks,
		yTicks,
		xMin,
		xMax,
		yMin,
		yMax
	);

	// -------------------------------------------------------------
	// Draw curve (unchanged)
	// -------------------------------------------------------------
	DrawCurve
	(
		g,
		rcInterior,
		xValues,
		yValues,
		xMin,
		xMax,
		yMin,
		yMax
	);

	// -------------------------------------------------------------
	// Draw trend line (unchanged)
	// -------------------------------------------------------------
	DrawTrendLine
	(
		g,
		rcInterior,
		xValues,
		yValues,
		xMin,
		xMax,
		yMin,
		yMax
	);

	// -------------------------------------------------------------
	// Draw title (unchanged)
	// -------------------------------------------------------------
	DrawTitle
	(
		g,
		rcPixels,
		csTitle
	);

	// -------------------------------------------------------------
	// Draw axis titles (centered using tick arrays)
	// -------------------------------------------------------------
	DrawAxisLabels
	(
		g,
		rcPixels,
		rcInterior,
		xTicks,
		yTicks,
		xMin,
		xMax,
		yMin,
		yMax,
		L"Year",
		L"Count"
	);

	// -------------------------------------------------------------
	// Draw numeric axis labels (uses tick arrays)
	// -------------------------------------------------------------
	DrawAxisNumbers
	(
		g,
		rcInterior,
		xTicks,
		yTicks,
		xMin,
		xMax,
		yMin,
		yMax,
		L"Year",
		L"Count"
	);

	return pBitmap;

} // RenderPlot

/////////////////////////////////////////////////////////////////////////////
// DrawGrid
//
// Updated to use Excel-style “nice number” ticks.
// Draws vertical and horizontal grid lines at *interior* tick positions.
//
// Uses:
//   • m_clrGridColor
//   • m_nGridLineStyle
//   • m_nGridLineWeight
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawGrid
(
	Gdiplus::Graphics& g,
	const CRect& rcInterior,
	const std::vector<double>& xTicks,
	const std::vector<double>& yTicks,
	double xMin, double xMax,
	double yMin, double yMax
)
{
	// convert COLORREF → GDI+ Color
	Gdiplus::Color clrGrid
	(
		255,
		GetRValue(m_clrGridColor),
		GetGValue(m_clrGridColor),
		GetBValue(m_clrGridColor)
	);

	// create pen
	Gdiplus::Pen penGrid(clrGrid, (Gdiplus::REAL)m_nGridLineWeight);
	penGrid.SetDashStyle((Gdiplus::DashStyle)m_nGridLineStyle);

	// interior dimensions
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	const double width = static_cast<double>(rcInterior.Width());
	const double height = static_cast<double>(rcInterior.Height());

	// -------------------------------------------------------------
	// Vertical grid lines (interior ticks only)
	// -------------------------------------------------------------
	for (size_t i = 1; i + 1 < xTicks.size(); ++i)
	{
		const double t = xTicks[i];

		const double xPixelD =
			left + ((t - xMin) * (width / (xMax - xMin)));

		const INT xPixel = static_cast<INT>(std::round(xPixelD));

		g.DrawLine
		(
			&penGrid,
			(Gdiplus::REAL)xPixel,
			(Gdiplus::REAL)top,
			(Gdiplus::REAL)xPixel,
			(Gdiplus::REAL)bottom
		);
	}

	// -------------------------------------------------------------
	// Horizontal grid lines (interior ticks only)
	// -------------------------------------------------------------
	for (size_t j = 1; j + 1 < yTicks.size(); ++j)
	{
		const double t = yTicks[j];

		const double yPixelD =
			bottom - ((t - yMin) * (height / (yMax - yMin)));

		const INT yPixel = static_cast<INT>(std::round(yPixelD));

		g.DrawLine
		(
			&penGrid,
			(Gdiplus::REAL)left,
			(Gdiplus::REAL)yPixel,
			(Gdiplus::REAL)right,
			(Gdiplus::REAL)yPixel
		);
	}

} // DrawGrid

/////////////////////////////////////////////////////////////////////////////
// DrawAxes
//
// Draws the left Y-axis and bottom X-axis inside the interior plot
// rectangle. Uses:
//   • m_clrLineColor
//   • m_nLineStyle
//   • m_nLineWeight
//
// No tick marks or labels yet — those come later.
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawAxes
(
	Gdiplus::Graphics& g,
	const CRect& rcInterior
)
{
	// convert COLORREF → GDI+ Color
	Gdiplus::Color clrAxis
	(
		255,
		GetRValue(m_clrLineColor),
		GetGValue(m_clrLineColor),
		GetBValue(m_clrLineColor)
	);

	// create pen
	Gdiplus::Pen penAxis(clrAxis, (Gdiplus::REAL)m_nLineWeight);
	penAxis.SetDashStyle((Gdiplus::DashStyle)m_nLineStyle);

	// interior dimensions
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	// -------------------------------------------------------------
	// Draw Y-axis (left)
	// -------------------------------------------------------------
	g.DrawLine
	(
		&penAxis,
		(Gdiplus::REAL)left,
		(Gdiplus::REAL)top,
		(Gdiplus::REAL)left,
		(Gdiplus::REAL)bottom
	);

	// -------------------------------------------------------------
	// Draw X-axis (bottom)
	// -------------------------------------------------------------
	g.DrawLine
	(
		&penAxis,
		(Gdiplus::REAL)left,
		(Gdiplus::REAL)bottom,
		(Gdiplus::REAL)right,
		(Gdiplus::REAL)bottom
	);

} // DrawAxes

/////////////////////////////////////////////////////////////////////////////
// DrawTicks
//
// Updated to use Excel-style “nice number” ticks.
// Draws tick marks at *all* tick positions (including ends).
//
// Uses:
//   • m_clrLineColor
//   • m_nLineStyle
//   • m_nLineWeight
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawTicks
(
	Gdiplus::Graphics& g,
	const CRect& rcInterior,
	const std::vector<double>& xTicks,
	const std::vector<double>& yTicks,
	double xMin, double xMax,
	double yMin, double yMax
)
{
	// convert COLORREF → GDI+ Color
	Gdiplus::Color clrTick
	(
		255,
		GetRValue(m_clrLineColor),
		GetGValue(m_clrLineColor),
		GetBValue(m_clrLineColor)
	);

	// create pen
	Gdiplus::Pen penTick(clrTick, (Gdiplus::REAL)m_nLineWeight);
	penTick.SetDashStyle((Gdiplus::DashStyle)m_nLineStyle);

	// interior dimensions
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	const double width = static_cast<double>(rcInterior.Width());
	const double height = static_cast<double>(rcInterior.Height());

	const INT nTickSize = 12;   // length of tick marks in pixels

	// -------------------------------------------------------------
	// X-axis tick marks (bottom axis)
	// -------------------------------------------------------------
	for (size_t i = 0; i < xTicks.size(); ++i)
	{
		const double t = xTicks[i];

		const double xPixelD =
			left + ((t - xMin) * (width / (xMax - xMin)));

		const INT xPixel = static_cast<INT>(std::round(xPixelD));

		g.DrawLine
		(
			&penTick,
			(Gdiplus::REAL)xPixel,
			(Gdiplus::REAL)bottom,
			(Gdiplus::REAL)xPixel,
			(Gdiplus::REAL)(bottom + nTickSize)
		);
	}

	// -------------------------------------------------------------
	// Y-axis tick marks (left axis)
	// -------------------------------------------------------------
	for (size_t j = 0; j < yTicks.size(); ++j)
	{
		const double t = yTicks[j];

		const double yPixelD =
			bottom - ((t - yMin) * (height / (yMax - yMin)));

		const INT yPixel = static_cast<INT>(std::round(yPixelD));

		g.DrawLine
		(
			&penTick,
			(Gdiplus::REAL)left,
			(Gdiplus::REAL)yPixel,
			(Gdiplus::REAL)(left - nTickSize),
			(Gdiplus::REAL)yPixel
		);
	}

} // DrawTicks

/////////////////////////////////////////////////////////////////////////////
// DrawCurve
//
// Draws the main data curve inside the interior plot rectangle.
// Maps (x, y) data values to pixel coordinates using linear scaling.
//
// Uses:
//   • m_clrLineColor
//   • m_nLineStyle
//   • m_nLineWeight
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawCurve
(
	Gdiplus::Graphics& g,
	const CRect& rcInterior,
	const std::vector<double>& xValues,
	const std::vector<double>& yValues,
	double xMin, double xMax,
	double yMin, double yMax
)
{
	// convert COLORREF → GDI+ Color
	Gdiplus::Color clrCurve
	(
		255,
		GetRValue(m_clrLineColor),
		GetGValue(m_clrLineColor),
		GetBValue(m_clrLineColor)
	);

	// create pen
	Gdiplus::Pen penCurve(clrCurve, (Gdiplus::REAL)m_nLineWeight);
	penCurve.SetDashStyle((Gdiplus::DashStyle)m_nLineStyle);

	// interior dimensions
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	const double width = (double)rcInterior.Width();
	const double height = (double)rcInterior.Height();

	// -------------------------------------------------------------
	// Compute scale factors
	// -------------------------------------------------------------
	const double xScale = width / (xMax - xMin);
	const double yScale = height / (yMax - yMin);

	// -------------------------------------------------------------
	// Convert data points → pixel coordinates
	// -------------------------------------------------------------
	std::vector<Gdiplus::PointF> points;
	points.reserve(xValues.size());

	for (size_t i = 0; i < xValues.size(); ++i)
	{
		const double xVal = xValues[i];
		const double yVal = yValues[i];

		// map x → pixel
		const double xPixel =
			left + (xVal - xMin) * xScale;

		// map y → pixel (invert Y axis)
		const double yPixel =
			bottom - (yVal - yMin) * yScale;

		points.emplace_back
		(
			(Gdiplus::REAL)xPixel,
			(Gdiplus::REAL)yPixel
		);
	}

	// -------------------------------------------------------------
	// Draw polyline
	// -------------------------------------------------------------
	if (points.size() >= 2)
	{
		g.DrawLines(&penCurve, points.data(), (INT)points.size());
	}

} // DrawCurve

/////////////////////////////////////////////////////////////////////////////
// DrawTrendLine
//
// Draws a 10-year moving average trend line inside the interior plot
// rectangle. Uses the trend-line appearance properties:
//
//   • m_clrTrendLineColor
//   • m_nTrendLineStyle
//   • m_nTrendLineWeight
//
// If m_bTrendLine is FALSE, the method returns immediately.
//
// The moving average is computed over a fixed window of 10 points.
// Later this can be replaced with a time-based window or a more
// sophisticated smoothing algorithm.
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawTrendLine
(
	Gdiplus::Graphics& g,
	const CRect& rcInterior,
	const std::vector<double>& xValues,
	const std::vector<double>& yValues,
	double xMin, double xMax,
	double yMin, double yMax
)
{
	// trend line disabled?
	if (!m_bTrendLine)
	{
		return;
	}

	// convert COLORREF → GDI+ Color
	Gdiplus::Color clrTrend
	(
		255,
		GetRValue(m_clrTrendLineColor),
		GetGValue(m_clrTrendLineColor),
		GetBValue(m_clrTrendLineColor)
	);

	// create pen
	Gdiplus::Pen penTrend(clrTrend, (Gdiplus::REAL)m_nTrendLineWeight);
	penTrend.SetDashStyle((Gdiplus::DashStyle)m_nTrendLineStyle);

	// interior dimensions
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	const double width = (double)rcInterior.Width();
	const double height = (double)rcInterior.Height();

	// -------------------------------------------------------------
	// Compute scale factors
	// -------------------------------------------------------------
	const double xScale = width / (xMax - xMin);
	const double yScale = height / (yMax - yMin);

	// -------------------------------------------------------------
	// Compute 10-year moving average
	// -------------------------------------------------------------
	const size_t n = yValues.size();
	const size_t window = 10;

	if (n < window)
	{
		return; // not enough data
	}

	std::vector<double> avgX;
	std::vector<double> avgY;

	avgX.reserve(n - window + 1);
	avgY.reserve(n - window + 1);

	for (size_t i = 0; i <= n - window; ++i)
	{
		double sum = 0.0;

		for (size_t j = 0; j < window; ++j)
		{
			sum += yValues[i + j];
		}

		const double mean = sum / (double)window;

		avgX.push_back(xValues[i + window / 2]); // center of window
		avgY.push_back(mean);
	}

	// -------------------------------------------------------------
	// Convert averaged points → pixel coordinates
	// -------------------------------------------------------------
	std::vector<Gdiplus::PointF> points;
	points.reserve(avgX.size());

	for (size_t i = 0; i < avgX.size(); ++i)
	{
		const double xVal = avgX[i];
		const double yVal = avgY[i];

		const double xPixel =
			left + (xVal - xMin) * xScale;

		const double yPixel =
			bottom - (yVal - yMin) * yScale;

		points.emplace_back
		(
			(Gdiplus::REAL)xPixel,
			(Gdiplus::REAL)yPixel
		);
	}

	// -------------------------------------------------------------
	// Draw trend polyline
	// -------------------------------------------------------------
	if (points.size() >= 2)
	{
		g.DrawLines(&penTrend, points.data(), (INT)points.size());
	}

} // DrawTrendLine

/////////////////////////////////////////////////////////////////////////////
// DrawTitle
//
// Draws the plot title centered at the top of the bitmap.
// Uses the main line color for now. Later this can be replaced with
// a dedicated title color and font selection.
//
// Title is drawn above the interior plot rectangle.
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawTitle
(
	Gdiplus::Graphics& g,
	const CRect& rcPixels,
	const CString& csTitle
)
{
	if (csTitle.IsEmpty())
	{
		return;
	}

	Gdiplus::Color clrTitle
	(
		255,
		GetRValue(m_clrLineColor),
		GetGValue(m_clrLineColor),
		GetBValue(m_clrLineColor)
	);

	Gdiplus::SolidBrush brushTitle(clrTitle);

	// Larger title font
	Gdiplus::Font fontTitle(L"Segoe UI", 72.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

	// Taller title rectangle
	Gdiplus::RectF rcTitle
	(
		(Gdiplus::REAL)rcPixels.left,
		(Gdiplus::REAL)rcPixels.top,
		(Gdiplus::REAL)rcPixels.Width(),
		200.0f
	);

	Gdiplus::StringFormat fmt;
	fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
	fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	g.DrawString(csTitle, -1, &fontTitle, rcTitle, &fmt, &brushTitle);

} // DrawTitle

/////////////////////////////////////////////////////////////////////////////
// DrawAxisLabels
//
// Updated to use Excel-style “nice number” ticks.
// Centers the Y-axis label between the first and last tick values.
//
// Uses:
//   • LineColor (temporary text color)
//   • m_nFontSizeAxisLabel (recommended default: 32)
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawAxisLabels
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
)
{
	// -------------------------------------------------------------
	// Colors and fonts
	// -------------------------------------------------------------
	COLORREF rgbLine = LineColor;

	Gdiplus::Color clrText
	(
		255,
		GetRValue(rgbLine),
		GetGValue(rgbLine),
		GetBValue(rgbLine)
	);

	Gdiplus::SolidBrush brushText(clrText);

	Gdiplus::Font fontLabel
	(
		L"Segoe UI",
		(Gdiplus::REAL)32,   // larger axis title font
		Gdiplus::FontStyleBold,
		Gdiplus::UnitPixel
	);

	Gdiplus::StringFormat fmtCenter;
	fmtCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
	fmtCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	// -------------------------------------------------------------
	// Geometry
	// -------------------------------------------------------------
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	const double width = static_cast<double>(rcInterior.Width());
	const double height = static_cast<double>(rcInterior.Height());

	// -------------------------------------------------------------
	// X-axis label (centered horizontally under the axis)
	// -------------------------------------------------------------
	{
		const INT xCenter = (left + right) / 2;
		const INT yPos = bottom + 100;   // increased from +80

		Gdiplus::RectF rcText
		(
			(Gdiplus::REAL)(xCenter - 4000),
			(Gdiplus::REAL)(yPos - 2000),
			(Gdiplus::REAL)8000,
			(Gdiplus::REAL)4000
		);

		g.DrawString
		(
			csXLabel,
			-1,
			&fontLabel,
			rcText,
			&fmtCenter,
			&brushText
		);
	}

	// -------------------------------------------------------------
	// Y-axis label (centered between first and last tick values)
	// -------------------------------------------------------------
	if (!yTicks.empty())
	{
		const double tMin = yTicks.front();
		const double tMax = yTicks.back();
		const double tMid = (tMin + tMax) * 0.5;

		const double yPixelD =
			bottom - ((tMid - yMin) * (height / (yMax - yMin)));

		const INT yCenter = static_cast<INT>(std::round(yPixelD));
		const INT xPos = left - 120;

		// Rotation transform
		Gdiplus::Matrix m;
		m.RotateAt
		(
			-90.0f,
			Gdiplus::PointF
			(
				(Gdiplus::REAL)xPos,
				(Gdiplus::REAL)yCenter
			)
		);

		g.SetTransform(&m);

		Gdiplus::RectF rcText
		(
			(Gdiplus::REAL)(xPos - 2000),
			(Gdiplus::REAL)(yCenter - 1990),   // raised slightly
			(Gdiplus::REAL)4000,
			(Gdiplus::REAL)4000
		);

		g.DrawString
		(
			csYLabel,
			-1,
			&fontLabel,
			rcText,
			&fmtCenter,
			&brushText
		);

		g.ResetTransform();
	}

} // DrawAxisLabels

/////////////////////////////////////////////////////////////////////////////
// DrawAxisNumbers
//
// Updated to use Excel-style “nice number” ticks.
// Draws numeric labels at each tick position for both axes.
//
// Uses:
//   • LineColor (temporary text color)
//   • m_nFontSizeAxis (recommended default: 26)
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawAxisNumbers
(
	Gdiplus::Graphics& g,
	const CRect& rcInterior,
	const std::vector<double>& xTicks,
	const std::vector<double>& yTicks,
	double xMin, double xMax,
	double yMin, double yMax,
	const CString& csXLabel,
	const CString& csYLabel
)
{
	// -------------------------------------------------------------
	// Colors and fonts
	// -------------------------------------------------------------
	COLORREF rgbText = LineColor;

	Gdiplus::Color clrText
	(
		255,
		GetRValue(rgbText),
		GetGValue(rgbText),
		GetBValue(rgbText)
	);

	Gdiplus::SolidBrush brushText(clrText);

	Gdiplus::Font fontNum
	(
		L"Segoe UI",
		(Gdiplus::REAL)26,   // larger numeric labels
		Gdiplus::FontStyleRegular,
		Gdiplus::UnitPixel
	);

	Gdiplus::StringFormat fmtCenter;
	fmtCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
	fmtCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	// -------------------------------------------------------------
	// Geometry
	// -------------------------------------------------------------
	const INT left = rcInterior.left;
	const INT right = rcInterior.right;
	const INT top = rcInterior.top;
	const INT bottom = rcInterior.bottom;

	const double width = static_cast<double>(rcInterior.Width());
	const double height = static_cast<double>(rcInterior.Height());

	// -------------------------------------------------------------
	// X-axis numeric labels (bottom)
	// -------------------------------------------------------------
	for (size_t i = 0; i < xTicks.size(); ++i)
	{
		const double t = xTicks[i];

		const double xPixelD =
			left + ((t - xMin) * (width / (xMax - xMin)));

		const INT xPixel = static_cast<INT>(std::round(xPixelD));

		CString cs;
		if (std::fabs(t - std::round(t)) < 0.0001)
		{
			cs.Format(L"%d", (int)std::round(t));
		}
		else
		{
			cs.Format(L"%.2f", t);
		}

		Gdiplus::RectF rcText
		(
			(Gdiplus::REAL)(xPixel - 2000),
			(Gdiplus::REAL)(bottom + 30 - 2000),   // raised from +20
			(Gdiplus::REAL)4000,
			(Gdiplus::REAL)4000
		);

		g.DrawString
		(
			cs,
			-1,
			&fontNum,
			rcText,
			&fmtCenter,
			&brushText
		);
	}

	// -------------------------------------------------------------
	// Y-axis numeric labels (left, rotated)
	// -------------------------------------------------------------
	for (size_t j = 0; j < yTicks.size(); ++j)
	{
		const double t = yTicks[j];

		const double yPixelD =
			bottom - ((t - yMin) * (height / (yMax - yMin)));

		const INT yPixel = static_cast<INT>(std::round(yPixelD));

		CString cs;
		if (std::fabs(t - std::round(t)) < 0.0001)
		{
			cs.Format(L"%d", (int)std::round(t));
		}
		else
		{
			cs.Format(L"%.2f", t);
		}

		// Rotation transform
		Gdiplus::Matrix m;
		m.RotateAt
		(
			-90.0f,
			Gdiplus::PointF
			(
				(Gdiplus::REAL)(left - 40),
				(Gdiplus::REAL)(yPixel - 10)   // raised anchor
			)
		);

		g.SetTransform(&m);

		Gdiplus::RectF rcText
		(
			(Gdiplus::REAL)(left - 40 - 2000),
			(Gdiplus::REAL)(yPixel - 1990),   // raised from -2000
			(Gdiplus::REAL)4000,
			(Gdiplus::REAL)4000
		);

		g.DrawString
		(
			cs,
			-1,
			&fontNum,
			rcText,
			&fmtCenter,
			&brushText
		);

		g.ResetTransform();
	}

} // DrawAxisNumbers

/////////////////////////////////////////////////////////////////////////////
// MakeNiceTicks
//
// Generates Excel-style “nice number” ticks for an axis.
// Returns a vector<double> containing tick values.
//
// Behavior:
//   • Chooses a “nice” step: 1, 2, 5 × 10^n
//   • Extends min/max outward to nice boundaries
//   • Produces ticks at all boundaries (including ends)
//   • Works for any numeric range
/////////////////////////////////////////////////////////////////////////////
std::vector<double> CGraphPlotter::MakeNiceTicks
(
	double minVal,
	double maxVal,
	int targetCount
)
{
	std::vector<double> ticks;

	if (targetCount <= 0)
	{
		return ticks;
	}

	// Ensure min < max
	if (maxVal < minVal)
	{
		std::swap(minVal, maxVal);
	}

	const double range = maxVal - minVal;

	if (range <= 0.0)
	{
		// Degenerate case: single-value axis
		ticks.push_back(minVal);
		return ticks;
	}

	// Raw step size
	const double rawStep = range / static_cast<double>(targetCount);

	// Determine magnitude (power of 10)
	const double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));

	// Determine normalized step (1, 2, or 5)
	double norm = rawStep / magnitude;

	double niceNorm = 1.0;

	if (norm <= 1.0)
	{
		niceNorm = 1.0;
	}
	else if (norm <= 2.0)
	{
		niceNorm = 2.0;
	}
	else if (norm <= 5.0)
	{
		niceNorm = 5.0;
	}
	else
	{
		niceNorm = 10.0;
	}

	const double niceStep = niceNorm * magnitude;

	// Compute nice min/max
	const double niceMin = std::floor(minVal / niceStep) * niceStep;
	const double niceMax = std::ceil(maxVal / niceStep) * niceStep;

	// Generate ticks
	for (double v = niceMin; v <= niceMax + 0.5 * niceStep; v += niceStep)
	{
		ticks.push_back(v);
	}

	return ticks;
} // MakeNiceTicks

/////////////////////////////////////////////////////////////////////////////
// RenderStationCount
//
// Thin wrapper that converts integer station-count data into double
// vectors and calls the general RenderPlot() method.
//
// Title:
//   "USHCN Number of Stations"
/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Gdiplus::Bitmap> CGraphPlotter::RenderStationCount
(
	const CRect& rcPixels,
	const std::vector<int>& years,
	const std::vector<int>& counts
)
{
	// convert int → double
	std::vector<double> xValues;
	std::vector<double> yValues;

	xValues.reserve(years.size());
	yValues.reserve(counts.size());

	for (auto& year : years)
	{
		xValues.push_back((double)year);
	}

	for (auto& count : counts)
	{
		yValues.push_back((double)count);
	}

	// call general renderer
	return RenderPlot
	(
		rcPixels,
		xValues,
		yValues,
		L"USHCN Number of Stations"
	);

} // RenderStationCount

/////////////////////////////////////////////////////////////////////////////
