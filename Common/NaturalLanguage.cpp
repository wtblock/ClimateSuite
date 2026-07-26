/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "NaturalLanguage.h"
#include "ClimateTemperature.h"
#pragma warning(disable: 26451)

CNaturalLanguage::CNaturalLanguage(const CString& csQuery)
{
	InitializeDefaults();

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

	// queries from the application's usage examples
	static vector<CString> arrQueries =
	{
		L"List all stations.",
		L"List stations in Texas.",
		L"List stations in North Dakota.",
		L"Show stations in CA.",
		L"Show active stations.",
		L"List active stations with cityand state.",
		L"Show active stations in Colorado.",
		L"Show stations active from 1900 through 2025.",
		L"List stations active from 1895 through 2020.",
		L"Show stations active from 1910 through 2026.",
		L"Minimum monthly temperatures for station USH00419532.",
		L"Maximum monthly temperatures for station USH00419532.",
		L"Average monthly temperatures for station USH00419532.",
		L"Monthly temperatures in Texas.",
		L"Monthly temperatures in California.",
		L"Monthly temperatures in Texas units degF.",
		L"Monthly temperatures in Texas units degC.",
		L"Monthly temperatures in Texas units raw.",
		L"Annual averages in Oklahoma.",
		L"Annual averages in Colorado.",
		L"Annual maximums in Texas.",
		L"Annual maximums in Texas from 1900 through 2025.",
		L"Annual minimums in Montana.",
		L"Temperature trend in Colorado.",
		L"Temperature trend in Texas.",
		L"Pure annual averages in Oklahoma.",
		L"Monthly temperatures in Utah including estimated values.",
		L"Annual maximums in Kansas including QC flags.",
		L"Monthly temperatures in Texas.",
		L"Monthly temperatures in California.",
		L"Monthly temperatures in Texas units degF.",
		L"Monthly temperatures in Texas units degC.",
		L"Monthly temperatures in Texas units raw.",
		L"Monthly temperatures in Utah including estimated values.",
		L"Monthly temperatures in Arizona with source B.",
		L"Minimum monthly temperatures for station USH00419532.",
		L"Maximum monthly temperatures for station USH00419532.",
		L"Annual averages in Oklahoma.",
		L"Annual averages in Colorado.",
		L"Annual maximums in Texas.",
		L"Annual maximums in Texas from 1900 through 2025.",
		L"Annual maximums from 1900 through 2025.",
		L"Annual minimums in Montana.",
		L"Verify greater - than counts for station USH00419532.",
		L"Show the percent of months above 100 for station USH00419532.",
		L"Show the percent of months above 90 from 1900 to 1940.",
		L"Show the percent of months above all from 1900 to 1940.",
		L"List your table schemas.",
		L"What version of the data is in the database?",
		L"Reproduce decade 1 for station USH00419532 minimums.",
		L"Reproduce decade 3 for station USH00419532 maximums.",
		L"Reproduce last decade for station USH00419532 averages.",
		L"Compare Kansas annual maximum temperatures QC vs non QC.",
		L"Annual maximums in Kansas, QC difference.",
		L"Show QC influence on Kansas annual maximum temperatures.",
		L"Verify annual maximums for station USH00419532 from 1900 to 1920.",
		L"Verify annual minimums for station USH00419532 units degC from 1900 to 1920.",
		L"Verify annual averages for station USH00419532 from 1900 to 1920.",
		L"Temperature Statistics.",
		L"Temperature Statistics from 1900 to 1920 units raw.",
		L"Station statistics.",
		L"Station statistics from 1900 to 1920.",
		L"Reading statistics.",
		L"Reading statistics from 1900 to 1920.",
		L"CSV statistics.",
		L"CSV statistics from 1900 to 1920 units degC.",
		L"All statistics.",
		L"All statistics from 1900 to 1920 units degC.",
	};

	// copy the queries into a map to remove duplicates
	for (auto& query : arrQueries)
	{
		if (m_mapQueries.Exists[query])
		{
			continue;
		}
		m_mapQueries.add(query, shared_ptr<int>(new int(0)));
	}

	Normalize(csQuery);
	Tokenize();
	DetectStation();
	DetectState();
	DetectThreshold();
	Intent = DetectIntent();
	DetectDecade();
	DetectYears();
	DetectMeasurementType();
	DetectRunningAverage();
	DetectFlags();
	DetectUnits();
	DetectPlotting();
	DetectActive();

} // CNaturalLanguage

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::Normalize(const CString& csQuery)
{
	OriginalQuery = csQuery;

	CString csNormalized = csQuery;
	csNormalized.MakeLower();

	NormalizedQuery = csNormalized;

} // Normalize

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::Tokenize()
{
	std::vector<CString> arrTokens;
	arrTokens.clear();

	int nInToken = -1;

	CString csNormalized = NormalizedQuery;

	int nStart = 0;
	int nIndex = 0;

	CString csToken = csNormalized.Tokenize(L" ", nStart);

	while (!csToken.IsEmpty())
	{
		csToken.TrimRight(L",:;.?!"); 

		arrTokens.push_back(csToken);

		if (csToken == L"in")
		{
			nInToken = nIndex;
		}

		nIndex++;
		csToken = csNormalized.Tokenize(L" ", nStart);
	}

	Tokens = arrTokens;
	InToken = nInToken;

} // Tokenize

