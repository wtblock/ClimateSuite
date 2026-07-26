/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ClimateTemperature.h"
#include "KeyedCollection.h"

/////////////////////////////////////////////////////////////////////////////
// 
// 
// 
/////////////////////////////////////////////////////////////////////////////
class CNaturalLanguage
{
public:
	// data types
	// ---------------------------------------------------------
	// Natural-language intent classification (Phase 1)
	// ---------------------------------------------------------
	enum class IntentType
	{
		Unknown = 0,

		// Meta / system
		RawSQL,
		Version,
		Schema,
		ExplainDSFlags,

		// Station-level
		StationSummary,
		StationReproduceDecade,

		// Station listings
		AllStations,
		StationsByState,
		ActiveStations,

		// State-level temperature queries
		StateMonthly,
		StateTrend,
		AnnualTemperatures,

		// annual statistics: min, max, and avg
		StatisticsTemperature, 
		StatisticsStations, 
		StatisticsReadings,
		StatisticsAll,

		// threshold counts are specialized statistics counting
		// temperatures above given thresholds
		PercentThresholdCounts,
		PercentMultiThresholdCounts,

		// QC
		QCImpact,
		VerifyYears,

		// Debug
		Dump
	};

	enum class UnitType
	{
		Raw = 0,     // hundredths of degC (CentigradeRaw)
		DegC = 1,    // degrees Celsius
		DegF = 2     // degrees Fahrenheit
	};

// protected data
protected:
	// map of state names vs. postal codes
	CKeyedCollection<CString, CString> m_mapStates;

	// map of queries to dump
	CKeyedCollection<CString, int> m_mapQueries;

	// original and normalized query
	CString    m_csOriginalQuery;
	CString    m_csNormalizedQuery;	
	
	// backing fields (Hungarian-style naming)
	IntentType m_eIntent;
	UnitType   m_eUnits;
	CString    m_csStationID;
	CString    m_csState;
	CString    m_csTemperature;
	int        m_nYearStart;
	int        m_nYearEnd;
	int        m_nDecade;
	int        m_nMeasureType;
	int        m_nThreshold;
	bool       m_bRunningAverage;
	int        m_nRunningWindow;
	bool       m_bExcludeEstimated;
	bool       m_bExcludeQC;
	bool       m_bPureRaw;
	CString    m_csDSFlagFilter;
	bool       m_bWantsPlot;
	bool       m_bWantsComparison;
	bool       m_bActive;
	bool       m_bCSV;
	std::vector<CString> m_arrTokens;
	int m_nInToken;

// public properties
public:
	// map of queries
	inline CKeyedCollection<CString, int>* GetMapQueries()
	{
		return &m_mapQueries;
	}
	// map of queries
	__declspec(property(get = GetMapQueries))
		CKeyedCollection<CString, int>* MapQueries;

	// postal code
	inline CString GetPostalCode(CString csState)
	{
		if (m_mapStates.Exists[csState])
		{
			return *m_mapStates.find(csState);
		}
		return csState;
	}
	// postal code
	inline void SetPostalCode(CString csState, CString csCode)
	{
		m_mapStates.add(csState, shared_ptr<CString>(new CString(csCode)));
	}
	// postal code
	__declspec(property(get = GetPostalCode, put = SetPostalCode))
		CString PostalCode[];

	// ---------------------------------------------------------
	// Original query
	// ---------------------------------------------------------
	__declspec(property(get = GetOriginalQuery, put = SetOriginalQuery))
		CString OriginalQuery;

	CString GetOriginalQuery() const
	{
		return m_csOriginalQuery;
	}

	void SetOriginalQuery(const CString& value)
	{
		m_csOriginalQuery = value;
	}

	// ---------------------------------------------------------
	// Normalized query
	// ---------------------------------------------------------
	__declspec(property(get = GetNormalizedQuery, put = SetNormalizedQuery))
		CString NormalizedQuery;

	CString GetNormalizedQuery()
	{
		CString csValue = m_csNormalizedQuery;
		if (csValue.IsEmpty())
		{
			csValue = OriginalQuery;
			csValue.MakeLower();
			NormalizedQuery = csValue;
		}
		return csValue;
	}

	void SetNormalizedQuery(CString value)
	{
		m_csNormalizedQuery = value;
	}


	// ---------------------------------------------------------
	// Intent classification
	// ---------------------------------------------------------
	__declspec(property(get = GetIntent, put = SetIntent))
		IntentType Intent;

	IntentType GetIntent() const
	{
		return m_eIntent;
	}

	void SetIntent(IntentType value)
	{
		m_eIntent = value;
	}

	// ---------------------------------------------------------
	// display units
	// ---------------------------------------------------------
	UnitType GetUnits() const
	{
		return m_eUnits;
	}

	void SetUnits(UnitType value)
	{
		m_eUnits = value;
	}

	__declspec(property(get = GetUnits, put = SetUnits))
		UnitType Units;

	// ---------------------------------------------------------
	// Station ID
	// ---------------------------------------------------------
	__declspec(property(get = GetStationID, put = SetStationID))
		CString StationID;

