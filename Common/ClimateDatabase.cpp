/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ClimateDatabase.h"

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
		L" Centigrade REAL, "
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

/////////////////////////////////////////////////////////////////////////////
