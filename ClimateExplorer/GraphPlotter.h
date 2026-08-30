/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ClimateTemperature.h"
#include "NaturalLanguage.h"
#include "SmartArray.h"
#include <memory>
#include <vector>

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
class CGraphPlotter
{
	// public definitions
public:
	/////////////////////////////////////////////////////////////////////////////
	// Row structures for each Subtype
	/////////////////////////////////////////////////////////////////////////////

	struct CClimateTempRow
	{
		int     nYear;
		//int     nMonth;
		double  dTemperature;

		CClimateTempRow()
			: nYear(0)
			//, nMonth(0)
			, dTemperature(0.0)
		{
		}
	};

	struct CClimateThresholdRow
	{
		int     nYear;
		double  dPercent;

		CClimateThresholdRow()
			: nYear(0)
			, dPercent(0.0)
		{
		}
	};

	struct CClimateStationRow
	{
		int     nYear;
		int     nCount;

		CClimateStationRow()
			: nYear(0)
			, nCount(0)
		{
		}
	};


// protected data
protected:
	// in GraphPlotter.h
	std::vector<uint8_t> m_arrPNG;

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
	// Type (Maximum, Minimum, Average)
	CClimateTemperature::MEASURE_TYPE m_eMeasurementType;
	// Units (degF, degC, or raw)
	CString             m_csUnits;
	// Output (Plot, Table, or Map+Plot
	CString             m_csOutput;
	// the SQL created by the above
	CString             m_csSQL;

	// -------------------------------------------------------------
	// Graph text properties
	// -------------------------------------------------------------
	CString             m_csContentTitle;
	CString             m_csAxisLabelX;
	CString             m_csAxisLabelY;

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	CString             m_csLineColor;
	Gdiplus::DashStyle  m_dsLineStyle;
	double              m_fLineThicknessInches;

	// -------------------------------------------------------------
	// Running Average line appearance
	// -------------------------------------------------------------
	CString             m_csRunningAvgColor;
	Gdiplus::DashStyle  m_dsRunningAvgStyle;
	double              m_fRunningAvgThicknessInches;

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
	double              m_fTickLengthInches;

	// -------------------------------------------------------------
	// Trending properties
	// -------------------------------------------------------------
	bool				m_bTrendOneEnable;
	CString				m_csTrendOneColor;
	Gdiplus::DashStyle	m_dsTrendOneStyle;
	double				m_dTrendOneThickness;
	long				m_lTrendOneYear;

	bool				m_bTrendTwoEnable;
	CString				m_csTrendTwoColor;
	Gdiplus::DashStyle	m_dsTrendTwoStyle;
	double				m_dTrendTwoThickness;
	long				m_lTrendTwoYear;

	bool				m_bTrendThreeEnable;
	CString				m_csTrendThreeColor;
	Gdiplus::DashStyle	m_dsTrendThreeStyle;
	double				m_dTrendThreeThickness;
	long				m_lTrendThreeYear;

	// Layout (Full, Half, or Quarter page)
	CString m_csLayout;

	std::vector<double> m_arrYears;
	std::vector<double> m_arrValues;

	CString m_csStation;
	CString m_csState;
	CString m_csLocation;
	float m_fLatitude;
	float m_fLongitude;

	// ---------------------------------------------------------------------
	// Temperature results: Year, Month, Temperature
	// ---------------------------------------------------------------------
	std::vector<CClimateTempRow> m_arrTemperatureRows;

	// ---------------------------------------------------------------------
	// Threshold results: Year, Percent
	// ---------------------------------------------------------------------
	std::vector<CClimateThresholdRow> m_arrThresholdRows;

	// ---------------------------------------------------------------------
	// Station count results: Year, Count
	// ---------------------------------------------------------------------
	std::vector<CClimateStationRow> m_arrStationRows;

// public properties
public:
	/////////////////////////////////////////////////////////////////////////////
	// Query Properties
	/////////////////////////////////////////////////////////////////////////////
	
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

