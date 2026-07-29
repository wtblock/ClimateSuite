/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList / COutputWnd
//
// The output window subsystem for Photo Explorer. This pane provides a
// centralized location for diagnostic messages, progress updates, warnings,
// and error reporting. It is implemented as a dockable pane containing a
// tab control with three list boxes—one for each output category.
//
// Purpose:
//   • Display progress messages during long-running operations.
//   • Log warnings and non-fatal issues encountered while processing images.
//   • Log errors that require user attention.
//   • Provide a scrollable, dockable pane that integrates with the MFC
//     docking architecture.
//   • Allow users to copy, clear, or inspect output via context menus.
//
// Why this subsystem exists:
//   Photo Explorer performs many background operations: metadata extraction,
//   image loading, thumbnail generation, album labeling, and file I/O.
//   Users need visibility into what the application is doing, especially
//   when processing large batches of images. The output pane provides a
//   structured, categorized, and persistent log of these operations.
//
// Responsibilities:
//   • Maintain three output channels:
//       – Progress (m_wndOutputSQL)
//       – Warnings (m_wndOutputText)
//       – Errors (m_wndOutputCSV)
//   • Append new messages to the appropriate list box.
//   • Automatically switch to the relevant tab when new messages arrive.
//   • Scroll to the bottom of the list to show the latest entry.
//   • Provide context menu actions (copy, clear, view).
//   • Manage fonts and appearance for readability.
//   • Integrate with the docking system and respond to resizing events.
//
// Interaction with other components:
//   • CMainFrame — hosts the dockable pane and manages layout.
//   • ThreadHelp — background operations write progress/warning/error text.
//   • Metadata and image-processing subsystems — report issues via this pane.
//   • MFC tab control (CMFCTabCtrl) — manages the three output tabs.
//   • COutputList — specialized list box with context menu support.
//
// Key Features:
//   • Three independent output channels with automatic tab switching.
//   • Scroll-to-bottom behavior for real-time logging.
//   • Context menu for copying or clearing output.
//   • Dockable pane that can be repositioned anywhere in the UI.
//   • Font customization for improved readability.
//   • Simple property-based API for writing messages:
//       – SQLText = "Loading image…"
//       – FormattedText = "Metadata missing for file."
//       – CSVText   = "Failed to save image."
//
// Internal Structure:
//   • COutputList — derived from CListBox, handles context menu actions.
//   • COutputWnd — derived from CDockablePane, contains:
//       – m_wndTabs (tab control)
//       – m_wndOutputSQL (progress list)
//       – m_wndOutputText (warnings list)
//       – m_wndOutputCSV (errors list)
//       – m_csProgress / m_csWarning / m_csError (last written messages)
//       – m_pFont (custom font pointer)
//   • ShowOutputTab — switches to the correct tab.
//   • ScrollToBottom — ensures newest messages are visible.
//   • ClearXXXOutput — resets individual channels.
//
// This subsystem provides a clean, organized, and responsive logging interface,
// giving users full visibility into Photo Explorer’s internal operations and
// helping diagnose issues during metadata processing, image loading, and file
// management.
/////////////////////////////////////////////////////////////////////////////
class COutputList : public CListBox
{
	// Construction
public:
	COutputList() noexcept;

	// Implementation
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
	// public properties
public:
	// write text to the progress tab
	void SetSQLText(CString value)
	{
		if (value != m_csProgress)
		{
			m_csProgress = value;
			m_wndOutputSQL.AddString(value);
			ShowProgressOutput();
			ScrollToBottom(m_wndOutputSQL);
		}
	}
	// write text to the progress tab
	__declspec(property(put = SetSQLText))
		CString SQLText;

	// write text to the warnings tab
	void SetFormattedText(CString value)
	{
		if (value != m_csWarning)
		{
			m_csWarning = value;
			m_wndOutputText.AddString(value);
			ShowWarningOutput();
			ScrollToBottom(m_wndOutputText);
		}
	}
	// write text to the warnings tab
	__declspec(property(put = SetFormattedText))
		CString FormattedText;

	// write text to the errors tab
	void SetCSVText(CString value)
	{
		if (value != m_csError)
		{
			m_csError = value;
			m_wndOutputCSV.AddString(value);
			ShowErrorOutput();
			ScrollToBottom(m_wndOutputCSV);
		}
	}
	// write text to the errors tab
	__declspec(property(put = SetCSVText))
		CString CSVText;

	// public methods
public:
	void ScrollToBottom(COutputList& listBox)
	{
		int count = listBox.GetCount();
		if (count > 0)
		{
			listBox.SetTopIndex(count - 1);
		}
	}

	// show the error output
	inline void ShowErrorOutput()
	{
		ShowOutputTab(m_wndOutputCSV);
	}
	// show the warning output
	inline void ShowWarningOutput()
	{
		ShowOutputTab(m_wndOutputText);
	}
	// show the progress output
	inline void ShowProgressOutput()
	{
		ShowOutputTab(m_wndOutputSQL);
	}

	// clear the error output
	inline void ClearErrorOutput()
	{
		m_wndOutputCSV.ResetContent();
	}
	// clear the warning output
	inline void ClearWarningOutput()
	{
		m_wndOutputText.ResetContent();
	}
	// clear the progress output
	inline void ClearProgressOutput()
	{
		m_wndOutputSQL.ResetContent();
	}

	// Construction
public:
	COutputWnd() noexcept;

	void UpdateFonts();

	// Attributes
protected:
	CMFCTabCtrl	m_wndTabs;
	COutputList m_wndOutputSQL;
	COutputList m_wndOutputText;
	COutputList m_wndOutputCSV;
	CString m_csProgress;
	CString m_csError;
	CString m_csWarning;
	CFont* m_pFont;

protected:
	// show one of the output tabs
	void ShowOutputTab(COutputList& refWnd);

	void FillSQLWindow();
	void FillTextWindow();
	void FillCSVWindow();

	void AdjustHorzScroll(CListBox& wndListBox);

	// Implementation
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

