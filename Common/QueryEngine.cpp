/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "QueryEngine.h"
#include "ClimateTemperature.h"
#include "SmartArray.h"

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
bool CQueryEngine::StationExists(const CString& csStationID)
{
	CString csSQL;
	csSQL.Format(L"SELECT COUNT(*) FROM Stations WHERE StationID = '%s';", csStationID);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	if (arrRows.Count == 0)
		return false;

	shared_ptr<CSmartArray<CString> > pArray = arrRows.get(0);
	CString csRow = *pArray->get(0);
	return (_ttol(csRow) > 0);
} // StationExists


/////////////////////////////////////////////////////////////////////////////
bool CQueryEngine::DetectStationID(const CString& csQuery, CString& csStationID)
{
	CString csNorm = csQuery;
	csNorm.MakeLower();

	// USHCN station IDs are always 11 characters: ush + 8 digits
	// Example: ush00419532
	// We search for tokens starting with "us" followed by letters/digits.

	CSmartArray<CString> tokens;
	int nStart = 0;
	CString csToken = csNorm.Tokenize(L" ", nStart);
	while (!csToken.IsEmpty())
	{
		tokens.append(csToken);
		csToken = csNorm.Tokenize(L" ", nStart);
	}

	for (auto& node : tokens.Items)
	{
		CString tok = *node;
		tok.Trim();

		if (tok.GetLength() >= 11 &&
			tok.Left(2) == L"us" &&
			IsAlpha(tok[2]) &&
			IsDigit(tok[3]) &&
			IsDigit(tok[4]) &&
			IsDigit(tok[5]) &&
			IsDigit(tok[6]) &&
			IsDigit(tok[7]) &&
			IsDigit(tok[8]) &&
			IsDigit(tok[9]) &&
			IsDigit(tok[10]))
		{
			// Normalize to uppercase for DB lookup
			CString csCandidate = tok;
			csCandidate.MakeUpper();

			if (StationExists(csCandidate))
			{
				csStationID = csCandidate;
				return true;
			}
		}
	}

	return false;
} // DetectStationID

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
	// Purity and data-source filters
	//---------------------------------------------------------------------
	bool bExcludeEstimated = false;   // DMFLAG != 'E'
	bool bExcludeQC = false;   // QCFLAG == ''
	bool bPureRaw = false;   // both of the above
	CString csDSFlagFilter;

	// detect "pure raw" style queries
	if
	(
		csNormalized.Find(L"pure") >= 0 ||
		csNormalized.Find(L"raw only") >= 0 ||
		csNormalized.Find(L"no flags") >= 0
	)
	{
		bPureRaw = true;
	}

	// detect "exclude estimated"
	if
	(
		csNormalized.Find(L"exclude estimated") >= 0 ||
		csNormalized.Find(L"no estimated") >= 0
	)
	{
		bExcludeEstimated = true;
	}

	// detect "exclude qc" / "exclude questionable"
	if
	(
		csNormalized.Find(L"exclude qc") >= 0 ||
		csNormalized.Find(L"no qc") >= 0 ||
		csNormalized.Find(L"exclude questionable") >= 0
	)
	{
		bExcludeQC = true;
	}

	// normalize pure raw
	if (bPureRaw)
	{
		bExcludeEstimated = true;
		bExcludeQC = true;
	}

	// optional: simple DSFLAG filter like "dsflag 2" or "source B"
	int nDSPos = csNormalized.Find(L"dsflag ");
	if (nDSPos < 0)
		nDSPos = csNormalized.Find(L"source ");

	if (nDSPos >= 0)
	{
		// length of "dsflag " or "source " is 7
		CString csTail = csQuery.Mid(nDSPos + 7).Trim();

		// take only the first token
		int     nSpace = csTail.Find(L' ');
		CString csToken = (nSpace >= 0) ? csTail.Left(nSpace) : csTail;

		csToken.Trim();

		if (!csToken.IsEmpty())
			csDSFlagFilter = csToken.Left(1); // DSFLAG is one character
	}

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
	// DSFLAG explanation query
	//---------------------------------------------------------------------
	if
	(
		csNormalized.Find(L"dsflag") >= 0 &&
		(
			csNormalized.Find(L"explain") >= 0 ||
			csNormalized.Find(L"meaning") >= 0 ||
			csNormalized.Find(L"list") >= 0 ||
			csNormalized.Find(L"describe") >= 0
		)
	)
	{
		csResult = QueryExplainDSFlags();
		return true;
	}

	//---------------------------------------------------------------------
	// NEW: Station-specific data queries
	//---------------------------------------------------------------------
	CString csStationID;
	if (DetectStationID(csQuery, csStationID))  // you’d implement this
	{
		// Example: monthly temperature for a station
		if (csNormalized.Find(L"monthly") >= 0 &&
			csNormalized.Find(L"temperature") >= 0)
		{
			// decide measurement type: average / max / min
			int nMeasureType = CClimateTemperature::MEASURE_TYPE::mtAverage;
			if (csNormalized.Find(L"maximum") >= 0)
				nMeasureType = CClimateTemperature::MEASURE_TYPE::mtMaximum;
			else if (csNormalized.Find(L"minimum") >= 0)
				nMeasureType = CClimateTemperature::MEASURE_TYPE::mtMinimum;

			csResult = QueryMonthlyTemperaturesByStation
			(
				csStationID,
				nMeasureType,
				bExcludeEstimated,
				bExcludeQC,
				csDSFlagFilter
			);
			return true;
		}

		// you can add annual / trend station queries here later

		// bare "station USH00419532" → maybe a summary:
		csResult = QueryStationSummary(csStationID);
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

			csResult = QueryMonthlyTemperaturesByState
			(
				csState,
				bExcludeEstimated,
				bExcludeQC,
				csDSFlagFilter
			);
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

			csResult = QueryAnnualAveragesByState
			(
				csState,
				bActive,
				bExcludeEstimated,
				bExcludeQC,
				csDSFlagFilter
			);

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

			csResult = QueryAnnualMaximumsByState
			(
				csState,
				bActive,
				bExcludeEstimated,
				bExcludeQC,
				csDSFlagFilter
			);

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

			csResult = QueryAnnualMinimumsByState
			(
				csState,
				bActive,
				bExcludeEstimated,
				bExcludeQC,
				csDSFlagFilter
			);

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

			csResult = QueryTemperatureTrendByState
			(
				csState,
				bExcludeEstimated,
				bExcludeQC,
				csDSFlagFilter
			);
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
CString CQueryEngine::QueryMonthlyTemperaturesByState
(
	const CString& csState,
	bool           bExcludeEstimated,
	bool           bExcludeQC,
	const CString& csDSFlagFilter
)
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
		L"AND m.MeasurementType = 3 ",
		csState.GetString()
	);

	if (bExcludeEstimated)
	{
		csSQL += L"AND m.DMFLAG != 'E' ";
	}

	if (bExcludeQC)
	{
		csSQL += L"AND m.QCFLAG = '' ";
	}

	if (!csDSFlagFilter.IsEmpty())
	{
		csSQL += L"AND m.DSFLAG = '";
		csSQL += csDSFlagFilter;
		csSQL += L"' ";
	}

	csSQL +=
		L"GROUP BY m.Year, m.Month "
		L"ORDER BY m.Year, m.Month;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"Month");
	arrColumns.append(L"AvgTemp");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // QueryMonthlyTemperaturesByState

