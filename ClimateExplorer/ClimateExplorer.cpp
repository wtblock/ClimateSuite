/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ClimateExplorer.h"
#include "QueryEngine.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"
#include "AboutVersions.h"
#include "Splash.h"

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CClimateExplorerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CClimateExplorerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, &CClimateExplorerApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, &CClimateExplorerApp::OnFileNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClimateExplorerApp Implementation
//
// This source file contains the full application‑level initialization and
// shutdown logic for PhotoPrinter. As the global MFC application object,
// CClimateExplorerApp is responsible for preparing the entire runtime
// environment before any documents or views are created.
//
// Major Responsibilities:
//   • Initialize common controls, MFC, OLE, COM, and GDI+
//   • Parse command‑line arguments (CCommandLine)
//   • Establish the working folder used by PhotoPrinterDoc
//   • Create and register the document/view/frame template
//   • Create the main MDI frame window (CMainFrame)
//   • Dispatch shell commands (open file, automation, etc.)
//   • Manage application‑wide UI components (tooltips, context menus)
//   • Provide idle‑time processing for progress dialogs
//   • Shut down COM and GDI+ cleanly on exit
//
// Initialization Flow (InitInstance):
//   1. Initialize common controls (required for modern UI).
//   2. Allocate and parse command‑line data.
//   3. Determine and set the working folder.
//   4. Initialize MFC core (CWinAppEx::InitInstance).
//   5. Initialize OLE and COM (AfxOleInit, CoInitializeEx).
//   6. Initialize GDI+ (GdiplusStartup).
//   7. Create splash/progress dialog during startup.
//   8. Initialize context menu, keyboard, and tooltip managers.
//   9. Register the document template connecting:
//        • CPhotoPrinterDoc (document)
//        • CChildFrame       (MDI child frame)
//        • CPhotoPrinterView (view)
//  10. Create the main MDI frame window.
//  11. Process shell commands (open file, automation, etc.).
//  12. Show and update the main window.
//
// Shutdown Flow (ExitInstance):
//   • Terminate OLE
//   • Uninitialize COM
//   • Shut down GDI+
//   • Delegate final cleanup to CWinAppEx
//
// Idle Processing (OnIdle):
//   • Allows progress dialogs (ThumbnailDialog, splash screens, etc.)
//     to remain responsive during long operations such as page building
//     or image caching.
//   • Transfers queued quit messages from background threads.
//
// File Open Handling:
//   • Presents a file dialog for .pp project files.
//   • Opens the selected document using the registered template.
//
// About Dialog:
//   • Displays version information using CAboutVersions.
//
// This file contains the operational “glue” that binds together the
// document, view, and frame classes, ensuring PhotoPrinter starts,
// runs, and shuts down as a cohesive application.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerApp::CClimateExplorerApp() noexcept
{
	m_bHiColorIcons = TRUE;
	m_nAppLook = 0;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("WTBlock.ClimateSuite.ClimateExplorer"));

	Publisher = L"William T. Block III, Publisher";
	Copyright = L"Copyright © 2026, by William Block, All rights reserved.";

} // CClimateExplorerApp

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerApp theApp;

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// create a command line instance
	m_pCommandLine = unique_ptr<CCommandLine>(new CCommandLine);

	// Parse command line for standard shell commands, DDE, file open
	ParseCommandLine(*m_pCommandLine);

	WorkingFolder = m_pCommandLine->WorkingFolder;
	if (WorkingFolder.IsEmpty())
	{
		CString csWorkingFolder(char(0), MAX_PATH);
		DWORD dwLen = ::GetCurrentDirectory(MAX_PATH, csWorkingFolder.GetBuffer());
		csWorkingFolder.ReleaseBuffer();
		WorkingFolder = csWorkingFolder;
	}
	else
	{
		::SetCurrentDirectory(WorkingFolder);
	}

	CString csTest = WorkingFolder;

	CWinAppEx::InitInstance();

	// Initialize COM
	HRESULT hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	if ( FAILED( hr ) )
	{
		AfxMessageBox( _T( "Failed to initialize COM library" ) );
		return FALSE; // Terminate application if COM initialization fails
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("WTBLOCK.ORG"));
	LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

	// test to see if the database is present
	CString csMessage;
	const BOOL bDB = ::PathFileExists(L"ClimateUSHCN.db");
	if (!bDB)
	{
		csMessage.Format
		( 
			L"Database file, ClimateUSHCN.db, is missing from the working folder:\n%s", 
			WorkingFolder 
		);
		AfxMessageBox(csMessage);
		return FALSE;
	}

	CClimateDatabase* pDB = ClimateDatabase;

	if ( !pDB->Open( L"ClimateUSHCN.db" ))
	{
		csMessage.Format
		(
			L"Failed to open database, ClimateUSHCN.db, in the working folder:\n%s",
			WorkingFolder
		);
		AfxMessageBox(csMessage);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// build the query string
	CString csQuery = L"What version of the data is in the database?";

	// create an instance of the query engine and pass in a pointer 
	// to the open database
	CQueryEngine Engine(pDB);

	CString csResult;
	Engine.ProcessQuery(csQuery, csResult);
	DatabaseVersion = csResult;
	DatabaseCredits = pDB->Metadata[L"Data_Credit"];

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

#ifndef _DEBUG
	CSplash ProgressDlg(CWnd::GetDesktopWindow());
	CWnd* pTopMostWnd = nullptr;
	pTopMostWnd = new CWnd;
	pTopMostWnd->Attach(HWND_TOPMOST);

	ProgressDlg.CreateDlg();
	ProgressDlg.SetWindowPos
	(
		pTopMostWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE
	);
	ThreadHelp::UpdateUI();
#endif

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams
	(
		AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams
	);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views

	// the CEx template stores the document in a pure XML format which
	// when opened forces the database to be queried for each image in
	// the document which can be very slow for large documents.
	CMultiDocTemplate* pCexTemplate;
	pCexTemplate = new CMultiDocTemplate
	(
		IDR_CEX_TYPE,
		RUNTIME_CLASS(CClimateExplorerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CClimateExplorerView)
	);
	if (!pCexTemplate)
	{
		// Shutdown GDI+
		GdiplusShutdown(m_gdiplusToken);

		return FALSE;
	}

	AddDocTemplate(pCexTemplate);

	// the CE template stores the document in a binary format which
	// when opened does not access the database but instead stores
	// the images to be displayed in the document which for very 
	// large documents can be much faster.
	CMultiDocTemplate* pCeTemplate;
	pCeTemplate = new CMultiDocTemplate
	(
		IDR_CE_TYPE,
		RUNTIME_CLASS(CClimateExplorerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CClimateExplorerView)
	);
	if (!pCeTemplate)
	{
		// Shutdown GDI+
		GdiplusShutdown(m_gdiplusToken);

		return FALSE;
	}

	AddDocTemplate(pCeTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	m_pMainWnd = pMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
#ifndef _DEBUG
		delete pTopMostWnd;
		pTopMostWnd = nullptr;
		ProgressDlg.DestroyWindow();
#endif
		delete pMainFrame;

		// Shutdown GDI+
		GdiplusShutdown(m_gdiplusToken);

		return FALSE;
	}

	//// do not allow new file here
	//if (m_pCommandLine->m_nShellCommand == CCommandLineInfo::FileNew)
	//{
	//	m_pCommandLine->m_nShellCommand = CCommandLineInfo::FileNothing;
	//}

	// Application was launched with /Embedding or /Automation switch.
	// Run application as automation server.
	if (m_pCommandLine->m_bRunEmbedded || m_pCommandLine->m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}

	// Dispatch commands specified on the command line.  Will return FALSE if
	// application was launched with /RegServer, /Register, /Unregserver or 
	// /Unregister.
	if (!ProcessShellCommand(*m_pCommandLine))
		return FALSE;

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->Wait(2000);

	return TRUE;
} // InitInstance

