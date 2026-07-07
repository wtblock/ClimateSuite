/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ClimateStation.h"

/////////////////////////////////////////////////////////////////////////////
void CClimateStation::WriteToDatabase(CClimateDatabase& db)
{
	sqlite3_stmt* stmt = db.Prepare
	(
		L"INSERT OR REPLACE INTO Stations "
		L"(StationID, GUID, Latitude, Longitude, Elevation, State, "
		L" Location, Component1, Component2, Component3, UTCOffset) "
		L"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	);

	db.BindText(stmt, 1, Station);
	db.BindText(stmt, 2, GUID);
	db.BindDouble(stmt, 3, Latitude);
	db.BindDouble(stmt, 4, Longitude);
	db.BindDouble(stmt, 5, Elevation);
	db.BindText(stmt, 6, State);
	db.BindText(stmt, 7, Location);
	db.BindText(stmt, 8, Component1);
	db.BindText(stmt, 9, Component2);
	db.BindText(stmt, 10, Component3);
	db.BindInt(stmt, 11, OffsetUTC);

	db.Step(stmt);
	db.Finalize(stmt);

	// Write yearly data
	for (auto& kv : m_Max.Items)
	{
		kv.second->WriteToDatabase(db, Station);
	}
	for (auto& kv : m_Min.Items)
	{
		kv.second->WriteToDatabase(db, Station);
	}
	for (auto& kv : m_Avg.Items)
	{
		kv.second->WriteToDatabase(db, Station);
	}
} // WriteToDatabase

/////////////////////////////////////////////////////////////////////////////