/////////////////////////////////////////////////////////////////////////////
// check if a specific word exists in the token list
bool CNaturalLanguage::HasWord
(
	const wchar_t* pszWord
)
{
	CString csWord(pszWord);

	for (auto& tok : Tokens)
	{
		if (tok.Find(csWord) != -1)   // substring match
		{
			return true;
		}
	}

	return false;
} // HasWord

/////////////////////////////////////////////////////////////////////////////
// check if a station ID was parsed
bool CNaturalLanguage::HasStationID()
{
	return !StationID.IsEmpty();

} // HasStationID

/////////////////////////////////////////////////////////////////////////////
// check if a state name was parsed
bool CNaturalLanguage::HasStateName()
{
	return !State.IsEmpty();

} // HasStateName

/////////////////////////////////////////////////////////////////////////////
// check if a valid year range was parsed
bool CNaturalLanguage::HasYearRange()
{
	if (YearStart > 0 && YearEnd > 0)
	{
		if (YearEnd >= YearStart)
		{
			return true;
		}
	}

	return false;
} // HasYearRange

/////////////////////////////////////////////////////////////////////////////
CNaturalLanguage::IntentType CNaturalLanguage::DetectIntent()
{
	// ---------------------------------------------------------
	// Meta / system queries
	// ---------------------------------------------------------
	if (HasWord(L"sql"))
	{
		return IntentType::RawSQL;
	}

	if (HasWord(L"version"))
	{
		return IntentType::Version;
	}

	if (HasWord(L"schema") || HasWord(L"table") || HasWord(L"tables"))
	{
		return IntentType::Schema;
	}

	if (HasWord(L"explain") && HasWord(L"ds"))
	{
		return IntentType::ExplainDSFlags;
	}

	// ---------------------------------------------------------
	// Debug
	// ---------------------------------------------------------
	if (HasWord(L"/dump"))
	{
		return IntentType::Dump;
	}

	// ---------------------------------------------------------
	// annual statistics: min, max, and avg
	// ---------------------------------------------------------
	if (HasWord(L"statistic") && HasWord(L"temperature"))
	{
		return IntentType::StatisticsTemperature;
	}
	if (HasWord(L"statistic") && HasWord(L"station"))
	{
		return IntentType::StatisticsStations;
	}
	if (HasWord(L"statistic") && HasWord(L"reading"))
	{
		return IntentType::StatisticsReadings;
	}

	CSV = HasWord(L"csv");
	if (HasWord(L"statistic") && ( HasWord(L"all") || CSV))
	{
		return IntentType::StatisticsAll;
	}

	// ---------------------------------------------------------
	// Threshold report, i.e. percent of months greater than
	// a given temperature or all (90 to 125)
	// ---------------------------------------------------------
	if (HasWord(L"percent") && HasWord(L"above") && HasWord(L"all"))
		return IntentType::PercentMultiThresholdCounts;

	if
	(
		HasWord(L"percent") &&
		HasWord(L"above") &&
		Threshold > 0
	)
	{
		return IntentType::PercentThresholdCounts;
	}

	// ---------------------------------------------------------
	// QC impact
	// ---------------------------------------------------------
	if (HasWord(L"qc") || HasWord(L"quality") || HasWord(L"influence"))
	{
		return IntentType::QCImpact;
	}

	// Threshold / greater-than / above queries also map here
	if (HasWord(L"above") || HasWord(L"greater") || HasWord(L"threshold"))
	{
		return IntentType::QCImpact;
	}

	// ---------------------------------------------------------
	// Station listings
	// ---------------------------------------------------------
	// 1. Active stations with year range
	if (HasWord(L"active") && HasWord(L"stations") && HasYearRange())
	{
		return IntentType::ActiveStations;
	}

	// 2. Active stations (no year range)
	if (HasWord(L"active") && HasWord(L"stations"))
	{
		return IntentType::ActiveStations;
	}

	// 3. Stations by state
	if ((HasWord(L"list") || HasWord(L"show")) &&
		HasWord(L"stations") &&
		HasStateName())
	{
		return IntentType::StationsByState;
	}

	// 4. All stations
	if (HasWord(L"list") && HasWord(L"stations") && HasWord(L"all"))
	{
		return IntentType::AllStations;
	}

	// ---------------------------------------------------------
	// Station summary
	// ---------------------------------------------------------
	if (HasWord(L"summary") && HasStationID())
	{
		return IntentType::StationSummary;
	}

	// ---------------------------------------------------------
	// Reproduce decade
	// ---------------------------------------------------------
	if (HasWord(L"reproduce") && HasWord(L"decade") && HasStationID())
	{
		return IntentType::StationReproduceDecade;
	}

	// ---------------------------------------------------------
	// State-level monthly
	// ---------------------------------------------------------
	if (HasWord(L"monthly") || HasWord(L"month"))
	{
		return IntentType::StateMonthly;
	}

	// ---------------------------------------------------------
	// Annual averages / maximums / minimums
	// ---------------------------------------------------------
	if (HasWord(L"verify") && HasWord(L"annual") && HasWord(L"average"))
	{
		return IntentType::VerifyYears;
	}

	if (HasWord(L"verify") && HasWord(L"annual") && HasWord(L"maximum"))
	{
		return IntentType::VerifyYears;
	}

	if (HasWord(L"verify") && HasWord(L"annual") && HasWord(L"minimum"))
	{
		return IntentType::VerifyYears;
	}

	if (HasWord(L"annual") && HasWord(L"average"))
	{
		return IntentType::AnnualTemperatures;
	}

	if (HasWord(L"annual") && HasWord(L"maximum"))
	{
		return IntentType::AnnualTemperatures;
	}

	if (HasWord(L"annual") && HasWord(L"minimum"))
	{
		return IntentType::AnnualTemperatures;
	}

	// ---------------------------------------------------------
	// Trend
	// ---------------------------------------------------------
	if (HasWord(L"trend"))
	{
		return IntentType::StateTrend;
	}

	// ---------------------------------------------------------
	// Fallback
	// ---------------------------------------------------------
	return IntentType::Unknown;

} // DetectIntent

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectThreshold()
{
	int value = 0;

	// get tokens using property
	std::vector<CString> arrTokens = Tokens;

	int nToken = 0;
	int nTokens = (int)arrTokens.size();
	for (auto& token : arrTokens)
	{
		// above
		if (token == L"above")
		{
			if (nToken < nTokens - 1)
			{
				CString csNext = arrTokens[nToken + 1];
				if (CHelper::ValidateNumeric(csNext))
				{
					value = (int)_tstol(csNext);
					
					// default to degrees F
					Units = UnitType::DegF;

					break;
				}
			}
		}

		nToken++;
	}

	Threshold = value;

} // DetectThreshold

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectDecade()
{
	int value = -1;

	// get tokens using property
	std::vector<CString> arrTokens = Tokens;

	int nToken = 0;
	int nTokens = (int)arrTokens.size();
	for (auto& token : arrTokens)
	{
		// last decade?
		if (token == L"last")
		{
			if (nToken < nTokens - 1)
			{
				CString csNext = arrTokens[nToken + 1];
				if (csNext == L"decade")
				{
					break;
				}
			}
		}
		else if ( token == L"decade")
		{
			if (nToken < nTokens - 1)
			{
				CString csNext = arrTokens[nToken + 1];
				if (CHelper::ValidateNumeric(csNext))
				{
					value = (int)_tstol(csNext);
					break;
				}
			}
		}

		nToken++;
	}

	Decade = value;
} // DetectDecade

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectStation()
{
	CString csStationID = L"";

	// get tokens using property
	std::vector<CString> arrTokens = Tokens;

	for (const CString& csTok : arrTokens)
	{
		CString tok = csTok;
		tok.Trim();

		// USHCN station IDs are always at least 11 characters:
		// "us" + letter + 8 digits
		if (tok.GetLength() >= 11 &&
			tok.Left(2) == L"us" &&
			iswalpha(tok[2]) &&
			iswdigit(tok[3]) &&
			iswdigit(tok[4]) &&
			iswdigit(tok[5]) &&
			iswdigit(tok[6]) &&
			iswdigit(tok[7]) &&
			iswdigit(tok[8]) &&
			iswdigit(tok[9]) &&
			iswdigit(tok[10]))
		{
			// normalize to uppercase for DB lookup
			CString csCandidate = tok;
			csCandidate.Trim();

			// remove trailing punctuation
			csCandidate = CHelper::StripPunctuation(csCandidate);

			csCandidate.MakeUpper();

			// assign and stop scanning
			csStationID = csCandidate;
			break;
		}
	}

	// assign using property
	StationID = csStationID;

} // DetectStation