	// Type (Maximum, Minimum, Average)
	CClimateTemperature::MEASURE_TYPE GetMeasurementType()
	{
		return m_eMeasurementType;
	}
	// Type (Maximum, Minimum, Average)
	void SetMeasurementType(CClimateTemperature::MEASURE_TYPE value)
	{
		m_eMeasurementType = value;
	}
	// Type (Maximum, Minimum, Average)
	__declspec(property(get = GetMeasurementType, put = SetMeasurementType))
		CClimateTemperature::MEASURE_TYPE MeasurementType;

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

	// natural language unit type
	CNaturalLanguage::UnitType GetUnitType()
	{
		CNaturalLanguage::UnitType value = CNaturalLanguage::UnitType::DegF;
		CString csUnits = Units.MakeLower();
		if (csUnits == L"degc")
		{
			value = CNaturalLanguage::UnitType::DegC;
		}
		else if (csUnits == L"raw")
		{
			value = CNaturalLanguage::UnitType::Raw;
		}

		return value;
	}
	// natural language unit type
	void SetUnitType(CNaturalLanguage::UnitType value)
	{
		CString csUnits = L"degF";
		switch (value)
		{
		case CNaturalLanguage::UnitType::DegF:
			csUnits = L"degF";
			break;
		case CNaturalLanguage::UnitType::DegC:
			csUnits = L"degC";
			break;
		case CNaturalLanguage::UnitType::Raw:
			csUnits = L"raw";
		}

		Units = csUnits;
	}
	// natural language unit type
	__declspec(property(get = GetUnitType, put = SetUnitType))
		CNaturalLanguage::UnitType UnitType;

	// format the given value based on current units
	CString GetFormatValue(double value)
	{
		CString csOut;

		// Format using right‑justified, fixed width
		switch (UnitType)
		{
		case CNaturalLanguage::UnitType::Raw:
			csOut.Format(L"% 6.0f", value);
			break;
		case CNaturalLanguage::UnitType::DegC:
			csOut.Format(L"% 7.2f", value);
			break;
		case CNaturalLanguage::UnitType::DegF:
			csOut.Format(L"% 8.3f", value);
			break;
		}

		return csOut;
	}
	// format the given value based on current units
	__declspec(property(get = GetFormatValue))
		CString FormatValue[];

	// convert from raw units to the user's choice
	double GetConvertUnits(double dRaw)
	{
		double value = dRaw;
		CNaturalLanguage::UnitType eType = UnitType;
		switch (eType)
		{
		case CNaturalLanguage::UnitType::DegC:
			value /= 100;
			break;
		case CNaturalLanguage::UnitType::DegF:
			value /= 100;
			value *= 1.8f;
			value += 32.0f;
		}
		return value;
	}
	// convert from raw units to the user's choice
	__declspec(property(get = GetConvertUnits))
		double ConvertUnits[];

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

	// Layout (Full, Half, or Quarter page)
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
	// 
	// The title of the graph
	CString GetContentTitle();
	// The title of the graph
	void SetContentTitle(CString value)
	{
		m_csContentTitle = value;
	}
	// The title of the graph
	__declspec(property(get = GetContentTitle, put = SetContentTitle))
		CString ContentTitle;


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
	// RunningAvgColor
	// -------------------------------------------------------------
	CString GetRunningAvgColor()
	{
		return m_csRunningAvgColor;
	}

	void SetRunningAvgColor(CString value)
	{
		m_csRunningAvgColor = value;
	}

	__declspec(property(get = GetRunningAvgColor, put = SetRunningAvgColor))
		CString RunningAvgColor;

	// -------------------------------------------------------------
	// RunningAvgStyle
	// -------------------------------------------------------------
	Gdiplus::DashStyle GetRunningAvgStyle()
	{
		return m_dsRunningAvgStyle;
	}

	void SetRunningAvgStyle(Gdiplus::DashStyle value)
	{
		m_dsRunningAvgStyle = value;
	}

	__declspec(property(get = GetRunningAvgStyle, put = SetRunningAvgStyle))
		Gdiplus::DashStyle RunningAvgStyle;

	// -------------------------------------------------------------
	// RunningAvgThicknessInches
	// -------------------------------------------------------------
	double GetRunningAvgThicknessInches()
	{
		return m_fRunningAvgThicknessInches;
	}

