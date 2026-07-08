/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "sqlite3.h"

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
	CClimateDatabase();
	~CClimateDatabase();

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

};

/////////////////////////////////////////////////////////////////////////////
