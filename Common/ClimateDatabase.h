/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "sqlite3.h"
#include "SmartArray.h"
#include "ClimateTemperature.h"

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the SQLite3.c code
class CClimateDatabase
{
// protected data
protected:
	sqlite3* m_db;

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

	CClimateDatabase();
	~CClimateDatabase();

};

/////////////////////////////////////////////////////////////////////////////
