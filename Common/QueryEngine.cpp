/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "QueryEngine.h"
#include "ClimateTemperature.h"
#include "StationYear.h"
#include "SmartArray.h"

#pragma warning(disable: 6284)

/////////////////////////////////////////////////////////////////////////////
// The constructor initializes the database and maps state names to postal 
// codes.
/////////////////////////////////////////////////////////////////////////////
CQueryEngine::CQueryEngine( CClimateDatabase* pDB)
{
	Database = pDB;
	DebugMode = false;

} // CQueryEngine

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
// return the version of the USHCN data the database is based on
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleVersion()
{
	// Use the SQL helper to describe the schema of the database
	CString value;
	CString csVer = Database->Metadata[L"Data_Version"];
	CString csDate = Database->Metadata[L"Data_ReleaseDate"];
	CString csDataset = Database->Metadata[L"Dataset_Type"];
	value.Format(L"USHCN.%s.%s.%s", csVer, csDate, csDataset);
	return value;
} // HandleVersion

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleSchema()
{
	// Use the SQL helper to describe the schema of the database
	return QuerySQL->DescribeSchema(Database);

} // HandleSchema

/////////////////////////////////////////////////////////////////////////////
// handle a debug case where the user requested on the command line "/dump"
// which is a request to run all of the example natural language queries
// and return the status of those queries
CString CQueryEngine::HandleDumpQueries(CNaturalLanguage& nlp)
{
	CString value;

	CKeyedCollection<CString, int>* pQueries = nlp.MapQueries;

	for (auto& query : pQueries->Items)
	{
		CString csQuery = query.first;

		CNaturalLanguage nlp2(csQuery);
		CNaturalLanguage::IntentType eIntent = nlp2.Intent;

		// ---------------------------------------------------------
		// Dump NLP fields for this query
		// ---------------------------------------------------------
		//value.AppendFormat
		//(
		//	L"    Parsed Fields:\n"
		//	L"        StationID        : %s\n"
		//	L"        State            : %s\n"
		//	L"        YearStart        : %d\n"
		//	L"        YearEnd          : %d\n"
		//	L"        Decade           : %d\n"
		//	L"        MeasureType      : %d\n"
		//	L"        RunningAverage   : %d\n"
		//	L"        PureRaw          : %d\n"
		//	L"        ExcludeQC        : %d\n"
		//	L"        ExcludeEstimated : %d\n"
		//	L"        WantsPlot        : %d\n"
		//	L"        WantsComparison  : %d\n",
		//	nlp2.StationID.GetString(),
		//	nlp2.State.GetString(),
		//	nlp2.YearStart,
		//	nlp2.YearEnd,
		//	nlp2.Decade,
		//	(int)nlp2.MeasureType,
		//	nlp2.RunningAverage,
		//	nlp2.PureRaw,
		//	nlp2.ExcludeQC,
		//	nlp2.ExcludeEstimated,
		//	nlp2.WantsPlot,
		//	nlp2.WantsComparison
		//);

		CString csHandler;

		switch (eIntent)
		{
		case CNaturalLanguage::IntentType::StatisticsTemperature:
			csHandler = L"HandleStatisticsTemperature";
			break;

		case CNaturalLanguage::IntentType::StatisticsStations:
			csHandler = L"HandleStatisticsStations";
			break;

		case CNaturalLanguage::IntentType::StatisticsReadings:
			csHandler = L"HandleStatisticsReadings";
			break;

		case CNaturalLanguage::IntentType::StatisticsAll:
			csHandler = L"HandleStatisticsAll";
			break;

		case CNaturalLanguage::IntentType::PercentMultiThresholdCounts:
			csHandler = L"PercentMultiThresholdCounts";
			break;

		case CNaturalLanguage::IntentType::PercentThresholdCounts:
			csHandler = L"HandlePercentThresholdCounts";
			break;

		case CNaturalLanguage::IntentType::AllStations:
			csHandler = L"HandleAllStations";
			break;

		case CNaturalLanguage::IntentType::StationsByState:
			csHandler = L"HandleStationsByState";
			break;

		case CNaturalLanguage::IntentType::ActiveStations:
			csHandler = L"HandleActiveStations";
			break;

		case CNaturalLanguage::IntentType::StationSummary:
			csHandler = L"HandleStationSummary";
			break;

		case CNaturalLanguage::IntentType::StationReproduceDecade:
			csHandler = L"HandleStationReproduceDecade";
			break;

		case CNaturalLanguage::IntentType::StateMonthly:
			csHandler = L"HandleMonthlyTemperatures";
			break;

		case CNaturalLanguage::IntentType::VerifyYears:
			csHandler = L"HandleVerifyYears";
			break;

		case CNaturalLanguage::IntentType::AnnualTemperatures:
			csHandler = L"HandleAnnualTemperatures";
			break;

		case CNaturalLanguage::IntentType::StateTrend:
			csHandler = L"HandleStateTrend";
			break;

		case CNaturalLanguage::IntentType::ExplainDSFlags:
			csHandler = L"HandleExplainDSFlags";
			break;

		case CNaturalLanguage::IntentType::QCImpact:
			csHandler = L"HandleQCImpact";
			break;

		case CNaturalLanguage::IntentType::Version:
			csHandler = L"HandleVersion";
			break;

		case CNaturalLanguage::IntentType::Schema:
			csHandler = L"HandleSchema";
			break;

		case CNaturalLanguage::IntentType::RawSQL:
			csHandler = L"HandleRawSQL";
			break;

		case CNaturalLanguage::IntentType::Dump:
			csHandler = L"HandleDumpQueries";
			break;

		default:
			csHandler = L"(none)";
			break;
		}

		value.AppendFormat
		(
			L"QueryUSHCN %-60s  Intent: %-3d  Handler: %s\n",
			csQuery.GetString(),
			(int)eIntent,
			csHandler.GetString()
		);
	}

	return value;
} // HandleDumpQueries

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleRawSQL(const CString& csQuery)
{
	// Strip the leading "sql " prefix
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

	// Format the table for output
	return QuerySQL->FormatTable(arrColumns, arrRows);

} // HandleRawSQL

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleUnknownIntent(const CString& csQuery)
{
	return L"Unrecognized query.";

} // HandleUnknownIntent

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStationSummary(const CNaturalLanguage& nlp)
{
	// The NLP parser already extracted the station ID
	const CString& csStationID = nlp.StationID;

	CString csSQL;
	csSQL.Format
	(
		L"SELECT Location, State, Latitude, Longitude, Elevation "
		L"FROM Stations WHERE StationID = '%s';",
		csStationID
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	if (arrRows.Count == 0)
		return L"Station not found.";

	shared_ptr<CSmartArray<CString> > pArray = arrRows.get(0);

	CString csLocation = *pArray->get(0);
	CString csState = *pArray->get(1);
	CString csLat = *pArray->get(2);
	CString csLon = *pArray->get(3);
	CString csElev = *pArray->get(4);
	csLocation.Trim();

	CString csOut;
	csOut.Format
	(
		L"Station: %s, %s (%s)\n"
		L"Location: %s, %s\n"
		L"Elevation: %s meters\n\n"
		L"Available data:\n"
		L"  - Monthly Maximum (MeasurementType = 1)\n"
		L"  - Monthly Minimum (MeasurementType = 2)\n"
		L"  - Monthly Average (MeasurementType = 3)\n",
		csLocation, csState, csStationID,
		csLat, csLon,
		csElev
	);

	return csOut;

} // HandleStationSummary

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStationReproduceDecade(const CNaturalLanguage& nlp)
{
	const CString& csStationID = nlp.StationID;
	const int      nDecade = nlp.Decade;
	const int      nMeasureType = nlp.MeasureType;

	CString csResult;

	if (csStationID.IsEmpty())
	{
		csResult = L"Error: Station ID not specified.";
		return csResult;
	}

	CClimateTemperature::MEASURE_TYPE eType =
		(CClimateTemperature::MEASURE_TYPE)nMeasureType;

	// first year for this station + measurement type
	CClimateDatabase* pDB = Database;
	const int nFirstYear = pDB->GetFirstYear(csStationID, eType);
	if (nFirstYear <= 0)
	{
		csResult = L"Error: No data for station.";
		return csResult;
	}

	int nStartYear = nFirstYear;
	int nEndYear = nFirstYear + 9;
	const int nLastYear = pDB->GetLastYear(csStationID, eType);

	if (nDecade > 0)
	{
		// Positive decades: forward from first year
		nStartYear = nFirstYear + (nDecade - 1) * 10;
		nEndYear = nStartYear + 9;

		// Clamp forward overflow
		if (nEndYear > nLastYear)
		{
			nEndYear = nLastYear;
			nStartYear = nEndYear - 9;
		}
	}
	else
	{
		// Negative decades: backward from last decade
		int offset = (-nDecade - 1) * 10;   // -1 → 0, -2 → 10, -3 → 20, etc.

		nEndYear = nLastYear - offset;
		nStartYear = nEndYear - 9;

		// Clamp backward overflow
		if (nStartYear < nFirstYear)
		{
			nStartYear = nFirstYear;
			nEndYear = nStartYear + 9;
		}
	}

	for (int year = nStartYear; year <= nEndYear; ++year)
	{
		CStationYear stationYear(csStationID, year, eType, pDB);
		CString csLine = stationYear.ReproduceRawLine();
		csResult += csLine;
		csResult += L"\n";
	}

	return csResult;
} // HandleStationReproduceDecade

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of all stations
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleAllStations()
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

} // HandleAllStations

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of all active stations (still recording data)
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleActiveStations(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const int      nYearStart = nlp.YearStart;
	const int      nYearEnd = nlp.YearEnd;

	CString csHaving;

	if (nYearStart <= 0 && nYearEnd <= 0)
	{
		// Original definition: active means station has data in the max year
		csHaving = L"HAVING MAX(y.Year) = (SELECT MAX(Year) FROM Years) ";
	}
	else
	{
		// Range-based definition
		csHaving = L"HAVING 1=1 ";  // anchor for AND clauses

		if (nYearStart > 0)
			csHaving.AppendFormat(L"AND MAX(y.Year) >= %d ", nYearStart);

		if (nYearEnd > 0)
			csHaving.AppendFormat(L"AND MIN(y.Year) <= %d ", nYearEnd);
	}

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
		+ csHaving;
		

	if (!csState.IsEmpty())
	{
		csSQL.AppendFormat(L"AND s.State = '%s' ", csState.GetString());
	}

	csSQL += L"ORDER BY s.State, s.Location;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"StationID");
	arrColumns.append(L"City");
	arrColumns.append(L"State");
	arrColumns.append(L"LastYear");

	return QuerySQL->FormatTable(arrColumns, arrRows);

} // HandleActiveStations