/////////////////////////////////////////////////////////////////////////////
int CClimateExplorerApp::ExitInstance()
{
	// close the database
	CClimateDatabase* pDB = ClimateDatabase;
	pDB->Close();

	AfxOleTerm(FALSE);

	// Uninitialize COM
	CoUninitialize();

	// Shutdown GDI+
	GdiplusShutdown(m_gdiplusToken);

	return CWinAppEx::ExitInstance();
} // ExitInstance

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerApp::OnIdle(LONG lCount)
{
	BOOL bMore = CWinAppEx::OnIdle(lCount);
	if (!bMore)
	{
		((CMainFrame*)m_pMainWnd)->TransferQuitMessages();
	}
	return bMore;
}

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerApp::OnFileOpen()
{
	const CString csFilter =
		_T("All Possible|*.CEx;*.CE|")
		_T("Climate Explorer XML (*.CEx)|*.CEx|")
		_T("Climate Explorer (*.CE)|*.CE|")
		_T("All Files (*.*)|*.*||");
	CString strFilePath;
	CFileDialog fileDlg
	(
		TRUE, L"CE", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		csFilter
	);

	if (fileDlg.DoModal() == IDOK)
	{
		strFilePath = fileDlg.GetPathName();
		OpenDocumentFile(strFilePath);
	}
} // OnFileOpen

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerApp::OnFileNew()
{
	// Choose the XML template explicitly
	CDocTemplate* pTemplate = nullptr;

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != nullptr)
	{
		CDocTemplate* p = GetNextDocTemplate(pos);
		CString csDoc;
		if (p->GetDocString(csDoc, CDocTemplate::regFileTypeId))
		{
			if ( csDoc == L"ClimateExplorer.Document")
			{
				pTemplate = p;
				break;
			}
		}
	}

	if (pTemplate)
		pTemplate->OpenDocumentFile(nullptr);
} // OnFileNew

/////////////////////////////////////////////////////////////////////////////
// App command to run the dialog
void CClimateExplorerApp::OnAppAbout()
{
	CAboutVersions aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerApp::LoadCustomState()
{
}

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerApp::SaveCustomState()
{
}

/////////////////////////////////////////////////////////////////////////////


