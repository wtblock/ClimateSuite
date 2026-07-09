/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ClimateTemperature.h"

/////////////////////////////////////////////////////////////////////////////
void CClimateTemperature::WriteToDatabase
(
	CClimateDatabase& db,
	LPCTSTR stationID,
	int year,
	int month
)
{
	sqlite3_stmt* stmt = db.Prepare
	(
		L"INSERT OR REPLACE INTO Months "
		L"(StationID, Year, Month, MeasurementType, Centigrade, "
		L" DMFlag, QCFlag, DSFlag) "
		L"VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
	);

	db.BindText(stmt, 1, stationID);
	db.BindInt(stmt, 2, year);
	db.BindInt(stmt, 3, month);
	db.BindText(stmt, 4, MeasurementName);
	db.BindDouble(stmt, 5, Centigrade);
	db.BindText(stmt, 6, DataMeasurementFlag);
	db.BindText(stmt, 7, QualityControlFlag);
	db.BindText(stmt, 8, DataSourceFlag);

	db.Step(stmt);
	db.Finalize(stmt);

} // WriteToDatabase

/////////////////////////////////////////////////////////////////////////////
