/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "sqlite3.h"

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the SQLite3.c code
class CClimateDatabase
{
protected:
	sqlite3* m_db;

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
