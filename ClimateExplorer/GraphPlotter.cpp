/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ClimateExplorerDoc.h"
#include "GraphPlotter.h"
#include <gdiplus.h>
#include <vector>
#include <cmath>
#include <string>

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
CGraphPlotter::CGraphPlotter(CClimateExplorerDoc* pDoc)
{
	SetDefaults(pDoc);

	//std::vector< std::pair<int, int> > arrTest =
	//{
	//	//{ 1868,    1 }, { 1869,    2 }, { 1870,    1 }, { 1871,    3 },
	//	//{ 1872,    7 }, { 1873,    9 }, { 1874,   12 }, { 1875,   12 },
	//	//{ 1876,   12 }, { 1877,   14 }, { 1878,   15 }, { 1879,   17 },
	//	//{ 1880,   18 }, { 1881,   19 }, { 1882,   20 }, { 1883,   22 },
	//	//{ 1884,   30 }, { 1885,   40 }, { 1886,   42 }, { 1887,   73 },
	//	//{ 1888,  100 }, { 1889,  133 }, { 1890,  146 }, { 1891,  241 },
	//	//{ 1892,  294 }, { 1893,  524 }, { 1894,  565 }, { 1895,  612 },
	//	//{ 1896,  662 }, { 1897,  711 }, { 1898,  737 }, { 1899,  758 },
	//	/*{ 1900,  783 },*/ { 1901,  802 }, { 1902,  827 }, { 1903,  863 },
	//	{ 1904,  899 }, { 1905,  914 }, { 1906,  940 }, { 1907,  971 },
	//	{ 1908,  986 }, { 1909, 1012 }, { 1910, 1014 }, { 1911, 1041 },
	//	{ 1912, 1048 }, { 1913, 1070 }, { 1914, 1086 }, { 1915, 1094 },
	//	{ 1916, 1113 }, { 1917, 1119 }, { 1918, 1135 }, { 1919, 1140 },
	//	{ 1920, 1147 }, { 1921, 1153 }, { 1922, 1156 }, { 1923, 1159 },
	//	{ 1924, 1163 }, { 1925, 1170 }, { 1926, 1172 }, { 1927, 1176 },
	//	{ 1928, 1188 }, { 1929, 1191 }, { 1930, 1192 }, { 1931, 1199 },
	//	{ 1932, 1203 }, { 1933, 1203 }, { 1934, 1204 }, { 1935, 1203 },
	//	{ 1936, 1206 }, { 1937, 1207 }, { 1938, 1208 }, { 1939, 1207 },
	//	{ 1940, 1209 }, { 1941, 1211 }, { 1942, 1213 }, { 1943, 1213 },
	//	{ 1944, 1213 }, { 1945, 1210 }, { 1946, 1211 }, { 1947, 1211 },
	//	{ 1948, 1215 }, { 1949, 1216 }, { 1950, 1215 }, { 1951, 1217 },
	//	{ 1952, 1217 }, { 1953, 1216 }, { 1954, 1217 }, { 1955, 1217 },
	//	{ 1956, 1215 }, { 1957, 1218 }, { 1958, 1217 }, { 1959, 1217 },
	//	{ 1960, 1216 }, { 1961, 1216 }, { 1962, 1216 }, { 1963, 1217 },
	//	{ 1964, 1217 }, { 1965, 1216 }, { 1966, 1217 }, { 1967, 1216 },
	//	{ 1968, 1216 }, { 1969, 1215 }, { 1970, 1216 }, { 1971, 1216 },
	//	{ 1972, 1217 }, { 1973, 1217 }, { 1974, 1215 }, { 1975, 1214 },
	//	{ 1976, 1214 }, { 1977, 1213 }, { 1978, 1217 }, { 1979, 1215 },
	//	{ 1980, 1213 }, { 1981, 1213 }, { 1982, 1213 }, { 1983, 1214 },
	//	{ 1984, 1210 }, { 1985, 1213 }, { 1986, 1212 }, { 1987, 1216 },
	//	{ 1988, 1215 }, { 1989, 1215 }, { 1990, 1213 }, { 1991, 1209 },
	//	{ 1992, 1207 }, { 1993, 1206 }, { 1994, 1203 }, { 1995, 1198 },
	//	{ 1996, 1191 }, { 1997, 1181 }, { 1998, 1184 }, { 1999, 1178 },
	//	{ 2000, 1177 }, { 2001, 1167 }, { 2002, 1158 }, { 2003, 1149 },
	//	{ 2004, 1139 }, { 2005, 1129 }, { 2006, 1115 }, { 2007, 1097 },
	//	{ 2008, 1091 }, { 2009, 1071 }, { 2010, 1046 }, { 2011, 1026 },
	//	{ 2012, 1003 }, { 2013,  960 }, { 2014,  942 }, { 2015,  924 },
	//	{ 2016,  913 }, { 2017,  904 }, { 2018,  880 }, { 2019,  861 },
	//	{ 2020,  848 }, { 2021,  841 }, { 2022,  825 }, { 2023,  815 },
	//	{ 2024,  802 }, { 2025,  789 }, /*{ 2026,  753 },*/
	//};

	//for (auto& node : arrTest)
	//{
	//	Years.push_back(node.first);
	//	Values.push_back(node.second);
	//}

} // CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
CGraphPlotter::CGraphPlotter
(
	CClimateExplorerDoc* pDoc,
	std::vector<double> years, 
	std::vector<double>values
)
{
	SetDefaults(pDoc);
	Years = years;
	Values = values;

} // CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
CGraphPlotter::~CGraphPlotter()
{

} // ~CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::SetDefaults(CClimateExplorerDoc* pDoc)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	QueryType = pDoc->QueryType;
	Pure = pDoc->Pure;
	Scope = pDoc->Scope;
	YearStart = pDoc->YearStart;
	YearEnd = pDoc->YearEnd;
	Subtype = pDoc->Subtype;
	Threshold = pDoc->Threshold;
	Units = pDoc->Units;
	Output = pDoc->Output;
	SQL = pDoc->SQL;
	State = pDoc->State;
	Location = pDoc->Location;

	// -------------------------------------------------------------
	// Station data
	// -------------------------------------------------------------
	Station = L"";
	Longitude = 0.0f;
	Latitude = 0.0f;

	// -------------------------------------------------------------
	// Title and labels
	// -------------------------------------------------------------
	GraphTitle = pDoc->GraphTitle;
	AxisLabelX = pDoc->AxisLabelX;
	AxisLabelY = pDoc->AxisLabelY;

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	LineColor = pDoc->LineColor;
	LineStyle = pDoc->LineStyle;
	LineThicknessInches = pDoc->LineThicknessInches;

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	TrendLineColor = pDoc->TrendLineColor;
	TrendLineStyle = pDoc->TrendLineStyle;
	TrendLineThicknessInches = pDoc->TrendLineThicknessInches;

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	GridColor = pDoc->GridColor;
	GridLineStyle = pDoc->GridLineStyle;
	GridLineThicknessInches = pDoc->GridLineThicknessInches;

	// -------------------------------------------------------------
	// Text sizes (points)
	// -------------------------------------------------------------
	TitleFontSizePoints = pDoc->TitleFontSizePoints;
	AxisLabelFontSizePoints = pDoc->AxisLabelFontSizePoints;
	TickLabelFontSizePoints = pDoc->TickLabelFontSizePoints;

	// -------------------------------------------------------------
	// Padding (inches)
	// -------------------------------------------------------------
	LeftPaddingInches = pDoc->LeftPaddingInches;
	RightPaddingInches = pDoc->RightPaddingInches;
	TopPaddingInches = pDoc->TopPaddingInches;
	BottomPaddingInches = pDoc->BottomPaddingInches;

	// -------------------------------------------------------------
	// Tick mark length (inches)
	// -------------------------------------------------------------
	TickLengthInches = pDoc->TickLengthInches;

	// -------------------------------------------------------------
	// Layout (Full, Half, or Quarter page)
	// -------------------------------------------------------------
	Layout = pDoc->Layout;

} // SetDefaults

