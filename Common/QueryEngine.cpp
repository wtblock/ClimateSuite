/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "QueryEngine.h"

/////////////////////////////////////////////////////////////////////////////
// The constructor initializes the database and maps state names to postal 
// codes.
/////////////////////////////////////////////////////////////////////////////
CQueryEngine::CQueryEngine( CClimateDatabase* pDB)
{
	Database = pDB;

	static const vector<pair<const wchar_t*, const wchar_t*>> arrStates =
	{
		{ L"ALABAMA",         L"AL" },
		{ L"ALASKA",          L"AK" },
		{ L"ARIZONA",         L"AZ" },
		{ L"ARKANSAS",        L"AR" },
		{ L"CALIFORNIA",      L"CA" },
		{ L"COLORADO",        L"CO" },
		{ L"CONNECTICUT",     L"CT" },
		{ L"DELAWARE",        L"DE" },
		{ L"FLORIDA",         L"FL" },
		{ L"GEORGIA",         L"GA" },
		{ L"HAWAII",          L"HI" },
		{ L"IDAHO",           L"ID" },
		{ L"ILLINOIS",        L"IL" },
		{ L"INDIANA",         L"IN" },
		{ L"IOWA",            L"IA" },
		{ L"KANSAS",          L"KS" },
		{ L"KENTUCKY",        L"KY" },
		{ L"LOUISIANA",       L"LA" },
		{ L"MAINE",           L"ME" },
		{ L"MARYLAND",        L"MD" },
		{ L"MASSACHUSETTS",   L"MA" },
		{ L"MICHIGAN",        L"MI" },
		{ L"MINNESOTA",       L"MN" },
		{ L"MISSISSIPPI",     L"MS" },
		{ L"MISSOURI",        L"MO" },
		{ L"MONTANA",         L"MT" },
		{ L"NEBRASKA",        L"NE" },
		{ L"NEVADA",          L"NV" },
		{ L"NEW HAMPSHIRE",   L"NH" },
		{ L"NEW JERSEY",      L"NJ" },
		{ L"NEW MEXICO",      L"NM" },
		{ L"NEW YORK",        L"NY" },
		{ L"NORTH CAROLINA",  L"NC" },
		{ L"NORTH DAKOTA",    L"ND" },
		{ L"OHIO",            L"OH" },
		{ L"OKLAHOMA",        L"OK" },
		{ L"OREGON",          L"OR" },
		{ L"PENNSYLVANIA",    L"PA" },
		{ L"RHODE ISLAND",    L"RI" },
		{ L"SOUTH CAROLINA",  L"SC" },
		{ L"SOUTH DAKOTA",    L"SD" },
		{ L"TENNESSEE",       L"TN" },
		{ L"TEXAS",           L"TX" },
		{ L"UTAH",            L"UT" },
		{ L"VERMONT",         L"VT" },
		{ L"VIRGINIA",        L"VA" },
		{ L"WASHINGTON",      L"WA" },
		{ L"WEST VIRGINIA",   L"WV" },
		{ L"WISCONSIN",       L"WI" },
		{ L"WYOMING",         L"WY" }
	};

	// map the state to their corresponding postal codes
	for (auto& node : arrStates)
	{
		CString csState(node.first);
		CString csCode(node.second);
		PostalCode[csState] = csCode;
	}

} // CQueryEngine

/////////////////////////////////////////////////////////////////////////////
// NormalizeState
//
// Converts natural-language state names into 2-letter postal codes.
// Removes punctuation and handles mixed case.
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::NormalizeState(const CString& csInput)
{
	CString cs = csInput;
	cs.Trim();

	// remove trailing punctuation
	while (!cs.IsEmpty())
	{
		WCHAR ch = cs[cs.GetLength() - 1];
		if (iswalpha(ch))
			break;
		cs.Delete(cs.GetLength() - 1);
	}

	cs.MakeUpper();

	// try full-name lookup
	CString csCode = PostalCode[cs];

	// if lookup succeeded (2-letter code), return it
	if (csCode.GetLength() == 2)
		return csCode;

	// if user typed "TX" or "tx" or "Tx"
	if (cs.GetLength() == 2)
		return cs;

	// fallback: return uppercase input
	return cs;

} // NormalizeState

