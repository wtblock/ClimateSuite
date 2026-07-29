/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Resource.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "ThreadHelp.h"

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerView;
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
//
// Main MDI frame window for ClimateExplorer. This class hosts the application’s
// primary user interface, including the menu bar, toolbars, status bar,
// docking panes, and the Properties window. It acts as the central hub
// connecting the document/view architecture to the UI framework.
//
// Purpose:
//   • Provide the main window chrome for the ClimateExplorer application.
//   • Manage toolbars, menus, docking windows, and the Properties pane.
//   • Coordinate active document and active view retrieval.
//   • Support Visual Studio–style docking, auto‑hide panes, and customizable
//     toolbars using MFC’s extended frame window classes.
//   • Provide modal UI helpers (GoModal / Wait) used during long operations
//     such as page generation, image caching, and PDF export.
//
// Why this class exists:
//   ClimateExplorer is a multi‑document MDI application with a rich UI. MFC’s
//   CMDIFrameWndEx provides the foundation, but ClimateExplorer requires:
//       – A persistent Properties pane for document metadata
//       – Customizable toolbars and menu bars
//       – Docking window behavior similar to Visual Studio
//       – Quick‑customization support for user toolbars
//       – A mechanism to keep the UI responsive during long tasks
//
// Responsibilities:
//   • Create and initialize all major UI components:
//       – Menu bar
//       – Standard toolbar
//       – Status bar
//       – Properties pane (CPropertiesWnd)
//       – User‑defined toolbars
//       – MDI tabbed groups
//   • Manage application look-and-feel (Office 2003, VS2005, Office 2007,
//     Windows 7, etc.) and persist the selected theme.
//   • Provide accessors for the active ClimateExplorer document and view.
//   • Provide modal wait helpers used by background operations (ThreadHelp).
//   • Handle toolbar creation, customization, and docking behavior.
//   • Load and apply user images for custom toolbars.
//   • Manage window manager dialogs and menu personalization.
//
// Interaction with other components:
//   • CClimateExplorerView calls PropertiesPane->UpdatePropertiesFromDocument()
//     to refresh metadata whenever the view renders a new page.
//   • CClimateExplorerDoc uses the main frame to access the Properties pane and
//     to run modal waits during page generation and export.
//   • CClimateExplorerApp creates the main frame during InitInstance.
//   • ThreadHelp methods (GoModal / Wait) rely on the main frame’s message
//     pump to keep the UI responsive.
//
// This class provides the structural backbone of the ClimateExplorer UI,
// integrating the document/view system with a modern, customizable MDI
// interface and ensuring a smooth, responsive user experience.
/////////////////////////////////////////////////////////////////////////////
class CMainFrame : public CMDIFrameWndEx, public ThreadHelp
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

	// Attributes
public:
	// Get the output pane
	COutputWnd* GetOutputPane()
	{
		return &m_wndOutput;
	}
	// Get the output pane
	__declspec(property(get = GetOutputPane))
		COutputWnd* OutputPane;

	// Get the properties pane
	CPropertiesWnd* GetPropertiesPane()
	{
		return &m_wndProperties;
	}
	// Get the properties pane
	__declspec(property(get = GetPropertiesPane))
		CPropertiesWnd* PropertiesPane;

	// Get the active document.
	CClimateExplorerDoc* GetClimateExplorerDocument();
	// Get the active document.
	__declspec(property(get = GetClimateExplorerDocument))
		CClimateExplorerDoc* ClimateExplorerDocument;

	// Get the pointer for the image view.
	CClimateExplorerView* GetClimateExplorerView();
	// Get the pointer for the image view.
	__declspec(property(get = GetClimateExplorerView))
		CClimateExplorerView* ClimateExplorerView;


// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame
	(
		UINT nIDResource,
		DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
		CWnd* pParentWnd = nullptr,
		CCreateContext* pContext = nullptr
	);

// Implementation
public:
	// go modal on them
	void GoModal()
	{
		ThreadHelp::UpdateUI();
	}

	// modal wait given number of milliseconds
	void Wait(DWORD dwMilliseconds)
	{
		const ULONGLONG ullStart = GetTickCount64();
		ULONGLONG ullDelta = 0;
		do
		{
			GoModal();
			::Sleep(10);
			const ULONGLONG ullNow = GetTickCount64();
			ullDelta = ullNow - ullStart;
		} while (ullDelta < dwMilliseconds);
	}

	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar			m_wndMenuBar;
	CMFCToolBar			m_wndToolBar;
	CMFCStatusBar		m_wndStatusBar;
	CMFCToolBarImages	m_UserImages;
	COutputWnd			m_wndOutput;
	CPropertiesWnd		m_wndProperties;

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnQueueQuitMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

};

/////////////////////////////////////////////////////////////////////////////