/////////////////////////////////////////////////////////////////////////////
// RenderPlot
//
// Updated to use Excel-style “nice number” ticks. Generates tick arrays
// for both axes and passes them to all rendering functions. The primary
// curve and optional running-average curve are mapped using tick ranges
// for proper Excel-style scaling.
//
// Rendering sequence:
//
//   • DrawGrid
//   • DrawTicks
//   • DrawAxisNumbers
//   • DrawAxisLabels
//   • DrawCurve
//   • DrawTrendLine (optional)
//   • DrawTitle
//
// All geometry is inch-based. All text uses point-based font sizes.
//
/////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Bitmap> CGraphPlotter::RenderPlot(const CRect& rcPixels)
{
	// If we have a CE PNG, render from it instead of recomputing
	if (!m_arrPNG.empty())
	{
		// Create IStream over m_arrPNG
		CComPtr<IStream> pStream;
		CreateStreamOnHGlobal(NULL, TRUE, &pStream);

		ULONG written = 0;
		pStream->Write(m_arrPNG.data(), (ULONG)m_arrPNG.size(), &written);

		LARGE_INTEGER liZero = {};
		pStream->Seek(liZero, STREAM_SEEK_SET, NULL);

		// Load Bitmap from stream
		std::unique_ptr<Bitmap> pBmp(new Bitmap(pStream, FALSE));
		return pBmp;
	}

	// -------------------------------------------------------------
	// Convert pixel rectangle to inches (based on 400 DPI)
	// -------------------------------------------------------------
	const double dpi = 400.0;

	const double widthInches = static_cast<double>(rcPixels.Width()) / dpi;
	const double heightInches = static_cast<double>(rcPixels.Height()) / dpi;

	// -------------------------------------------------------------
	// Create the bitmap at 400 DPI
	// -------------------------------------------------------------
#ifdef _DEBUG
#undef new
#endif
	std::unique_ptr<Bitmap> pBmp
	(
		new Bitmap(rcPixels.Width(), rcPixels.Height(), PixelFormat32bppARGB)
	);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	pBmp->SetResolution(dpi, dpi);

	// -------------------------------------------------------------
	// Graphics object
	// -------------------------------------------------------------
	Graphics g(pBmp.get());
	g.SetPageUnit(UnitInch);
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	// -------------------------------------------------------------
	// Background
	// -------------------------------------------------------------
	//SolidBrush brWhite(Color(255, 255, 255, 255));
	//g.FillRectangle(&brWhite, 0.0f, 0.0f, (REAL)widthInches, (REAL)heightInches);
	g.Clear(Gdiplus::Color(0, 0, 0, 0));   // fully transparent background

	// -------------------------------------------------------------
	// Interior rectangle (in inches)
	// -------------------------------------------------------------
	const double left = LeftPaddingInches;
	const double right = widthInches - RightPaddingInches;
	const double top = TopPaddingInches;
	const double bottom = heightInches - BottomPaddingInches;

	// -------------------------------------------------------------
	// Compute real data ranges
	// -------------------------------------------------------------
	if (Values.size() == 0 || Years.size() == 0)
	{
		DrawTitle(g, left, top, right);
		return pBmp;
	}

	double xMin = Years.front();
	double xMax = Years.front();
	double yMin = Values.front();
	double yMax = Values.front();
	for (size_t i = 1; i < Years.size(); i++)
	{
		xMin = min(xMin, Years[i]);
		xMax = max(xMax, Years[i]);
		yMin = min(yMin, Values[i]);
		yMax = max(yMax, Values[i]);
	}

	// -------------------------------------------------------------
	// Generate tick arrays from real ranges
	// -------------------------------------------------------------
	std::vector<double> xTicks;
	std::vector<double> yTicks;

	GenerateTicks(xMin, xMax, 10, xTicks);
	GenerateTicks(yMin, yMax, 10, yTicks);
	if (yTicks.size() == 0)
	{
		DrawTitle(g, left, top, right);
		return pBmp;
	}

	// -------------------------------------------------------------
	// Tick range (Excel-style)
	// -------------------------------------------------------------
	const double xTickMin = xTicks.front();
	const double xTickMax = xTicks.back();
	const double yTickMin = yTicks.front();
	const double yTickMax = yTicks.back();

	// -------------------------------------------------------------
	// Draw grid
	// -------------------------------------------------------------
	DrawGrid(g, left, top, right, bottom, xTicks, yTicks);

	// -------------------------------------------------------------
	// Draw ticks
	// -------------------------------------------------------------
	DrawTicks(g, left, top, right, bottom, xTicks, yTicks);

	// -------------------------------------------------------------
	// Draw axis numbers
	// -------------------------------------------------------------
	DrawAxisNumbers(g, left, top, right, bottom, xTicks, yTicks);

	// -------------------------------------------------------------
	// Draw axis labels
	// -------------------------------------------------------------
	DrawAxisLabels(g, left, top, right, bottom);

	// -------------------------------------------------------------
	// Draw title
	// -------------------------------------------------------------
	DrawTitle(g, left, top, right);

	// -------------------------------------------------------------
	// Draw curve
	// -------------------------------------------------------------
	DrawCurve(g, left, top, right, bottom, xTickMin, xTickMax, yTickMin, yTickMax);

	// -------------------------------------------------------------
	// Draw trend line
	// -------------------------------------------------------------
	DrawTrendLine(g, left, top, right, bottom, xTickMin, xTickMax, yTickMin, yTickMax);

	return pBmp;
} // RenderPlot

