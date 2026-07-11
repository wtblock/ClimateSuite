/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "StationYear.h"

/////////////////////////////////////////////////////////////////////////////
// write our data into the database
void CStationYear::WriteToDatabase
(
	CClimateDatabase& db,
	LPCTSTR stationID
)
{
	const int nYear = _tstoi(Year);

	sqlite3_stmt* stmt = db.Prepare
	(
		L"INSERT OR REPLACE INTO Years "
		L"(StationID, Year, MeasurementType, MaxValue, MinValue, "
		L" AvgValue, ValidReadings) "
		L"VALUES (?, ?, ?, ?, ?, ?, ?);"
	);

	db.BindText(stmt, 1, stationID);
	db.BindInt(stmt, 2, nYear);
	db.BindInt(stmt, 3, MeasurementType);
	db.BindDouble(stmt, 4, Maximum);
	db.BindDouble(stmt, 5, Minimum);
	db.BindDouble(stmt, 6, Average);
	db.BindInt(stmt, 7, ValidReadings);

	db.Step(stmt);
	db.Finalize(stmt);

	// Write all 12 months
	for (int i = 0; i < 12; i++)
	{
		shared_ptr<CClimateTemperature> pMonth = Month[i];
		pMonth->WriteToDatabase(db, stationID, nYear, i + 1);
	}

	// Write greater-than counts
	for (auto& gc : m_GreaterCounts)
	{
		sqlite3_stmt* stmtGC = db.Prepare
		(
			L"INSERT OR REPLACE INTO GreaterCounts "
			L"(StationID, Year, MeasurementType, ThresholdF, Count) "
			L"VALUES (?, ?, ?, ?, ?);"
		);

		db.BindText(stmtGC, 1, stationID);
		db.BindInt(stmtGC, 2, nYear);
		db.BindInt(stmtGC, 3, MeasurementType);
		db.BindInt(stmtGC, 4, gc.first);
		db.BindInt(stmtGC, 5, gc.second);

		db.Step(stmtGC);
		db.Finalize(stmtGC);
	}

} // WriteToDatabase

/////////////////////////////////////////////////////////////////////////////
