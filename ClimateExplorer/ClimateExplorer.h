/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <gdiplus.h>
#include "CommandLine.h"
#include <memory>
#include "ClimateDatabase.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CClimateExplorerApp
//
// Application class for PhotoPrinter. This class represents the top‑level
// MFC application object and is responsible for initializing the program,
// managing global state, and coordinating high‑level behaviors shared across
// the document/view architecture.
//
// Purpose:
//   • Initialize MFC, OLE/COM, and GDI+ for the entire application.
//   • Create the main frame window and connect it to the document/view
//     architecture (CPhotoPrinterDoc + CPhotoPrinterView).
//   • Maintain global configuration such as the working folder, command‑line
//     parameters, application look, and high‑color icon support.
//   • Provide helper functions used throughout the application, including
//     idle‑time processing, custom state loading/saving, and UI updates.
//   • Serve as the central access point for application‑wide objects such as
//     the Properties pane, context menu manager, and progress dialogs.
//
// Why this class exists:
//   MFC applications rely on a single global application object derived from
//   CWinAppEx. PhotoPrinter uses this class to unify initialization, global
//   state, and shared UI components so that the document and view classes can
//   focus solely on book‑building and rendering logic.
//
// Responsibilities:
//   • Initialize the application (InitInstance) and shut it down cleanly
//     (ExitInstance), including GDI+ startup/shutdown.
//   • Parse and store command‑line arguments using CCommandLine.
//   • Manage the optional working folder used by CPhotoPrinterDoc.
//   • Provide OnIdle() processing for progress dialogs and background tasks.
//   • Load and save custom application state (window layout, settings).
//   • Handle application‑level commands (File Open, About).
//
// Interaction with other components:
//   • CPhotoPrinterDoc retrieves the working folder from this class.
//   • CPhotoPrinterView uses the application object to access the main frame
//     and update the Properties pane.
//   • ExportDocument uses the application’s idle loop to keep the UI responsive.
//   • Thumbnail dialogs use the application object as their parent/owner.
//   • Command‑line data is consumed once and then destroyed to prevent stale
//     parameters from affecting subsequent document openings.
//
// This class provides the “glue” that binds together the document, view,
// and UI framework, ensuring PhotoPrinter behaves as a cohesive application.
/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerApp : public CWinAppEx
{
	// public data
public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	// protected methods
protected:
	// the climate database is a relational database of USHCN data
	CClimateDatabase m_db;

	// document publisher
	CString m_csPublisher;

	// document copyright
	CString m_csCopyright;

	// Gdiplus initialization
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
	
	// database version
	CString m_csDatabaseVersion;

	// database credits
	CString m_csDatabaseCredits;

	// optional working folder 
	CString m_csWorkingFolder;

	// pointer to the command line data
	unique_ptr<CCommandLine> m_pCommandLine;

	// public properties
public:
	// the climate database is a relational database of USHCN data
	CClimateDatabase* GetClimateDatabase()
	{
		return &m_db;
	}
	// the climate database is a relational database of USHCN data
	__declspec(property(get = GetClimateDatabase))
		CClimateDatabase* ClimateDatabase;

	// document publisher
	CString GetPublisher()
	{
		return m_csPublisher;
	}
	// document publisher
	void SetPublisher(CString value)
	{
		m_csPublisher = value;
	}
	// document publisher
	__declspec(property(get = GetPublisher, put = SetPublisher))
		CString Publisher;

	// document copyright
	CString GetCopyright()
	{
		return m_csCopyright;
	}
	// document copyright
	void SetCopyright(CString value)
	{
		m_csCopyright = value;
	}
	// document copyright
	__declspec(property(get = GetCopyright, put = SetCopyright))
		CString Copyright;

	// pointer to the command line data which will only be read once
	// because reading it will destroy it (prevents subsequent opening
	// of documents from having the wrong command line data).
	unique_ptr<CCommandLine>& GetCommandLine()
	{
		return m_pCommandLine;
	}
	// pointer to the command line data which will only be read once
	// because reading it will destroy it (prevents subsequent opening
	// of documents from having the wrong command line data).
	inline void SetCommandLine(unique_ptr<CCommandLine> value)
	{
		m_pCommandLine = move(value);
	}
	// pointer to the command line data which will only be read once
	// because reading it will destroy it (prevents subsequent opening
	// of documents from having the wrong command line data).
	__declspec(property(get = GetCommandLine, put = SetCommandLine))
		unique_ptr<CCommandLine> CommandLine;

	// optional working folder 
	inline CString GetWorkingFolder()
	{
		return m_csWorkingFolder;
	}
	// optional working folder 
	inline void SetWorkingFolder(LPCTSTR value)
	{
		m_csWorkingFolder = value;
	}
	// optional working folder 
	__declspec(property(get = GetWorkingFolder, put = SetWorkingFolder))
		CString WorkingFolder;

	// database version
	inline CString GetDatabaseVersion()
	{
		return m_csDatabaseVersion;
	}
	// database version
	inline void SetDatabaseVersion(LPCTSTR value)
	{
		m_csDatabaseVersion = value;
	}
	// database version
	__declspec(property(get = GetDatabaseVersion, put = SetDatabaseVersion))
		CString DatabaseVersion;

	// database version
	inline CString GetDatabaseCredits()
	{
		return m_csDatabaseCredits;
	}
	// database version
	inline void SetDatabaseCredits(LPCTSTR value)
	{
		m_csDatabaseCredits = value;
	}
	// database version
	__declspec(property(get = GetDatabaseCredits, put = SetDatabaseCredits))
		CString DatabaseCredits;

// public overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	// public constructor/destructor
public:
	CClimateExplorerApp() noexcept;

	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};

extern CClimateExplorerApp theApp;