	void SetRunningAvgThicknessInches(double value)
	{
		m_fRunningAvgThicknessInches = value;
	}

	__declspec(property(get = GetRunningAvgThicknessInches, put = SetRunningAvgThicknessInches))
		double RunningAvgThicknessInches;

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
	// Trend curve properties
	// -------------------------------------------------------------
	
	// Trend One Enable
	bool GetTrendOneEnable()
	{
		return m_bTrendOneEnable;
	}
	// Trend One Enable
	void SetTrendOneEnable(bool value)
	{
		m_bTrendOneEnable = value;
	}
	// Trend One Enable
	__declspec(property(get = GetTrendOneEnable, put = SetTrendOneEnable))
		bool TrendOneEnable;

	// Trend One Color
	CString GetTrendOneColor()
	{
		return m_csTrendOneColor;
	}

	// Trend One Color
	void SetTrendOneColor(CString value)
	{
		m_csTrendOneColor = value;
	}

	// Trend One Color
	__declspec(property(get = GetTrendOneColor, put = SetTrendOneColor))
		CString TrendOneColor;

	// Trend One Style
	Gdiplus::DashStyle GetTrendOneStyle()
	{
		return m_dsTrendOneStyle;
	}

	// Trend One Style
	void SetTrendOneStyle(Gdiplus::DashStyle value)
	{
		m_dsTrendOneStyle = value;
	}

	// Trend One Style
	__declspec(property(get = GetTrendOneStyle, put = SetTrendOneStyle))
		Gdiplus::DashStyle TrendOneStyle;

	// Trend One Thickness
	double GetTrendOneThickness()
	{
		return m_dTrendOneThickness;
	}

	// Trend One Thickness
	void SetTrendOneThickness(double value)
	{
		m_dTrendOneThickness = value;
	}

	// Trend One Thickness
	__declspec(property(get = GetTrendOneThickness, put = SetTrendOneThickness))
		double TrendOneThickness;

	// Trend One Year
	long GetTrendOneYear()
	{
		return m_lTrendOneYear;
	}

	// Trend One Year
	void SetTrendOneYear(long value)
	{
		m_lTrendOneYear = value;
	}

	// Trend One Year
	__declspec(property(get = GetTrendOneYear, put = SetTrendOneYear))
		long TrendOneYear;

	// Trend Two Enable
	bool GetTrendTwoEnable()
	{
		return m_bTrendTwoEnable;
	}
	// Trend Two Enable
	void SetTrendTwoEnable(bool value)
	{
		m_bTrendTwoEnable = value;
	}
	// Trend Two Enable
	__declspec(property(get = GetTrendTwoEnable, put = SetTrendTwoEnable))
		bool TrendTwoEnable;

	// Trend Two Color
	CString GetTrendTwoColor()
	{
		return m_csTrendTwoColor;
	}

	// Trend Two Color
	void SetTrendTwoColor(CString value)
	{
		m_csTrendTwoColor = value;
	}

	// Trend Two Color
	__declspec(property(get = GetTrendTwoColor, put = SetTrendTwoColor))
		CString TrendTwoColor;

	// Trend Two Style
	Gdiplus::DashStyle GetTrendTwoStyle()
	{
		return m_dsTrendTwoStyle;
	}

	// Trend Two Style
	void SetTrendTwoStyle(Gdiplus::DashStyle value)
	{
		m_dsTrendTwoStyle = value;
	}

	// Trend Two Style
	__declspec(property(get = GetTrendTwoStyle, put = SetTrendTwoStyle))
		Gdiplus::DashStyle TrendTwoStyle;

	// Trend Two Thickness
	double GetTrendTwoThickness()
	{
		return m_dTrendTwoThickness;
	}

	// Trend Two Thickness
	void SetTrendTwoThickness(double value)
	{
		m_dTrendTwoThickness = value;
	}

	// Trend Two Thickness
	__declspec(property(get = GetTrendTwoThickness, put = SetTrendTwoThickness))
		double TrendTwoThickness;

	// Trend Two Year
	long GetTrendTwoYear()
	{
		return m_lTrendTwoYear;
	}

