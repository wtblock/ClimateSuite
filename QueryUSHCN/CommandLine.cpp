/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "CommandLine.h"

/////////////////////////////////////////////////////////////////////////////
CCommandLine::CCommandLine()
{
	LastType = paramNone;
	All = false;
	Help = false;
	Station = false;
	Statistics = false;
	State = false;
	Monthly = false;
	Annual = false;
	Counts = false;
	Database = false;
	QualityControl = false;
	Dump = false;

	CString csHelp =
		L"Help parameters begin with a forward slash (/):\n"
		L".\n"
		L".    /help        this output to explain the command line parameters\n"
		L".    /all         list all query examples\n"
		L".    /station     list station queries\n"
		L".    /statistics  list statistical queries\n"
		L".    /state       list state specific queries\n"
		L".    /month       list monthly queries\n"
		L".    /year        list annual queries\n"
		L".    /count       list queries that count limits\n"
		L".    /db          list database queries\n"
		L".    /QC          list quality control queries\n\n";
	CString csStation =
		L"Examples of queries specific to stations:\n"
		L".\n"
		L".    QueryUSHCN List all stations.\n"
		L".    QueryUSHCN List stations in Texas.\n"
		L".    QueryUSHCN List stations in North Dakota.\n"
		L".    QueryUSHCN Show stations in CA.\n"
		L".\n"
		L".    QueryUSHCN Show active stations.\n"
		L".    QueryUSHCN List active stations with city and state.\n"
		L".    QueryUSHCN Show active stations in Colorado.\n"
		L".\n"
		L".    QueryUSHCN Show stations active from 1900 through 2025.\n"
		L".    QueryUSHCN List stations active from 1895 through 2020.\n"
		L".    QueryUSHCN Show stations active from 1910 through 2026.\n"
		L".\n"
		L".    QueryUSHCN Minimum monthly temperatures for station USH00419532.\n"
		L".    QueryUSHCN Maximum monthly temperatures for station USH00419532.\n"
		L".    QueryUSHCN Average monthly temperatures for station USH00419532.\n\n";
	CString csStatistics =
		L"Examples of queries about statistics:\n"
		L".    QueryUSHCN Temperature Statistics.\n"
		L".    QueryUSHCN Temperature Statistics from 1900 to 1920 units raw.\n"
		L".    QueryUSHCN Station statistics.\n"
		L".    QueryUSHCN Station statistics from 1900 to 1920.\n"
		L".    QueryUSHCN Reading statistics.\n"
		L".    QueryUSHCN Reading statistics from 1900 to 1920.\n"
		L".    QueryUSHCN CSV statistics.\n"
		L".    QueryUSHCN CSV statistics from 1900 to 1920 units degC.\n"
		L".    QueryUSHCN All statistics.\n"
		L".    QueryUSHCN All statistics from 1900 to 1920 units degC.\n\n";
	CString csState =
		L"Examples of queries specific to states:\n"
		L".\n"
		L".    QueryUSHCN Monthly temperatures in Texas.\n"
		L".    QueryUSHCN Monthly temperatures in California.\n"
		L".    QueryUSHCN Monthly temperatures in Texas units degF.\n"
		L".    QueryUSHCN Monthly temperatures in Texas units degC.\n"
		L".    QueryUSHCN Monthly temperatures in Texas units raw.\n"
		L".\n"
		L".    QueryUSHCN Annual averages in Oklahoma.\n"
		L".    QueryUSHCN Annual averages in Colorado.\n"
		L".\n"
		L".    QueryUSHCN Annual maximums in Texas.\n"
		L".    QueryUSHCN Annual maximums in Texas from 1900 through 2025.\n"
		L".    QueryUSHCN Annual minimums in Montana.\n"
		L".\n"
		L".    QueryUSHCN Temperature trend in Colorado.\n"
		L".    QueryUSHCN Temperature trend in Texas.\n"
		L".\n"
		L".    QueryUSHCN Pure annual averages in Oklahoma.\n"
		L".    QueryUSHCN Monthly temperatures in Utah including estimated values.\n"
		L".    QueryUSHCN Annual maximums in Kansas including QC flags.\n\n";
	CString csMonthly =
		L"Examples of queries supplying data on a monthly basis:\n"
		L".\n"
		L".    QueryUSHCN Monthly temperatures in Texas.\n"
		L".    QueryUSHCN Monthly temperatures in California.\n"
		L".\n"
		L".    QueryUSHCN Monthly temperatures in Texas units degF.\n"
		L".    QueryUSHCN Monthly temperatures in Texas units degC.\n"
		L".    QueryUSHCN Monthly temperatures in Texas units raw.\n"
		L".\n"
		L".    QueryUSHCN Monthly temperatures in Utah including estimated values.\n"
		L".    QueryUSHCN Monthly temperatures in Arizona with source B.\n"
		L".    QueryUSHCN Minimum monthly temperatures for station USH00419532.\n"
		L".    QueryUSHCN Maximum monthly temperatures for station USH00419532.\n\n";
	CString csAnnual =
		L"Examples of queries supplying data on an annual basis:\n"
		L"NOTE: Monthly queries without Monthly in the query are annual.\n"
		L".\n"
		L".    QueryUSHCN Annual averages in Oklahoma.\n"
		L".    QueryUSHCN Annual averages in Colorado.\n"
		L".\n"
		L".    QueryUSHCN Annual maximums in Texas.\n"
		L".    QueryUSHCN Annual maximums in Texas from 1900 through 2025.\n"
		L".    QueryUSHCN Annual maximums from 1900 through 2025.\n"
		L".    QueryUSHCN Annual minimums in Montana.\n\n";
	CString csCounts = 
		L"Examples of queries counting values above a threshold in percent:\n"
		L".\n"
		L".    QueryUSHCN Show the percent of months above 100 from 1977 to 1987\n"
		L".    QueryUSHCN Show the percent of months above all from 1977 to 1987.\n"
		L".    QueryUSHCN Show the percent of months above 100 for station USH00419532.\n\n";
	CString csDatabase =
		L"Examples of queries about the database as a whole:\n"
		L".\n"
		L".    QueryUSHCN List your table schemas.\n"
		L".    QueryUSHCN What version of the data is in the database?\n\n";
	CString csQC =
		L"Examples of queries providing quality control by reproducing original\n"
		L"source lines a decade at a time to provide confidence in the\n"
		L"content of the database:\n"
		L".\n"
		L".    QueryUSHCN Reproduce decade 1 for station USH00419532 minimums.\n"
		L".    QueryUSHCN Reproduce decade 3 for station USH00419532 maximums.\n"
		L".    QueryUSHCN Reproduce last decade for station USH00419532 averages.\n"
		L".\n"
		L"Examples of queries comparing use of QC flagged data (unreliable data):\n"
		L".\n"
		L".    QueryUSHCN Compare Kansas annual maximum temperatures QC vs non QC.\n"
		L".    QueryUSHCN Annual maximums in Kansas, QC difference.\n"
		L".    QueryUSHCN Show QC influence on Kansas annual maximum temperatures.\n"
		L".\n"
		L"Verifying database values against caluculations made during import:\n"
		L".\n"
		L".    QueryUSHCN Verify annual maximums for station USH00419532 from 1900 to 1920.\n"
		L".    QueryUSHCN Verify annual minimums for station USH00419532 units degC from 1900 to 1920.\n"
		L".    QueryUSHCN Verify annual averages for station USH00419532 from 1900 to 1920.\n\n";
	CString csDump =
		L"A debug switch to test all queries:\n"
		L".\n"
		L".    The switch will run all of the queries in this usage\n"
		L".    list and determine which one are functioning correctly.\n\n";

	m_mapHelp.add(L"help", shared_ptr<CString>(new CString(csHelp)));
	m_mapHelp.add(L"station", shared_ptr<CString>(new CString(csStation)));
	m_mapHelp.add(L"statistics", shared_ptr<CString>(new CString(csStatistics)));
	m_mapHelp.add(L"state", shared_ptr<CString>(new CString(csState)));
	m_mapHelp.add(L"month", shared_ptr<CString>(new CString(csMonthly)));
	m_mapHelp.add(L"year", shared_ptr<CString>(new CString(csAnnual)));
	m_mapHelp.add(L"count", shared_ptr<CString>(new CString(csCounts)));
	m_mapHelp.add(L"db", shared_ptr<CString>(new CString(csDatabase)));
	m_mapHelp.add(L"qc", shared_ptr<CString>(new CString(csQC)));
	m_mapHelp.add(L"dump", shared_ptr<CString>(new CString(csDump)));
	m_mapHelp.add
	(
		L"all", shared_ptr<CString>
		(
			new CString
			(
				csHelp + csStation + csState + csMonthly + csAnnual + 
				csCounts + csDatabase + csQC
			)
		)
	);

	Output = *m_mapHelp.find(L"help");
}

