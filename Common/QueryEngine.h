/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "QuerySQL.h"
#include "ClimateDatabase.h"
#include "KeyedCollection.h"

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

	// generic SQL query class
	CQuerySQL m_sql;

	// map of state names vs. postal codes
	CKeyedCollection<CString, CString> m_mapStates;

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

	// postal code
	inline CString GetPostalCode( CString csState)
	{
		if (m_mapStates.Exists[csState])
		{
			return *m_mapStates.find(csState);
		}
		return csState;
	}
	// postal code
	inline void SetPostalCode(CString csState, CString csCode)
	{
		m_mapStates.add(csState, shared_ptr<CString>(new CString(csCode)));
	}
	// postal code
	__declspec(property(get = GetPostalCode, put = SetPostalCode))
		CString PostalCode[];

// protected methods
protected:
	bool DetectStationID(const CString& csQuery, CString& csStationID);

	CString StripTrailingPunctuation(const CString& csInput)
	{
		CString cs = csInput;
		cs.Trim();

		while (!cs.IsEmpty())
		{
			WCHAR ch = cs[cs.GetLength() - 1];
			if (iswalpha(ch))
				break;
			cs.Delete(cs.GetLength() - 1);
		}

		return cs;
	}

	inline bool IsAlpha(TCHAR ch)
	{
		return (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z');
	}

	inline bool IsDigit(TCHAR ch)
	{
		return (ch >= L'0' && ch <= L'9');
	}

// public methods
public:
	// dispatch a natural language query to the appropriate SQL query
	bool Dispatch(const CString& csQuery, CString& csResult);

	// return the version of the USHCN data the database is based on
	CString QueryMetadataVersion();

	// return a formatted description of all table schemas
	CString QueryTableSchemas();

	// return a formatted list of all stations
	CString QueryAllStations();

	// return a formatted list of stations filtered by state
	CString QueryStationsByState(const CString& csState);

	// return monthly average temperatures for a given state
	CString QueryMonthlyTemperaturesByState
	(
		const CString& csState,
		bool bExcludeEstimated,
		bool bExcludeQC,
		const CString& csDSFlagFilter
	);

	// return annual average temperatures for a given state
	CString QueryAnnualAveragesByState
	(
		const CString& csState,
		bool bActive,
		bool bExcludeEstimated,
		bool bExcludeQC,
		const CString& csDSFlagFilter
	);

	CString QueryAnnualByStateCommon
	(
		const CString& csState,
		int nMeasurementType,
		const CString& csColumnName,   // "AvgValue", "MaxValue", "MinValue"
		const CString& csOutputLabel,  // "AvgTemp", "MaxTemp", "MinTemp"
		bool bActive
	); 

	// return annual maximum temperatures for a given state
	CString QueryAnnualMaximumsByState
	(
		const CString& csState,
		bool bActive,
		bool bExcludeEstimated,
		bool bExcludeQC,
		const CString& csDSFlagFilter
	);

	// return annual minimum temperatures for a given state
	CString QueryAnnualMinimumsByState
	(
		const CString& csState,
		bool bActive,
		bool bExcludeEstimated,
		bool bExcludeQC,
		const CString& csDSFlagFilter
	);

	// return temperature trend (slope) for a given state
	CString QueryTemperatureTrendByState
	(
		const CString& csState,
		bool bExcludeEstimated,
		bool bExcludeQC,
		const CString& csDSFlagFilter
	);

	// Converts natural-language state names into 2-letter postal codes.
	// Removes punctuation and handles mixed case.
	CString NormalizeState(const CString& csInput);

	// return a formatted list of all active stations (still recording data)
	CString QueryActiveStations();

	// return a formatted list of active stations filtered by state
	CString QueryActiveStationsByState(const CString& csState);

	// the USHCN data source flags as described in the readme.txt file
	CString QueryExplainDSFlags();

	bool StationExists(const CString& csStationID);

	CString QueryMonthlyTemperaturesByStation
	(
		const CString& csStationID,
		int nMeasurementType,
		bool bExcludeEstimated,
		bool bExcludeQC,
		const CString& csDSFlagFilter
	);

	CString QueryStationSummary(const CString& csStationID);

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
