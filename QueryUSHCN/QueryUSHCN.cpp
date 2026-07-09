/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "QueryUSHCN.h"
#include "CHelper.h"
#include "QueryEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// a console application that can crawl through the file
// system and troll for climate data
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL)
	{
		_tprintf(L"Fatal Error: GetModuleHandle failed\n");
		return 1;
	}

	// initialize MFC and error on failure
	if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		_tprintf(L"Fatal Error: MFC initialization failed\n ");
		return 2;
	}

	// do some common command line argument corrections
	vector<CString> arrArgs = CHelper::CorrectedCommandLine(argc, argv);
	size_t nArgs = arrArgs.size();

	CStdioFile fOut(stdout);
	CStdioFile fErr(stderr);
	CString csMessage;

	// display the number of arguments if not 1 to help the user 
	// understand what went wrong if there is an error in the
	// command line syntax
	if (nArgs == 1)
	{
		fErr.WriteString(L".\n");
		csMessage.Format
		(
			L"The number of parameters are %d\n.\n", (LPCTSTR)nArgs - 1
		);
		fErr.WriteString(csMessage);

		// display the arguments
		for (int i = 1; i < nArgs; i++)
		{
			csMessage.Format( L"Parameter %d is %s\n.\n", i, (LPCTSTR)arrArgs[i] );
			fErr.WriteString(csMessage);
		}

		fErr.WriteString(L".\n");
		fErr.WriteString
		(
			L"QueryUSHCN, Copyright (c) 2026, "
			L"by W. T. Block.\n"
		);

		fErr.WriteString
		(
			L".\n"
			L"A Windows command line program to query a climate history\n"
			L"  database created by the ClimateUSHCN program.\n"
			L".\n"
		);

		fErr.WriteString
		(
			L".\n"
			L"Usage:\n"
			L".\n"
			L".  QueryUSHCN \"query_text\"\n"
			L".\n"
			L"Where:\n"
			L".\n"
		);

		fErr.WriteString
		(
			L".  query_text is a natural language question about the data. \n"
			L".\n"
			L".  Examples:\n"
			L".    List your table schemas.\n"
			L".    What version of the data is in the database?\n"
			L".\n"
			L".    List stations.\n"
			L".    List stations in Texas.\n"
			L".    List stations in North Dakota.\n"
			L".    Show stations in CA.\n"
			L".\n"
			L".    Monthly temperatures in Texas.\n"
			L".    Monthly temperatures in California.\n"
			L".\n"
			L".    Annual averages in Oklahoma.\n"
			L".    Annual averages in Colorado.\n"
			L".\n"
			L".    Temperature trend in Colorado.\n"
			L".    Temperature trend in Texas.\n"
			L".\n"
			L".  You may copy and paste any of the above queries.\n"
			L".\n"
		);

		return 3;
	}

	// display the executable path
	csMessage.Format(L"Executable pathname: %s\n", (LPCTSTR)arrArgs[0]);
	fErr.WriteString(L".\n");
	fErr.WriteString(csMessage);
	fErr.WriteString(L".\n");

	// display the current directory
	CString csWorkspace = CHelper::GetCurrentDirectory();
	csMessage.Format(L"Workspace: %s\n", (LPCTSTR)csWorkspace);
	fErr.WriteString(L".\n");
	fErr.WriteString(csMessage);
	fErr.WriteString(L".\n");

	// test to see if the database is present
	const BOOL bDB = ::PathFileExists(L"ClimateUSHCN.db");
	if (!bDB)
	{
		csMessage = L"Database file ClimateUSHCN.db is missing.";
		fErr.WriteString(L".\n");
		fErr.WriteString(csMessage);
		fErr.WriteString(L".\n");
		return 4;
	}

	CClimateDatabase db;

	if ( !db.Open( L"ClimateUSHCN.db" ))
	{
		csMessage = L"Failed to open database file ClimateUSHCN.db.";
		fErr.WriteString(L".\n");
		fErr.WriteString(csMessage);
		fErr.WriteString(L".\n");
		return 5;
	}

	// start up COM
	const BOOL bOK = AfxOleInit();
	if (bOK == FALSE)
	{
		return 6;
	}

	// build the query string
	CString csQuery;
	int nToken = 0;
	for (auto& token : arrArgs)
	{
		if (nToken++ == 0)
		{
			continue;
		}
		
		csQuery += token;
		csQuery += L" ";
	}

	// create an instance of the query engine and pass in a pointer 
	// to the open database
	CQueryEngine Engine(&db);

	CString csResult;
	if (Engine.Dispatch(csQuery, csResult))
	{
		fOut.WriteString(csResult);
	}
	else
	{
		fErr.WriteString(L"Unrecognized query.\n");
	}


	//CString csCredit = db.Metadata[L"Data_Credit"];
	//_tprintf(L"\n%s\n", csCredit.GetString());
	db.Close();

	// all is good
	return 0;
}
