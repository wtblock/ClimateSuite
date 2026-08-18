/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <vector>

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
class CGraphPlotter
{
// protected data
protected:
	// in GraphPlotter.h
	std::vector<uint8_t> m_arrPNG;

	// Query Type (Picker, Natural Language, or SQL)
	CString             m_csQueryType;
	// when true, estimated data is ignored in addition to QC flagged data
	bool                m_bPure;
	// Scope (National, State, and Location)
	CString             m_csScope;
	// The year when the query begins
	int                 m_nYearStart;
	// The year when the query ends
	int                 m_nYearEnd;
	// Subtype of query (Maximum, Minimum, Average, Threshold, and Stations)
	CString             m_csSubtype;
	// threshold temperature in the current units selection
	// for Subtype Threshold
	int                 m_nThreshold;
	// Units (degF, degC, or raw)
	CString             m_csUnits;
	// Output (Plot, Table, or Map+Plot
	CString             m_csOutput;
	// the SQL created by the above
	CString             m_csSQL;

	// -------------------------------------------------------------
	// Graph text properties
	// -------------------------------------------------------------
	CString             m_csGraphTitle;
	CString             m_csAxisLabelX;
	CString             m_csAxisLabelY;

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	CString             m_csLineColor;
	Gdiplus::DashStyle  m_dsLineStyle;
	double              m_fLineThicknessInches;

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	BOOL                m_bTrendLine;
	CString             m_csTrendLineColor;
	Gdiplus::DashStyle  m_dsTrendLineStyle;
	double              m_fTrendLineThicknessInches;

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	CString             m_csGridColor;
	Gdiplus::DashStyle  m_dsGridLineStyle;
	double              m_fGridLineThicknessInches;

	// -------------------------------------------------------------
	// Text sizes (points)
	// -------------------------------------------------------------
	long                m_nTitleFontSizePoints;
	long                m_nAxisLabelFontSizePoints;
	long                m_nTickLabelFontSizePoints;

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
	double             m_fTickLengthInches;

	// Layout (Full, Half, or Quarter page)
	CString m_csLayout;

	std::vector<double> m_arrYears;
	std::vector<double> m_arrValues;

	CString m_csStation;
	CString m_csState;
	CString m_csLocation;
	float m_fLatitude;
	float m_fLongitude;

// public properties
public:
	/////////////////////////////////////////////////////////////////////////////
	// Query Properties
	/////////////////////////////////////////////////////////////////////////////
	
	// Query Type (Picker, Natural Language, or SQL)
	CString GetQueryType()
	{
		return m_csQueryType;
	}
	// Query Type (Picker, Natural Language, or SQL)
	void SetQueryType(CString value)
	{
		m_csQueryType = value;
	}
	// Query Type (Picker, Natural Language, or SQL)
	__declspec(property(get = GetQueryType, put = SetQueryType))
		CString QueryType;

	// when true, estimated data is ignored in addition to QC flagged data
	bool GetPure()
	{
		return m_bPure;
	}
	// when true, estimated data is ignored in addition to QC flagged data
	void SetPure(bool value)
	{
		m_bPure = value;
	}
	// when true, estimated data is ignored in addition to QC flagged data
	__declspec(property(get = GetPure, put = SetPure))
		bool Pure;

	// Scope (National, State, and Location)
	CString GetScope()
	{
		return m_csScope;
	}
	// Scope (National, State, and Location)
	void SetScope(CString value)
	{
		m_csScope = value;
	}
	// Scope (National, State, and Location)
	__declspec(property(get = GetScope, put = SetScope))
		CString Scope;

	// The year when the query begins
	int GetYearStart()
	{
		return m_nYearStart;
	}
	// The year when the query begins
	void SetYearStart(int value)
	{
		m_nYearStart = value;
	}
	// The year when the query begins
	__declspec(property(get = GetYearStart, put = SetYearStart))
		int YearStart;

	// The year when the query ends
	int GetYearEnd()
	{
		return m_nYearEnd;
	}
	// The year when the query ends
	void SetYearEnd(int value)
	{
		m_nYearEnd = value;
	}
	// The year when the query ends
	__declspec(property(get = GetYearEnd, put = SetYearEnd))
		int YearEnd;