/////////////////////////////////////////////////////////////////////////////
// DrawAxisLabels
//
// Draws the axis titles (“Year”, “Count”, etc.) using inch-based
// geometry. Font size, color, and alignment are user-configurable.
// Vertical axis labels are rotated -90 degrees around a precise
// anchor point.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//   bottomInches  Bottom boundary of the interior plot area
//
// NOTES:
//
//   • Uses AxisLabelFontSizePoints.
//   • Rotation uses RotateTransform / ResetTransform.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawAxisLabels
(
	Graphics& g,
	double     leftInches,
	double     topInches,
	double     rightInches,
	double     bottomInches
)
{
	const REAL fontSize = (REAL)AxisLabelFontSizePoints;

	Gdiplus::Font font
	(
		L"Segoe UI",
		fontSize,
		FontStyleRegular,
		UnitPoint
	);

	SolidBrush br
	(
		theApp.ColorPlus->ARGBByName[LineColor]
	);

	// -------------------------------------------------------------
	// X-axis label
	// -------------------------------------------------------------
	{
		StringFormat fmt;
		fmt.SetAlignment(StringAlignmentCenter);
		fmt.SetLineAlignment(StringAlignmentNear);

		const double width = rightInches - leftInches;

		RectF rc
		(
			(REAL)leftInches,
			(REAL)(bottomInches + 0.25),
			(REAL)width,
			(REAL)0.35
		);
		
		CString csLabel = AxisLabelX;
		g.DrawString
		(
			csLabel,
			-1,
			&font,
			rc,
			&fmt,
			&br
		);
	}

	// -------------------------------------------------------------
	// Y-axis label (rotated)
	// -------------------------------------------------------------
	{
		StringFormat fmt;
		fmt.SetAlignment(StringAlignmentCenter);
		fmt.SetLineAlignment(StringAlignmentCenter);

		const double height = bottomInches - topInches;

		GraphicsState state = g.Save();

		g.TranslateTransform
		(
			(REAL)(leftInches - 0.40),
			(REAL)(topInches + height / 2.0)
		);

		g.RotateTransform(-90.0f);

		RectF rc
		(
			(REAL)(-height / 2.0),
			(REAL)-0.20f,
			(REAL)height,
			(REAL)0.40f
		);

		CString csLabel = AxisLabelY;
		g.DrawString
		(
			csLabel,
			-1,
			&font,
			rc,
			&fmt,
			&br
		);

		g.Restore(state);
	}
} // DrawAxisLabels