/////////////////////////////////////////////////////////////////////////////
// return monthly temperatures for a given state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleMonthlyTemperatures(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const CString& csStationID = nlp.StationID;
	const int      nMeasureType = nlp.MeasureType;
	const bool     bExcludeEst = nlp.ExcludeEstimated;
	const bool     bExcludeQC = nlp.ExcludeQC;
	const CString& csDSFlagFilter = nlp.DSFlagFilter;
	const int      nYearStart = nlp.YearStart;
	const int      nYearEnd = nlp.YearEnd;
	const bool     bActive = nlp.Active;

	CString csSQL;

	csSQL.Format
	(
		L"SELECT "
		L"m.Year, "
		L"m.Month, "
		L"AVG(m.CentigradeRaw) AS AvgTemp "
		L"FROM Months m "
		L"JOIN Stations s ON m.StationID = s.StationID "
		L"WHERE m.MeasurementType = %d "
		L"AND m.CentigradeRaw > -9000 ",
		nMeasureType
	);

	if (!csState.IsEmpty())
	{
		csSQL.AppendFormat(L"AND s.State = '%s' ", csState.GetString());
	}
	else if (!csStationID.IsEmpty())
	{
		csSQL.AppendFormat(L"AND m.StationID = '%s' ", csStationID.GetString());
	}

	if (bActive)
	{
		csSQL += L"AND s.Active = 1 ";
	}

	if (nYearStart > 0)
	{
		csSQL.AppendFormat(L"AND m.Year >= %d ", nYearStart);
	}

	if (nYearEnd > 0)
	{
		csSQL.AppendFormat(L"AND m.Year <= %d ", nYearEnd);
	}

	if (bExcludeEst)
	{
		csSQL += L"AND m.DMFLAG != 'E' ";
	}

	if (bExcludeQC)
	{
		csSQL += L"AND TRIM(m.QCFlag) = '' ";
	}

	if (!csDSFlagFilter.IsEmpty())
	{
		csSQL.AppendFormat(L"AND m.DSFLAG = '%s' ", csDSFlagFilter.GetString());
	}

	csSQL +=
		L"GROUP BY m.Year, m.Month "
		L"ORDER BY m.Year, m.Month;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Convert and format the AvgTemp column
	for (long i = 0; i < arrRows.Count; i++)
	{
		shared_ptr<CSmartArray<CString>> pRow = arrRows.get(i);

		if
		(
			pRow &&
			pRow->Count >= 3
		)
		{
			CString csRaw = *pRow->get(2);

			// Parse AVG() result as double
			double avgValue = _wtof(csRaw);

			// Convert units using the new double-based helper
			double converted = ConvertUnitsFromDouble
			(
				avgValue,
				nlp.Units
			);

			// Format the converted value
			CString csFormatted = FormatValue(converted, nlp.Units);

			// Store into SmartArray using shared_ptr<CString>
			shared_ptr<CString> pFormatted =
				shared_ptr<CString>(new CString(csFormatted));

			pRow->set(2, pFormatted);
		}
	}

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"Month");

	CString csLabel(L"Value");
	if (nMeasureType == 1)      csLabel = L"MaxTemp";
	else if (nMeasureType == 2) csLabel = L"MinTemp";
	else if (nMeasureType == 3) csLabel = L"AvgTemp";

	arrColumns.append(csLabel);

	CSmartArray<CString> arrUnits;
	arrUnits.append(L""); // Year
	arrUnits.append(L""); // Month

	switch (nlp.Units)
	{
	case CNaturalLanguage::UnitType::DegC:
		arrUnits.append(L"degC");
		break;

	case CNaturalLanguage::UnitType::DegF:
		arrUnits.append(L"degF");
		break;

	case CNaturalLanguage::UnitType::Raw:
	default:
		arrUnits.append(L"raw");
		break;
	}

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleMonthlyTemperatures

