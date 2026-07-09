/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ClimateDatabase.h"
#include "SmartArray.h"

/////////////////////////////////////////////////////////////////////////////
// 
// Generic SQL helper class
// Provides reusable SQL utilities for schema introspection, table listing,
// column listing, and formatted output. This class is intentionally
// domain-agnostic so it can be reused in future projects.
//
/////////////////////////////////////////////////////////////////////////////
class CQuerySQL
{
	// protected data
protected:

	// public properties
public:

	// protected methods
protected:

	// public methods
public:
	// return a list of all tables in the database
	CSmartArray<CString> ListTables(CClimateDatabase* pDB);

	// return a list of column names for a given table
	CSmartArray<CString> ListColumns(CClimateDatabase* pDB, const CString& csTable);

	// return a formatted schema description for all tables
	CString DescribeSchema(CClimateDatabase* pDB);

	// format a table (rows/columns) into aligned text
	CString FormatTable
	(
		const CSmartArray<CString>& arrColumns,
		const CSmartArray<CSmartArray<CString> >& arrRows
	);

	// protected overrides
protected:

	// public overrides
public:

	// public constructor/destructor
public:
	CQuerySQL()
	{
	}
	~CQuerySQL()
	{
	}
}; // class CQuerySQL

/////////////////////////////////////////////////////////////////////////////