/////////////////////////////////////////////////////////////////////////////
// GenerateTicks
//
// Computes “nice number” tick positions for both axes using the same
// algorithm employed by Excel and other professional charting tools.
// The function analyzes the data range, selects an aesthetically
// pleasing tick interval, and generates arrays of tick positions.
//
// PARAMETERS:
//
//   dataMin        Minimum data value
//   dataMax        Maximum data value
//   maxTicks       Maximum number of ticks desired
//   outTicks       Output vector of tick positions
//
// NOTES:
//
//   • Produces evenly spaced ticks using nice-number intervals.
//   • Ensures ticks fall on round values (1, 2, 5 × 10^n).
//   • Used for grid lines, tick marks, axis numbers, and scaling.
//   • All values are in data units (not inches).
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::GenerateTicks
(
	double dataMin,
	double dataMax,
	int maxTicks,
	std::vector<double>& outTicks
)
{
	outTicks.clear();

	if (dataMax <= dataMin)
		return;

	// -------------------------------------------------------------
	// Compute raw range
	// -------------------------------------------------------------
	const double range = dataMax - dataMin;

	// -------------------------------------------------------------
	// Compute nice range
	// -------------------------------------------------------------
	const double exponent = floor(log10(range));
	const double fraction = range / pow(10.0, exponent);

	double niceFraction;

	if (fraction < 1.5)
		niceFraction = 1.0;
	else if (fraction < 3.0)
		niceFraction = 2.0;
	else if (fraction < 7.0)
		niceFraction = 5.0;
	else
		niceFraction = 10.0;

	const double niceRange = niceFraction * pow(10.0, exponent);

	// -------------------------------------------------------------
	// Compute tick spacing
	// -------------------------------------------------------------
	const double spacing = niceRange / (double)maxTicks;

	// -------------------------------------------------------------
	// Compute nice minimum and maximum
	// -------------------------------------------------------------
	const double niceMin = floor(dataMin / spacing) * spacing;
	const double niceMax = ceil(dataMax / spacing) * spacing;

	// -------------------------------------------------------------
	// Generate ticks
	// -------------------------------------------------------------
	for (double v = niceMin; v <= niceMax + (spacing * 0.5); v += spacing)
	{
		outTicks.push_back(v);
	}
} // GenerateTicks