/////////////////////////////////////////////////////////////////////////////
// Unified annual temperature handler (avg, max, min)
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleAnnualTemperatures(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const CString& csStationID = nlp.StationID;
	const bool     bActive = nlp.Active;
	const bool     bExcludeEst = nlp.ExcludeEstimated;
	const bool     bExcludeQC = nlp.ExcludeQC;
	const bool     bPureRaw = nlp.PureRaw;
	const CString& csDSFlagFilter = nlp.DSFlagFilter;
	const int      nYearStart = nlp.YearStart;
	const int      nYearEnd = nlp.YearEnd;
	const int      nType = nlp.MeasureType;   // 1=max, 2=min, 3=avg

	// Column label
	CString csOutputLabel;
	switch (nType)
	{
	case 1: csOutputLabel = L"MaxTemp"; break;
	case 2: csOutputLabel = L"MinTemp"; break;
	case 3:
	default: csOutputLabel = L"AvgTemp"; break;
	}

	// Aggregation function
	CString csAggFunc;
	switch (nType)
	{
	case 1: csAggFunc = L"MAX"; break;
	case 2: csAggFunc = L"MIN"; break;
	case 3:
	default: csAggFunc = L"AVG"; break;
	}

	// Base SQL
	CString csSQL;
	csSQL.Format
	(
		L"SELECT "
		L"m.Year, "
		L"%s(m.CentigradeRaw) AS RawValue "
		L"FROM Months m "
		L"JOIN Stations s ON m.StationID = s.StationID "
		L"WHERE m.MeasurementType = %d "
		L"AND m.CentigradeRaw > -9000 ",
		csAggFunc,
		nType
	);

	// State / station / national selection
	if (!csState.IsEmpty())
		csSQL.AppendFormat(L"AND s.State = '%s' ", csState.GetString());
	else if (!csStationID.IsEmpty())
		csSQL.AppendFormat(L"AND m.StationID = '%s' ", csStationID.GetString());

	// Active stations
	if (bActive)
		csSQL += L"AND s.Active = 1 ";

	// Year range
	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND m.Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND m.Year <= %d ", nYearEnd);

	// QC / estimated / pure raw
	if (bPureRaw)
	{
		csSQL += L"AND TRIM(m.QCFlag) = '' ";
		csSQL += L"AND TRIM(m.DMFlag) <> 'E' ";
	}
	else
	{
		if (bExcludeQC)
			csSQL += L"AND TRIM(m.QCFlag) = '' ";

		if (bExcludeEst)
			csSQL += L"AND TRIM(m.DMFlag) <> 'E' ";
	}

	// DS flag filter
	if (!csDSFlagFilter.IsEmpty())
		csSQL.AppendFormat(L"AND m.DSFlag = '%s' ", csDSFlagFilter.GetString());

	// Grouping
	csSQL += L"GROUP BY m.Year ORDER BY m.Year;";

	// Execute
	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Convert and format RawValue column
	for (long i = 0; i < arrRows.Count; i++)
	{
		auto pRow = arrRows.get(i);
		if (pRow && pRow->Count >= 2)
		{
			CString csRaw = *pRow->get(1);
			double rawValue = _wtof(csRaw);

			double converted = ConvertUnitsFromDouble(rawValue, nlp.Units);
			CString csFormatted = FormatValue(converted, nlp.Units);

			pRow->set(1, shared_ptr<CString>(new CString(csFormatted)));
		}
	}

	// Column labels
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(csOutputLabel);

	// Units row
	CSmartArray<CString> arrUnits;
	arrUnits.append(L""); // Year has no units

	switch (nlp.Units)
	{
	case CNaturalLanguage::UnitType::DegC:
		arrUnits.append(L"degC");
		break;

	case CNaturalLanguage::UnitType::DegF:
		arrUnits.append(L"degF");
		break;

	case CNaturalLanguage::UnitType::Raw:
	default:
		arrUnits.append(L"raw");
		break;
	}

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleAnnualTemperatures

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStateTrend(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const bool     bExcludeEst = nlp.ExcludeEstimated;
	const bool     bExcludeQC = nlp.ExcludeQC;
	const CString& csDSFlagFilter = nlp.DSFlagFilter;

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

	if (bExcludeEst)
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
		sumXX += double(year) * year;
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
} // HandleStateTrend

/////////////////////////////////////////////////////////////////////////////
// return a formatted list of stations filtered by state
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStationsByState(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;

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

} // HandleStationsByState

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleExplainDSFlags(const CNaturalLanguage& /*nlp*/)
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

} // HandleExplainDSFlags

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleQCImpact(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const int      nType = nlp.MeasureType;
	const int      nYearStart = nlp.YearStart;
	const int      nYearEnd = nlp.YearEnd;

	CString csAggFunc;

	// MeasurementType: 1=max, 2=min, 3=avg
	switch (nType)
	{
	case 1: // maximum
		csAggFunc = L"MAX";
		break;
	case 2: // minimum
		csAggFunc = L"MIN";
		break;
	case 3: // average
	default:
		csAggFunc = L"AVG";
		break;
	}

	CString csSQL;
	csSQL.Format
	(
		L"SELECT m.Year, "
		L"%s(CASE WHEN TRIM(m.QCFlag)='' THEN m.CentigradeRaw END) AS ExcludeQC, "
		L"%s(m.CentigradeRaw) AS IncludeQC, "
		L"%s(m.CentigradeRaw) - %s(CASE WHEN TRIM(m.QCFlag)='' THEN m.CentigradeRaw END) AS Difference, "
		L"COUNT(CASE WHEN TRIM(m.QCFlag)<>'' THEN 1 END) AS QCMonths "
		L"FROM Months m "
		L"JOIN Stations s ON m.StationID = s.StationID "
		L"WHERE m.MeasurementType = %d "
		L"AND m.CentigradeRaw > -9000 ",
		csAggFunc, csAggFunc, csAggFunc, csAggFunc,
		nType
	);

	if (!csState.IsEmpty())
		csSQL.AppendFormat(L"AND s.State = '%s' ", csState.GetString());

	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND m.Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND m.Year <= %d ", nYearEnd);

	csSQL += L"GROUP BY m.Year ORDER BY m.Year;";

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// ---------------------------------------------------------
	// Apply unit conversion to ExcludeQC, IncludeQC, Difference
	// ---------------------------------------------------------
	for (long i = 0; i < arrRows.Count; i++)
	{
		auto pRow = arrRows.get(i);
		if (!pRow || pRow->Count < 4)
			continue;

		// Column indices:
		// 0 = Year
		// 1 = ExcludeQC
		// 2 = IncludeQC
		// 3 = Difference
		// 4 = QCMonths

		// Convert ExcludeQC
		{
			CString csRaw = *pRow->get(1);
			double rawValue = _wtof(csRaw);

			double converted = ConvertUnitsFromDouble(rawValue, nlp.Units);
			CString csFormatted = FormatValue(converted, nlp.Units);

			pRow->set(1, shared_ptr<CString>(new CString(csFormatted)));
		}

		// Convert IncludeQC
		{
			CString csRaw = *pRow->get(2);
			double rawValue = _wtof(csRaw);

			double converted = ConvertUnitsFromDouble(rawValue, nlp.Units);
			CString csFormatted = FormatValue(converted, nlp.Units);

			pRow->set(2, shared_ptr<CString>(new CString(csFormatted)));
		}

		// Convert Difference (delta temperature)
		{
			CString csRaw = *pRow->get(3);
			double rawValue = _wtof(csRaw);   // raw hundredths of °C

			double converted = rawValue;      // default: Raw

			switch (nlp.Units)
			{
			case CNaturalLanguage::UnitType::DegC:
				converted = rawValue / 100.0;   // hundredths → °C
				break;

			case CNaturalLanguage::UnitType::DegF:
				converted = (rawValue / 100.0) * 9.0 / 5.0;   // delta °C → delta °F
				break;

			case CNaturalLanguage::UnitType::Raw:
			default:
				break;
			}

			CString csFormatted = FormatValue(converted, nlp.Units);
			pRow->set(3, shared_ptr<CString>(new CString(csFormatted)));
		}
	}

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"ExcludeQC");
	arrColumns.append(L"IncludeQC");
	arrColumns.append(L"Difference");
	arrColumns.append(L"QCMonths");

	CSmartArray<CString> arrUnits;
	arrUnits.append(L""); // Year has no units

	switch (nlp.Units)
	{
	case CNaturalLanguage::UnitType::DegC:
		arrUnits.append(L"degC");
		arrUnits.append(L"degC");
		arrUnits.append(L"delta degC");
		break;

	case CNaturalLanguage::UnitType::DegF:
		arrUnits.append(L"degF");
		arrUnits.append(L"degF");
		arrUnits.append(L"delta degF");
		break;

	case CNaturalLanguage::UnitType::Raw:
	default:
		arrUnits.append(L"raw");
		arrUnits.append(L"raw");
		arrUnits.append(L"raw");
		break;
	}

	arrUnits.append(L"count");

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleQCImpact

/////////////////////////////////////////////////////////////////////////////
// annual temperature statistics (max, min, avg across all stations) SQL
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::BuildStatisticsTemperatureSQL(const CNaturalLanguage& nlp)
{
	const int nYearStart = nlp.YearStart;
	const int nYearEnd = nlp.YearEnd;

	// Build SQL
	CString csSQL;
	csSQL =
		L"SELECT Year, "
		L"       AVG(StationMax) AS MaxRaw, "
		L"       AVG(StationMin) AS MinRaw, "
		L"       AVG(StationAvg) AS AvgRaw "
		L"FROM ( "
		L"    SELECT Year, StationID, "
		L"           MAX(CASE WHEN MeasurementType = 1 THEN CentigradeRaw END) AS StationMax, "
		L"           MIN(CASE WHEN MeasurementType = 2 THEN CentigradeRaw END) AS StationMin, "
		L"           AVG(CASE WHEN MeasurementType = 3 THEN CentigradeRaw END) AS StationAvg "
		L"    FROM Months "
		L"    WHERE CentigradeRaw > -9000 ";

	// Year range
	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND Year <= %d ", nYearEnd);

	csSQL +=
		L"    GROUP BY Year, StationID "
		L") AS StationAverages "
		L"GROUP BY Year "
		L"ORDER BY Year;";

	return csSQL;

} // BuildStatisticsTemperatureSQL

/////////////////////////////////////////////////////////////////////////////
// annual temperature statistics (max, min, avg across all stations)
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStatisticsTemperature(const CNaturalLanguage& nlp)
{
	CString csSQL = BuildStatisticsTemperatureSQL(nlp);

	// Execute
	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Column labels
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"Maximum");
	arrColumns.append(L"Minimum");
	arrColumns.append(L"Average");

	// Units row
	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");

	switch (nlp.Units)
	{
	case CNaturalLanguage::UnitType::DegC:
		arrUnits.append(L"degC");
		arrUnits.append(L"degC");
		arrUnits.append(L"degC");
		break;

	case CNaturalLanguage::UnitType::Raw:
		arrUnits.append(L"raw");
		arrUnits.append(L"raw");
		arrUnits.append(L"raw");
		break;

	case CNaturalLanguage::UnitType::DegF:
	default:
		arrUnits.append(L"degF");
		arrUnits.append(L"degF");
		arrUnits.append(L"degF");
		break;
	}

	CClimateTemperature ct;

	// Convert raw values to requested units
	for (auto& row : arrRows.Items)
	{
		for (int col = 1; col <= 3; col++)
		{
			shared_ptr<CString> pCol = row->get(col);
			if (!pCol) continue;

			ct.CentigradeRaw = _wtoi(*pCol);   // raw hundredths °C
			double value = ct.CentigradeRaw;   // default raw

			switch (nlp.Units)
			{
			case CNaturalLanguage::UnitType::DegC:
				value = ct.Centigrade;
				break;

			case CNaturalLanguage::UnitType::DegF:
				value = ct.Fahrenheit;
				break;

			case CNaturalLanguage::UnitType::Raw:
			default:
				break;
			}

			CString csFormatted = FormatValue( value, nlp.Units);
			*pCol = csFormatted;
		}
	}

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleStatisticsTemperature

/////////////////////////////////////////////////////////////////////////////
// annual station statistics SQL
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::BuildStatisticsStationsSQL(const CNaturalLanguage& nlp)
{
	const int nYearStart = nlp.YearStart;
	const int nYearEnd = nlp.YearEnd;

	CString csSQL;
	csSQL =
		L"SELECT Year, "
		L"       COUNT(DISTINCT CASE WHEN MeasurementType = 1 AND CentigradeRaw > -9000 THEN StationID END) AS MaxStat, "
		L"       COUNT(DISTINCT CASE WHEN MeasurementType = 2 AND CentigradeRaw > -9000 THEN StationID END) AS MinStat, "
		L"       COUNT(DISTINCT CASE WHEN MeasurementType = 3 AND CentigradeRaw > -9000 THEN StationID END) AS AvgStat "
		L"FROM Months "
		L"WHERE 1=1 ";

	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND Year <= %d ", nYearEnd);

	csSQL +=
		L"GROUP BY Year "
		L"ORDER BY Year;";

	return csSQL;

} // BuildStatisticsStationsSQL

/////////////////////////////////////////////////////////////////////////////
// annual station statistics 
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStatisticsStations(const CNaturalLanguage& nlp)
{
	CString csSQL = BuildStatisticsStationsSQL(nlp);

	// Execute
	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Column labels
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"MaxStat");
	arrColumns.append(L"MinStat");
	arrColumns.append(L"AvgStat");

	// Units row (none for station counts)
	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleStatisticsStations

/////////////////////////////////////////////////////////////////////////////
// annual reading statistics SQL
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::BuildStatisticsReadingsSQL(const CNaturalLanguage& nlp)
{
	const int nYearStart = nlp.YearStart;
	const int nYearEnd = nlp.YearEnd;

	CString csSQL;
	csSQL =
		L"SELECT Year, "
		L"       COUNT(CASE WHEN MeasurementType = 1 AND CentigradeRaw > -9000 THEN 1 END) AS MaxRead, "
		L"       COUNT(CASE WHEN MeasurementType = 2 AND CentigradeRaw > -9000 THEN 1 END) AS MinRead, "
		L"       COUNT(CASE WHEN MeasurementType = 3 AND CentigradeRaw > -9000 THEN 1 END) AS AvgRead "
		L"FROM Months "
		L"WHERE 1=1 ";

	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND Year <= %d ", nYearEnd);

	csSQL +=
		L"GROUP BY Year "
		L"ORDER BY Year;";

	return csSQL;

} // BuildStatisticsReadingsSQL

/////////////////////////////////////////////////////////////////////////////
// annual reading statistics
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStatisticsReadings(const CNaturalLanguage& nlp)
{
	CString csSQL = BuildStatisticsReadingsSQL(nlp);

	// Execute
	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Column labels
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"MaxRead");
	arrColumns.append(L"MinRead");
	arrColumns.append(L"AvgRead");

	// Units row (none for counts)
	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleStatisticsReadings

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::FormatCSV
(
	const CSmartArray<CString>& arrColumns,
	const CSmartArray<CSmartArray<CString>>& arrRows
)
{
	CString cs;

	// Header
	for (int i = 0; i < arrColumns.Count; ++i)
	{
		cs += *arrColumns.get(i);
		if (i < arrColumns.Count - 1)
			cs += L",";
	}
	cs += L"\n";

	// Rows
	for (auto& row : arrRows.Items)
	{
		for (int i = 0; i < row->Count; ++i)
		{
			cs += *row->get(i);
			if (i < row->Count - 1)
				cs += L",";
		}
		cs += L"\n";
	}

	return cs;
} // FormatCSV

