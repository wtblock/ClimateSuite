/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "sqlite3.h"
#include "SmartArray.h"
#include "ClimateTemperature.h"
#include "KeyedCollection.h"
#include <vector>
#include <winnt.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
class CClimateStation;

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the SQLite3.c code
class CClimateDatabase
{
// public types
public:
	// latitude, longitude pair
	typedef pair<float, float> GPS_COORDINATE;

// protected data
protected:
	sqlite3* m_db;

	// map of state postal code keys with a vector of the member city names
	CKeyedCollection<CString, vector<CString> > m_mapCities;

	// map of station IDs to station data
	CKeyedCollection<CString, CClimateStation> m_mapStations;

	// map of location names (state, city) to station data
	CKeyedCollection<CString, CClimateStation> m_mapLocations;

	// map of active station IDs to locations (PC, Location)
	// where PC is the postal code of the state and location
	// is the name of the station (city in most cases)
	CKeyedCollection<CString, CString> m_mapActiveStations;

	// map of active location keys (PC, Location) to station ID
	// where PC is the postal code of the state and location
	// is the name of the station (city in most cases)
	CKeyedCollection<CString, CString> m_mapActiveLocations;

// public properties
public:
	// database metadata
	CString GetMetadata(LPCTSTR key)
	{
		sqlite3_stmt* stmt = Prepare
		(
			L"SELECT Value FROM Metadata WHERE Key = ?;"
		);

		BindText(stmt, 1, key);

		CString result;

		if (Step(stmt))
		{
			const unsigned char* text = sqlite3_column_text(stmt, 0);
			if (text)
				result = (LPCSTR)text;
		}

		Finalize(stmt);
		return result;
	}
	// database metadata
	bool SetMetadata(LPCTSTR key, LPCTSTR value)
	{
		sqlite3_stmt* stmt = Prepare
		(
			L"INSERT OR REPLACE INTO Metadata (Key, Value) "
			L"VALUES (?, ?);"
		);

		BindText(stmt, 1, key);
		BindText(stmt, 2, value);

		bool ok = Step(stmt);
		Finalize(stmt);
		return ok;
	}
	// database metadata
	__declspec(property(get = GetMetadata, put = SetMetadata))
		CString Metadata[];

	// retrieve a map of climate stations
	CKeyedCollection<CString, CClimateStation>* GetStations()
	{
		if (m_mapStations.Count == 0)
		{
			PopulateStations();
		}
		
		return &m_mapStations;
	}
	// retrieve a map of climate stations
	__declspec(property(get = GetStations))
		CKeyedCollection<CString, CClimateStation>* Stations;

	// retrieve a vector of state postal codes in alphabetical order 
	vector<CString> GetStates()
	{
		vector<CString> value;
		if (m_mapCities.Count == 0)
		{
			PopulateStates();
		}

		for (auto& node : m_mapCities.Items)
		{
			value.push_back(node.first);
		}
		return value;
	}
	// retrieve a vector of state postal codes in alphabetical order 
	__declspec(property(get = GetStates))
		vector<CString> States;

	// retrieve a vector of city names in alphabetical order 
	// given the state they are members of
	vector<CString> GetCities( CString csPostalCode)
	{
		vector<CString> value;
		if (m_mapCities.Count == 0)
		{
			PopulateStates();
		}

		if (m_mapCities.Exists[csPostalCode])
		{
			value = *m_mapCities.find(csPostalCode);
		}

		return value;
	}
	// retrieve a vector of city names in alphabetical order 
	// given the state they are members of
	__declspec(property(get = GetCities))
		vector<CString> Cities[];

	// get a pointer to station data from a station ID
	shared_ptr<CClimateStation> GetStationByID( CString csStationID)
	{
		shared_ptr<CClimateStation> value;
		if (m_mapStations.Count == 0)
		{
			PopulateStations();
		}

		if (m_mapStations.Exists[csStationID])
		{
			value = m_mapStations.find(csStationID);
		}

		return value;
	}
	// get a pointer to station data from a station ID
	__declspec(property(get = GetStationByID))
		shared_ptr<CClimateStation> StationByID[];

	// the given station ID returns the location key ("PC, Location")
	// where PC is the state's postal code. Returning an empty 
	// value indicates the station is not active
	CString GetActiveLocation(CString csID)
	{
		CString value;
		if (m_mapActiveStations.Count == 0)
		{
			PopulateActiveStations();
		}
		if (m_mapActiveStations.Exists[csID])
		{
			value = *m_mapActiveStations.find(csID);
		}

		return value;
	}
	// the given station ID returns the location key ("PC, Location")
	// where PC is the state's postal code. Returning an empty 
	// value indicates the station is not active
	__declspec(property(get = GetActiveLocation))
		CString ActiveLocation[];