	// Trend Two Year
	void SetTrendTwoYear(long value)
	{
		m_lTrendTwoYear = value;
	}

	// Trend Two Year
	__declspec(property(get = GetTrendTwoYear, put = SetTrendTwoYear))
		long TrendTwoYear;

	// Trend Three Enable
	bool GetTrendThreeEnable()
	{
		return m_bTrendThreeEnable;
	}
	// Trend Three Enable
	void SetTrendThreeEnable(bool value)
	{
		m_bTrendThreeEnable = value;
	}
	// Trend Three Enable
	__declspec(property(get = GetTrendThreeEnable, put = SetTrendThreeEnable))
		bool TrendThreeEnable;

	// Trend Three Color
	CString GetTrendThreeColor()
	{
		return m_csTrendThreeColor;
	}

	// Trend Three Color
	void SetTrendThreeColor(CString value)
	{
		m_csTrendThreeColor = value;
	}

	// Trend Three Color
	__declspec(property(get = GetTrendThreeColor, put = SetTrendThreeColor))
		CString TrendThreeColor;

	// Trend Three Style
	Gdiplus::DashStyle GetTrendThreeStyle()
	{
		return m_dsTrendThreeStyle;
	}

	// Trend Three Style
	void SetTrendThreeStyle(Gdiplus::DashStyle value)
	{
		m_dsTrendThreeStyle = value;
	}

	// Trend Three Style
	__declspec(property(get = GetTrendThreeStyle, put = SetTrendThreeStyle))
		Gdiplus::DashStyle TrendThreeStyle;

	// Trend Three Thickness
	double GetTrendThreeThickness()
	{
		return m_dTrendThreeThickness;
	}

	// Trend Three Thickness
	void SetTrendThreeThickness(double value)
	{
		m_dTrendThreeThickness = value;
	}

	// Trend Three Thickness
	__declspec(property(get = GetTrendThreeThickness, put = SetTrendThreeThickness))
		double TrendThreeThickness;

	// Trend Three Year
	long GetTrendThreeYear()
	{
		return m_lTrendThreeYear;
	}

	// Trend Three Year
	void SetTrendThreeYear(long value)
	{
		m_lTrendThreeYear = value;
	}

	// Trend Three Year
	__declspec(property(get = GetTrendThreeYear, put = SetTrendThreeYear))
		long TrendThreeYear;


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

	// return the slope and intercept of the given data points
	// and the starting index
	std::vector<PointF> OrdinaryLeastSquares
	(
		std::vector<PointF>& pts,
		size_t nStart
	);

	// -------------------------------------------------------------
	// DrawRunningAvg
	// -------------------------------------------------------------
	void DrawRunningAvg
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

	// -------------------------------------------------------------
	// Conversion methods
	// -------------------------------------------------------------
	void ConvertTemperatureRows(const CSmartArray<CSmartArray<CString>>& arrRaw);
	void ConvertTemperatureRows
	(
		const CSmartArray<CSmartArray<CString>>& arrRaw,
		vector<double>& outYears,
		vector<double>& outValues
	);

	void ConvertThresholdRows(const CSmartArray<CSmartArray<CString>>& arrRaw);
	void ConvertThresholdRows
	(
		const CSmartArray<CSmartArray<CString>>& arrRaw,
		vector<double>& outYears,
		vector<double>& outValues
	);

	void ConvertStationRows(const CSmartArray<CSmartArray<CString>>& arrRaw);
	void ConvertStationRows
	(
		const CSmartArray<CSmartArray<CString>>& arrRaw,
		vector<double>& outYears,
		vector<double>& outValues
	);

	void FormatTemperatureText();
	void FormatThresholdText();
	void FormatStationText();


// public methods
public:
	// -------------------------------------------------------------
	// allow the document to get some defaults from here
	// -------------------------------------------------------------
	void GetDefaults(CClimateExplorerDoc* pDoc);

	std::unique_ptr<Bitmap> RenderPlot(const CRect& rcPixels);

	std::shared_ptr<Bitmap> CreatePlot(const CRect& rcPixels);

	CString BuildPickerSQL();

	void ExecutePickerQuery();

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