/////////////////////////////////////////////////////////////////////////////
CCommandLine::~CCommandLine()
{
}

/////////////////////////////////////////////////////////////////////////////
// The framework calls this function to parse/interpret individual 
// parameters from the command line
void CCommandLine::ParseParam( LPCTSTR pszParam, BOOL bFlag, BOOL bLast )
{
	if ( bFlag )
	{	if ( !ParseParamFlag( pszParam ))
		{	return CCommandLineInfo::ParseParam( pszParam, bFlag, bLast );
		}
	} else
	{	if ( !ParseParamNotFlag( pszParam ))
		{	return CCommandLineInfo::ParseParam( pszParam, bFlag, bLast );
		}
	}

	ParseLast( bLast );
} // ParseParam

/////////////////////////////////////////////////////////////////////////////
// parse a parameter line flag
bool CCommandLine::ParseParamFlag( LPCTSTR flag )
{	bool bOK = true;
	CString csFlag = CString( flag ).MakeLower();
	if (csFlag == _T("help"))
	{
		Help = true;
		LastType = paramHelp;
	}
	else if (csFlag == L"all")
	{
		All = true;
		LastType = paramAll;
	}
	else if (csFlag == L"station")
	{
		LastType = paramStation;
		Station = true;
	}
	else if (csFlag == L"statistics")
	{
		LastType = paramStatistics;
		State = true;
	}
	else if (csFlag == L"state")
	{
		LastType = paramState;
		State = true;
	}
	else if (csFlag == L"month")
	{
		LastType = paramMonthly;
		Monthly = true;
	}
	else if (csFlag == L"year")
	{
		LastType = paramAnnual;
		Annual = true;
	}
	else if (csFlag == L"count")
	{
		LastType = paramCounts;
		Counts = true;
	}
	else if (csFlag == L"db")
	{
		LastType = paramDatabase;
		Database = true;
	}
	else if (csFlag == L"qc")
	{
		LastType = paramQC;
		QualityControl = true;
	}
	else if (csFlag == L"dump")
	{
		LastType = paramDump;
		Dump = true;
	}
	else // default processing
	{
		bOK = false;
		LastType = paramNone;
	}

	if (bOK)
	{
		Output = *m_mapHelp.find(csFlag);
	}

	return bOK;
} // ParseParamFlag