	CString GetStationID() const
	{
		return m_csStationID;
	}

	void SetStationID(const CString& value)
	{
		m_csStationID = value;
	}

	// ---------------------------------------------------------
	// State
	// ---------------------------------------------------------
	__declspec(property(get = GetState, put = SetState))
		CString State;

	CString GetState() const
	{
		return m_csState;
	}

	void SetState(const CString& value)
	{
		m_csState = value;
	}

	// ---------------------------------------------------------
	// Temperature
	// ---------------------------------------------------------
	__declspec(property(get = GetTemperature, put = SetTemperature))
		CString Temperature;

	CString GetTemperature()
	{
		if (m_csTemperature.IsEmpty())
		{
			CString csTemps = L"90f 95f 100f 105f 110f 115f 120f 125f 130f";
			int nStart = 0;
			CString csToken = csTemps.Tokenize(L" ", nStart);
			while (!csToken.IsEmpty())
			{

				if (m_csNormalizedQuery.Find(csToken) >= 0)
				{
					m_csTemperature = csToken;
					break;
				}
				csToken = csTemps.Tokenize(L" ", nStart);
			}
		}
		return m_csTemperature;
	}

	void SetTemperature(CString& value)
	{
		m_csTemperature = value;
	}

	// ---------------------------------------------------------
	// Year range
	// ---------------------------------------------------------
	__declspec(property(get = GetYearStart, put = SetYearStart))
		int YearStart;

	int GetYearStart() const
	{
		return m_nYearStart;
	}

	void SetYearStart(int value)
	{
		m_nYearStart = value;
	}

	__declspec(property(get = GetYearEnd, put = SetYearEnd))
		int YearEnd;

	int GetYearEnd() const
	{
		return m_nYearEnd;
	}

	void SetYearEnd(int value)
	{
		m_nYearEnd = value;
	}

	// ---------------------------------------------------------
	// Decade
	// ---------------------------------------------------------
	__declspec(property(get = GetDecade, put = SetDecade))
		int Decade;

	int GetDecade() const
	{
		return m_nDecade;
	}

	void SetDecade(int value)
	{
		m_nDecade = value;
	}

	// ---------------------------------------------------------
	// Measurement type (avg / max / min)
	// ---------------------------------------------------------
	__declspec(property(get = GetMeasureType, put = SetMeasureType))
		int MeasureType;

	int GetMeasureType() const
	{
		return m_nMeasureType;
	}

	void SetMeasureType(int value)
	{
		m_nMeasureType = value;
	}

	// ---------------------------------------------------------
	// Threshold percent
	// ---------------------------------------------------------
	__declspec(property(get = GetThreshold, put = SetThreshold))
		int Threshold;

	int GetThreshold() const
	{
		return m_nThreshold;
	}

	void SetThreshold(int value)
	{
		m_nThreshold = value;
	}

	// ---------------------------------------------------------
	// Running average
	// ---------------------------------------------------------
	__declspec(property(get = GetRunningAverage, put = SetRunningAverage))
		bool RunningAverage;

	bool GetRunningAverage() const
	{
		return m_bRunningAverage;
	}

	void SetRunningAverage(bool value)
	{
		m_bRunningAverage = value;
	}

	__declspec(property(get = GetRunningWindow, put = SetRunningWindow))
		int RunningWindow;

	int GetRunningWindow() const
	{
		return m_nRunningWindow;
	}

	void SetRunningWindow(int value)
	{
		m_nRunningWindow = value;
	}

	// ---------------------------------------------------------
	// Purity / flags
	// ---------------------------------------------------------
	__declspec(property(get = GetExcludeEstimated, put = SetExcludeEstimated))
		bool ExcludeEstimated;

	bool GetExcludeEstimated() const
	{
		return m_bExcludeEstimated;
	}

	void SetExcludeEstimated(bool value)
	{
		m_bExcludeEstimated = value;
	}

	__declspec(property(get = GetExcludeQC, put = SetExcludeQC))
		bool ExcludeQC;

	bool GetExcludeQC() const
	{
		return m_bExcludeQC;
	}

	void SetExcludeQC(bool value)
	{
		m_bExcludeQC = value;
	}

	__declspec(property(get = GetPureRaw, put = SetPureRaw))
		bool PureRaw;

	bool GetPureRaw() const
	{
		return m_bPureRaw;
	}

	void SetPureRaw(bool value)
	{
		m_bPureRaw = value;
	}

	__declspec(property(get = GetDSFlagFilter, put = SetDSFlagFilter))
		CString DSFlagFilter;

	CString GetDSFlagFilter() const
	{
		return m_csDSFlagFilter;
	}

	void SetDSFlagFilter(const CString& value)
	{
		m_csDSFlagFilter = value;
	}

	// ---------------------------------------------------------
	// Plotting / comparison
	// ---------------------------------------------------------
	__declspec(property(get = GetWantsPlot, put = SetWantsPlot))
		bool WantsPlot;

	bool GetWantsPlot() const
	{
		return m_bWantsPlot;
	}

	void SetWantsPlot(bool value)
	{
		m_bWantsPlot = value;
	}