	// Subtype of query (Maximum, Minimum, Average, Threshold, and Stations)
	CString GetSubtype()
	{
		return m_csSubtype;
	}
	// Subtype of query (Maximum, Minimum, Average, Threshold, and Stations)
	void SetSubtype(CString value)
	{
		m_csSubtype = value;
	}
	// Subtype of query (Maximum, Minimum, Average, Threshold, and Stations)
	__declspec(property(get = GetSubtype, put = SetSubtype))
		CString Subtype;

	// threshold temperature in the current units selection
	// for Subtype Threshold
	int GetThreshold()
	{
		return m_nThreshold;
	}
	// threshold temperature in the current units selection
	// for Subtype Threshold
	void SetThreshold(int value)
	{
		m_nThreshold = value;
	}
	// threshold temperature in the current units selection
	// for Subtype Threshold
	__declspec(property(get = GetThreshold, put = SetThreshold))
		int Threshold;

	// Units (degF, degC, or raw)
	CString GetUnits()
	{
		return m_csUnits;
	}
	// Units (degF, degC, or raw)
	void SetUnits(CString value)
	{
		m_csUnits = value;
	}
	// Units (degF, degC, or raw)
	__declspec(property(get = GetUnits, put = SetUnits))
		CString Units;

	// Output (Plot, Table, or Map+Plot
	CString GetOutput()
	{
		return m_csOutput;
	}
	// Output (Plot, Table, or Map+Plot
	void SetOutput(CString value)
	{
		m_csOutput = value;
	}
	// Output (Plot, Table, or Map+Plot
	__declspec(property(get = GetOutput, put = SetOutput))
		CString Output;

	// the SQL created by the above
	CString GetSQL()
	{
		return m_csSQL;
	}
	// the SQL created by the above
	void SetSQL(CString value)
	{
		m_csSQL = value;
	}
	// the SQL created by the above
	__declspec(property(get = GetSQL, put = SetSQL))
		CString SQL;

	/////////////////////////////////////////////////////////////////////////////
	// Station data
	/////////////////////////////////////////////////////////////////////////////
	
	// Station ID
	CString GetStation()
	{
		return m_csStation;
	}
	// Station ID
	void SetStation(CString value)
	{
		m_csStation = value;
	}
	// Station ID
	__declspec(property(get = GetStation, put = SetStation))
		CString Station;

	// State postal code (AL, TX, CA, etc.) location of station
	CString GetState()
	{
		return m_csState;
	}
	// State postal code (AL, TX, CA, etc.) location of station
	void SetState(CString value)
	{
		m_csState = value;
	}
	// State postal code (AL, TX, CA, etc.) location of station
	__declspec(property(get = GetState, put = SetState))
		CString State;

	// Location is the name of the cite the station is in
	CString GetLocation()
	{
		return m_csLocation;
	}
	// Location is the name of the cite the station is in
	void SetLocation(CString value)
	{
		m_csLocation = value;
	}
	// Location is the name of the cite the station is in
	__declspec(property(get = GetLocation, put = SetLocation))
		CString Location;

	// station latitude
	float GetLatitude()
	{
		return m_fLatitude;
	}
	// station latitude
	void SetLatitude(float value)
	{
		m_fLatitude = value;
	}
	// station latitude
	__declspec(property(get = GetLatitude, put = SetLatitude))
		float Latitude;

	// station longitude
	float GetLongitude()
	{
		return m_fLongitude;
	}
	// station longitude
	void SetLongitude(float value)
	{
		m_fLongitude = value;
	}
	// station longitude
	__declspec(property(get = GetLongitude, put = SetLongitude))
		float Longitude;


	/////////////////////////////////////////////////////////////////////////////
	// Graphing data
	/////////////////////////////////////////////////////////////////////////////

	std::vector<uint8_t>& GetBytesPNG()
	{
		return m_arrPNG;
	}

	void SetBytesPNG(std::vector<uint8_t>& value)
	{
		m_arrPNG = value;
	}

	__declspec(property(get = GetBytesPNG, put = SetBytesPNG))
		std::vector<uint8_t> BytesPNG;

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
	// The title of the graph
	CString GetGraphTitle()
	{
		return m_csGraphTitle;
	}
	// The title of the graph
	void SetGraphTitle(CString value)
	{
		m_csGraphTitle = value;
	}
	// The title of the graph
	__declspec(property(get = GetGraphTitle, put = SetGraphTitle))
		CString GraphTitle;