/////////////////////////////////////////////////////////////////////////////
// NormalizeState
//
// Converts natural-language state names into 2-letter postal codes.
// Removes punctuation and handles mixed case.
/////////////////////////////////////////////////////////////////////////////
CString CNaturalLanguage::NormalizeState(const CString& csInput)
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
void CNaturalLanguage::DetectState()
{
	State.Empty();
	InToken = -1;

	const int nCount = (int)m_arrTokens.size();
	int nIn = -1;

	// Find the index of the word "in"
	for (int i = 0; i < nCount; ++i)
	{
		if (m_arrTokens[i].CompareNoCase(L"in") == 0)
		{
			nIn = i;
			break;
		}
	}

	// If "in" was found, extract the state using our internal GetState()
	if (nIn != -1)
	{
		State = GetState(m_arrTokens, nIn);
		InToken = nIn; 
	}
} // DetectState

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectYears()
{
	int nYearStart = 0;
	int nYearEnd = 0;

	std::vector<CString> arrTokens = Tokens;

	// ---------------------------------------------------------
	// Scan tokens for years and decades
	// ---------------------------------------------------------
	std::vector<int> arrYears;
	arrYears.clear();

	for (const CString& csTok : arrTokens)
	{
		CString tok = csTok;
		tok.Trim();

		// strip punctuation (commas, periods, etc.)
		tok = CHelper::StripPunctuation(tok);

		// detect 4-digit year
		if (tok.GetLength() == 4 &&
			iswdigit(tok[0]) &&
			iswdigit(tok[1]) &&
			iswdigit(tok[2]) &&
			iswdigit(tok[3]))
		{
			int nYear = _wtoi(tok);
			if (nYear >= 1800 && nYear <= 2100)
			{
				arrYears.push_back(nYear);
			}
		}
	}

	// ---------------------------------------------------------
	// Interpret results
	// ---------------------------------------------------------

	if (arrYears.size() == 1)
	{
		// single year
		nYearStart = arrYears[0];
		nYearEnd = arrYears[0];
	}
	else if (arrYears.size() >= 2)
	{
		// range: use first two years
		nYearStart = arrYears[0];
		nYearEnd = arrYears[1];
	}

	// assign using properties
	YearStart = nYearStart;
	YearEnd = nYearEnd;

} // DetectYears

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectMeasurementType()
{
	CString csNormalized = NormalizedQuery;

	// ---------------------------------------------------------
	// Measurement type: maximum, minimum, average
	// ---------------------------------------------------------

	if (csNormalized.Find(L"maximum") >= 0)
	{
		MeasureType = CClimateTemperature::mtMaximum;
	}
	else if (csNormalized.Find(L"minimum") >= 0)
	{
		MeasureType = CClimateTemperature::mtMinimum;
	}
	else if (csNormalized.Find(L"average") >= 0)
	{
		MeasureType = CClimateTemperature::mtAverage;
	} 
	else
	{
		MeasureType = CClimateTemperature::mtMaximum;
	}

} // DetectMeasurementType

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectFlags()
{
	// These represent user overrides
	bool bIncludeEstimated = false;
	bool bIncludeQC = false;
	bool bPureRawKeyword = false;
	bool bWantsComparison = false;

	CString csNormalized = NormalizedQuery;

	// ---------------------------------------------------------
	// Include flags (user overrides default raw behavior)
	// ---------------------------------------------------------
	if 
	(
		csNormalized.Find(L"include estimate") >= 0 ||
		csNormalized.Find(L"including estimate") >= 0

	)
		bIncludeEstimated = true;

	if 
	(
		csNormalized.Find(L"include qc") >= 0 ||
		csNormalized.Find(L"including qc") >= 0 ||
		csNormalized.Find(L"include flag") >= 0 ||
		csNormalized.Find(L"including flag") >= 0 ||
		csNormalized.Find(L"include suspect") >= 0 ||
		csNormalized.Find(L"including suspect") >= 0
	)
		bIncludeQC = true;

	// ---------------------------------------------------------
	// Pure raw flags (user explicitly requests raw-only)
	// ---------------------------------------------------------
	if 
	(
		csNormalized.Find(L"pure") >= 0 ||
		csNormalized.Find(L"pure only") >= 0 ||
		csNormalized.Find(L"pure data") >= 0
	)
		bPureRawKeyword = true;

	// ---------------------------------------------------------
	// Comparison
	// ---------------------------------------------------------
	if 
	(
		csNormalized.Find(L"compare") >= 0 ||
		csNormalized.Find(L"comparison") >= 0 ||
		csNormalized.Find(L"versus") >= 0 ||
		csNormalized.Find(L"vs") >= 0
	)
		bWantsComparison = true;

	// ---------------------------------------------------------
	// Apply logic
	// ---------------------------------------------------------

	// Default from InitializeDefaults():
	// PureRaw = true
	// ExcludeEstimated = true
	// ExcludeQC = true

	// If user explicitly requests raw-only, enforce raw-only
	if (bPureRawKeyword)
	{
		PureRaw = true;
		ExcludeEstimated = true;
		ExcludeQC = true;
	}

	// If user explicitly requests inclusion, raw-only must be disabled
	if (bIncludeEstimated || bIncludeQC)
	{
		PureRaw = false;  // user wants non-raw behavior
	}

	// Apply include/exclude overrides
	if (bIncludeEstimated)
		ExcludeEstimated = false;

	if (bIncludeQC)
		ExcludeQC = false;

	WantsComparison = bWantsComparison;
} // DetectFlags

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectRunningAverage()
{
	bool bRunningAverage = false;

	CString csNormalized = NormalizedQuery;

	// ---------------------------------------------------------
	// Running average / running window detection
	// ---------------------------------------------------------

	if 
	(
		csNormalized.Find(L"running average") >= 0 ||
		csNormalized.Find(L"moving average") >= 0 ||
		csNormalized.Find(L"running window") >= 0 ||
		csNormalized.Find(L"moving window") >= 0 ||
		csNormalized.Find(L"running mean") >= 0 ||
		csNormalized.Find(L"moving mean") >= 0
	)
	{
		bRunningAverage = true;
	}

	// assign using property
	RunningAverage = bRunningAverage;

} // DetectRunningAverage

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectPlotting()
{
	bool bWantsPlot = false;

	CString csNormalized = NormalizedQuery;

	// ---------------------------------------------------------
	// Plotting intent
	// ---------------------------------------------------------

	if (csNormalized.Find(L"plot") >= 0 ||
		csNormalized.Find(L"graph") >= 0 ||
		csNormalized.Find(L"chart") >= 0)
	{
		bWantsPlot = true;
	}

	// assign using property
	WantsPlot = bWantsPlot;

} // DetectPlotting

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectActive()
{
	// Default is false; only set true if found
	Active = false;

	for (const auto& token : m_arrTokens)
	{
		if (token.CompareNoCase(L"active") == 0)
		{
			Active = true;
			return;
		}
	}
} // DetectActive