/////////////////////////////////////////////////////////////////////////////
// DrawGrid
//
// Renders the background grid inside the plot area using inch-based
// geometry. Grid lines are drawn at positions defined by the tick
// arrays generated in RenderPlot. Grid color, dash style, and line
// thickness are user-configurable.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//   bottomInches  Bottom boundary of the interior plot area
//   xTicks        Tick positions for the horizontal axis (data units)
//   yTicks        Tick positions for the vertical axis   (data units)
//
// NOTES:
//
//   • Uses GridColor, GridLineStyle, GridLineThicknessInches.
//   • Tick arrays determine exact grid line placement.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawGrid
(
	Graphics& g,
	double                      leftInches,
	double                      topInches,
	double                      rightInches,
	double                      bottomInches,
	const std::vector<double>& xTicks,
	const std::vector<double>& yTicks
)
{
	// -------------------------------------------------------------
	// Pen for grid lines
	// -------------------------------------------------------------
	Pen penGrid
	(
		Color
		(
			theApp.ColorPlus->ARGBByName[GridColor]
		),
		(REAL)GridLineThicknessInches
	);

	penGrid.SetDashStyle(GridLineStyle);

	// -------------------------------------------------------------
	// Compute interior dimensions
	// -------------------------------------------------------------
	const double widthInches = rightInches - leftInches;
	const double heightInches = bottomInches - topInches;

	// -------------------------------------------------------------
	// Determine data ranges from tick arrays
	// -------------------------------------------------------------
	if (xTicks.size() < 2 || yTicks.size() < 2)
		return;

	const double xMin = xTicks.front();
	const double xMax = xTicks.back();

	const double yMin = yTicks.front();
	const double yMax = yTicks.back();

	const double xRange = xMax - xMin;
	const double yRange = yMax - yMin;

	if (xRange <= 0.0 || yRange <= 0.0)
		return;

	// -------------------------------------------------------------
	// Vertical grid lines (from xTicks)
	// -------------------------------------------------------------
	for (double xv : xTicks)
	{
		const double t = (xv - xMin) / xRange;
		const double x = leftInches + (widthInches * t);

		g.DrawLine
		(
			&penGrid,
			(REAL)x,
			(REAL)topInches,
			(REAL)x,
			(REAL)bottomInches
		);
	}

	// -------------------------------------------------------------
	// Horizontal grid lines (from yTicks)
	// -------------------------------------------------------------
	for (double yv : yTicks)
	{
		const double t = (yv - yMin) / yRange;
		const double y = bottomInches - (heightInches * t);

		g.DrawLine
		(
			&penGrid,
			(REAL)leftInches,
			(REAL)y,
			(REAL)rightInches,
			(REAL)y
		);
	}
} // DrawGrid

/////////////////////////////////////////////////////////////////////////////
// DrawTicks
//
// Draws tick marks along both axes using inch-based geometry. Tick
// length, line thickness, and color are user-configurable. Tick
// positions are defined by the tick arrays generated in RenderPlot.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//   bottomInches  Bottom boundary of the interior plot area
//   xTicks        Tick positions for the horizontal axis (data units)
//   yTicks        Tick positions for the vertical axis   (data units)
//
// NOTES:
//
//   • Uses TickLengthInches and LineThicknessInches.
//   • Tick arrays determine exact tick placement.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawTicks
(
	Graphics& g,
	double                      leftInches,
	double                      topInches,
	double                      rightInches,
	double                      bottomInches,
	const std::vector<double>& xTicks,
	const std::vector<double>& yTicks
)
{
	// -------------------------------------------------------------
	// Pen for tick marks
	// -------------------------------------------------------------
	Pen penTicks
	(
		Color
		(
			theApp.ColorPlus->ARGBByName[LineColor]
		),
		(REAL)LineThicknessInches
	);

	penTicks.SetDashStyle(LineStyle);

	// -------------------------------------------------------------
	// Compute interior dimensions
	// -------------------------------------------------------------
	const double widthInches = rightInches - leftInches;
	const double heightInches = bottomInches - topInches;

	// -------------------------------------------------------------
	// Determine data ranges from tick arrays
	// -------------------------------------------------------------
	if (xTicks.size() < 2 || yTicks.size() < 2)
		return;

	const double xMin = xTicks.front();
	const double xMax = xTicks.back();

	const double yMin = yTicks.front();
	const double yMax = yTicks.back();

	const double xRange = xMax - xMin;
	const double yRange = yMax - yMin;

	if (xRange <= 0.0 || yRange <= 0.0)
		return;

	// -------------------------------------------------------------
	// Horizontal axis ticks (bottom axis)
	// -------------------------------------------------------------
	for (double xv : xTicks)
	{
		const double t = (xv - xMin) / xRange;
		const double x = leftInches + (widthInches * t);

		g.DrawLine
		(
			&penTicks,
			(REAL)x,
			(REAL)bottomInches,
			(REAL)x,
			(REAL)(bottomInches + TickLengthInches)
		);
	}

	// -------------------------------------------------------------
	// Vertical axis ticks (left axis)
	// -------------------------------------------------------------
	for (double yv : yTicks)
	{
		const double t = (yv - yMin) / yRange;
		const double y = bottomInches - (heightInches * t);

		g.DrawLine
		(
			&penTicks,
			(REAL)leftInches,
			(REAL)y,
			(REAL)(leftInches - TickLengthInches),
			(REAL)y
		);
	}
} // DrawTicks