/////////////////////////////////////////////////////////////////////////////
// all annual statistics (composite of temperature, stations, readings,
// and multi-threshold counts equivalent to a line of CSV)
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleStatisticsAll(const CNaturalLanguage& nlp)
{
	const int nYearStart = nlp.YearStart;
	const int nYearEnd = nlp.YearEnd;
	const bool bCSV = nlp.CSV;

	//
	// 1. Run each statistical query internally (raw rows only)
	//

	// Temperature
	CString sqlTemp = BuildStatisticsTemperatureSQL(nlp);
	CSmartArray<CSmartArray<CString>> rowsTemp;
	Database->ExecuteTable(sqlTemp, rowsTemp);

	// Stations
	CString sqlStations = BuildStatisticsStationsSQL(nlp);
	CSmartArray<CSmartArray<CString>> rowsStations;
	Database->ExecuteTable(sqlStations, rowsStations);

	// Readings
	CString sqlReadings = BuildStatisticsReadingsSQL(nlp);
	CSmartArray<CSmartArray<CString>> rowsReadings;
	Database->ExecuteTable(sqlReadings, rowsReadings);

	// Thresholds
	CString sqlThresh = BuildPercentMultiThresholdCountsSQL(nlp);
	CSmartArray<CSmartArray<CString>> rowsThresh;
	Database->ExecuteTable(sqlThresh, rowsThresh);

	//
	// 2. Convert each result set into a map keyed by Year
	//

	struct TempRow { double max, min, avg; };
	struct StatRow { int maxStat, minStat, avgStat; };
	struct ReadRow { int maxRead, minRead, avgRead; };
	struct ThreshRow { double pct[7]; };

	std::map<int, TempRow>   mapTemp;
	std::map<int, StatRow>   mapStations;
	std::map<int, ReadRow>   mapReadings;
	std::map<int, ThreshRow> mapThresh;

	// Temperature rows
	for (auto& r : rowsTemp.Items)
	{
		int year = _wtoi(*r->get(0));
		TempRow tr;
		tr.max = _wtof(*r->get(1));
		tr.min = _wtof(*r->get(2));
		tr.avg = _wtof(*r->get(3));
		mapTemp[year] = tr;
	}

	// Station rows
	for (auto& r : rowsStations.Items)
	{
		int year = _wtoi(*r->get(0));
		StatRow sr;
		sr.maxStat = _wtoi(*r->get(1));
		sr.minStat = _wtoi(*r->get(2));
		sr.avgStat = _wtoi(*r->get(3));
		mapStations[year] = sr;
	}

	// Reading rows
	for (auto& r : rowsReadings.Items)
	{
		int year = _wtoi(*r->get(0));
		ReadRow rr;
		rr.maxRead = _wtoi(*r->get(1));
		rr.minRead = _wtoi(*r->get(2));
		rr.avgRead = _wtoi(*r->get(3));
		mapReadings[year] = rr;
	}

	// Threshold rows
	for (auto& r : rowsThresh.Items)
	{
		int year = _wtoi(*r->get(0));
		ThreshRow tr;
		for (int i = 0; i < 7; ++i)
			tr.pct[i] = _wtof(*r->get(i + 1));
		mapThresh[year] = tr;
	}

	//
	// 3. Merge all maps into a single table
	//

	CSmartArray<CSmartArray<CString>> arrRows;

	for (auto& kv : mapTemp)
	{
		int year = kv.first;

		if 
		(
			!mapStations.count(year) ||
			!mapReadings.count(year) ||
			!mapThresh.count(year)
		)
			continue;

		auto& t = mapTemp[year];
		auto& s = mapStations[year];
		auto& rd = mapReadings[year];
		auto& th = mapThresh[year];

		auto row = make_shared<CSmartArray<CString>>();

		CString csLabel;
		csLabel.Format(L"%d", year);
		row->append(shared_ptr<CString>( new CString(csLabel)));

		csLabel.Format(L"%d", s.maxStat);
		row->append(shared_ptr<CString>( new CString(csLabel)));
		csLabel.Format(L"%d", s.minStat);
		row->append(shared_ptr<CString>( new CString(csLabel)));
		csLabel.Format(L"%d", s.avgStat);
		row->append(shared_ptr<CString>( new CString(csLabel)));

		csLabel.Format(L"%d", rd.maxRead);
		row->append(shared_ptr<CString>( new CString(csLabel)));
		csLabel.Format(L"%d", rd.minRead);
		row->append(shared_ptr<CString>( new CString(csLabel)));
		csLabel.Format(L"%d", rd.avgRead);
		row->append(shared_ptr<CString>( new CString(csLabel)));

		CClimateTemperature ct;

		ct.CentigradeRaw = (int)t.max;
		double maxF = ct.Fahrenheit;

		ct.CentigradeRaw = (int)t.min;
		double minF = ct.Fahrenheit;

		ct.CentigradeRaw = (int)t.avg;
		double avgF = ct.Fahrenheit;

		csLabel.Format(L"%0.2f", maxF);
		row->append(shared_ptr<CString>( new CString(csLabel)));
		csLabel.Format(L"%0.2f", minF);
		row->append(shared_ptr<CString>( new CString(csLabel)));
		csLabel.Format(L"%0.2f", avgF);
		row->append(shared_ptr<CString>( new CString(csLabel)));

		for (int i = 0; i < 7; ++i)
		{
			csLabel.Format(L"%0.2f", th.pct[i]);
			row->append(shared_ptr<CString>(new CString(csLabel)));
		}
		arrRows.append(row);
	}

	//
	// 4. Column labels (matching ClimateHistory CSV)
	//

	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");
	arrColumns.append(L"Max Stat");
	arrColumns.append(L"Min Stat");
	arrColumns.append(L"Avg Stat");
	arrColumns.append(L"Max Read");
	arrColumns.append(L"Min Read");
	arrColumns.append(L"Avg Read");
	arrColumns.append(L"Maximum");
	arrColumns.append(L"Minimum");
	arrColumns.append(L"Average");
	arrColumns.append(L"%>=90F");
	arrColumns.append(L"%>=95F");
	arrColumns.append(L"%>=100F");
	arrColumns.append(L"%>=105F");
	arrColumns.append(L"%>=110F");
	arrColumns.append(L"%>=120F");
	arrColumns.append(L"%>=125F");

	// Units row
	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(L"degF");
	arrUnits.append(L"degF");
	arrUnits.append(L"degF");
	for (int i = 0; i < 7; ++i)
		arrUnits.append(L"%");

	//
	// 5. Format output
	//

	if (bCSV)
	{
		return FormatCSV(arrColumns, arrRows);
	}
	else
	{
		return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);
	}

} // HandleStatisticsAll

