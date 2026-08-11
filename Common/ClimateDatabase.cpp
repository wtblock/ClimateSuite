/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ClimateDatabase.h"
#include "ClimateStation.h"
#include "KeyedCollection.h"
#include <algorithm>   // for std::sort

/////////////////////////////////////////////////////////////////////////////
CClimateDatabase::CClimateDatabase()
{
	m_db = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
CClimateDatabase::~CClimateDatabase()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::Open(LPCTSTR path)
{
	CStringA utf8(path);
	int rc = sqlite3_open(utf8, &m_db);
	return rc == SQLITE_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::Close()
{
	if (m_db)
	{
		sqlite3_close(m_db);
		m_db = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::Exec(LPCTSTR sql)
{
	CStringA utf8(sql);
	char* errMsg = nullptr;

	int rc = sqlite3_exec(m_db, utf8, nullptr, nullptr, &errMsg);

	if (errMsg)
	{
		sqlite3_free(errMsg);
	}

	return rc == SQLITE_OK;
}

/////////////////////////////////////////////////////////////////////////////
sqlite3_stmt* CClimateDatabase::Prepare(LPCTSTR sql)
{
	CStringA utf8(sql);
	sqlite3_stmt* stmt = nullptr;

	int rc = sqlite3_prepare_v2(m_db, utf8, -1, &stmt, nullptr);
	return (rc == SQLITE_OK) ? stmt : nullptr;
}

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::Step(sqlite3_stmt* stmt)
{
	int rc = sqlite3_step(stmt);

	if (rc == SQLITE_ROW)
		return true;        // row available

	if (rc == SQLITE_DONE)
		return false;       // no more rows

	// Any other rc is an error
	// (you can log it if you want)
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::Finalize(sqlite3_stmt* stmt)
{
	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::BeginTransaction()
{
	Exec(L"BEGIN TRANSACTION;");
}

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::Commit()
{
	Exec(L"COMMIT;");
}

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::BindInt(sqlite3_stmt* stmt, int index, int value)
{
	return sqlite3_bind_int(stmt, index, value) == SQLITE_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::BindDouble(sqlite3_stmt* stmt, int index, double value)
{
	return sqlite3_bind_double(stmt, index, value) == SQLITE_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::BindText(sqlite3_stmt* stmt, int index, LPCTSTR value)
{
	CStringA utf8(value);
	return sqlite3_bind_text(stmt, index, utf8, -1, SQLITE_TRANSIENT) == SQLITE_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::CreateSchema()
{
	Exec
	(
		L"CREATE TABLE IF NOT EXISTS Metadata "
		L"(Key TEXT PRIMARY KEY, "
		L" Value TEXT);"
	);

	Exec
	(
		L"CREATE TABLE IF NOT EXISTS Stations "
		L"(StationID TEXT PRIMARY KEY, "
		L" GUID TEXT, "
		L" Latitude REAL, "
		L" Longitude REAL, "
		L" Elevation REAL, "
		L" State TEXT, "
		L" Location TEXT, "
		L" Component1 TEXT, "
		L" Component2 TEXT, "
		L" Component3 TEXT, "
		L" UTCOffset INTEGER);"
	);

	Exec
	(
		L"CREATE TABLE IF NOT EXISTS Years "
		L"(StationID TEXT, "
		L" Year INTEGER, "
		L" MeasurementType INTEGER, "
		L" MaxValue REAL, "
		L" MinValue REAL, "
		L" AvgValue REAL, "
		L" ValidReadings INTEGER, "
		L" PRIMARY KEY (StationID, Year, MeasurementType));"
	);

	Exec
	(
		L"CREATE TABLE IF NOT EXISTS Months "
		L"(StationID TEXT, "
		L" Year INTEGER, "
		L" Month INTEGER, "
		L" MeasurementType INTEGER, "
		L" CentigradeRaw INTEGER, "
		L" DMFlag TEXT, "
		L" QCFlag TEXT, "
		L" DSFlag TEXT, "
		L" PRIMARY KEY (StationID, Year, Month, MeasurementType));"
	);

	Exec
	(
		L"CREATE TABLE IF NOT EXISTS GreaterCounts "
		L"(StationID TEXT, "
		L" Year INTEGER, "
		L" MeasurementType INTEGER, "
		L" ThresholdF INTEGER, "
		L" Count INTEGER, "
		L" PRIMARY KEY (StationID, Year, MeasurementType, ThresholdF));"
	);
}

/////////////////////////////////////////////////////////////////////////////
// Execute a SQL query and return a table of rows and columns
/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::ExecuteTable
(
	const CString& csSQL,
	CSmartArray<CSmartArray<CString>>& arrRows
)
{
	arrRows.clear();

	sqlite3_stmt* stmt = nullptr;

	int rc = sqlite3_prepare16_v2
	(
		m_db,
		csSQL.GetString(),
		-1,
		&stmt,
		nullptr
	);

	if (rc != SQLITE_OK)
		return false;

	while
	(
		(rc = sqlite3_step(stmt)) == SQLITE_ROW
	)
	{
		CSmartArray<CString> arrRow;

		int nCols = sqlite3_column_count(stmt);

		for (int i = 0; i < nCols; i++)
		{
			const void* pText = sqlite3_column_text16(stmt, i);

			CString csValue = (pText != nullptr) ? (LPCTSTR)pText : L"";

			arrRow.append(csValue);
		}

		arrRows.append(arrRow);
	}

	sqlite3_finalize(stmt);

	return (rc == SQLITE_DONE);
} // ExecuteTable

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::LoadStationYear
(
	const CString& csStation,
	int nYear,
	int nType,
	vector<shared_ptr<CClimateTemperature>>& months
)
{
	months.clear();

	sqlite3_stmt* stmt = Prepare(
		L"SELECT Month, CentigradeRaw, DMFlag, QCFlag, DSFlag "
		L"FROM Months "
		L"WHERE StationID = ? AND Year = ? AND MeasurementType = ? "
		L"ORDER BY Month;"
	);

	CClimateTemperature::MEASURE_TYPE eType = (CClimateTemperature::MEASURE_TYPE)nType;
	BindText(stmt, 1, csStation);
	BindInt(stmt, 2, nYear);
	BindInt(stmt, 3, (int)eType);

	while (Step(stmt))   // Step() returns true if SQLITE_ROW
	{
		int month = sqlite3_column_int(stmt, 0);
		short raw = (short)sqlite3_column_int(stmt, 1);

		CString dm = UTF8ToCString((const char*)sqlite3_column_text(stmt, 2));
		CString qc = UTF8ToCString((const char*)sqlite3_column_text(stmt, 3));
		CString ds = UTF8ToCString((const char*)sqlite3_column_text(stmt, 4));

		auto pTemp = make_shared<CClimateTemperature>();

		pTemp->CentigradeRaw = raw;
		pTemp->DataMeasurementFlag = dm;
		pTemp->QualityControlFlag = qc;
		pTemp->DataSourceFlag = ds;

		months.push_back(pTemp);
	}

	Finalize(stmt);

	return months.size() == 12;

} // LoadStationYear

/////////////////////////////////////////////////////////////////////////////
bool CClimateDatabase::LoadGreaterCounts
(
	const CString& csStation,
	int nYear,
	int nType,
	vector<pair<int,int> >& counts
)
{
	counts.clear();

	sqlite3_stmt* stmt = Prepare
	(
		L"SELECT ThresholdF, Count "
		L"FROM GreaterCounts "
		L"WHERE StationID = ? AND Year = ? AND MeasurementType = ? "
		L"ORDER BY ThresholdF;"
	);

	BindText(stmt, 1, csStation);
	BindInt(stmt, 2, nYear);
	BindInt(stmt, 3, nType);

	while (Step(stmt))
	{
		int threshold = sqlite3_column_int(stmt, 0);
		int count = sqlite3_column_int(stmt, 1);

		counts.push_back(pair<int,int>(threshold, count));
	}

	Finalize(stmt);

	return !counts.empty();
} // LoadGreaterCounts

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::PopulateStates()
{
	// Clear any existing cached data
	m_mapCities.clear();

	// Query all distinct (State, Location) pairs
	CSmartArray<CSmartArray<CString>> rows;

	CString sql =
		L"SELECT DISTINCT State, Location "
		L"FROM Stations "
		L"WHERE State IS NOT NULL AND State <> '' "
		L"ORDER BY State, Location;";

	if (!ExecuteTable(sql, rows))
		return;

	// Build the keyed collection: State → vector<Location>
	for (int i = 0; i < rows.Count; i++)
	{
		CString state = *rows.get(i)->get(0);
		CString city = *rows.get(i)->get(1);

		// If this state is not yet in the map, add it
		if (!m_mapCities.Exists[state])
		{
			vector<CString> emptyList;
			m_mapCities.add(state, emptyList);
		}

		// Append the city to the state's vector
		auto pList = m_mapCities.find(state);
		if (pList != nullptr)
		{
			pList->push_back(city);
		}
	}

	// Sort each state's city list alphabetically
	for (auto& kv : m_mapCities.Items)
	{
		auto& vec = *kv.second;
		std::sort(vec.begin(), vec.end());
	}
} // PopulateStates

/////////////////////////////////////////////////////////////////////////////
void CClimateDatabase::PopulateStations()
{
	// we only need to do this once
	if (m_mapStations.Count > 0)
	{
		return;
	}

	// Query all distinct (State, Location) pairs
	CSmartArray<CSmartArray<CString>> rows;

	CString sql =
		L"SELECT "
		L"StationID, State, Latitude, Longitude, Elevation, Location "
		L"FROM Stations "
		L"WHERE State IS NOT NULL AND State <> '' "
		L"ORDER BY State, StationID; ";

	if (!ExecuteTable(sql, rows))
		return;
	
	for (auto& pRow : rows.Items)
	{
		shared_ptr<CClimateStation> pStation =
			shared_ptr<CClimateStation>(new(CClimateStation));

		auto& pColumn = pRow->Items;

		int nCol = 0;
		for (auto& pCol : pColumn)
		{
			switch (nCol)
			{
			case 0:
				pStation->Station = *pCol;
				break;
			case 1:
				pStation->State = *pCol;
				break;
			case 2:
				pStation->Latitude = _tstof(*pCol);
				break;
			case 3:
				pStation->Longitude = _tstof(*pCol);
				break;
			case 4:
				pStation->Elevation = _tstof(*pCol);
				break;
			case 5:
				pStation->Location = *pCol;
				break;
			}

			nCol++;
		}

		CString csStation = pStation->Station;
		csStation.Trim();
		CString csState = pStation->State;
		csState.Trim();
		CString csLocation = pStation->Location;
		csLocation.Trim();

		CString csKey;
		csKey.Format(L"%s, %s", csState, csLocation);

		// these two maps create a cross reference between station IDs and 
		// locations. Since the pointer is shared, it is safe for both maps
		// to reference the same pointer
		m_mapStations.add(csStation, pStation);
		m_mapLocations.add(csKey, pStation);
	}

} // PopulateStations


/////////////////////////////////////////////////////////////////////////////