	// the given location key ("PC, Location") returns the station ID
	// where PC is the state's postal code. Returning an empty 
	// value indicates the location is not active
	CString GetActiveStation(CString csKey)
	{
		CString value;
		if (m_mapActiveLocations.Count == 0)
		{
			PopulateActiveStations();
		}
		if (m_mapActiveLocations.Exists[csKey])
		{
			value = *m_mapActiveLocations.find(csKey);
		}

		return value;
	}
	// the given location key ("PC, Location") returns the station ID
	// where PC is the state's postal code. Returning an empty 
	// value indicates the location is not active
	__declspec(property(get = GetActiveStation))
		CString ActiveStation[];

	// get a pointer to station data from a location 
	// (format: "postal_code, city", i.e.: TX, Weatherford)
	shared_ptr<CClimateStation> GetStationByLocation( CString csLocation)
	{
		shared_ptr<CClimateStation> value;
		if (m_mapLocations.Count == 0)
		{
			PopulateStations();
		}

		if (m_mapLocations.Exists[csLocation])
		{
			value = m_mapLocations.find(csLocation);
		}

		return value;
	}
	// get a pointer to station data from a location 
	// (format: "postal_code, city", i.e.: TX, Weatherford)
	__declspec(property(get = GetStationByLocation))
		shared_ptr<CClimateStation> StationByLocation[];

	// center coordinates for the nation
	GPS_COORDINATE GetCenterNational();
	// center coordinates for the nation
	__declspec(property(get = GetCenterNational))
		GPS_COORDINATE CenterNational;

	// center coordinates for the given a state's postal code
	GPS_COORDINATE GetCenterState(CString csPC);
	// center coordinates for the given a state's postal code
	__declspec(property(get = GetCenterState))
		GPS_COORDINATE CenterState[];

	// coordinates for the given state and location
	GPS_COORDINATE GetCoordinates(CString csPC, CString csLoc);
	// coordinates for the given state and location
	__declspec(property(get = GetCoordinates))
		GPS_COORDINATE Coordinates[][];

public:
	/////////////////////////////////////////////////////////////////////////////
	// Execute a SQL query and return a table of rows and columns.
	//
	// This method executes any SQL SELECT statement and returns the results
	// in a two-dimensional array:
	//
	//   arrRows[rowIndex][columnIndex]
	//
	// Each row is represented as a CStringArray containing the column values.
	// This method is generic and reusable across all future projects.
	//
	/////////////////////////////////////////////////////////////////////////////
	bool ExecuteTable
	(
		const CString& csSQL,
		CSmartArray<CSmartArray<CString>>& arrRows
	);

	bool LoadStationYear
	(
		const CString& csStation,
		int nYear,
		int nType,
		vector<shared_ptr<CClimateTemperature>>& months
	);

	bool LoadGreaterCounts
	(
		const CString& csStation,
		int nYear,
		int nType,
		vector<pair<int,int> >& counts
	);

	inline CString UTF8ToCString(const char* utf8)
	{
		if (utf8 == nullptr)
			return CString();

		int len = (int)strlen(utf8);
		if (len == 0)
			return CString();

		// compute required UTF‑16 buffer size
		int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, len, nullptr, 0);

		CString result;
		LPWSTR buffer = result.GetBuffer(wlen);

		MultiByteToWideChar(CP_UTF8, 0, utf8, len, buffer, wlen);

		result.ReleaseBuffer(wlen);
		return result;
	}

	int GetFirstYear(const CString& stationID, CClimateTemperature::MEASURE_TYPE eType)
	{
		int year = 0;

		sqlite3_stmt* stmt = Prepare(
			L"SELECT MIN(Year) "
			L"FROM Months "
			L"WHERE StationID = ? AND MeasurementType = ?;"
		);

		BindText(stmt, 1, stationID);
		BindInt(stmt, 2, (int)eType);

		if (Step(stmt))   // Step() returns true if SQLITE_ROW
		{
			year = sqlite3_column_int(stmt, 0);
		}

		Finalize(stmt);
		return year;
	}

	int GetLastYear(const CString& stationID, CClimateTemperature::MEASURE_TYPE eType)
	{
		int year = 0;

		sqlite3_stmt* stmt = Prepare(
			L"SELECT MAX(Year) "
			L"FROM Months "
			L"WHERE StationID = ? AND MeasurementType = ?;"
		);

		BindText(stmt, 1, stationID);
		BindInt(stmt, 2, (int)eType);

		if (Step(stmt))   // Step() returns true if SQLITE_ROW
		{
			year = sqlite3_column_int(stmt, 0);
		}

		Finalize(stmt);
		return year;
	}

	bool Open(LPCTSTR path);
	void Close();

	bool Exec(LPCTSTR sql);

	sqlite3_stmt* Prepare(LPCTSTR sql);
	bool Step(sqlite3_stmt* stmt);
	void Finalize(sqlite3_stmt* stmt);

	void BeginTransaction();
	void Commit();

	bool BindInt(sqlite3_stmt* stmt, int index, int value);
	bool BindDouble(sqlite3_stmt* stmt, int index, double value);
	bool BindText(sqlite3_stmt* stmt, int index, LPCTSTR value);

	void CreateSchema();

	void PopulateStates();

	void PopulateStations();

	void PopulateActiveStations();

	CClimateDatabase();
	~CClimateDatabase();

};

/////////////////////////////////////////////////////////////////////////////
