/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList / COutputWnd
//
// The output window subsystem for Climate Explorer. This pane provides a
// centralized location for query progress updates, a SQL, formatted text,
// and CSV reporting. It is implemented as a dockable pane containing a
// tab control with three list boxes—one for each output category.
//
// Purpose:
//   • Display the SQL to implement queries.
//   • Display the result of the query output as formatted text.
//   • Display the result of the query output in command separated value format.
//   • Provide a scrollable, dockable pane that integrates with the MFC
//     docking architecture.
//   • Allow users to copy, clear, or inspect output via context menus.
//
// Why this subsystem exists:
//   Climate Explorer performs many background operations: creating SQL queries and
//   executing that query based on the user's imput. This provides feedback to
//   offer a form of quality control to understand what is happening as it happens.
//
// Responsibilities:
//   • Maintain three output channels:
//       – SQL (m_wndOutputSQL)
//       – Formatted Text table (m_wndOutputText)
//       – Comma Separted Value i.e. CSV (m_wndOutputCSV)
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
//       – SQLText = SQL Query text
//       – FormattedText = A table formatted as a text string
//       – CSVText   = Comma Separated Value string
//
// Internal Structure:
//   • COutputList — derived from CListBox, handles context menu actions.
//   • COutputWnd — derived from CDockablePane, contains:
//       – m_wndTabs (tab control)
//       – m_wndOutputSQL (SQL line list)
//       – m_wndOutputText (Formatted table line list)
//       – m_wndOutputCSV (lines of CSV)
//       – m_pFont (custom font pointer)
//   • ShowOutputTab — switches to the correct tab.
//   • ScrollToBottom — ensures newest messages are visible.
//   • ClearXXXOutput — resets individual channels.
//
// This subsystem provides a clean, organized, and responsive logging interface,
// giving users full visibility into Climate Explorer’s internal operations and
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
		if (value != m_csSQL)
		{
			m_csSQL = value;
			m_wndOutputSQL.ResetContent();

			// each row of the list box will get a separate line
			// of text
			int nStart = 0;
			CString csToken = m_csSQL.Tokenize( L"\n", nStart );
			while (!csToken.IsEmpty())
			{
				m_wndOutputSQL.AddString(csToken);
				csToken = m_csSQL.Tokenize(L"\n", nStart);
			}

			//ShowOutputSQL();
			//ScrollToBottom(m_wndOutputSQL);
		}
	}
	// write text to the progress tab
	__declspec(property(put = SetSQLText))
		CString SQLText;

	// write text to the text tab
	void SetFormattedText(CString value)
	{
		if (value != m_csText)
		{
			m_csText = value;
			m_wndOutputText.ResetContent();

			// each row of the list box will get a separate line
			// of text
			int nStart = 0;
			CString csToken = m_csText.Tokenize( L"\n", nStart );
			while (!csToken.IsEmpty())
			{
				m_wndOutputText.AddString(csToken);
				csToken = m_csText.Tokenize(L"\n", nStart);
			}

			//ShowOutputText();
			//ScrollToBottom(m_wndOutputText);
		}
	}
	// write text to the text tab
	__declspec(property(put = SetFormattedText))
		CString FormattedText;

	// write text to the CSV tab
	void SetCSVText(CString value)
	{
		if (value != m_csCSV)
		{
			m_csCSV = value;
			m_wndOutputCSV.ResetContent();

			// each row of the list box will get a separate line
			// of text
			int nStart = 0;
			CString csToken = m_csCSV.Tokenize( L"\n", nStart );
			while (!csToken.IsEmpty())
			{
				m_wndOutputCSV.AddString(csToken);
				csToken = m_csCSV.Tokenize(L"\n", nStart);
			}

			//ShowOutputCSV();
			//ScrollToBottom(m_wndOutputCSV);
		}
	}
	// write text to the CSV tab
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
	inline void ShowOutputCSV()
	{
		ShowOutputTab(m_wndOutputCSV);
	}
	// show the warning output
	inline void ShowOutputText()
	{
		ShowOutputTab(m_wndOutputText);
	}
	// show the progress output
	inline void ShowOutputSQL()
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
	CString m_csSQL;
	CString m_csCSV;
	CString m_csText;
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

