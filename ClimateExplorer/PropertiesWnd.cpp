/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"
#include "ClimateExplorer.h"
#include "PropertyGridMultilineText.h"
#include "ClimateDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar
CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
	m_pTableOfContents = nullptr;

}

/////////////////////////////////////////////////////////////////////////////
CPropertiesWnd::~CPropertiesWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES_GO_TO, OnGoTo)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES_GO_TO, OnUpdateGoTo)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerDoc* CPropertiesWnd::GetClimateExplorerDocument()
{
	CClimateExplorerDoc* value = nullptr;
	CMainFrame* pFrame = nullptr;
	try
	{
		pFrame = (CMainFrame*)AfxGetMainWnd();
		value = pFrame->ClimateExplorerDocument;
	}
	catch (...)
	{
		return value;
	}

	return value;
} // GetClimateExplorerDocument

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerView* CPropertiesWnd::GetClimateExplorerView()
{
	CClimateExplorerView* value = nullptr;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	value = pFrame->ClimateExplorerView;
	return value;
} // GetClimateExplorerView

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::PopulateStatesForScope(const CString& scope)
{
	m_pPropState->RemoveAllOptions();

	CClimateDatabase* pDB = ((CClimateExplorerApp*)AfxGetApp())->ClimateDatabase;

	if (scope.CompareNoCase(L"National") == 0)
	{
		m_pPropState->AddOption(L"None");
		m_pPropState->AddOption(L"All");
	}
	else
	{
		// Actual states only
		for (auto& csState : pDB->States)
			m_pPropState->AddOption(csState);
	}
} // PopulateStatesForScope

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::PopulateLocationsForState(const CString& scope, const CString& state)
{
	m_pPropLocation->RemoveAllOptions();

	CClimateDatabase* pDB = ((CClimateExplorerApp*)AfxGetApp())->ClimateDatabase;

	if (scope.CompareNoCase(L"State") == 0)
	{
		m_pPropLocation->AddOption(L"All");

		auto cities = pDB->Cities[state];
		for (auto& city : cities)
			m_pPropLocation->AddOption(city);
	}
	else if (scope.CompareNoCase(L"Location") == 0)
	{
		auto cities = pDB->Cities[state];
		for (auto& city : cities)
			m_pPropLocation->AddOption(city);
	}
} // PopulateLocationsForState