/////////////////////////////////////////////////////////////////////////////
// parse a parameter line value (since none of our switches have values,
// this method is not called; here in case we add one)
bool CCommandLine::ParseParamNotFlag( LPCTSTR value )
{	bool bOK = true;
	
	PARAM_TYPES eType = LastType;
	switch ( eType )
	{
		case paramAll : 			// /all list all query examples
		{
			Output = *m_mapHelp.find(L"all");
			break;
		}
		case paramStation : 		// /station list station queries
		{
			Output = *m_mapHelp.find(L"station");
			break;
		}
		case paramState : 			// /state list state specific queries
		{
			Output = *m_mapHelp.find(L"state");
			break;
		}
		case paramMonthly : 		// /month list monthly queries
		{
			Output = *m_mapHelp.find(L"month");
			break;
		}
		case paramAnnual : 			// /year list annual queries
		{
			Output = *m_mapHelp.find(L"year");
			break;
		}
		case paramCounts : 			// /count list queries that count limits
		{
			Output = *m_mapHelp.find(L"count");
			break;
		}
		case paramDatabase : 		// /db list database queries
		{
			Output = *m_mapHelp.find(L"db");
			break;
		}
		case paramQC : 				// /QC list quality control queries
		{
			Output = *m_mapHelp.find(L"qc");
			break;
		}
		case paramHelp : 			// /help explain the command line controls
		{
			Output = *m_mapHelp.find(L"help");
			break;
		}
		default : // base processing needed
		{
			Output = *m_mapHelp.find(L"help");
			bOK = false;
		}
	}

	return bOK;
} // ParseParamNotFlag

/////////////////////////////////////////////////////////////////////////////
