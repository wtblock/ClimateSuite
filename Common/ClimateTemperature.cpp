/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ClimateTemperature.h"
#include "ClimateDatabase.h"

/////////////////////////////////////////////////////////////////////////////
void CClimateTemperature::WriteToDatabase
(
	void* pDB,
	LPCTSTR stationID,
	int year,
	int month
)
{
	CClimateDatabase* db = (CClimateDatabase*)(pDB);
	sqlite3_stmt* stmt = db->Prepare
	(
		L"INSERT OR REPLACE INTO Months "
		L"(StationID, Year, Month, MeasurementType, CentigradeRaw, "
		L" DMFlag, QCFlag, DSFlag) "
		L"VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
	);

	db->BindText(stmt, 1, stationID);
	db->BindInt(stmt, 2, year);
	db->BindInt(stmt, 3, month);
	db->BindInt(stmt, 4, MeasurementType);
	db->BindInt(stmt, 5, CentigradeRaw);
	db->BindText(stmt, 6, DataMeasurementFlag);
	db->BindText(stmt, 7, QualityControlFlag);
	db->BindText(stmt, 8, DataSourceFlag);

	db->Step(stmt);
	db->Finalize(stmt);

} // WriteToDatabase

/////////////////////////////////////////////////////////////////////////////
