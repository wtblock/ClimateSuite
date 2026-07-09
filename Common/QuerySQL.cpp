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
CString CQuerySQL::FormatTable
(
	const CSmartArray<CString>& arrColumns,
	const CSmartArray<CSmartArray<CString>>& arrRows
)
{
	CString csResult;

	// compute column widths
	CSmartArray<int> arrWidths;
	arrColumns.Count;

	arrWidths.resize(arrColumns.Count);

	for (long c = 0; c < arrColumns.Count; c++)
	{
		auto pCol = arrColumns.get(c);
		arrWidths.set(c, std::make_shared<int>(pCol ? pCol->GetLength() : 0));
	}

	for (long r = 0; r < arrRows.Count; r++)
	{
		auto pRow = arrRows.get(r);
		if (!pRow)
			continue;

		for (long c = 0; c < pRow->Count; c++)
		{
			auto pValue = pRow->get(c);
			if (pValue)
			{
				int len = pValue->GetLength();
				auto pWidth = arrWidths.get(c);
				if (pWidth && len > *pWidth)
					arrWidths.set(c, std::make_shared<int>(len));
			}
		}
	}

	// header
	for (long c = 0; c < arrColumns.Count; c++)
	{
		auto pCol = arrColumns.get(c);
		auto pWidth = arrWidths.get(c);

		CString csTemp;
		csTemp.Format(L"%-*s  ", pWidth ? *pWidth : 0, pCol ? pCol->GetString() : L"");
		csResult += csTemp;
	}
	csResult += L"\n";

	// separator
	for (long c = 0; c < arrColumns.Count; c++)
	{
		auto pWidth = arrWidths.get(c);
		CString csTemp;
		csTemp.Format(L"%-*s  ", pWidth ? *pWidth : 0, CString(L"-").Left(pWidth ? *pWidth : 0));
		csResult += csTemp;
	}
	csResult += L"\n";

	// rows
	for (long r = 0; r < arrRows.Count; r++)
	{
		auto pRow = arrRows.get(r);
		if (!pRow)
			continue;

		for (long c = 0; c < pRow->Count; c++)
		{
			auto pValue = pRow->get(c);
			auto pWidth = arrWidths.get(c);

			CString csTemp;
			csTemp.Format(L"%-*s  ", pWidth ? *pWidth : 0, pValue ? pValue->GetString() : L"");
			csResult += csTemp;
		}

		csResult += L"\n";
	}

	return csResult;
}

/////////////////////////////////////////////////////////////////////////////