/////////////////////////////////////////////////////////////////////////////
LRESULT CPropertiesWnd::OnPropertyChange
(
	WPARAM /*WP*/, // not used
	LPARAM lp // property grid property pointer
)
{
	LRESULT value = 0;

	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;
	if (pDoc == nullptr)
	{
		return value;
	}
	CClimateExplorerView* pView = ClimateExplorerView;
	if (pView == nullptr)
	{
		return value;
	}

	// the message passed in the property panel property that changed
	// which is displaying a copy of the information in the selected
	// image's corresponding property
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lp;

	// the current value from the property panel
	const _variant_t varIn = pProp->GetValue();

	// the property panel name of the property
	CString csName = pProp->GetName();

	// the parent of the property on the panel which contains
	// the group the property belongs to
	CMFCPropertyGridProperty* pParent = pProp->GetParent();
	if (pParent == nullptr)
	{
		return value;
	}

	CString csGroup = pParent->GetName();
	CString csLabel = csName;

	if (csGroup == L"Table of Contents")
	{
		OnGoTo();
		return value;
	}

	// test to see if an album property changed
	if (csGroup == L"Title Page Properties")
	{
		if (csName == L"Title")
		{
			pDoc->Title = CString(varIn);

		}
		else if (csName == L"Subtitle")
		{
			pDoc->Subtitle = CString(varIn);

		}
		else if (csName == L"Publisher")
		{
			pDoc->Publisher = CString(varIn);

		}
		else if (csName == L"ISBN")
		{
			pDoc->ISBN = CString(varIn);

		}
		else if (csName == L"Description")
		{
			pDoc->Description = CString(varIn);

		}
		else if (csName == L"Copyright")
		{
			pDoc->Copyright = CString(varIn);
		}
	}
	else if (csGroup == L"Export Properties")
	{
		if (csName == L"Export Folder")
		{
			pDoc->ExportFolder = CString(varIn);
		}
		else if (csName == L"Export Pages")
		{
			pDoc->ExportPages = CString(varIn);
		}
		else if (csName == L"Export DPI")
		{
			CString csDPI = CString(varIn);
			csDPI.Trim();
			if (csDPI.IsEmpty())
			{
				csDPI = L"400";
			}
			pDoc->ExportDPI = (UINT)_tstol(csDPI);
		}
		else if (csName == L"Export Quality")
		{
			CString csQuality = CString(varIn);
			csQuality.Trim();
			if (csQuality.IsEmpty())
			{
				csQuality = L"75";
			}
			pDoc->ExportQuality = (UINT)_tstol(csQuality);
		}
	}
	else if (csGroup == L"Query Properties")
	{
		if (csName == L"Type of Query")
		{
			CString value = CString(varIn);
			pDoc->QueryType = value;
			value.MakeLower();
			if (value == L"picker")
			{
				EnablePickers();

				// Disable NL + SQL
				m_pPropNaturalLanguage->SetValue(COleVariant(L"None"));
				m_pPropSQL->SetValue(COleVariant(L""));

				pDoc->NaturalLanguage = L"None";
				pDoc->SQL = L"";

				m_pPropNaturalLanguage->Enable(FALSE);
				m_pPropSQL->Enable(FALSE);
			}
			else if (value == L"natural language")
			{
				m_pPropSQL->SetValue(COleVariant(L""));
				m_pPropScope->SetValue(COleVariant(L"National"));
				m_pPropState->SetValue(COleVariant(L"None"));
				m_pPropLocation->SetValue(COleVariant(L"None"));

				pDoc->SQL = L"";
				pDoc->Scope = L"National";
				pDoc->State = L"None";
				pDoc->Location = L"None";

				EnablePickers(FALSE);
				m_pPropNaturalLanguage->Enable();
				m_pPropSQL->Enable(FALSE);
			}
			else if (value == L"sql")
			{
				m_pPropNaturalLanguage->SetValue(COleVariant(L"None"));
				m_pPropScope->SetValue(COleVariant(L"National"));
				m_pPropState->SetValue(COleVariant(L"None"));
				m_pPropLocation->SetValue(COleVariant(L"None"));

				pDoc->NaturalLanguage = L"None";
				pDoc->Scope = L"National";
				pDoc->State = L"None";
				pDoc->Location = L"None";

				EnablePickers(FALSE);
				m_pPropNaturalLanguage->Enable(FALSE);
				m_pPropSQL->Enable();
			}

			m_wndPropList.RedrawWindow();
		}
		else if (csName == L"Natural Language")
		{
			pDoc->NaturalLanguage = CString(varIn);
		}
		else if (csName == L"SQL")
		{
			pDoc->SQL = CString(varIn);
		}
		else if (csName == L"Units")
		{
			pDoc->Units = CString(varIn);
		}
		else if (csName == L"Pure")
		{
			pDoc->Pure = bool(varIn);
		}
		else if (csName == L"Active")
		{
			pDoc->Active = bool(varIn);
		}
		else if (csName == L"Scope")
		{
			CString scope = CString(varIn);
			pDoc->Scope = scope;

			PopulateStatesForScope(scope);

			if (scope.CompareNoCase(L"National") == 0)
			{
				m_pPropState->Enable(TRUE);
				m_pPropLocation->Enable(FALSE);

				m_pPropState->SetValue(L"None");
				m_pPropLocation->SetValue(L"None");
			}
			else if (scope.CompareNoCase(L"State") == 0)
			{
				m_pPropState->Enable(TRUE);
				m_pPropLocation->Enable(TRUE);

				m_pPropState->SetValue(L"AL"); // or first state
				PopulateLocationsForState(scope, L"AL");
				m_pPropLocation->SetValue(L"All");
			}
			else if (scope.CompareNoCase(L"Location") == 0)
			{
				m_pPropState->Enable(TRUE);
				m_pPropLocation->Enable(TRUE);

				m_pPropState->SetValue(L"AL");
				PopulateLocationsForState(scope, L"AL");
				m_pPropLocation->SetValue(L"BREWTON 3 SSE"); // or first city
			}

			m_wndPropList.RedrawWindow();
		}
		else if (csName == L"State")
		{
			HandleStateChanged(pProp);
		}
		else if (csName == L"Location")
		{
			pDoc->Location = CString(varIn);
		}
		else if (csName == L"Starting Year")
		{
			pDoc->YearStart = long(varIn);
		}
		else if (csName == L"Ending Year")
		{
			pDoc->YearEnd = long(varIn);
		}
		else if (csName == L"Type")
		{
			pDoc->MeasurementText = CString(varIn);
		}
		else if (csName == L"Threshold")
		{
			pDoc->ThresholdText = CString(varIn);
		}
		else if (csName == L"Output")
		{
			CString value = CString(varIn);
			pDoc->Output = value;
			value.MakeLower();
			if (value == L"table")
			{
				m_pPropLayout->SetValue(COleVariant(L"Full"));
				pDoc->Layout = L"Full";
				m_pPropLayout->Enable(FALSE);
			}
			else
			{
				m_pPropLayout->Enable(TRUE);
			}
		}
		else if (csName == L"Layout")
		{
			pDoc->Layout = CString(varIn);
		}
	}

	pProp->SetOriginalValue(varIn);

	// update the view
	pView->SetupScrollBars();
	pView->Invalidate();

	return value;
} // OnPropertyChange

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::UpdatePropertiesFromDocument(CClimateExplorerDoc* pDoc)
{
	CClimateExplorerView* pView = ClimateExplorerView;

	// Iterate through top-level properties
	for (int i = 0; i < m_wndPropList.GetPropertyCount(); ++i)
	{
		CMFCPropertyGridProperty* pCategory = m_wndPropList.GetProperty(i);

		if (!pCategory) continue;

		// Iterate through child properties within the category
		for (int j = 0; j < pCategory->GetSubItemsCount(); ++j)
		{
			CMFCPropertyGridProperty* pProp = pCategory->GetSubItem(j);

			CString csName = pProp->GetName();
			if (csName == L"Title")
			{
				CString csTitle = pDoc->Title;
				pProp->SetValue(csTitle);
			}
			else if (csName == L"Subtitle")
			{
				CString csSubtitle = pDoc->Subtitle;
				pProp->SetValue(csSubtitle);
			}
			else if (csName == L"Publisher")
			{
				CString csPublisher = pDoc->Publisher;
				pProp->SetValue(csPublisher);
			}
			else if (csName == L"ISBN")
			{
				CString csISBN = pDoc->ISBN;
				pProp->SetValue(csISBN);
			}
			else if (csName == L"Description")
			{
				CString csDescription = pDoc->Description;
				pProp->SetValue(csDescription);
			}
			else if (csName == L"Copyright")
			{
				CString csCopyright = pDoc->Copyright;
				pProp->SetValue(csCopyright);
			}
			else if (csName == L"Export Folder")
			{
				CString csExportFolder = pDoc->ExportFolder;
				pProp->SetValue(csExportFolder);
			}
			else if (csName == L"Export Pages")
			{
				CString csExportPages = pDoc->ExportPages;
				pProp->SetValue(csExportPages);
			}
			else if (csName == L"Export DPI")
			{
				CString csValue;
				csValue.Format(L"%d", pDoc->ExportDPI);
				if (csValue.IsEmpty())
				{
					csValue = L"400";
				}
				pProp->SetValue(csValue);
			}
			else if (csName == L"Export Quality")
			{
				CString csValue;
				csValue.Format(L"%d", pDoc->ExportQuality);
				if (csValue.IsEmpty())
				{
					csValue = L"75";
				}
				pProp->SetValue(csValue);
			}
			else if (csName == L"Type of Query")
			{
				CString value = pDoc->QueryType;
				pProp->SetValue(value);
			}
			else if (csName == L"Natural Language")
			{
				CString value = pDoc->NaturalLanguage;
				pProp->SetValue(value);
			}
			else if (csName == L"SQL")
			{
				CString value = pDoc->SQL;
				pProp->SetValue(value);
			}
			else if (csName == L"Units")
			{
				CString value = pDoc->Units;
				pProp->SetValue(value);
			}
			else if (csName == L"Pure")
			{
				bool value = pDoc->Pure;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Active")
			{
				bool value = pDoc->Pure;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Scope")
			{
				CString value = pDoc->Scope;
				pProp->SetValue(value);
			}
			else if (csName == L"State")
			{
				CString value = pDoc->State;
				pProp->SetValue(value);
			}
			else if (csName == L"Location")
			{
				CString value = pDoc->Location;
				pProp->SetValue(value);
			}
			else if (csName == L"Starting Year")
			{
				long value = pDoc->YearStart;
				pProp->SetValue(value);
			}
			else if (csName == L"Ending Year")
			{
				long value = pDoc->YearEnd;
				pProp->SetValue(value);
			}
			else if (csName == L"Type")
			{
				CString value = pDoc->MeasurementText;
				pProp->SetValue(value);
			}
			else if (csName == L"Threshold")
			{
				CString value = pDoc->ThresholdText;
				pProp->SetValue(value);
			}
			else if (csName == L"Output")
			{
				CString value = pDoc->Output;
				pProp->SetValue(value);
			}
			else if (csName == L"Layout")
			{
				CString value = pDoc->Layout;
				pProp->SetValue(value);
			}
		}
	}

	// Refresh the property grid to reflect updates
	m_wndPropList.RedrawWindow();

} // UpdatePropertiesFromDocument

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers
void CPropertiesWnd::AdjustLayout()
{
	if
	(
		GetSafeHwnd() == nullptr ||
		(AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic())
	)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos
	(
		nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(),
		cyTlb, SWP_NOACTIVATE | SWP_NOZORDER
	);
	m_wndPropList.SetWindowPos
	(
		nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(),
		rectClient.Height() - (m_nComboHeight + cyTlb), SWP_NOACTIVATE | SWP_NOZORDER
	);
} // AdjustLayout

/////////////////////////////////////////////////////////////////////////////
int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap
	(
		theApp.m_bHiColorIcons ?
		IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */
	);

	m_wndToolBar.SetPaneStyle
	(
		m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY
	);
	m_wndToolBar.SetPaneStyle
	(
		m_wndToolBar.GetPaneStyle() &
		~(
			CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP |
			CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT
		)
	);

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control, not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
} // OnCreate

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnGoTo()
{
	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;
	if (!pDoc)
	{
		return;
	}
	CClimateExplorerView* pView = ClimateExplorerView;

	CMFCPropertyGridProperty* pSelectedProp = m_wndPropList.GetCurSel();
	CString propValue = pSelectedProp->GetValue().bstrVal;
	const int nPage = (int)_tstoi(propValue);
	pDoc->Page = nPage;
	const double dPageHeight = pDoc->HeightOfPage;
	const double dTop = dPageHeight * (nPage - 1);
	pView->TopOfView = dTop;
	pView->SetupScrollBars();
	pView->Invalidate();
} // OnGoTo

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnUpdateGoTo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	int nPages = 0;
	try
	{
		CClimateExplorerDoc* pDoc = ClimateExplorerDocument;
		if (!pDoc)
		{
			return;
		}
		nPages = pDoc->Pages;
	}
	catch (...)
	{
		return;
	}

	CMFCPropertyGridProperty* pSelectedProp = m_wndPropList.GetCurSel();
	if (pSelectedProp)
	{
		CMFCPropertyGridProperty* pParent = pSelectedProp->GetParent();
		if (pParent)
		{
			CString csParent = pParent->GetName();
			if (csParent == L"Table of Contents")
			{
				CString propName = pSelectedProp->GetName();
				CString propValue = pSelectedProp->GetValue().bstrVal;
				const int nPage = (int)_tstoi(propValue);
				if (1 <= nPage && nPage <= nPages)
				{
					pCmdUI->Enable(TRUE);
				}
			}
		}
	}
} // OnUpdateGoTo

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitPropList()
{
	SetPropListFont();
	CClimateExplorerApp* pApp = (CClimateExplorerApp*)AfxGetApp();
	CString csDescription;
	CString csCopyright;
	CString csPublisher;
	CString csSubtitle;
	if (pApp)
	{
		csSubtitle = pApp->DatabaseVersion;
		csDescription = pApp->DatabaseCredits;
		csCopyright = pApp->Copyright;
		csPublisher = pApp->Publisher;
	}

	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	m_wndPropList.Host = this;

	// ===============================================
	// Title Page Properties 
	// ===============================================

	CMFCPropertyGridProperty* pTitleGroup =
		new CMFCPropertyGridProperty(L"Title Page Properties");
	pTitleGroup->SetDescription
	(
		L"Properties of the document to be displayed on the title page."
	);

	pTitleGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Title",
			(_variant_t)L"Climate Explorer",
			L"The title of the document being created."
		)
	);

	pTitleGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Subtitle",
			(_variant_t)csSubtitle,
			L"Brief additional information about this title."
		)
	);

	pTitleGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Publisher",
			(_variant_t)csPublisher,
			L"The name of the person or organization publishing the document."
		)
	);

	pTitleGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"ISBN",
			(_variant_t)L"ISBN: ",
			L"The International Standard Book Number of the document."
		)
	);

	pTitleGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Copyright",
			(_variant_t)csCopyright,
			L"Copyright information for the document."
		)
	);

	pTitleGroup->AddSubItem
	(
		new CPropertyGridMultilineText
		(
			L"Description",
			csDescription,
			L"A description of the contents of this document."
		)
	);

	m_wndPropList.AddProperty(pTitleGroup);

	// ===============================================
	// Export Properties 
	// ===============================================

	CMFCPropertyGridProperty* pExportGroup =
		new CMFCPropertyGridProperty(L"Export Properties");
	pExportGroup->SetDescription
	(
		L"Specifies the information on how and where the document "
		L"will be exported."
	);

	CMFCPropertyGridFileProperty* pProp3 = new CMFCPropertyGridFileProperty
	(
		L"Export Folder",
		L""
	);
	pProp3->SetDescription(L"The folder where exported documents are written to.");
	pExportGroup->AddSubItem(pProp3);

	pExportGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Export Pages",
			(_variant_t)L"",
			L"The pages to be exported where:\n"
			L"  * A blank value is for all pages.\n"
			L"  * A range is spcified by a start and end separated with a dash.\n"
			L"  * Individual page numbers separated by commas.\n"
			L"  * Example: 5-8, 10-12, 15, 20\n"
			L"      would output pages 5, 6, 7, 8, 10, 11, 12, 15 and 20\n"
		)
	);

	pExportGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Export DPI",
			(_variant_t)L"400",
			L"The dots per inch used when exporting page images from the document."
		)
	);

	pExportGroup->AddSubItem
	(
		new CMFCPropertyGridProperty
		(
			L"Export Quality",
			(_variant_t)L"75",
			L"The quality of images in percent between 1 and 100 where a higher quality generates sharper images and a lower quality generates smaller images."
		)
	);

	m_wndPropList.AddProperty(pExportGroup);

	// ===============================================
	// Query Properties 
	// ===============================================

	CMFCPropertyGridProperty* pQueryGroup =
		new CMFCPropertyGridProperty(L"Query Properties");

	pQueryGroup->SetDescription
	(
		L"Properties that define how the climate query is constructed."
	);

	// ---------------------------------------------------------------
	// 1. Type of Query
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropQueryType =
		new CMFCPropertyGridProperty
		(
			L"Type of Query",
			(_variant_t)L"Picker",
			L"Specifies how the query will be constructed:\n"
			L"  • Natural Language — enter or select a natural-language query.\n"
			L"  • SQL — manually enter SQL text.\n"
			L"  • Picker — choose individual properties to build a query."
		);

	// Add dropdown options
	pPropQueryType->AddOption(L"Picker");
	pPropQueryType->AddOption(L"Natural Language");
	pPropQueryType->AddOption(L"SQL");

	// Default selection
	pPropQueryType->SetValue((_variant_t)L"Picker");

	// remember the property
	m_pPropQueryType = pPropQueryType;

	// Add to group
	pQueryGroup->AddSubItem(pPropQueryType);

	// Add the group to the property list
	m_wndPropList.AddProperty(pQueryGroup);

	// ---------------------------------------------------------------
	// 2. Natural Language Query (dropdown + editable text)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropNaturalLanguage =
		new CMFCPropertyGridProperty
		(
			L"Natural Language",
			(_variant_t)L"None",
			L"Select a predefined natural-language query or type your own."
		);

	// Make this an editable combo box
	pPropNaturalLanguage->AllowEdit(TRUE);          // allow manual typing

	// Add predefined natural-language queries
	pPropNaturalLanguage->AddOption(L"None");
	pPropNaturalLanguage->AddOption(L"List your table schemas.");
	pPropNaturalLanguage->AddOption(L"Show me the hottest states in July.");
	pPropNaturalLanguage->AddOption(L"List stations with rising trends.");
	pPropNaturalLanguage->AddOption(L"Find extreme temperature events.");
	pPropNaturalLanguage->AddOption(L"Show climate trends for Texas.");

	// Default value (empty)
	pPropNaturalLanguage->SetValue((_variant_t)L"None");

	// remember the property
	m_pPropNaturalLanguage = pPropNaturalLanguage;
	m_pPropNaturalLanguage->Enable(FALSE);

	// Add to group
	pQueryGroup->AddSubItem(pPropNaturalLanguage);

	// ---------------------------------------------------------------
	// 3. SQL (multiline editor)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropSQL =
		new CPropertyGridMultilineText
		(
			L"SQL",
			L"",
			L"Enter a SQL query. This field is enabled only when "
			L"'Type of Query' is set to SQL."
		);

	// remember the property
	m_pPropSQL = pPropSQL;
	m_pPropSQL->Enable(FALSE);

	// Add to group
	pQueryGroup->AddSubItem(pPropSQL);

	// ---------------------------------------------------------------
	// 4A. Units (degF, degC, raw)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropUnits =
		new CMFCPropertyGridProperty
		(
			L"Units",
			(_variant_t)L"degF",
			L"Select the output units for the query:\n"
			L"  • degF — Fahrenheit\n"
			L"  • degC — Celsius\n"
			L"  • raw — unconverted database values"
		);

	// Add dropdown options
	pPropUnits->AddOption(L"degF");
	pPropUnits->AddOption(L"degC");
	pPropUnits->AddOption(L"raw");

	// Default selection
	pPropUnits->SetValue((_variant_t)L"degF");

	// remember the property
	m_pPropUnits = pPropUnits;

	// Add to group
	pQueryGroup->AddSubItem(pPropUnits);

	// ---------------------------------------------------------------
	// 4B. Pure (boolean)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropPure =
		new CMFCPropertyGridProperty
		(
			L"Pure",
			(_variant_t)true,
			L"Include only pure (non-estimated) values in the query."
		);

	// remember the property
	m_pPropPure = pPropPure;

	// Add to group
	pQueryGroup->AddSubItem(pPropPure);

	// ---------------------------------------------------------------
	// 4C. Active (boolean)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropActive =
		new CMFCPropertyGridProperty
		(
			L"Active",
			(_variant_t)true,
			L"Include only active stations in the query."
		);

	// remember the property
	m_pPropActive = pPropActive;

	// Add to group
	pQueryGroup->AddSubItem(pPropActive);

	// ---------------------------------------------------------------
	// 4D. Scope (National, State, or Location)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropScope =
		new CMFCPropertyGridProperty
		(
			L"Scope",
			(_variant_t)L"National",
			L"Defines the geographic scope of the query:\n"
			L"  • National — entire United States\n"
			L"  • State — restrict to a specific state\n"
			L"  • Location — restrict to a specific station"
		);

	// Add dropdown options
	pPropScope->AddOption(L"National");
	pPropScope->AddOption(L"State");
	pPropScope->AddOption(L"Location");

	// Default selection
	pPropScope->SetValue((_variant_t)L"National");

	// remember the property
	m_pPropScope = pPropScope;

	// Add to group
	pQueryGroup->AddSubItem(pPropScope);

	// ---------------------------------------------------------------
	// 4E. State (dropdown, initially empty)
	// ---------------------------------------------------------------
	m_pPropState =
		new CMFCPropertyGridProperty
		(
			L"State",
			(_variant_t)L"None",
			L"Select a state when Scope is set to 'State'. "
			L"This list will be populated from the climate database."
		);

	//m_pPropState->AddOption(L"None");

	CClimateDatabase* pDB = pApp->ClimateDatabase;
	vector<CString> arrStates = pDB->States;
	for (auto& csState : arrStates)
	{
		m_pPropState->AddOption(csState);
	}

	// intially the scope is national
	m_pPropState->Enable();

	// Add to group
	pQueryGroup->AddSubItem(m_pPropState);

	// ---------------------------------------------------------------
	// 4F. Location (dropdown, initially empty)
	// ---------------------------------------------------------------
	m_pPropLocation =
		new CMFCPropertyGridProperty
		(
			L"Location",
			(_variant_t)L"None",
			L"Select a location when Scope is set to 'Location'. "
			L"This list will be populated from the climate database."
		);

	//m_pPropLocation->AddOption(L"None");

	// intially the scope is national
	m_pPropLocation->Enable(FALSE);

	// Add to group
	pQueryGroup->AddSubItem(m_pPropLocation);

	// ---------------------------------------------------------------
	// 4G. Starting Year (integer)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropStartYear =
		new CMFCPropertyGridProperty
		(
			L"Starting Year",
			(_variant_t)(long)1900,
			L"Enter the starting year for the query. "
			L"Database limits will be applied later."
		);

	// Optional: allow spin control (safe for integers)
	pPropStartYear->EnableSpinControl(TRUE, 1853, 3000);

	// remember the property
	m_pPropYearStart = pPropStartYear;

	// Add to group
	pQueryGroup->AddSubItem(pPropStartYear);

	// ---------------------------------------------------------------
	// 4H. Ending Year (integer)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropEndYear =
		new CMFCPropertyGridProperty
		(
			L"Ending Year",
			(_variant_t)(long)2025,
			L"Enter the ending year for the query. "
			L"Database limits will be applied later."
		);

	// Optional: allow spin control (safe for integers)
	pPropEndYear->EnableSpinControl(TRUE, 1853, 3000);

	// remember the property
	m_pPropYearEnd = pPropEndYear;

	// Add to group
	pQueryGroup->AddSubItem(pPropEndYear);

	// ---------------------------------------------------------------
	// 4I. Type (Maximum, Minimum, Average)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropType =
		new CMFCPropertyGridProperty
		(
			L"Type",
			(_variant_t)L"Maximum",
			L"Select the measurement type:\n"
			L"  • Maximum — daily/monthly high\n"
			L"  • Minimum — daily/monthly low\n"
			L"  • Average — mean temperature"
		);

	// Add dropdown options
	pPropType->AddOption(L"Maximum");
	pPropType->AddOption(L"Minimum");
	pPropType->AddOption(L"Average");

	// Default selection
	pPropType->SetValue((_variant_t)L"Maximum");

	// remember the property
	m_pPropType = pPropType;

	// Add to group
	pQueryGroup->AddSubItem(pPropType);

	// ---------------------------------------------------------------
	// 4J. Threshold (dropdown)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropThreshold =
		new CMFCPropertyGridProperty
		(
			L"Threshold",
			(_variant_t)L"90",
			L"Select a threshold value. Values are shown in the current units."
		);

	// Add Fahrenheit defaults (90–125 in steps of 5)
	pPropThreshold->AddOption(L"90");
	pPropThreshold->AddOption(L"95");
	pPropThreshold->AddOption(L"100");
	pPropThreshold->AddOption(L"105");
	pPropThreshold->AddOption(L"110");
	pPropThreshold->AddOption(L"115");
	pPropThreshold->AddOption(L"120");
	pPropThreshold->AddOption(L"125");

	// remember the property
	m_pPropThreshold = pPropThreshold;

	// Add to group
	pQueryGroup->AddSubItem(pPropThreshold);

	// ---------------------------------------------------------------
	// 4K. Output (Plot, Table, Map + Plot)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropOutput =
		new CMFCPropertyGridProperty
		(
			L"Output",
			(_variant_t)L"Plot",
			L"Select the visual output format for the query."
		);

	// Add dropdown options
	pPropOutput->AddOption(L"Plot");
	pPropOutput->AddOption(L"Table");
	pPropOutput->AddOption(L"Map + Plot");

	// Default selection
	pPropOutput->SetValue((_variant_t)L"Plot");

	// remember the property
	m_pPropOutput = pPropOutput;

	// Add to group
	pQueryGroup->AddSubItem(pPropOutput);

	// ---------------------------------------------------------------
	// 4L. Layout (Full, Half, Quarter)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropLayout =
		new CMFCPropertyGridProperty
		(
			L"Layout",
			(_variant_t)L"Full",
			L"Select the page layout:\n"
			L"  • Full    — landscape plot or full-page table\n"
			L"  • Half    — portrait mode, stacked vertically\n"
			L"  • Quarter — landscape, quarter-page panel"
		);

	// Add dropdown options
	pPropLayout->AddOption(L"Full");
	pPropLayout->AddOption(L"Half");
	pPropLayout->AddOption(L"Quarter");

	// Default selection
	pPropLayout->SetValue((_variant_t)L"Full");

	// remember the property
	m_pPropLayout = pPropLayout;

	// Add to group
	pQueryGroup->AddSubItem(pPropLayout);

} // InitPropList