/////////////////////////////////////////////////////////////////////////////
// dispatch a natural language query to the appropriate SQL query
//
// This method is the central routing hub for all natural-language queries
// issued by either the QueryUSHCN command-line tool or the ClimateSuite
// desktop application. Its job is to:
//
//   1. Normalize the input (lowercase copy for keyword matching)
//   2. Identify the user's intent based on keywords or phrases
//   3. Forward the request to the appropriate handler method
//   4. Return the formatted result text to the caller
//
// The original query string (csQuery) is preserved exactly as the user
// typed it. Only the lowercase copy (csNormalized) is used for matching.
// This allows future handlers to extract tokens (e.g., station IDs,
// state abbreviations, date ranges) without losing case information.
//
// As the query engine grows, this dispatcher will become the central
// decision tree for dozens of supported natural-language commands.
//
/////////////////////////////////////////////////////////////////////////////
bool CQueryEngine::Dispatch(const CString& csQuery, CString& csResult)
{
	//---------------------------------------------------------------------
	// Step 1: Normalize the input for keyword matching
	//---------------------------------------------------------------------
	CString csNormalized = csQuery;
	csNormalized.MakeLower();

	//---------------------------------------------------------------------
	// Raw SQL passthrough
	//---------------------------------------------------------------------
	if (csNormalized.Left(4) == L"sql ")
	{
		CString csSQL = csQuery.Mid(4).Trim();

		CSmartArray<CSmartArray<CString>> arrRows;
		Database->ExecuteTable(csSQL, arrRows);

		// Build synthetic column names based on first row
		CSmartArray<CString> arrColumns;

		if (arrRows.Count > 0)
		{
			long nCols = arrRows.get(0)->Count;
			for (long i = 0; i < nCols; i++)
			{
				CString csCol;
				csCol.Format(L"Column%ld", i + 1);
				arrColumns.append(csCol);
			}
		}

		csResult = QuerySQL->FormatTable(arrColumns, arrRows);
		return true;
	}

	//---------------------------------------------------------------------
	// Step 2: Version query
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"version") >= 0
	)
	{
		csResult = QueryMetadataVersion();
		return true;
	}

	//---------------------------------------------------------------------
	// Step 3: Schema listing
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"schema") >= 0 ||
		csNormalized.Find(L"schemas") >= 0 ||
		csNormalized.Find(L"tables") >= 0
	)
	{
		csResult = QueryTableSchemas();
		return true;
	}

	//---------------------------------------------------------------------
	// Step 4: Station queries
	//---------------------------------------------------------------------
	if
	(
		(csNormalized.Find(L"station") >= 0 ||
			csNormalized.Find(L"stations") >= 0) &&
		csNormalized.Find(L"annual") < 0 &&
		csNormalized.Find(L"monthly") < 0 &&
		csNormalized.Find(L"temperature") < 0 &&
		csNormalized.Find(L"average") < 0 &&
		csNormalized.Find(L"maximum") < 0 &&
		csNormalized.Find(L"minimum") < 0
	)
	{
		// detect "active stations"
		if (csNormalized.Find(L"active") >= 0)
		{
			int nPos = csNormalized.Find(L"in ");
			if (nPos >= 0)
			{
				CString csTail = csQuery.Mid(nPos + 3).Trim();

				// take only the first token after "in "
				int nSpace = csTail.Find(L' ');
				CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

				csState = NormalizeState(csState);

				csResult = QueryActiveStationsByState(csState);
				return true;
			}

			csResult = QueryActiveStations();
			return true;
		}

		// detect "in <state>" pattern
		int nPos = csNormalized.Find(L"in ");
		if (nPos >= 0)
		{
			CString csTail = csQuery.Mid(nPos + 3).Trim();

			// take only the first token after "in "
			int nSpace = csTail.Find(L' ');
			CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

			csState = NormalizeState(csState);

			csResult = QueryStationsByState(csState);
			return true;
		}

		// otherwise list all stations
		csResult = QueryAllStations();
		return true;
	}

	//---------------------------------------------------------------------
	// Step 5: Monthly temperatures
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"monthly") >= 0 &&
		csNormalized.Find(L"temperature") >= 0
	)
	{
		int nPos = csNormalized.Find(L"in ");
		if (nPos >= 0)
		{
			CString csTail = csQuery.Mid(nPos + 3).Trim();

			// take only the first token after "in "
			int nSpace = csTail.Find(L' ');
			CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

			csState = NormalizeState(csState);

			csResult = QueryMonthlyTemperaturesByState(csState);
			return true;
		}

		csResult = L"Please specify a state.";
		return true;
	}

	//---------------------------------------------------------------------
	// Step 6: Annual averages
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"annual") >= 0 &&
		csNormalized.Find(L"average") >= 0
	)
	{
		bool bActive = (csNormalized.Find(L"active") >= 0);

		int nPos = csNormalized.Find(L"in ");
		if (nPos >= 0)
		{
			CString csTail = csQuery.Mid(nPos + 3).Trim();

			// take only the first token after "in "
			int nSpace = csTail.Find(L' ');
			CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

			csState = NormalizeState(csState);

			csResult = QueryAnnualAveragesByState(csState, bActive);

			return true;
		}

		csResult = L"Please specify a state.";
		return true;
	}

	//---------------------------------------------------------------------
	// Step 6: Annual maximums
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"annual") >= 0 &&
		csNormalized.Find(L"maximum") >= 0
	)
	{
		bool bActive = (csNormalized.Find(L"active") >= 0);

		int nPos = csNormalized.Find(L"in ");
		if (nPos >= 0)
		{
			CString csTail = csQuery.Mid(nPos + 3).Trim();

			// take only the first token after "in "
			int nSpace = csTail.Find(L' ');
			CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

			csState = NormalizeState(csState);

			csResult = QueryAnnualMaximumsByState(csState, bActive);

			return true;
		}

		csResult = L"Please specify a state.";
		return true;
	}

	//---------------------------------------------------------------------
	// Step 6: Annual minimums
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"annual") >= 0 &&
		csNormalized.Find(L"minimum") >= 0
	)
	{
		bool bActive = (csNormalized.Find(L"active") >= 0);

		int nPos = csNormalized.Find(L"in ");
		if (nPos >= 0)
		{
			CString csTail = csQuery.Mid(nPos + 3).Trim();

			// take only the first token after "in "
			int nSpace = csTail.Find(L' ');
			CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

			csState = NormalizeState(csState);

			csResult = QueryAnnualMinimumsByState(csState, bActive);

			return true;
		}

		csResult = L"Please specify a state.";
		return true;
	}


	//---------------------------------------------------------------------
	// Step 7: Trend queries (future)
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"trend") >= 0 &&
		csNormalized.Find(L"temperature") >= 0
	)
	{
		int nPos = csNormalized.Find(L"in ");
		if (nPos >= 0)
		{
			CString csTail = csQuery.Mid(nPos + 3).Trim();

			// take only the first token after "in "
			int nSpace = csTail.Find(L' ');
			CString csState = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

			csState = NormalizeState(csState);

			csResult = QueryTemperatureTrendByState(csState);
			return true;
		}

		csResult = L"Please specify a state.";
		return true;
	}


	//---------------------------------------------------------------------
	// Step n: No match found
	//---------------------------------------------------------------------
	csResult = L"Unrecognized query.";
	return false;

} // Dispatch