/////////////////////////////////////////////////////////////////////////////
// DrawAxisNumbers
//
// Renders numeric labels for both axes using inch-based geometry.
// Font size, color, and alignment are user-configurable. Numeric
// labels are placed at positions defined by the tick arrays
// generated in RenderPlot.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//   bottomInches  Bottom boundary of the interior plot area
//   xTicks        Tick positions for the horizontal axis (data units)
//   yTicks        Tick positions for the vertical axis   (data units)
//
// NOTES:
//
//   • Uses TickLabelFontSizePoints.
//   • Tick arrays determine exact label placement.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawAxisNumbers
(
	Graphics& g,
	double                      leftInches,
	double                      topInches,
	double                      rightInches,
	double                      bottomInches,
	const std::vector<double>& xTicks,
	const std::vector<double>& yTicks
)
{
	// -------------------------------------------------------------
	// Validate tick arrays
	// -------------------------------------------------------------
	if (xTicks.size() < 2 || yTicks.size() < 2)
		return;

	// -------------------------------------------------------------
	// Compute interior dimensions
	// -------------------------------------------------------------
	const double widthInches = rightInches - leftInches;
	const double heightInches = bottomInches - topInches;

	const double xMin = xTicks.front();
	const double xMax = xTicks.back();
	const double yMin = yTicks.front();
	const double yMax = yTicks.back();

	const double xRange = xMax - xMin;
	const double yRange = yMax - yMin;

	if (xRange <= 0.0 || yRange <= 0.0)
		return;

	// -------------------------------------------------------------
	// Font for numeric labels
	// -------------------------------------------------------------
	Gdiplus::Font fontNumbers
	(
		L"Segoe UI",
		(REAL)TickLabelFontSizePoints,
		FontStyleRegular,
		UnitPoint
	);

	// -------------------------------------------------------------
	// Brush for text
	// -------------------------------------------------------------
	SolidBrush brText
	(
		Color
		(
			theApp.ColorPlus->ARGBByName[LineColor]
		)
	);

	// -------------------------------------------------------------
	// Format for horizontal axis numbers (centered)
	// -------------------------------------------------------------
	StringFormat fmtCenter;
	fmtCenter.SetAlignment(StringAlignmentCenter);
	fmtCenter.SetLineAlignment(StringAlignmentNear);

	// -------------------------------------------------------------
	// Format for vertical axis numbers (right-aligned)
	// -------------------------------------------------------------
	StringFormat fmtRight;
	fmtRight.SetAlignment(StringAlignmentFar);
	fmtRight.SetLineAlignment(StringAlignmentCenter);

	// -------------------------------------------------------------
	// Horizontal axis numbers (bottom axis)
	// -------------------------------------------------------------
	for (double xv : xTicks)
	{
		const double t = (xv - xMin) / xRange;
		const double x = leftInches + (widthInches * t);

		WCHAR buf[64];
		swprintf_s(buf, L"%.6g", xv);

		RectF rcText
		(
			(REAL)(x - 0.25),
			(REAL)(bottomInches + TickLengthInches + 0.02),
			(REAL)0.50,
			(REAL)0.30
		);

		g.DrawString(buf, -1, &fontNumbers, rcText, &fmtCenter, &brText);
	}

	// -------------------------------------------------------------
	// Y-axis numbers (rotated)
	// -------------------------------------------------------------
	{
		const REAL fontSize = (REAL)TickLabelFontSizePoints;

		Gdiplus::Font font
		(
			L"Segoe UI",
			fontSize,
			FontStyleRegular,
			UnitPoint
		);

		SolidBrush br
		(
			Color
			(
				theApp.ColorPlus->ARGBByName[LineColor]
			)
		);

		StringFormat fmt;
		fmt.SetAlignment(StringAlignmentCenter);
		fmt.SetLineAlignment(StringAlignmentCenter);

		const double heightInches = bottomInches - topInches;
		const double yRange = yTicks.back() - yTicks.front();

		for (size_t i = 0; i < yTicks.size(); i++)
		{
			const double yv = yTicks[i];
			const double ty = (yv - yTicks.front()) / yRange;

			const double y = bottomInches - (heightInches * ty);

			GraphicsState state = g.Save();

			g.TranslateTransform
			(
				(REAL)(leftInches - 0.15),
				(REAL)y
			);

			g.RotateTransform(-90.0f);

			RectF rc
			(
				(REAL)-0.20f,
				(REAL)-0.10f,
				(REAL)0.40f,
				(REAL)0.20f
			);

			//std::wstring s = std::to_wstring((int)yv);
			CString s;
			s.Format(L"%g", yv);

			g.DrawString
			(
				s,
				-1,
				&font,
				rc,
				&fmt,
				&br
			);

			g.Restore(state);
		}
	}
} // DrawAxisNumbers

