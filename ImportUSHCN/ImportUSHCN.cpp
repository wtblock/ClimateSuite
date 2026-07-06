/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ImportUSHCN.h"
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
using namespace std;

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// parse a given line of source and persist it
shared_ptr<CClimateStation> ParseSource
(
	CString& source, CClimateTemperature::MEASURE_TYPE eType
)
{
	shared_ptr<CClimateStation> pStation;

	// create a new CStationYear object based on the source and type
	shared_ptr< CStationYear > StationYear = shared_ptr< CStationYear >
	(
		new CStationYear(source, eType)
	);

	pStation = m_Stations.AddStationYear(StationYear);

	return pStation;
} // ParseSource

/////////////////////////////////////////////////////////////////////////////
// crawl through the directory tree looking for given climate extension
void RecursePath
(
	LPCTSTR path, // pathname to recurse
	// extension of climate files to process (tmax, tmin, or tavg)
	LPCTSTR ext,
	CStdioFile& fOut, // standard output
	CStdioFile& fErr // error output
)
{
	USES_CONVERSION;

	int nStation = 0;

	// determine measurement type from the given extension
	CClimateTemperature::MEASURE_TYPE eType = CClimateTemperature::mtMaximum;
	const CString csExtention = CString(ext).MakeLower();
	if (csExtention == L".tmin")
	{
		eType = CClimateTemperature::mtMinimum;

	}
	else if (csExtention == L".tavg")
	{
		eType = CClimateTemperature::mtAverage;
	}

	// get the folder which will trim any wild card data
	CString csPathname = CString(path).Trim(L"\\");

	// build a pathname with wild-card extension
	CString strWildcard;
	strWildcard.Format(L"%s\\*.*", (LPCTSTR)csPathname);

	// start trolling for files we are interested in
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip "." and ".." folder names
		if (finder.IsDots())
		{
			continue;
		}

		// if it's a directory, recursively search it
		if (finder.IsDirectory())
		{
			// get the new folder name
			const CString folder =
				finder.GetFilePath().TrimRight(L"\\");

			// recurse into the new directory with wild cards
			RecursePath(folder, ext, fOut, fErr);

		}
		else // write the properties for valid extension
		{
			const CString csPath = finder.GetFilePath();
			const CString csExt = CHelper::GetExtension(csPath).MakeLower();
			if (csExt == ext)
			{
				// open the stations text file
				CStdioFile file;
				const bool value =
					file.Open(csPath, CFile::modeRead | CFile::shareDenyNone);

				// if the open was successful, read each line of the file and 
				// collect the station data
				if (value == true)
				{
					bool bFirst = true;

					CString csLine;
					while (file.ReadString(csLine))
					{
						shared_ptr<CClimateStation> pStation = ParseSource(csLine, eType);
						if (bFirst)
						{
							CString csMessage;
							m_nCount++;
							if (m_nCount % 10 == 0)
							{
								csMessage.Format(L"%05d ", m_nCount);
								fErr.WriteString(csMessage);
								if (m_nCount % 100 == 0)
								{
									fErr.WriteString(L"\n");
								}
							}
							//CString csStation = pStation->Station;
							//CString csState = pStation->State;
							//CString csLocation = pStation->Location;
							//CString csMessage;
							//csMessage.Format
							//(
							//	L"%5d %s, %s, %s\n", ++nStation, 
							//	(LPCTSTR)csStation, csState, csLocation
							//);
							//fErr.WriteString(csMessage);
							bFirst = false;
						}
					}
				}

				//fErr.WriteString( csPath );
				//fErr.WriteString( L"\n" );
			}
		}
	}

	finder.Close();

} // RecursePath

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
	if (nArgs != 1)
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
	}

	// two arguments if a pathname to the climate data is given
	// three arguments if the station text file name is also given
	if (nArgs != 2 && nArgs != 3)
	{
		fErr.WriteString(L".\n");
		fErr.WriteString
		(
			L"ClimateUSHCN, Copyright (c) 2026, "
			L"by W. T. Block.\n"
		);

		fErr.WriteString
		(
			L".\n"
			L"A Windows command line program to read climate history\n"
			L"  and build a SQL database which can be queried.\n"
			L".\n"
		);

		fErr.WriteString
		(
			L".\n"
			L"Usage:\n"
			L".\n"
			L".  ClimateUSHCN pathname [station_file_name]\n"
			L".\n"
			L"Where:\n"
			L".\n"
		);

		fErr.WriteString
		(
			L".  pathname is the root of the tree to be scanned \n"
			L".  for climate data files with extensions:\n"
			L".    \"*.tavg - average temperature files\"\n"
			L".    \"*.tmax - maximum temperature files\"\n"
			L".    \"*.tmin - minimum temperature files\"\n"
			L".  station_file_name is the optional station file name: \n"
			L".    defaults to: \"ushcn-v2.5-stations.txt\"\n"
			L".\n"
		);

		return 3;
	}

	// display the executable path
	csMessage.Format(L"Executable pathname: %s\n", (LPCTSTR)arrArgs[0]);
	fErr.WriteString(L".\n");
	fErr.WriteString(csMessage);
	fErr.WriteString(L".\n");

	// retrieve the pathname which may be a single period
	CString csPath = arrArgs[1].Trim(L"//");

	// test for current folder character (a period)
	bool bExists = csPath == L".";

	// if it is not a period, test for existence of the given folder name
	if (!bExists)
	{
		if (::PathFileExists(csPath))
		{
			bExists = true;
		}
	}

	if (!bExists)
	{
		csMessage.Format(L"Invalid pathname:\n\t%s\n", (LPCTSTR)csPath);
		fErr.WriteString(L".\n");
		fErr.WriteString(csMessage);
		fErr.WriteString(L".\n");
		return 4;

	}
	else
	{
		csMessage.Format(L"Given pathname:\n\t%s\n", (LPCTSTR)csPath);
		fErr.WriteString(L".\n");
		fErr.WriteString(csMessage);
	}

	// read the station data
	CString csStationFile(L"ushcn-v2.5-stations.txt");
	if (nArgs == 3)
	{
		csStationFile = arrArgs[2];
	}

	// full pathname of the station file
	const CString csStationPath = csPath + L"\\" + (LPCTSTR)csStationFile;

	if (!::PathFileExists(csStationPath))
	{
		csMessage.Format
		(
			L"Invalid Station File Name:\n\t%s\n", (LPCTSTR)csStationFile
		);
		fErr.WriteString(L".\n");
		fErr.WriteString(csMessage);
		fErr.WriteString(L".\n");
		return 5;

	}

	// populate the climate station data
	m_Stations.StationPath = csStationPath;
	m_Stations.ReadStationData();

	// start up COM
	const BOOL bOK = AfxOleInit();
	if (bOK == FALSE)
	{
		return 6;
	}

	HRESULT hr = ::CoInitialize((LPVOID)nullptr);
	if (hr != S_OK)
	{
		return 7;
	}

	// the pathname of the executable
	const CString csExe = arrArgs[0];

	// crawl through directory tree defined by the command line
	// parameter trolling for given climate file extensions
	m_nCount = 0;
	fErr.WriteString(L"Processing Maximum Values\n");
	RecursePath(csPath, L".tmax", fOut, fErr);
	fErr.WriteString(L"\n\n");

	m_nCount = 0;
	fErr.WriteString(L"Processing Minimum Values\n");
	RecursePath(csPath, L".tmin", fOut, fErr);
	fErr.WriteString(L"\n\n");

	m_nCount = 0;
	fErr.WriteString(L"Processing Average Values\n");
	RecursePath(csPath, L".tavg", fOut, fErr);
	fErr.WriteString(L"\n\n");

	// all is good
	return 0;

} // _tmain

/////////////////////////////////////////////////////////////////////////////
