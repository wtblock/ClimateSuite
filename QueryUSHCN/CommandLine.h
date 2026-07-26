/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "afxwin.h"
#include "KeyedCollection.h"

class CCommandLine : public CCommandLineInfo
{
// public definitions
public:
	// types of command line parameters
	typedef enum PARAM_TYPES
	{
		paramNone, 
		paramAll,			// /all list all query examples
		paramStation,		// /station list station queries
		paramState,			// /state list state specific queries
		paramMonthly,		// /month list monthly queries
		paramAnnual,		// /year list annual queries
		paramCounts,		// /count list queries that count limits
		paramDatabase,		// /db list database queries
		paramQC,			// /QC list quality control queries
		paramStatistics,	// /stats annual statistics
		paramDump,			// /dump test all queries
		paramHelp,			// /help explain the command line controls
	} PARAM_TYPES;

// protected data
protected:
	// collection of help categories
	CKeyedCollection<CString, CString> m_mapHelp;

	// last command line parameter flag
	PARAM_TYPES m_eLastType;

	// list all example queries
	bool m_bAll;

	// display help with the types of command line switches
	bool m_bHelp;

	// /station list station queries
	bool m_bStation;

	// /state list state specific queries
	bool m_bState;

	// /month list monthly queries
	bool m_bMonthly;

	// /year list annual queries
	bool m_bAnnual;

	// /count list queries that count limits
	bool m_bCounts;

	// /db list database queries
	bool m_bDatabase;

	// /QC list quality control queries
	bool m_bQualityControl;

	// /statistics list statistics queries
	bool m_bStatistics;

	// /dump debug switch to dump results of multiple queries
	bool m_bDump;

	// the string to be output
	CString m_csOutput;

// public properties
public:
	// type of command line help
	inline CCommandLine::PARAM_TYPES GetLastType()
	{
		return CCommandLine::m_eLastType;
	}
	// type of command line help
	inline void SetLastType(CCommandLine::PARAM_TYPES value)
	{
		CCommandLine::m_eLastType = value;
	}
	// type of command line help
	__declspec(property(get = GetLastType, put = SetLastType))
		CCommandLine::PARAM_TYPES LastType;

	// /all list all query examples
	inline bool GetAll()
	{
		return m_bAll;
	}
	// /all list all query examples
	inline void SetAll(bool value)
	{
		m_bAll = value;
	}
	// /all list all query examples
	__declspec(property(get = GetAll, put = SetAll))
		bool All;

	// display help with the types of command line switches
	inline bool GetHelp()
	{
		return m_bHelp;
	}
	// display help with the types of command line switches
	inline void SetHelp(bool value)
	{
		m_bHelp = value;
	}
	// display help with the types of command line switches
	__declspec(property(get = GetHelp, put = SetHelp))
		bool Help;

	// /station list station queries
	inline bool GetStation()
	{
		return m_bStation;
	}
	// /station list station queries
	inline void SetStation(bool value)
	{
		m_bStation = value;
	}
	// /station list station queries
	__declspec(property(get = GetStation, put = SetStation))
		bool Station;

	// /state list state specific queries
	inline bool GetState()
	{
		return m_bState;
	}
	// /state list state specific queries
	inline void SetState(bool value)
	{
		m_bState = value;
	}
	// /state list state specific queries
	__declspec(property(get = GetState, put = SetState))
		bool State;

	// /statistics list statistics queries
	inline bool GetStatistics()
	{
		return m_bStatistics;
	}
	// /statistics list statistics queries
	inline void SetStatistics(bool value)
	{
		m_bStatistics = value;
	}
	// /statistics list statistics queries
	__declspec(property(get = GetStatistics, put = SetStatistics))
		bool Statistics;

	// /month list monthly queries
	inline bool GetMonthly()
	{
		return m_bMonthly;
	}
	// /month list monthly queries
	inline void SetMonthly(bool value)
	{
		m_bMonthly = value;
	}
	// /month list monthly queries
	__declspec(property(get = GetMonthly, put = SetMonthly))
		bool Monthly;

	// /year list annual queries
	inline bool GetAnnual()
	{
		return m_bAnnual;
	}
	// /year list annual queries
	inline void SetAnnual(bool value)
	{
		m_bAnnual = value;
	}
	// /year list annual queries
	__declspec(property(get = GetAnnual, put = SetAnnual))
		bool Annual;

	// /count list queries that count limits
	inline bool GetCounts()
	{
		return m_bCounts;
	}
	// /count list queries that count limits
	inline void SetCounts(bool value)
	{
		m_bCounts = value;
	}
	// /count list queries that count limits
	__declspec(property(get = GetCounts, put = SetCounts))
		bool Counts;

	// /db list database queries
	inline bool GetDatabase()
	{
		return m_bDatabase;
	}
	// /db list database queries
	inline void SetDatabase(bool value)
	{
		m_bDatabase = value;
	}
	// /db list database queries
	__declspec(property(get = GetDatabase, put = SetDatabase))
		bool Database;

	// /QC list quality control queries
	inline bool GetQualityControl()
	{
		return m_bQualityControl;
	}
	// /QC list quality control queries
	inline void SetQualityControl(bool value)
	{
		m_bQualityControl = value;
	}
	// /QC list quality control queries
	__declspec(property(get = GetQualityControl, put = SetQualityControl))
		bool QualityControl;

	// /dump debug switch to dump results of multiple queries
	inline bool GetDump()
	{
		return m_bDump;
	}
	// /dump debug switch to dump results of multiple queries
	inline void SetDump(bool value)
	{
		m_bDump = value;
	}
	// /dump debug switch to dump results of multiple queries
	__declspec(property(get = GetDump, put = SetDump))
		bool Dump;

	// the string to be output
	inline CString GetOutput()
	{
		return m_csOutput;
	}
	// the string to be output
	inline void SetOutput(CString value)
	{
		m_csOutput = value;
	}
	// the string to be output
	__declspec(property(get = GetOutput, put = SetOutput))
		CString Output;

// protected methods
protected:
	// parse a parameter line flag
	bool ParseParamFlag( LPCTSTR flag );

	// parse a parameter line value
	bool ParseParamNotFlag( LPCTSTR value );

// public methods
public:

// protected overrides
protected:

// public overrides
public:
	// The framework calls this function to parse/interpret individual 
	// parameters from the command line
	virtual void ParseParam( LPCTSTR pszParam, BOOL bFlag, BOOL bLast );

// public construction / destruction
public:
	CCommandLine();
	virtual ~CCommandLine();
};