/////////////////////////////////////////////////////////////////////////////
// DrawCurve
//
// Renders the primary data curve using inch-based geometry. Line color,
// dash style, and thickness are user-configurable. Data points are mapped
// from user-provided arrays into the interior plot rectangle using the
// tick ranges generated in RenderPlot. This ensures Excel-style scaling
// and alignment with grid lines, axis numbers, and running-average curves.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//   bottomInches  Bottom boundary of the interior plot area
//   xTickMin      Minimum tick value on the X axis
//   xTickMax      Maximum tick value on the X axis
//   yTickMin      Minimum tick value on the Y axis
//   yTickMax      Maximum tick value on the Y axis
//
// NOTES:
//
//   • Uses LineColor, LineStyle, LineThicknessInches.
//   • Maps data using tick ranges for proper Excel-style scaling.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawCurve
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
)
{
	if (Years.empty() || Values.empty())
		return;

	const size_t count = min(Years.size(), Values.size());
	if (count < 2)
		return;

	Pen penCurve
	(
		Color
		(
			theApp.ColorPlus->ARGBByName[LineColor]
		),
		(REAL)LineThicknessInches
	);

	penCurve.SetDashStyle(LineStyle);

	const double widthInches = rightInches - leftInches;
	const double heightInches = bottomInches - topInches;

	const double xRange = xTickMax - xTickMin;
	const double yRange = yTickMax - yTickMin;

	if (xRange <= 0.0 || yRange <= 0.0)
		return;

	std::vector<PointF> pts;
	pts.reserve(count);

	for (size_t i = 0; i < count; i++)
	{
		const double xv = Years[i];
		const double yv = Values[i];

		const double tx = (xv - xTickMin) / xRange;
		const double ty = (yv - yTickMin) / yRange;

		const double x = leftInches + (widthInches * tx);
		const double y = bottomInches - (heightInches * ty);

		pts.emplace_back((REAL)x, (REAL)y);
	}

	if (pts.size() >= 2)
		g.DrawLines(&penCurve, pts.data(), (INT)pts.size());
} // DrawCurve

/////////////////////////////////////////////////////////////////////////////
// DrawRunningAvg
//
// Renders an optional 10‑year running‑average curve using inch‑based
// geometry. The running average is computed from the real data set by
// averaging each 10‑year window. The first valid point occurs at the
// tenth data element. All running‑average points are mapped into the
// interior plot rectangle using the tick ranges generated in RenderPlot.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//   bottomInches  Bottom boundary of the interior plot area
//
// NOTES:
//
//   • Uses TrendLineColor, TrendLineStyle, TrendLineThicknessInches.
//   • Computes a trailing 10‑year average (i.e., years i‑9 through i).
//   • Uses tick ranges for proper Excel‑style scaling.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawTrendLine
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
)
{
	CString csSubtype = Subtype;
	if (csSubtype == L"Stations")
	{
		return;
	}

	int nYears = (int)Years.size();
	if (Years.size() < 10 || Values.size() < 10)
		return;

	double dFirstYear = Years[0];

	Pen penAvg
	(
		Color
		(
			theApp.ColorPlus->ARGBByName[TrendLineColor]
		),
		(REAL)TrendLineThicknessInches
	);

	penAvg.SetDashStyle(TrendLineStyle);

	const size_t count = min(Years.size(), Values.size());
	const double widthInches = rightInches - leftInches;
	const double heightInches = bottomInches - topInches;

	const double xRange = xTickMax - xTickMin;
	const double yRange = yTickMax - yTickMin;

	if (xRange <= 0.0 || yRange <= 0.0)
		return;

	std::vector<PointF> pts;
	pts.reserve(count - 9);

	// -------------------------------------------------------------
	// Compute 10-year running average
	// -------------------------------------------------------------

	for (size_t i = 9; i < count; i++)
	{
		double sum = 0.0;
		for (size_t j = i - 9; j <= i; j++)
			sum += Values[j];

		const double avg = sum / 10.0;
		const double year = Years[i];

		const double tx = (year - xTickMin) / xRange;
		const double ty = (avg - yTickMin) / yRange;

		const double x = leftInches + (widthInches * tx);
		const double y = bottomInches - (heightInches * ty);

		pts.emplace_back((REAL)x, (REAL)y);
	}


	// -------------------------------------------------------------
	// Draw running-average curve
	// -------------------------------------------------------------
	INT nCount = (INT)pts.size();
	if (nCount >= 2)
	{

		g.DrawLines(&penAvg, pts.data(), nCount);

		{
			double dSumY = 0;
			for (auto& pt : pts)
			{
				dSumY += pt.Y;
			}

			double dTrendY = dSumY / nCount;
			PointF pt1(pts[0]);
			PointF pt2(pts.back());
			pt2.Y = dTrendY;

			Pen penTrend
			(
				Gdiplus::Color(Gdiplus::Color::Orange),
				(REAL)TrendLineThicknessInches
			);

			penTrend.SetDashStyle(Gdiplus::DashStyleDashDotDot);

			vector<PointF> ptsTrend;
			ptsTrend.push_back(pt1);
			ptsTrend.push_back(pt2);
			g.DrawLines(&penTrend, ptsTrend.data(), 2);
		}

		if ( nCount > 50)
		{
			double dSumY = 0;

			int nPt = 0;
			int nCnt = 0;
			for (auto& pt : pts)
			{
				if (nPt++ < 50)
				{
					continue;
				}
				dSumY += pt.Y;
				nCnt++;
			}

			double dTrendY = dSumY / nCnt;
			PointF pt1(pts[50]);
			PointF pt2(pts.back());
			pt2.Y = dTrendY;

			Pen penTrend
			(
				Gdiplus::Color(Gdiplus::Color::DarkCyan),
				(REAL)TrendLineThicknessInches
			);

			penTrend.SetDashStyle(Gdiplus::DashStyleSolid);

			vector<PointF> ptsTrend;
			ptsTrend.push_back(pt1);
			ptsTrend.push_back(pt2);
			g.DrawLines(&penTrend, ptsTrend.data(), 2);
		}
		if ( nCount > 100)
		{
			double dSumY = 0;

			int nPt = 0;
			int nCnt = 0;
			for (auto& pt : pts)
			{
				if (nPt++ < 100)
				{
					continue;
				}
				dSumY += pt.Y;
				nCnt++;
			}

			double dTrendY = dSumY / nCnt;
			PointF pt1(pts[100]);
			PointF pt2(pts.back());
			pt2.Y = dTrendY;

			Pen penTrend
			(
				Gdiplus::Color(Gdiplus::Color::DarkGreen),
				(REAL)TrendLineThicknessInches
			);

			penTrend.SetDashStyle(Gdiplus::DashStyleSolid);

			vector<PointF> ptsTrend;
			ptsTrend.push_back(pt1);
			ptsTrend.push_back(pt2);
			g.DrawLines(&penTrend, ptsTrend.data(), 2);
		}
	}
} // DrawTrendLine

