/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "QuerySQL.h"
#include "ClimateDatabase.h"
#include "KeyedCollection.h"
#include "ClimateTemperature.h"
#include "NaturalLanguage.h"
#include "ClimateYear.h"

/////////////////////////////////////////////////////////////////////////////
// 
// 
// 
/////////////////////////////////////////////////////////////////////////////
class CQueryEngine
{
// protected data
protected:
	// a pointer to the existing climate database
	CClimateDatabase* m_pDB;
	bool m_bDebugMode;

	// generic SQL query class
	CQuerySQL m_sql;

// public properties
public:
	// a pointer to the existing climate database
	inline CClimateDatabase* GetDatabase()
	{
		return m_pDB;
	}
	// a pointer to the existing climate database
	inline void SetDatabase(CClimateDatabase* value)
	{
		m_pDB = value;
	}
	// a pointer to the existing climate database
	__declspec(property(get = GetDatabase, put = SetDatabase))
		CClimateDatabase* Database;

	// generic SQL query class
	inline CQuerySQL* GetQuerySQL()
	{
		return &m_sql;
	}
	// generic SQL query class
	__declspec(property(get = GetQuerySQL))
		CQuerySQL* QuerySQL;

	bool GetDebugMode()
	{
		return m_bDebugMode;
	}

	void SetDebugMode(bool value)
	{
		m_bDebugMode = value;
	}

	__declspec(property(get = GetDebugMode, put = SetDebugMode))
		bool DebugMode;

// protected methods
protected:
	CString BuildStatisticsTemperatureSQL(const CNaturalLanguage& nlp);
	CString BuildStatisticsStationsSQL(const CNaturalLanguage& nlp);
	CString BuildStatisticsReadingsSQL(const CNaturalLanguage& nlp);
	CString BuildPercentMultiThresholdCountsSQL(const CNaturalLanguage& nlp);
	CString FormatCSV
	(
		const CSmartArray<CString>& arrColumns,
		const CSmartArray<CSmartArray<CString>>& arrRows
	);

// public methods
public:
	// ---------------------------------------------------------
	// Phase‑3 handler forward declarations
	// ---------------------------------------------------------

	CString HandleDumpQueries(CNaturalLanguage& nlp);

	CString HandleRawSQL(const CString& csQuery);
	CString HandleVersion();
	CString HandleSchema();

	CString HandleStationSummary(const CNaturalLanguage& nlp);
	CString HandleStationReproduceDecade(const CNaturalLanguage& nlp);

	CString HandleAllStations();
	CString HandleStationsByState(const CNaturalLanguage& nlp);
	CString HandleActiveStations(const CNaturalLanguage& nlp);

	CString HandleMonthlyTemperatures(const CNaturalLanguage& nlp);
	CString HandleStateTrend(const CNaturalLanguage& nlp);
	CString HandleExplainDSFlags(const CNaturalLanguage& /*nlp*/);
	CString HandleUnknownIntent(const CString& csQuery);

	CString HandleAnnualTemperatures(const CNaturalLanguage& nlp);
	CString HandleStatisticsTemperature(const CNaturalLanguage& nlp);
	CString HandleStatisticsStations(const CNaturalLanguage& nlp);
	CString HandleStatisticsReadings(const CNaturalLanguage& nlp);
	CString HandleStatisticsAll(const CNaturalLanguage& nlp);

	CString HandlePercentThresholdCounts(const CNaturalLanguage& nlp);
	CString HandlePercentMultiThresholdCounts(const CNaturalLanguage& nlp);

	CString HandleVerifyYears(const CNaturalLanguage& nlp);


	bool StationExists(const CString& csStationID);

	bool ProcessQuery(const CString& csUserQuery, CString& csResult);

	CString HandleQCImpact(const CNaturalLanguage& nlp);

	double ConvertUnits(short rawValue, CNaturalLanguage::UnitType units);

	CString FormatValue(double value, CNaturalLanguage::UnitType units);

	CString PrepareOutputValue(short rawValue, CNaturalLanguage::UnitType units);

	double ConvertUnitsFromDouble(double rawValue, CNaturalLanguage::UnitType units);

	CString OutputCSV(CKeyedCollection<CString, CClimateYear>& ClimateYears);


// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CQueryEngine(CClimateDatabase* pDB);
	~CQueryEngine()
	{

	}
}; // class CQueryEngine

/////////////////////////////////////////////////////////////////////////////