	// The label of the values on the X axis
	CString GetAxisLabelX()
	{
		return m_csAxisLabelX;
	}
	// The label of the values on the X axis
	void SetAxisLabelX(CString value)
	{
		m_csAxisLabelX = value;
	}
	// The label of the values on the X axis
	__declspec(property(get = GetAxisLabelX, put = SetAxisLabelX))
		CString AxisLabelX;

	// The label of the values on the Y axis
	CString GetAxisLabelY()
	{
		return m_csAxisLabelY;
	}
	// The label of the values on the Y axis
	void SetAxisLabelY(CString value)
	{
		m_csAxisLabelY = value;
	}
	// The label of the values on the Y axis
	__declspec(property(get = GetAxisLabelY, put = SetAxisLabelY))
		CString AxisLabelY;


	// -------------------------------------------------------------
	// LineColor
	// -------------------------------------------------------------
	CString GetLineColor()
	{
		return m_csLineColor;
	}

	void SetLineColor(CString value)
	{
		m_csLineColor = value;
	}

	__declspec(property(get = GetLineColor, put = SetLineColor))
		CString LineColor;

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
	CString GetTrendLineColor()
	{
		return m_csTrendLineColor;
	}

	void SetTrendLineColor(CString value)
	{
		m_csTrendLineColor = value;
	}

	__declspec(property(get = GetTrendLineColor, put = SetTrendLineColor))
		CString TrendLineColor;

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
	CString GetGridColor()
	{
		return m_csGridColor;
	}

	void SetGridColor(CString value)
	{
		m_csGridColor = value;
	}

	__declspec(property(get = GetGridColor, put = SetGridColor))
		CString GridColor;

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
	long GetTitleFontSizePoints()
	{
		return m_nTitleFontSizePoints;
	}

	void SetTitleFontSizePoints(long value)
	{
		m_nTitleFontSizePoints = value;
	}

	__declspec(property(get = GetTitleFontSizePoints, put = SetTitleFontSizePoints))
		long TitleFontSizePoints;

	// -------------------------------------------------------------
	// AxisLabelFontSizePoints
	// -------------------------------------------------------------
	long GetAxisLabelFontSizePoints()
	{
		return m_nAxisLabelFontSizePoints;
	}

	void SetAxisLabelFontSizePoints(long value)
	{
		m_nAxisLabelFontSizePoints = value;
	}

	__declspec(property(get = GetAxisLabelFontSizePoints, put = SetAxisLabelFontSizePoints))
		long AxisLabelFontSizePoints;

	// -------------------------------------------------------------
	// TickLabelFontSizePoints
	// -------------------------------------------------------------
	long GetTickLabelFontSizePoints()
	{
		return m_nTickLabelFontSizePoints;
	}

	void SetTickLabelFontSizePoints(long value)
	{
		m_nTickLabelFontSizePoints = value;
	}

	__declspec(property(get = GetTickLabelFontSizePoints, put = SetTickLabelFontSizePoints))
		long TickLabelFontSizePoints;

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

	// -------------------------------------------------------------
	// Layout (Full, Half, or Quarter page)
	// -------------------------------------------------------------
	CString GetLayout()
	{
		return m_csLayout;
	}
	// Layout (Full, Half, or Quarter page)
	void SetLayout(CString value)
	{
		m_csLayout = value;
	}
	// Layout (Full, Half, or Quarter page)
	__declspec(property(get = GetLayout, put = SetLayout))
		CString Layout;

	// -------------------------------------------------------------
	// RenderPlot
	// -------------------------------------------------------------
	std::unique_ptr<Gdiplus::Bitmap> RenderPlot(const CRect& rcPixels);

// protected methods
protected:
	// -------------------------------------------------------------
	// Initialize default properties
	// -------------------------------------------------------------
	void SetDefaults(CClimateExplorerDoc* pDoc);

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
	void DrawTrendLine
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
	CGraphPlotter()
	{

	};
	CGraphPlotter(CClimateExplorerDoc* pDoc);
	CGraphPlotter
	(
		CClimateExplorerDoc* pDoc,
		std::vector<double> years, 
		std::vector<double>values
	);
	virtual ~CGraphPlotter();

}; // CGraphPlotter

/////////////////////////////////////////////////////////////////////////////