/////////////////////////////////////////////////////////////////////////////
// return the version of the USHCN data the database is based on
CString CQueryEngine::QueryMetadataVersion()
{
	return Database->Metadata[L"Data_Version"];

} // QueryMetadataVersion

/////////////////////////////////////////////////////////////////////////////
// return a formatted description of all table schemas
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryTableSchemas()
{
	// Use the SQL helper to describe the schema of the database
	return QuerySQL->DescribeSchema(Database);

} // QueryTableSchemas

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of all stations
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAllStations()
{
	CString csSQL =
		L"SELECT "
		L"StationID, "
		L"State, "
		L"Latitude, "
		L"Longitude, "
		L"Elevation, "
		L"Location "
		L"FROM Stations "
		L"ORDER BY State, StationID;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"StationID");
	arrColumns.append(L"State");
	arrColumns.append(L"Latitude");
	arrColumns.append(L"Longitude");
	arrColumns.append(L"Elevation");
	arrColumns.append(L"Location");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // QueryAllStations

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of stations filtered by state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryStationsByState(const CString& csState)
{
	CString csSQL;
	csSQL.Format
	(
		L"SELECT "
		L"StationID, "
		L"State, "
		L"Latitude, "
		L"Longitude, "
		L"Elevation, "
		L"Location "
		L"FROM Stations "
		L"WHERE State = '%s' "
		L"ORDER BY StationID;",
		csState.GetString()
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"StationID");
	arrColumns.append(L"State");
	arrColumns.append(L"Latitude");
	arrColumns.append(L"Longitude");
	arrColumns.append(L"Elevation");
	arrColumns.append(L"Location");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // QueryStationsByState

/////////////////////////////////////////////////////////////////////////////
// return monthly average temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryMonthlyTemperaturesByState(const CString& csState)
{
	CString csSQL;
	csSQL.Format
	(
		L"SELECT "
		L"m.Year, "
		L"m.Month, "
		L"AVG(m.Centigrade) AS AvgTemp "
		L"FROM Months m "
		L"JOIN Stations s ON m.StationID = s.StationID "
		L"WHERE s.State = '%s' "
		L"AND m.MeasurementType = 'TAVG' "
		L"GROUP BY m.Year, m.Month "
		L"ORDER BY m.Year, m.Month;",
		csState.GetString()
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"Month");
	arrColumns.append(L"AvgTemp");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // QueryMonthlyTemperaturesByState

/////////////////////////////////////////////////////////////////////////////
// return annual average temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualAveragesByState(const CString& csState, bool bActive)
{
	CString csSQL;

	csSQL.Format(
		L"SELECT y.Year, AVG(y.AvgValue) AS AvgTemp "
		L"FROM Years y "
		L"JOIN Stations s ON y.StationID = s.StationID "
		L"WHERE s.State = '%s' "
		L"AND y.MeasurementType IN ('TAVG','TAVE','TMEAN','TEMP') ",
		csState.GetString()
	);

	if (bActive)
	{
		csSQL +=
			L"AND y.StationID IN ("
			L"    SELECT StationID "
			L"    FROM Years "
			L"    GROUP BY StationID "
			L"    HAVING MAX(Year) = (SELECT MAX(Year) FROM Years)"
			L") ";
	}

	csSQL +=
		L"GROUP BY y.Year "
		L"ORDER BY y.Year;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"AvgTemp");

	return QuerySQL->FormatTable(arrColumns, arrRows);
} // QueryAnnualAveragesByState

/////////////////////////////////////////////////////////////////////////////
// return annual maximum temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualMaximumsByState(const CString& csState, bool bActive)
{
	CString csSQL;

	csSQL.Format(
		L"SELECT y.Year, AVG(y.MaxValue) AS MaxTemp "
		L"FROM Years y "
		L"JOIN Stations s ON y.StationID = s.StationID "
		L"WHERE s.State = '%s' "
		L"AND y.MeasurementType = 'TMAX' ",
		csState.GetString()
	);

	if (bActive)
	{
		csSQL +=
			L"AND y.StationID IN ("
			L"    SELECT StationID "
			L"    FROM Years "
			L"    GROUP BY StationID "
			L"    HAVING MAX(Year) = (SELECT MAX(Year) FROM Years)"
			L") ";
	}

	csSQL +=
		L"GROUP BY y.Year "
		L"ORDER BY y.Year;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"MaxTemp");

	return QuerySQL->FormatTable(arrColumns, arrRows);
} // QueryAnnualMaximumsByState

/////////////////////////////////////////////////////////////////////////////
// return annual minimum temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualMinimumsByState(const CString& csState, bool bActive)
{
	CString csSQL;

	csSQL.Format(
		L"SELECT y.Year, AVG(y.MinValue) AS MinTemp "
		L"FROM Years y "
		L"JOIN Stations s ON y.StationID = s.StationID "
		L"WHERE s.State = '%s' "
		L"AND y.MeasurementType = 'TMIN' ",
		csState.GetString()
	);

	if (bActive)
	{
		csSQL +=
			L"AND y.StationID IN ("
			L"    SELECT StationID "
			L"    FROM Years "
			L"    GROUP BY StationID "
			L"    HAVING MAX(Year) = (SELECT MAX(Year) FROM Years)"
			L") ";
	}

	csSQL +=
		L"GROUP BY y.Year "
		L"ORDER BY y.Year;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"MinTemp");

	return QuerySQL->FormatTable(arrColumns, arrRows);
} // QueryAnnualMinimumsByState

/////////////////////////////////////////////////////////////////////////////
// return temperature trend (slope) for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryTemperatureTrendByState(const CString& csState)
{
	CString csSQL;
	csSQL.Format
	(
		L"SELECT "
		L"y.Year, "
		L"AVG(y.AvgValue) AS AvgTemp "
		L"FROM Years y "
		L"JOIN Stations s ON y.StationID = s.StationID "
		L"WHERE s.State = '%s' "
		L"AND y.MeasurementType = 'TAVG' "
		L"GROUP BY y.Year "
		L"ORDER BY y.Year;",
		csState.GetString()
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	long n = arrRows.Count;
	double sumX = 0.0;
	double sumY = 0.0;
	double sumXY = 0.0;
	double sumXX = 0.0;

	for (long i = 0; i < n; i++)
	{
		auto pRow = arrRows.get(i);
		if (!pRow || pRow->Count < 2)
			continue;

		int year = _wtoi(pRow->get(0)->GetString());
		double temp = _wtof(pRow->get(1)->GetString());

		sumX += year;
		sumY += temp;
		sumXY += year * temp;
		sumXX += year * year;
	}

	double slope = 0.0;

	if (n > 1)
	{
		slope = (n * sumXY - sumX * sumY) /
			(n * sumXX - sumX * sumX);
	}

	CString csResult;
	csResult.Format
	(
		L"Temperature Trend for %s: %.4f degrees/year\n\n",
		csState.GetString(),
		slope
	);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"AvgTemp");

	csResult += QuerySQL->FormatTable(arrColumns, arrRows);

	return csResult;

} // QueryTemperatureTrendByState

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of all active stations (still recording data)
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryActiveStations()
{
	// SQL: stations whose last recorded year equals the max year in the dataset
	CString csSQL =
		L"SELECT "
		L"  s.StationID, "
		L"  s.Location AS City, "
		L"  s.State, "
		L"  MAX(y.Year) AS LastYear "
		L"FROM Stations s "
		L"JOIN Years y ON s.StationID = y.StationID "
		L"GROUP BY s.StationID, s.Location, s.State "
		L"HAVING MAX(y.Year) = (SELECT MAX(Year) FROM Years) "
		L"ORDER BY s.State, s.Location;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"StationID");
	arrColumns.append(L"City");
	arrColumns.append(L"State");
	arrColumns.append(L"LastYear");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // QueryActiveStations

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of active stations filtered by state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryActiveStationsByState(const CString& csState)
{
	CString csSQL;
	csSQL.Format
	(
		L"SELECT "
		L"  s.StationID, "
		L"  s.Location AS City, "
		L"  s.State, "
		L"  MAX(y.Year) AS LastYear "
		L"FROM Stations s "
		L"JOIN Years y ON s.StationID = y.StationID "
		L"WHERE s.State = '%s' "
		L"GROUP BY s.StationID, s.Location, s.State "
		L"HAVING MAX(y.Year) = (SELECT MAX(Year) FROM Years) "
		L"ORDER BY s.Location;",
		csState.GetString()
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"StationID");
	arrColumns.append(L"City");
	arrColumns.append(L"State");
	arrColumns.append(L"LastYear");

	return QuerySQL->FormatTable(arrColumns, arrRows);

}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