/////////////////////////////////////////////////////////////////////////////
// the user select a different state postal code
void CPropertiesWnd::HandleStateChanged(CMFCPropertyGridProperty* pProp)
{
	CString state = pProp->GetValue().bstrVal;
	CString scope = m_pPropScope->GetValue().bstrVal;

	PopulateLocationsForState(scope, state);

	if (scope.CompareNoCase(L"State") == 0)
		m_pPropLocation->SetValue(L"All");
	else
		m_pPropLocation->SetValue(L"None");
} // HandleStateChanged

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::UpdateTableOfContents()
{
	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;

	if (!pDoc || !m_wndPropList.GetSafeHwnd())
		return;

	// Ensure the Table of Contents group exists
	if (!m_pTableOfContents)
	{
		m_pTableOfContents = new CMFCPropertyGridProperty(_T("Table of Contents"));
		m_pTableOfContents->SetDescription
		(
			L"A list of albums with their corresponding page numbers."
		);
		m_wndPropList.AddProperty(m_pTableOfContents);
	}
	else
	{
		// Remove existing sub-items
		int subItemCount = m_pTableOfContents->GetSubItemsCount();
		for (int i = subItemCount - 1; i >= 0; --i) // Remove from last to first
		{
			CMFCPropertyGridProperty* pSubItem = m_pTableOfContents->GetSubItem(0);
			m_pTableOfContents->RemoveSubItem(pSubItem);
			delete pSubItem; // Prevent memory leaks
		}
	}

	// Populate new sub-items from the document
	vector<pair<CString, int>>& arrTableOfContents =
		pDoc->AlbumTableOfContents;
	for (auto& album : arrTableOfContents)
	{
		CString csAlbum = album.first;
		int nPage = album.second;
		CString csPage;
		csPage.Format(L"%d", nPage);

		CMFCPropertyGridProperty* pSubItem =
			new CMFCPropertyGridProperty
			(
				csAlbum, (_variant_t)csPage,
				L"Page number of the named album."
			);
		pSubItem->AllowEdit(FALSE); // Make it read-only
		m_pTableOfContents->AddSubItem(pSubItem);
	}

	m_pTableOfContents->Expand();
	m_wndPropList.ExpandAll();
	m_wndPropList.RedrawWindow(); // Ensure UI updates

} // UpdateTableOfContents

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
} // OnSetFocus

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);

}

/////////////////////////////////////////////////////////////////////////////