/////////////////////////////////////////////////////////////////////////////
void CNaturalLanguage::DetectUnits()
{
	// scan tokens for unit hints
	for (int i = 0; i < (int)m_arrTokens.size(); ++i)
	{
		CString tok = m_arrTokens[i];
		tok.Trim();
		tok.MakeLower();

		// pattern: "units degc", "format degf", "output raw"
		if (tok == L"units" || tok == L"format" || tok == L"output")
		{
			int nNext = i + 1;
			if (nNext < (int)m_arrTokens.size())
			{
				CString next = m_arrTokens[nNext];
				next.Trim();
				next.MakeLower();

				if (next == L"degc" || next == L"celsius")
				{
					Units = UnitType::DegC;
				}
				else if (next == L"degf" || next == L"fahrenheit")
				{
					Units = UnitType::DegF;
				}
				else if (next == L"raw" || next == L"hundredths")
				{
					Units = UnitType::Raw;
				}
			}
			return;
		}

		// bare unit tokens anywhere in the query
		if (tok == L"degc" || tok == L"celsius")
		{
			Units = UnitType::DegC;
		}
		else if (tok == L"degf" || tok == L"fahrenheit")
		{
			Units = UnitType::DegF;
		}
		else if (tok == L"raw" || tok == L"hundredths")
		{
			Units = UnitType::Raw;
		}
	}
} // DetectUnits

/////////////////////////////////////////////////////////////////////////////