/////////////////////////////////////////////////////////////////////////////
// return annual values for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualByStateCommon
(
	const CString& csState,
	int nMeasurementType,
	const CString& csColumnName,   // "AvgValue", "MaxValue", "MinValue"
	const CString& csOutputLabel,  // "AvgTemp", "MaxTemp", "MinTemp"
	bool bActive
)
{
	CString csSQL;

	csSQL.Format
	(
		L"SELECT Y.Year, AVG(Y.%s) "
		L"FROM Years Y "
		L"JOIN Stations S ON S.StationID = Y.StationID "
		L"WHERE S.State = '%s' "
		L"AND Y.MeasurementType = %d ",
		csColumnName,
		csState,
		nMeasurementType
	);

	if (bActive)
		csSQL += L"AND S.Active = 1 ";

	csSQL += L"GROUP BY Y.Year ORDER BY Y.Year;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(csOutputLabel);

	return QuerySQL->FormatTable(arrColumns, arrRows);

} //l QueryAnnualByStateCommon


/////////////////////////////////////////////////////////////////////////////
// return annual average temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualAveragesByState
(
	const CString& csState,
	bool bActive,
	bool /*bExcludeEstimated*/,
	bool /*bExcludeQC*/,
	const CString& /*csDSFlagFilter*/
)
{
	return QueryAnnualByStateCommon(csState, 3, L"AvgValue", L"AvgTemp", bActive);

} // QueryAnnualAveragesByState

/////////////////////////////////////////////////////////////////////////////
// return annual maximum temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualMaximumsByState
(
	const CString& csState,
	bool bActive,
	bool /*bExcludeEstimated*/,
	bool /*bExcludeQC*/,
	const CString& /*csDSFlagFilter*/
)
{
	return QueryAnnualByStateCommon(csState, 2, L"MaxValue", L"MaxTemp", bActive);

} // QueryAnnualMaximumsByState

/////////////////////////////////////////////////////////////////////////////
// return annual minimum temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryAnnualMinimumsByState
(
	const CString& csState,
	bool bActive,
	bool /*bExcludeEstimated*/,
	bool /*bExcludeQC*/,
	const CString& /*csDSFlagFilter*/
)
{
	return QueryAnnualByStateCommon(csState, 1, L"MinValue", L"MinTemp", bActive);

} // QueryAnnualMinimumsByState