	__declspec(property(get = GetWantsComparison, put = SetWantsComparison))
		bool WantsComparison;

	bool GetWantsComparison() const
	{
		return m_bWantsComparison;
	}

	void SetWantsComparison(bool value)
	{
		m_bWantsComparison = value;
	}

	__declspec(property(get = GetActive, put = SetActive))
		bool Active;

	bool GetActive() const
	{
		return m_bActive;
	}

	void SetActive(bool value)
	{
		m_bActive = value;
	}

	__declspec(property(get = GetCSV, put = SetCSV))
		bool CSV;

	bool GetCSV() const
	{
		return m_bCSV;
	}

	void SetCSV(bool value)
	{
		m_bCSV = value;
	}

	__declspec(property(get = GetTokens, put = SetTokens))
		std::vector<CString> Tokens;

	std::vector<CString> GetTokens() const
	{
		return m_arrTokens;
	}

	void SetTokens(const std::vector<CString>& value)
	{
		m_arrTokens = value;
	}

	__declspec(property(get = GetInToken, put = SetInToken))
		int InToken;

	int GetInToken() const
	{
		return m_nInToken;
	}

	void SetInToken(int value)
	{
		m_nInToken = value;
	}

// protected methods
protected:
	void InitializeDefaults()
	{
		Intent = IntentType::Unknown;
		Units = UnitType::DegF;
		MeasureType = CClimateTemperature::MEASURE_TYPE::mtAverage;

		StationID = L"";
		State = L"";
		YearStart = 0;
		YearEnd = 0;
		Decade = 0;
		Threshold = 0;
		RunningAverage = false;
		RunningWindow = 10;
		ExcludeEstimated = true;
		ExcludeQC = true;
		PureRaw = true;
		DSFlagFilter = L"";
		WantsPlot = false;
		CSV = false;
		WantsComparison = false;
		Tokens.clear();
		InToken = -1;
	}

	// internal parsing helpers
	void Normalize(const CString& csQuery);
	void Tokenize();
	IntentType DetectIntent();
	void DetectStation();
	void DetectThreshold();
	void DetectDecade();
	void DetectState();
	void DetectYears();
	void DetectMeasurementType();
	void DetectRunningAverage();
	void DetectFlags();
	void DetectPlotting();
	void DetectActive();
	void DetectUnits();

	// Converts natural-language state names into 2-letter postal codes.
	// Removes punctuation and handles mixed case.
	CString NormalizeState(const CString& csInput);

	// takes a tokenized query and the location of the "in" token
	// and returns the postal code of the state taking into account
	// states with multiword names and puctuation
	inline CString GetState(vector<CString>& tokens, int nIn)
	{
		// an empty value means a state was not found
		CString value;

		// number of tokens in the query
		int nTokens = (int)tokens.size();

		// an nIn value of -1 means the "in" keyword was not present
		if (nIn != -1)
		{
			// index of first word of state name
			int nState1 = nIn + 1;

			// index of optional second word of state name
			int nState2 = nIn + 2;

			// validate indices are in range
			if (nState1 < nTokens)
			{
				// strip puctuation and uppercase
				CString csState = NormalizeState(tokens[nState1]);

				// a two character state name is a postal code
				if (2 == csState.GetLength())
				{
					value = csState;
				}
				else
				{
					// look up the postal code of the state
					CString csPostal = PostalCode[csState];

					// a postal code equal to the state is a failure
					// so test for a two word state name
					if (csPostal == csState && nState2 < nTokens)
					{
						// append a space and the second word
						csState += L" ";
						csState += NormalizeState(tokens[nState2]);

						// lookup the postal code for the two word name
						csPostal = PostalCode[csState];
						if (csPostal != csState)
						{
							value = csPostal;
						}
					}
					else // the postal code was found
					{
						value = csPostal;
					}
				}
			}
		}
		return value;
	}

	// if a decade token was found parse the target decade number
	inline int ParseDecadeNumber()
	{
		int value = 0;
		vector<CString> tokens = Tokens;
		for (int i = 0; i < (int)tokens.size(); ++i)
		{
			const CString& t = tokens[i];

			if (t.CompareNoCase(L"first") == 0)  value = 1;
			if (t.CompareNoCase(L"second") == 0) value = 2;
			if (t.CompareNoCase(L"third") == 0)  value = 3;
			if (t.CompareNoCase(L"last") == 0)   value = -1; // special case
			if (value != 0)
			{
				break;
			}

			// numeric decade: "decade 3"
			if (t.SpanIncluding(L"0123456789").GetLength() == t.GetLength())
			{
				value = _wtoi(t);
			}
			if (value != 0)
			{
				break;
			}
		}
		return value; 
	}

	bool HasWord
	(
		const wchar_t* pszWord
	);

	bool HasStationID();

	bool HasStateName();

	bool HasYearRange();

// public methods
public:

// protected overrides
protected:

// public overrides
public:

	// public constructor/destructor
public:
	CNaturalLanguage(const CString& csQuery);
	~CNaturalLanguage()
	{

	}
}; // class CNaturalLanguage

/////////////////////////////////////////////////////////////////////////////
