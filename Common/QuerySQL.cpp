/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "QuerySQL.h"

/////////////////////////////////////////////////////////////////////////////
// return a list of all tables in the database
//
// Uses SQLite's built-in schema table sqlite_master to enumerate all
// user-defined tables. This method is generic and reusable across any
// SQLite-based project.
//
/////////////////////////////////////////////////////////////////////////////
CSmartArray<CString> CQuerySQL::ListTables(CClimateDatabase* pDB)
{
	CSmartArray<CString> arrTables;

	CString csSQL = L"SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;";

	CSmartArray<CSmartArray<CString>> arrRows;
	pDB->ExecuteTable(csSQL, arrRows);

	for (long i = 0; i < arrRows.Count; i++)
	{
		auto pRow = arrRows.get(i);
		if (pRow && pRow->Count > 0)
		{
			auto pValue = pRow->get(0);
			if (pValue)
				arrTables.append(*pValue);
		}
	}

	return arrTables;
}

/////////////////////////////////////////////////////////////////////////////
// return a list of column names for a given table
//
// Uses SQLite's PRAGMA table_info() to retrieve column metadata.
// This method is generic and reusable.
//
/////////////////////////////////////////////////////////////////////////////
CSmartArray<CString> CQuerySQL::ListColumns(CClimateDatabase* pDB, const CString& csTable)
{
	CSmartArray<CString> arrColumns;

	CString csSQL;
	csSQL.Format(L"PRAGMA table_info(%s);", csTable);

	CSmartArray<CSmartArray<CString>> arrRows;
	pDB->ExecuteTable(csSQL, arrRows);

	for (long i = 0; i < arrRows.Count; i++)
	{
		auto pRow = arrRows.get(i);
		if (pRow && pRow->Count > 1)
		{
			auto pValue = pRow->get(1);
			if (pValue)
				arrColumns.append(*pValue);
		}
	}

	return arrColumns;
}

/////////////////////////////////////////////////////////////////////////////
// return a formatted schema description for all tables
//
// Produces output similar to:
//
//   Table: Stations
//     StationID
//     Name
//     State
//     Latitude
//     Longitude
//
//   Table: Metadata
//     Key
//     Value
//
// This method is generic and reusable.
//
/////////////////////////////////////////////////////////////////////////////
CString CQuerySQL::DescribeSchema(CClimateDatabase* pDB)
{
	CString csResult;

	CSmartArray<CString> arrTables = ListTables(pDB);

	for (long i = 0; i < arrTables.Count; i++)
	{
		auto pTable = arrTables.get(i);
		if (!pTable)
			continue;

		csResult += L"Table: ";
		csResult += *pTable;
		csResult += L"\n";

		CSmartArray<CString> arrColumns = ListColumns(pDB, *pTable);

		for (long j = 0; j < arrColumns.Count; j++)
		{
			auto pCol = arrColumns.get(j);
			if (pCol)
			{
				csResult += L"  ";
				csResult += *pCol;
				csResult += L"\n";
			}
		}

		csResult += L"\n";
	}

	return csResult;
}

/////////////////////////////////////////////////////////////////////////////
// format a table (rows/columns) into aligned text
//
// This method produces aligned ASCII output for any table:
//
//   Column1   Column2   Column3
//   -------   -------   -------
//   value1    value2    value3
//   valueA    valueB    valueC
//
// This is generic and reusable.
//
/////////////////////////////////////////////////////////////////////////////
CString CQuerySQL::FormatTable(
	const CSmartArray<CString>& arrColumns,
	const CSmartArray<CSmartArray<CString>>& arrRows,
	const CSmartArray<CString>* pUnitsRow // NEW optional parameter
)
{
	CString csOut;

	// Determine column widths
	std::vector<int> colWidths(arrColumns.Count);
	for (int i = 0; i < arrColumns.Count; i++)
	{
		colWidths[i] = arrColumns.get(i)->GetLength();

		if (pUnitsRow)
			colWidths[i] = max(colWidths[i], pUnitsRow->get(i)->GetLength());
	}

	// Also consider row data
	for (long r = 0; r < arrRows.Count; r++)
	{
		auto pRow = arrRows.get(r);
		for (int c = 0; c < pRow->Count; c++)
		{
			colWidths[c] = max(colWidths[c], pRow->get(c)->GetLength());
		}
	}

	// Print column titles
	for (int i = 0; i < arrColumns.Count; i++)
	{
		csOut.AppendFormat(L"%-*s  ", colWidths[i], arrColumns.get(i)->GetString());
	}
	csOut += L"\n";

	// Print units row (if provided)
	if (pUnitsRow)
	{
		for (int i = 0; i < pUnitsRow->Count; i++)
		{
			csOut.AppendFormat(L"%-*s  ", colWidths[i], pUnitsRow->get(i)->GetString());
		}
		csOut += L"\n";
	}

	// Print separator
	for (int i = 0; i < arrColumns.Count; i++)
	{
		csOut.AppendFormat(L"%-*s  ", colWidths[i], L"-");
	}
	csOut += L"\n";

	// Print rows
	for (long r = 0; r < arrRows.Count; r++)
	{
		auto pRow = arrRows.get(r);
		for (int c = 0; c < pRow->Count; c++)
		{
			csOut.AppendFormat(L"%-*s  ", colWidths[c], pRow->get(c)->GetString());
		}
		csOut += L"\n";
	}

	return csOut;
}

/////////////////////////////////////////////////////////////////////////////
