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
// the version and release date are read from the workspace subfolder
// 
// \working folder
//   \ushcn.tavg.latest.raw
//     \ushcn.v2.5.5.20260706
// 
// where:
//   version is: v2.5.5
//   release date is: 20260706
//
bool ParseVersion(CString csPath)
{
	// just being pessimistic
	bool value = false;

	// has version been processed?
	if (m_bVersion == false)
	{
		CString csFile = CHelper::GetFileName(csPath);
		CString csExt = CHelper::GetExtension(csPath);
		csFile.MakeLower();

		// folder in the format of ushcn.v2.5.5.20260706
		if (csFile.Left(7) == L"ushcn.v")
		{
			// parse the version number
			csFile.TrimLeft(L"ushcn.");
			m_csVersion = csFile;

			// record the release date
			csExt.TrimLeft(L".");
			m_csReleaseDate = csExt;

			// we are done
			value = true;
		}
	}

	return value;
} // ParseVersion

/////////////////////////////////////////////////////////////////////////////
// the dataset read from the workspace subfolder
// 
// \working folder
//   \ushcn.tavg.latest.raw
// 
// where:
//   dataset is: raw
//
bool ParseDataset(CString csPath)
{
	// just being pessimistic
	bool value = false;

	// has version been processed?
	if (m_bDataset == false)
	{
		CString csExt = CHelper::GetExtension(csPath);
		csExt.MakeLower();
		csExt.TrimLeft(L".");
		if (csExt == L"raw")
		{
			m_csDataset = csExt;
			value = true;
		}
		else if (csExt == L"tob")
		{
			m_csDataset = csExt;
			value = true;
		}
		else if (csExt == L"52j")
		{
			m_csDataset = L"FLs.52j";
			value = true;
		}
	}

	return value;
} // ParseDataset

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

	// find dataset?
	if (m_bDataset == false)
	{
		m_bDataset = ParseDataset(path);
	}

	// find version?
	if (m_bVersion == false)
	{
		m_bVersion = ParseVersion(path);
	}

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

	// dataset has not been found
	m_bDataset = false;

	// version has not been found
	m_bVersion = false;

	// credit format
	m_csCreditFormat =
		L"Citations:\n"
		L"W.T. Block's Climate Suite, version %s.%s, dataset = %s.\n"
		L"M.J. Menne, C.N. Williams, and R.S. Vose, 2009: The United States Historical\n"
		L"Climatology Network monthly temperature data - Version 2.\n"
		L"Bulletin of the American Meteorological Society, 90, 993-1007.\n";

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

	const BOOL bDB = ::PathFileExists(L"ClimateUSHCN.db");

	if (!bDB)
	{
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

		fErr.WriteString(L"Writing stations to the database\n");
	}

	m_csCredit.Format
	(
		m_csCreditFormat,
		m_csVersion.GetString(),
		m_csReleaseDate.GetString(),
		m_csDataset.GetString()
	);

	CClimateDatabase db;

	db.Open
	(
		L"ClimateUSHCN.db"
	);

	if (!bDB)
	{
		db.CreateSchema();
		db.BeginTransaction();

		int nTotalStations = static_cast<int>(m_Stations.ClimateStations.Items.size());
		int nStationIndex = 0;

		// Write all stations and their yearly/monthly data
		for (auto& kv : m_Stations.ClimateStations.Items)
		{
			CString csMessage;
			nStationIndex++;
			if (nStationIndex % 10 == 0)
			{
				csMessage.Format(L"%05d ", nStationIndex);
				fErr.WriteString(csMessage);
				if (nStationIndex % 100 == 0)
				{
					fErr.WriteString(L"\n");
				}
			}
			shared_ptr<CClimateStation> pStation = kv.second;

			pStation->WriteToDatabase
			(
				db
			);
		}

		db.Metadata[L"Data_Credit"] = m_csCredit;
		db.Metadata[L"Data_Version"] = m_csVersion;
		db.Metadata[L"Data_ReleaseDate"] = m_csReleaseDate;
		db.Metadata[L"Dataset_Type"] = m_csDataset;

		_tprintf
		(
			L"Database build complete. %d stations written.\n",
			nTotalStations
		);

		db.Commit();
	}
	else
	{
		_tprintf(L"Database already exists. Skipping build.\n");

		// Update metadata without rebuilding the database
		m_csVersion = db.Metadata[L"Data_Version"];
		m_csReleaseDate = db.Metadata[L"Data_ReleaseDate"];
		m_csDataset = db.Metadata[L"Dataset_Type"];

		m_csCredit.Format
		(
			m_csCreditFormat,
			m_csVersion.GetString(),
			m_csReleaseDate.GetString(),
			m_csDataset.GetString()
		);

		db.Metadata[L"Data_Credit"] = m_csCredit;
	}

	// Now run your test query
	sqlite3_stmt* stmt = db.Prepare
	(
		L"SELECT COUNT(*) FROM Stations;"
	);

	if (stmt)
	{
		if (db.Step(stmt))
		{
			int nStations = sqlite3_column_int(stmt, 0);

			_tprintf
			(
				L"\nVerification: Stations table contains %d rows.\n",
				nStations
			);
		}

		db.Finalize(stmt);
	}

	// a more complex query
	stmt = db.Prepare
	(
		L"SELECT StationID, State, Latitude, Longitude "
		L"FROM Stations "
		L"WHERE StationID = 'USH00028619';"
	);

	if (db.Step(stmt))
	{
		CStringA id = (char*)sqlite3_column_text(stmt, 0);
		CStringA state = (char*)sqlite3_column_text(stmt, 1);
		double lat = sqlite3_column_double(stmt, 2);
		double lon = sqlite3_column_double(stmt, 3);

		_tprintf
		(
			L"Station %S (%S): lat=%f lon=%f\n",
			id.GetString(),
			state.GetString(),
			lat,
			lon
		);

		db.Finalize(stmt);
	}

	// count the years
	sqlite3_stmt* stmtYears = db.Prepare
	(
		L"SELECT COUNT(*) FROM Years;"
	);

	if (stmtYears)
	{
		if (db.Step(stmtYears))
		{
			int nYears = sqlite3_column_int(stmtYears, 0);

			_tprintf
			(
				L"Verification: Years table contains %d rows.\n",
				nYears
			);
		}

		db.Finalize(stmtYears);
	}

	// count the months
	sqlite3_stmt* stmtMonths = db.Prepare
	(
		L"SELECT COUNT(*) FROM Months;"
	);

	if (stmtMonths)
	{
		if (db.Step(stmtMonths))
		{
			int nMonths = sqlite3_column_int(stmtMonths, 0);

			_tprintf
			(
				L"Verification: Months table contains %d rows.\n",
				nMonths
			);
		}

		db.Finalize(stmtMonths);
	}

	sqlite3_stmt* stmtSchema = db.Prepare
	(
		L"PRAGMA table_info(Stations);"
	);

	if (stmtSchema)
	{
		_tprintf(L"\nStations table schema:\n");

		while (db.Step(stmtSchema))
		{
			int cid = sqlite3_column_int(stmtSchema, 0);
			CStringA name = (char*)sqlite3_column_text(stmtSchema, 1);
			CStringA type = (char*)sqlite3_column_text(stmtSchema, 2);

			_tprintf
			(
				L"  Column %d: %S (%S)\n",
				cid,
				name.GetString(),
				type.GetString()
			);
		}

		db.Finalize(stmtSchema);
	}

	sqlite3_stmt* stmtTX = db.Prepare
	(
		L"SELECT StationID, Location, Latitude, Longitude "
		L"FROM Stations "
		L"WHERE State = 'TX';"
	);

	if (stmtTX)
	{
		_tprintf(L"\nStations in Texas:\n");

		CString csMessage;
		while (db.Step(stmtTX))
		{
			CStringA id = (char*)sqlite3_column_text(stmtTX, 0);
			CStringA location = (char*)sqlite3_column_text(stmtTX, 1);
			double lat = sqlite3_column_double(stmtTX, 2);
			double lon = sqlite3_column_double(stmtTX, 3);

			csMessage.Format
			(
				L"  %S, %S (lat=%f lon=%f)\n",
				id.GetString(),
				location.GetString(),
				lat,
				lon
			);
			fErr.WriteString(csMessage);
		}

		db.Finalize(stmtTX);
	}

	CString csCredit = db.Metadata[L"Data_Credit"];
	_tprintf(L"\n%s\n", csCredit.GetString());
	db.Close();

	// all is good
	return 0;

} // _tmain

/////////////////////////////////////////////////////////////////////////////