/////////////////////////////////////////////////////////////////////////////
// Threshold SQL, i.e. percent of months greater than
// all temperatures from 90 to 125 degF in five degree steps.
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::BuildPercentMultiThresholdCountsSQL(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const CString& csStationID = nlp.StationID;
	const int nYearStart = nlp.YearStart;
	const int nYearEnd = nlp.YearEnd;

	// Threshold list
	static const int thresholds[] = { 90, 95, 100, 105, 110, 120, 125 };
	const int nThreshCount = sizeof(thresholds) / sizeof(thresholds[0]);

	// Convert thresholds to raw hundredths of °C
	int rawThresh[7];
	for (int i = 0; i < nThreshCount; ++i)
	{
		double thresholdC = 0.0;

		switch (nlp.Units)
		{
		case CNaturalLanguage::UnitType::DegF:
			thresholdC = (thresholds[i] - 32.0) * 5.0 / 9.0;
			break;

		case CNaturalLanguage::UnitType::DegC:
			thresholdC = (double)thresholds[i];
			break;

		case CNaturalLanguage::UnitType::Raw:
		default:
			thresholdC = (double)thresholds[i] / 100.0;
			break;
		}

		rawThresh[i] = (int)(thresholdC * 100.0);
	}

	// Build SQL
	CString csSQL;
	csSQL = L"SELECT m.Year, ";

	for (int i = 0; i < nThreshCount; ++i)
	{
		csSQL.AppendFormat(
			L"100.0 * SUM(CASE WHEN m.CentigradeRaw >= %d THEN 1 ELSE 0 END) / "
			L"SUM(CASE WHEN m.CentigradeRaw > -9000 THEN 1 ELSE 0 END) AS Pct%d",
			rawThresh[i], thresholds[i]);

		if (i < nThreshCount - 1)
			csSQL += L", ";
		else
			csSQL += L" ";
	}

	csSQL += L"FROM Months m JOIN Stations s ON m.StationID = s.StationID "
		L"WHERE m.MeasurementType = 1 AND m.CentigradeRaw > -9000 ";

	// Station / State / National selection
	if (!csStationID.IsEmpty())
		csSQL.AppendFormat(L"AND m.StationID = '%s' ", csStationID.GetString());
	else if (!csState.IsEmpty())
		csSQL.AppendFormat(L"AND s.State = '%s' ", csState.GetString());

	// Active stations (historical definition)
	if (nlp.Active)
	{
		if (nYearStart > 0)
			csSQL.AppendFormat
			(
				L"AND EXISTS (SELECT 1 FROM Months m2 "
				L"WHERE m2.StationID = m.StationID "
				L"AND m2.Year >= %d AND m2.CentigradeRaw > -9000) ",
				nYearStart
			);

		if (nYearEnd > 0)
			csSQL.AppendFormat
			(
				L"AND EXISTS (SELECT 1 FROM Months m3 "
				L"WHERE m3.StationID = m.StationID "
				L"AND m3.Year <= %d AND m3.CentigradeRaw > -9000) ",
				nYearEnd
			);
	}

	// Year range
	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND m.Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND m.Year <= %d ", nYearEnd);

	csSQL += L"GROUP BY m.Year ORDER BY m.Year;";
	return csSQL;

} // BuildPercentMultiThresholdCountsSQL