/////////////////////////////////////////////////////////////////////////////
// DrawTitle
//
// Renders the plot title using inch-based geometry. The title is centered
// horizontally above the interior plot area and may include a legend
// describing the primary curve and optional running-average curve.
// Title font size, color, and alignment are user-configurable.
//
// PARAMETERS:
//
//   g             Graphics object (UnitInch mode)
//   leftInches    Left boundary of the interior plot area
//   topInches     Top boundary of the interior plot area
//   rightInches   Right boundary of the interior plot area
//
// NOTES:
//
//   • Uses TitleFontSizePoints and TitleColor.
//   • Title is rendered above the plot area, centered horizontally.
//   • Legend text is included directly in the title.
//   • All measurements are in inches.
//
/////////////////////////////////////////////////////////////////////////////
void CGraphPlotter::DrawTitle
(
	Graphics& g,
	double     leftInches,
	double     topInches,
	double     rightInches
)
{
	const REAL fontSize = (REAL)TitleFontSizePoints;

	Gdiplus::Font font
	(
		L"Segoe UI",
		fontSize,
		FontStyleBold,
		UnitPoint
	);

	SolidBrush br
	(
		Color
		(
			theApp.ColorPlus->ARGBByName[LineColor]
		)
	);

	StringFormat fmt;
	fmt.SetAlignment(StringAlignmentCenter);
	fmt.SetLineAlignment(StringAlignmentNear);

	const double width = rightInches - leftInches;

	// account for font size of the title and optional trend line
	// label on second line
	REAL verticalOffset = TopPaddingInches;

	RectF rc
	(
		(REAL)leftInches,
		(REAL)(topInches - verticalOffset),
		(REAL)width,
		(REAL)verticalOffset
	);

	CString csTitle = GraphTitle;

	g.DrawString
	(
		csTitle,
		-1,
		&font,
		rc,
		&fmt,
		&br
	);

	// modify the title to indicate the running avg line's presense and color
	// on a second line
	{
		SolidBrush br
		(
			Color
			(
				theApp.ColorPlus->ARGBByName[TrendLineColor]
			)
		);

		g.DrawString
		(
			L"\nwith 10 Year Running Average",
			-1,
			&font,
			rc,
			&fmt,
			&br
		);
	}

} // DrawTitle

/////////////////////////////////////////////////////////////////////////////
// CreatePlot
//
// Public entry point for generating the plot image. This method
// calls the protected RenderPlot function, which performs all
// rendering using inch-based geometry and the configured plot
// properties.
//
// PARAMETERS:
//
//   rcPixels   Pixel rectangle defining the output bitmap size.
//
// RETURNS:
//
//   std::unique_ptr<Bitmap> containing the rendered plot.
//
/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Bitmap> CGraphPlotter::CreatePlot(const CRect& rcPixels)
{
	return RenderPlot(rcPixels);

} // CreatePlot

/////////////////////////////////////////////////////////////////////////////
