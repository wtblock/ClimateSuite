/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "StationYear.h"

/////////////////////////////////////////////////////////////////////////////
// used as a quality control test to verify the original USHCN
// can be reproduced from the database.
CString CStationYear::ReproduceRawLine()
{
	int nValueLen = CClimateTemperature::GetValueLength();
	int nFlagLen = CClimateTemperature::GetFlagLength();

	// Build a buffer large enough for the entire line
	CString line;
	line.Preallocate
	(
		StationLength + 1 + YearLength + 1 + 12 * (nValueLen + 3 * nFlagLen + 3)
	);

	// 1. Write StationID at fixed position
	line.Insert(StationStart, m_csStation + L" ");

	// 2. Write Year at fixed position
	line.Insert(YearStart, m_csYear);

	// 3. Write each month block
	int pos = MonthStart;

	for (auto& pTemp : m_arrMonths)
	{
		// Raw value (6 chars, right‑aligned)
		CString csValue;
		csValue.Format(L"%6d", pTemp->CentigradeRaw);
		line.Insert(pos, csValue);
		pos += pTemp->ValueLength;

		// DMFlag (1 char or space)
		CString csDM = pTemp->DataMeasurementFlag;
		if (csDM.IsEmpty()) csDM = L" ";
		line.Insert(pos, csDM);
		pos += pTemp->FlagLength;

		// QCFlag (1 char or space)
		CString csQC = pTemp->QualityControlFlag;
		if (csQC.IsEmpty()) csQC = L" ";
		line.Insert(pos, csQC);
		pos += pTemp->FlagLength;

		// DSFlag (1 char or space)
		CString csDS = pTemp->DataSourceFlag;
		if (csDS.IsEmpty()) csDS = L" ";
		line.Insert(pos, csDS);
		pos += pTemp->FlagLength;

		// USHCN uses a space between month blocks
		line.Insert(pos, L" ");
		pos += 1;
	}

	return line;
} // ReproduceRawLine

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
	void* pDatabase = (void*)&db;

	// Write all 12 months
	for (int i = 0; i < 12; i++)
	{
		shared_ptr<CClimateTemperature> pMonth = Month[i];

		pMonth->WriteToDatabase(pDatabase, stationID, nYear, i + 1);
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