/////////////////////////////////////////////////////////////////////////////
// Threshold report, i.e. percent of months greater than
// all temperatures from 90 to 125 degF in five degree steps.
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandlePercentMultiThresholdCounts(const CNaturalLanguage& nlp)
{
	CString csSQL = BuildPercentMultiThresholdCountsSQL(nlp);

	// Execute
	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Threshold list
	static const int thresholds[] = { 90, 95, 100, 105, 110, 120, 125 };
	const int nThreshCount = sizeof(thresholds) / sizeof(thresholds[0]);

	// Column labels
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");

	for (int i = 0; i < nThreshCount; ++i)
	{
		CString csLabel;
		csLabel.Format(L"%%>=%d%s",
			thresholds[i],
			(nlp.Units == CNaturalLanguage::UnitType::DegF ? L"F" :
				nlp.Units == CNaturalLanguage::UnitType::DegC ? L"C" :
				L" raw"));
		arrColumns.append(csLabel);
	}

	// Units row
	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");
	for (int i = 0; i < nThreshCount; ++i)
		arrUnits.append(L"%");

	// Format percent columns
	for (auto& row : arrRows.Items)
	{
		for (int col = 1; col <= nThreshCount; ++col)
		{
			shared_ptr<CString> pCol = row->get(col);
			if (pCol)
			{
				double pct = _wtof(*pCol);
				CString csFormatted;
				csFormatted.Format(L"% 5.2f", pct);
				*pCol = csFormatted;
			}
		}
	}

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandlePercentMultiThresholdCounts

/////////////////////////////////////////////////////////////////////////////
// Annual percent of months above a threshold temperature
/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandlePercentThresholdCounts(const CNaturalLanguage& nlp)
{
	const CString& csState = nlp.State;
	const CString& csStationID = nlp.StationID;
	const int nYearStart = nlp.YearStart;
	const int nYearEnd = nlp.YearEnd;

	// ---------------------------------------------------------
	// Convert threshold to raw hundredths of °C
	// ---------------------------------------------------------
	double thresholdC = 0.0;

	switch (nlp.Units)
	{
	case CNaturalLanguage::UnitType::DegF:
		thresholdC = (nlp.Threshold - 32.0) * 5.0 / 9.0;
		break;

	case CNaturalLanguage::UnitType::DegC:
		thresholdC = (double)nlp.Threshold;
		break;

	case CNaturalLanguage::UnitType::Raw:
	default:
		thresholdC = (double)nlp.Threshold / 100.0;
		break;
	}

	int thresholdRaw = (int)(thresholdC * 100.0);

	// ---------------------------------------------------------
	// Base SQL
	// ---------------------------------------------------------
	CString csSQL;
	csSQL.Format(
		L"SELECT m.Year, "
		L"100.0 * SUM(CASE WHEN m.CentigradeRaw >= %d THEN 1 ELSE 0 END) / "
		L"SUM(CASE WHEN m.CentigradeRaw > -9000 THEN 1 ELSE 0 END) AS Percent "
		L"FROM Months m "
		L"JOIN Stations s ON m.StationID = s.StationID "
		L"WHERE m.MeasurementType = 1 "
		L"AND m.CentigradeRaw > -9000 ",
		thresholdRaw
	);

	// ---------------------------------------------------------
	// Station / State / National selection
	// ---------------------------------------------------------
	if (!csStationID.IsEmpty())
	{
		csSQL.AppendFormat(L"AND m.StationID = '%s' ", csStationID.GetString());
	}
	else if (!csState.IsEmpty())
	{
		csSQL.AppendFormat(L"AND s.State = '%s' ", csState.GetString());
	}
	else
	{
		// National: no station restriction
	}

	// Active stations filter
	if (nlp.Active)
	{
		// Station must have at least one valid month in the requested period
		if (nYearStart > 0)
			csSQL.AppendFormat
			(
				L"AND EXISTS (SELECT 1 FROM Months m2 "
				L"WHERE m2.StationID = m.StationID "
				L"AND m2.Year >= %d "
				L"AND m2.CentigradeRaw > -9000) ",
				nYearStart
			);

		if (nYearEnd > 0)
			csSQL.AppendFormat
			(
				L"AND EXISTS (SELECT 1 FROM Months m3 "
				L"WHERE m3.StationID = m.StationID "
				L"AND m3.Year <= %d "
				L"AND m3.CentigradeRaw > -9000) ",
				nYearEnd
			);
	}

	// ---------------------------------------------------------
	// Year range
	// ---------------------------------------------------------
	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND m.Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND m.Year <= %d ", nYearEnd);

	csSQL += L"GROUP BY m.Year ORDER BY m.Year;";

	// ---------------------------------------------------------
	// Execute
	// ---------------------------------------------------------
	CSmartArray<CSmartArray<CString> > arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Format percent column: right-justified, width 5, two decimals
	for (auto& pRow : arrRows.Items)
	{
		if (pRow && pRow->Count >= 2)
		{
			shared_ptr<CString> pCol = pRow->get(1);
			if (pCol)
			{
				CString csCol = *pCol;
				double pct = _wtof(csCol);
				CString csFormated;
				csFormated.Format(L"% 5.2f", pct);
				*pCol = csFormated;
			}
		}
	}

	// ---------------------------------------------------------
	// Column labels
	// ---------------------------------------------------------
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"Year");

	CString csLabel;
	csLabel.Format
	(
		L"Percent >= %d%s",
		nlp.Threshold,
		(
			nlp.Units == CNaturalLanguage::UnitType::DegF ? L"F" :
			nlp.Units == CNaturalLanguage::UnitType::DegC ? L"C" :
			L" raw"
		)
	);

	arrColumns.append(csLabel);

	// ---------------------------------------------------------
	// Units row
	// ---------------------------------------------------------
	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");   // Year has no units
	arrUnits.append(L"%");  // Percent

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandlePercentThresholdCounts

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::HandleVerifyYears(const CNaturalLanguage& nlp)
{
	const int      nYearStart = nlp.YearStart;
	const int      nYearEnd = nlp.YearEnd;
	const int      mt = nlp.MeasureType;          // 1=Max, 2=Min, 3=Avg
	CString        station = nlp.StationID;
	CNaturalLanguage::UnitType units = nlp.Units;

	// Column heading based on measurement type
	CString csHeading =
		(mt == 1 ? L"MonthMax" :
			mt == 2 ? L"MonthMin" :
			L"MonthAvg");

	// SQL aggregate function
	CString csAgg =
		(mt == 1 ? L"MAX" :
			mt == 2 ? L"MIN" :
			L"AVG");

	// Correct Years column based on measurement type
	CString csYearCol =
		(mt == 1 ? L"y.MaxValue" :
			mt == 2 ? L"y.MinValue" :
			L"y.AvgValue");

	//
	// Per-station verification (restores earlier functionality)
	//
	CString csSQL;
	csSQL.Format
	(
		L"SELECT y.StationID, y.Year, "
		L"       %s(m.CentigradeRaw) AS MonthValue, "
		L"       %s                  AS YearValue "
		L"FROM Years y "
		L"JOIN Months m "
		L"     ON m.StationID = y.StationID "
		L"    AND m.Year      = y.Year "
		L"WHERE y.StationID       = '%s' "
		L"  AND m.StationID       = '%s' "
		L"  AND y.MeasurementType = %d "
		L"  AND m.MeasurementType = %d "
		L"  AND m.CentigradeRaw > -9000 ",
		csAgg,
		csYearCol,
		station, station,
		mt, mt
	);

	if (nYearStart > 0)
		csSQL.AppendFormat(L"AND m.Year >= %d ", nYearStart);

	if (nYearEnd > 0)
		csSQL.AppendFormat(L"AND m.Year <= %d ", nYearEnd);

	csSQL.AppendFormat
	(
		L"GROUP BY y.StationID, y.Year, %s ORDER BY y.Year;", csYearCol
	);

	CSmartArray<CSmartArray<CString>> arrRows;
	Database->ExecuteTable(csSQL, arrRows);

	// Convert + format
	for (auto& row : arrRows.Items)
	{
		double rawMonth = _wtof(*row->get(2)); // 100ths of a °C
		double rawYear = _wtof(*row->get(3));  // 100ths of a °C

		// Convert both using your unit converter
		double vMonth = ConvertUnitsFromDouble(rawMonth, units);
		double vYear = ConvertUnitsFromDouble(rawYear, units);
		double vDiff = vMonth - vYear;

		*row->get(2) = FormatValue(vMonth, units);
		*row->get(3) = FormatValue(vYear, units);
		row->append(FormatValue(vDiff, units));
	}

	// Columns
	CSmartArray<CString> arrColumns;
	arrColumns.append(L"StationID");
	arrColumns.append(L"Year");
	arrColumns.append(csHeading);
	arrColumns.append(L"YearValue");
	arrColumns.append(L"Diff");

	// Correct unit labels
	CString csUnit =
	(
		units == CNaturalLanguage::UnitType::DegF ? L"degF" :
		units == CNaturalLanguage::UnitType::DegC ? L"degC" :
		L"raw"
	);

	CSmartArray<CString> arrUnits;
	arrUnits.append(L"");
	arrUnits.append(L"");
	arrUnits.append(csUnit);
	arrUnits.append(csUnit);
	arrUnits.append(csUnit);

	return QuerySQL->FormatTable(arrColumns, arrRows, &arrUnits);

} // HandleVerifyYears

/////////////////////////////////////////////////////////////////////////////
double CQueryEngine::ConvertUnits(short rawValue, CNaturalLanguage::UnitType units)
{
	// Create a temporary temperature object for conversion
	CClimateTemperature temp;
	temp.CentigradeRaw = rawValue; 

	// Missing value stays missing
	if (temp.Missing)
	{
		return (double)rawValue;
	}

	// Convert based on requested units
	if (units == CNaturalLanguage::UnitType::Raw)
	{
		return (double)temp.CentigradeRaw;   // hundredths of °C
	}

	if (units == CNaturalLanguage::UnitType::DegC)
	{
		return (double)temp.Centigrade;      // °C
	}

	if (units == CNaturalLanguage::UnitType::DegF)
	{
		return (double)temp.Fahrenheit;      // °F
	}

	// fallback (should never happen)
	return (double)temp.CentigradeRaw;
} // ConvertUnits

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::FormatValue
(
	double value, CNaturalLanguage::UnitType units
)
{
	CString csOut;

	// Format using right‑justified, fixed width
	switch (units)
	{
	case CNaturalLanguage::UnitType::Raw:
		csOut.Format(L"% 6.0f", value);
		break;
	case CNaturalLanguage::UnitType::DegC:
		csOut.Format(L"% 7.2f", value);
		break;
	case CNaturalLanguage::UnitType::DegF:
		csOut.Format(L"% 8.3f", value);
		break;
	}

	return csOut;
} // FormatValue

/////////////////////////////////////////////////////////////////////////////
CString CQueryEngine::PrepareOutputValue
(
	short rawValue, CNaturalLanguage::UnitType units
)
{
	// Step 1: Convert raw hundredths-of-degree to desired units
	double converted = ConvertUnits(rawValue, units);

	// Step 2: Format the converted value for table output
	CString formatted = FormatValue(converted, units);

	return formatted;
} // PrepareOutputValue

/////////////////////////////////////////////////////////////////////////////
double CQueryEngine::ConvertUnitsFromDouble
(
	double rawValue,
	CNaturalLanguage::UnitType units
)
{
	// Create a temporary temperature object for conversion
	CClimateTemperature temp;

	// Convert double AVG value to raw hundredths-of-degree
	// Example: 1234.5678 → 1235 (rounded)
	short rawShort = (short)(rawValue + 0.5);

	temp.CentigradeRaw = rawShort;   // breakpoint here

	if (temp.Missing)
	{
		return rawValue;             // breakpoint here
	}

	if (units == CNaturalLanguage::UnitType::Raw)
	{
		return (double)temp.CentigradeRaw;
	}

	if (units == CNaturalLanguage::UnitType::DegC)
	{
		return (double)temp.Centigrade;
	}

	if (units == CNaturalLanguage::UnitType::DegF)
	{
		return (double)temp.Fahrenheit;
	}

	return rawValue;

} // ConvertUnitsFromDouble

/////////////////////////////////////////////////////////////////////////////
// output the yearly averages as comma separated values (CSV)
CString CQueryEngine::OutputCSV(CKeyedCollection<CString, CClimateYear>& ClimateYears)
{
	CString csHeading
	(
		L"Year,"
		L"Max Stat,"
		L"Min Stat,"
		L"Avg Stat,"
		L"Max Read,"
		L"Min Read,"
		L"Avg Read,"
		L"Maximum,"
		L"Minimum,"
		L"Average,"
		L"%>90,"
		L"%>95,"
		L"%>100,"
		L"%>105,"
		L"%>110,"
		L"%>120,"
		L"%>125"
		L"\n"
	);

	CString value(csHeading);

	const float fMissing = CClimateTemperature::GetMissingValue();

	for (auto& node : ClimateYears.Items)
	{
		CString csOut;
		CString csYear = node.first;
		vector<CStationYear::GREATER_COUNT> counts = node.second->GreaterCounts;
		const int nCounts = (int)counts.size();
		const int nMaxStat = node.second->MaxStations;
		const int nMinStat = node.second->MinStations;
		const int nAvgStat = node.second->AvgStations;
		const int nMaxRead = node.second->MaxReadings;
		const int nMinRead = node.second->MinReadings;
		const int nAvgRead = node.second->AvgReadings;
		const float fMaximum = CHelper::GetFahrenheit(node.second->Maximum, fMissing);
		const float fMinimum = CHelper::GetFahrenheit(node.second->Minimum, fMissing);
		const float fAverage = CHelper::GetFahrenheit(node.second->Average, fMissing);

		// convert the greater than counts into percentage of 
		// valid readings (nMaxRead)
		vector<float> percents(9, 0.0f);

		// handle exceptional cases
		if (counts.size() != 0 && nMaxRead > 0)
		{
			int index = 0;
			for (auto& count : counts)
			{
				if (index < 9)
				{
					percents[index] =
						float(counts[index].second * 100) / nMaxRead;
					index++;
				}
			}
		}

		csOut.Format
		(
			L"%s,%d,%d,%d,%d,%d,%d,%0.2f,%0.2f,%0.2f,"
			L"%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f\n",
			csYear, nMaxStat, nMinStat, nAvgStat, nMaxRead, nMinRead, nAvgRead,
			fMaximum, fMinimum, fAverage,
			percents[0], percents[1], percents[2],
			percents[3], percents[4], percents[5], percents[6]
		);

		value += csOut;
	}

	return value;
} // OutputCSV

/////////////////////////////////////////////////////////////////////////////
bool CQueryEngine::ProcessQuery(const CString& csUserQuery, CString& csResult)
{
	bool value = true;
	csResult.Empty();

	CNaturalLanguage nlp(csUserQuery);

	// Debug keyword detection using tokens
	bool bDebug = false;

	for (const auto& tok : nlp.Tokens)
	{
		if (tok.CompareNoCase(L"debug") == 0)
		{
			bDebug = true;
			break;
		}
	}

	DebugMode = bDebug;

	// ---------------------------------------------------------
	// Debug trace: show all parsed NLP fields
	// ---------------------------------------------------------
	TRACE(L"\n");
	TRACE(L"================ NLP DEBUG ================\n");
	TRACE(L"Original Query     : %s\n", nlp.OriginalQuery);
	TRACE(L"Normalized Query   : %s\n", nlp.NormalizedQuery);
	TRACE(L"Intent             : %d\n", (int)nlp.Intent);
	TRACE(L"StationID          : %s\n", nlp.StationID);
	TRACE(L"State              : %s\n", nlp.State);
	TRACE(L"YearStart          : %d\n", nlp.YearStart);
	TRACE(L"YearEnd            : %d\n", nlp.YearEnd);
	TRACE(L"Decade             : %d\n", nlp.Decade);
	TRACE(L"MeasureType        : %d\n", (int)nlp.MeasureType);
	TRACE(L"RunningAverage     : %d\n", nlp.RunningAverage);
	TRACE(L"PureRaw            : %d\n", nlp.PureRaw);
	TRACE(L"ExcludeQC          : %d\n", nlp.ExcludeQC);
	TRACE(L"ExcludeEstimated   : %d\n", nlp.ExcludeEstimated);
	TRACE(L"WantsPlot          : %d\n", nlp.WantsPlot);
	TRACE(L"WantsComparison    : %d\n", nlp.WantsComparison);
	TRACE(L"===========================================\n\n");

	// ---------------------------------------------------------
	// Phase‑2: Intent Router
	// ---------------------------------------------------------
	switch (nlp.Intent)
	{
	case CNaturalLanguage::IntentType::Dump:
		csResult = HandleDumpQueries(nlp);
		break;

	case CNaturalLanguage::IntentType::RawSQL:
		csResult = HandleRawSQL(csUserQuery);
		break;

	case CNaturalLanguage::IntentType::Version:
		csResult = HandleVersion();
		break;

	case CNaturalLanguage::IntentType::Schema:
		csResult = HandleSchema();
		break;

	case CNaturalLanguage::IntentType::StationSummary:
		csResult = HandleStationSummary(nlp);
		break;

	case CNaturalLanguage::IntentType::StationReproduceDecade:
		csResult = HandleStationReproduceDecade(nlp);
		break;

	case CNaturalLanguage::IntentType::AllStations:
		csResult = HandleAllStations();
		break;

	case CNaturalLanguage::IntentType::StationsByState:
		csResult = HandleStationsByState(nlp);
		break;

	case CNaturalLanguage::IntentType::ActiveStations:
		csResult = HandleActiveStations(nlp);
		break;

	case CNaturalLanguage::IntentType::StateMonthly:
		csResult = HandleMonthlyTemperatures(nlp);
		break;

	case CNaturalLanguage::IntentType::VerifyYears:
		csResult = HandleVerifyYears(nlp);
		break;

	case CNaturalLanguage::IntentType::AnnualTemperatures:
		csResult = HandleAnnualTemperatures(nlp);
		break;

	case CNaturalLanguage::IntentType::StateTrend:
		csResult = HandleStateTrend(nlp);
		break;

	case CNaturalLanguage::IntentType::ExplainDSFlags:
		csResult = HandleExplainDSFlags(nlp);
		break;

	case CNaturalLanguage::IntentType::QCImpact:
		csResult = HandleQCImpact(nlp);
		break;

	case CNaturalLanguage::IntentType::StatisticsTemperature:
		csResult = HandleStatisticsTemperature(nlp);
		break;

	case CNaturalLanguage::IntentType::StatisticsStations:
		csResult = HandleStatisticsStations(nlp);
		break;

	case CNaturalLanguage::IntentType::StatisticsReadings:
		csResult = HandleStatisticsReadings(nlp);
		break;

	case CNaturalLanguage::IntentType::StatisticsAll:
		csResult = HandleStatisticsAll(nlp);
		break;

	case CNaturalLanguage::IntentType::PercentThresholdCounts:
		csResult = HandlePercentThresholdCounts(nlp);
		break;

	case CNaturalLanguage::IntentType::PercentMultiThresholdCounts:
		csResult = HandlePercentMultiThresholdCounts(nlp);
		break;

	case CNaturalLanguage::IntentType::Unknown:
	default:
		csResult = HandleUnknownIntent(csUserQuery);
		value = false;
		break;
	}

	return value;
} // ProcessQuery

/////////////////////////////////////////////////////////////////////////////