/////////////////////////////////////////////////////////////////////////////
// return temperature trend (slope) for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryTemperatureTrendByState
(
	const CString& csState,
	bool           bExcludeEstimated,
	bool           bExcludeQC,
	const CString& csDSFlagFilter
)
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
		L"AND y.MeasurementType = 3 ",
		csState.GetString()
	);

	if (bExcludeEstimated)
	{
		csSQL += L"AND y.DMFLAG != 'E' ";
	}

	if (bExcludeQC)
	{
		csSQL += L"AND y.QCFLAG = '' ";
	}

	if (!csDSFlagFilter.IsEmpty())
	{
		csSQL += L"AND y.DSFLAG = '";
		csSQL += csDSFlagFilter;
		csSQL += L"' ";
	}

	csSQL +=
		L"GROUP BY y.Year "
		L"ORDER BY y.Year;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	long   n = arrRows.Count;
	double sumX = 0.0;
	double sumY = 0.0;
	double sumXY = 0.0;
	double sumXX = 0.0;

	for (long i = 0; i < n; i++)
	{
		auto pRow = arrRows.get(i);
		if (!pRow || pRow->Count < 2)
			continue;

		int    year = _wtoi(pRow->get(0)->GetString());
		double temp = _wtof(pRow->get(1)->GetString());

		sumX += year;
		sumY += temp;
		sumXY += year * temp;
		sumXX += year * year;
	}

	double slope = 0.0;

	if (n > 1)
	{
		slope =
			(n * sumXY - sumX * sumY) /
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

} // QueryActiveStationsByState

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryMonthlyTemperaturesByStation
(
	const CString& csStationID,
	int nMeasurementType,
	bool bExcludeEstimated,
	bool bExcludeQC,
	const CString& csDSFlagFilter
)
{
	CString csSQL;
	csSQL.Format
	(
		L"SELECT Year, Month, Centigrade, DMFlag, QCFlag, DSFlag "
		L"FROM Months "
		L"WHERE StationID = '%s' AND MeasurementType = %d ",
		csStationID,
		nMeasurementType
	);

	if (bExcludeEstimated)
		csSQL += L"AND DMFlag <> 'E' ";

	if (bExcludeQC)
		csSQL += L"AND QCFlag = '' ";

	if (!csDSFlagFilter.IsEmpty())
	{
		csSQL += L"AND DSFlag = '";
		csSQL += csDSFlagFilter;
		csSQL += L"' ";
	}

	csSQL += L"ORDER BY Year, Month;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Build synthetic column names
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"Month");
	arrColumns.append(L"Centigrade");
	arrColumns.append(L"DMFlag");
	arrColumns.append(L"QCFlag");
	arrColumns.append(L"DSFlag");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // QueryMonthlyTemperaturesByStation

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryStationSummary(const CString& csStationID)
{
	CString csSQL;
	csSQL.Format
	(
		L"SELECT Name, State, Latitude, Longitude, Elevation "
		L"FROM Stations WHERE StationID = '%s';",
		csStationID
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	if (arrRows.Count == 0)
		return L"Station not found.";

	shared_ptr<CSmartArray<CString> > pArray = arrRows.get(0);

	CString csName = *pArray->get(0);
	CString csState = *pArray->get(1);
	CString csLat = *pArray->get(2);
	CString csLon = *pArray->get(3);
	CString csElev = *pArray->get(4);

	CString csOut;
	csOut.Format
	(
		L"Station: %s (%s)\n"
		L"Location: %s, %s\n"
		L"Elevation: %s meters\n\n"
		L"Available data:\n"
		L"  - Monthly Minimum (MeasurementType = 1)\n"
		L"  - Monthly Maximum (MeasurementType = 2)\n"
		L"  - Monthly Average (MeasurementType = 3)\n",
		csName, csStationID,
		csLat, csLon,
		csElev
	);

	return csOut;

} // QueryStationSummary

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::QueryExplainDSFlags()
{
	CString cs =

	L"Data Source Flags (DSFLAG):\n"
	L"---------------------------------------------\n"
	L"(blank)  Value computed from daily GHCN-Daily data\n"
	L"1        NCDC Tape Deck 3220, Summary of the Month Element Digital File\n"
	L"2        Means Book - Smithsonian Institute (1876, 1881-1931)\n"
	L"3        Manuscript - Original Records, National Climatic Data Center\n"
	L"4        Climatological Data (CD), monthly NCDC publication\n"
	L"5        Climate Record Book - Weather Bureau (1960)\n"
	L"6        Bulletin W - Summary of Climatological Data (Bigelow, 1912)\n"
	L"7        Local Climatological Data (LCD), monthly NCDC publication\n"
	L"8        State Climatologists, various sources\n"
	L"B        Raymond Bradley - Western U.S. instrumental records (1982)\n"
	L"D        Henry Diaz - compilation from Bulletin W, LCD, Tape Deck 3220 (1983)\n"
	L"G        John Griffiths - primarily from Climatological Data\n\n";

	return cs;

} // QueryExplainDSFlags

/////////////////////////////////////////////////////////////////////////////
