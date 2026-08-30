/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "framework.h"
#include "CHelper.h"
#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"
#include "ClimateExplorer.h"
#include "PropertyGridMultilineText.h"
#include "ColorNameProperty.h"
#include "ClimateDatabase.h"
#include "ImagePlus.h"

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
	CString csStyles = L"Solid,Dash,Dot,DashDot,DashDotDot";
	
	int nStart = 0;
	CString csToken = csStyles.Tokenize(L",", nStart);
	while (!csToken.IsEmpty())
	{
		m_arrLineStyles.push_back(csToken);
		csToken = csStyles.Tokenize(L",", nStart);
	}
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
	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;

	CClimateDatabase* pDB = ((CClimateExplorerApp*)AfxGetApp())->ClimateDatabase;

	if (scope.CompareNoCase(L"National") == 0)
	{
		m_pPropState->AddOption(L"None");
	}
	else
	{
		m_pPropState->AddOption(L"All");
		CString csState;
		if (pDoc != nullptr)
		{
			csState = pDoc->State;
		}

		int nStart = 1;
		if (!csState.IsEmpty() && csState != L"None")
		{
			nStart = 2;
		}

		// Actual states only
		for (auto& csState : pDB->States)
		{
			m_pPropState->AddOption(csState);
			if (nStart++ == 1)
			{
				m_pPropState->SetValue(csState);
				if (pDoc != nullptr)
				{
					pDoc->State = csState;
				}
			}
		}
	}
} // PopulateStatesForScope

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::PopulateLocationsForState(const CString& scope, const CString& state)
{
	m_pPropLocation->RemoveAllOptions();
	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;

	CClimateDatabase* pDB = ((CClimateExplorerApp*)AfxGetApp())->ClimateDatabase;

	if (scope.CompareNoCase(L"State") == 0)
	{
		m_pPropLocation->AddOption(L"None");
	}
	else if (scope.CompareNoCase(L"Location") == 0)
	{
		m_pPropLocation->AddOption(L"All");
		if (state == L"All")
		{
			return;
		}

		CString csCity;
		if (pDoc != nullptr)
		{
			csCity = pDoc->Location;
		}

		int nStart = 1;
		if (!csCity.IsEmpty() && csCity != L"None")
		{
			nStart = 2;
		}

		auto cities = pDB->Cities[state];
		for (auto& city : cities)
		{
			m_pPropLocation->AddOption(city);
			if (nStart++ == 1)
			{
				m_pPropLocation->SetValue(city);
				if (pDoc != nullptr)
				{
					pDoc->Location = city;
				}
			}
		}
	}
} // PopulateLocationsForState

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::PopulateThresholdForUnits()
{
	m_pPropThreshold->RemoveAllOptions();
	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;
	if (pDoc == nullptr)
	{
		return;
	}

	CString csUnits = pDoc->Units;

	vector<CString> arrF = { L"90", L"95", L"100", L"105", L"110", L"115", L"120", L"125" };
	vector<CString> arrC = { L"30", L"35", L"40", L"45", L"50", L"55" };
	vector<CString> arrR = { L"3000", L"3500", L"4000", L"4500", L"5000", L"5500" };

	CString value;
	if (csUnits == L"degF")
	{
		for (auto& node : arrF)
		{
			if (value.IsEmpty())
			{
				value = node;
			}
			m_pPropThreshold->AddOption(node);
		}
		
	}
	else if (csUnits == L"degC")
	{
		for (auto& node : arrC)
		{
			if (value.IsEmpty())
			{
				value = node;
			}
			m_pPropThreshold->AddOption(node);
		}
	}
	else
	{
		for (auto& node : arrR)
		{
			if (value.IsEmpty())
			{
				value = node;
			}
			m_pPropThreshold->AddOption(node);
		}
	}

	m_pPropThreshold->SetValue(_variant_t(value));

	pDoc->ThresholdText = value;

} // PopulateThresholdForUnits

/////////////////////////////////////////////////////////////////////////////
// change the output from an external event like a selection change
void CPropertiesWnd::ChangeOutput(CString value)
{
	m_pPropOutput->SetValue(_variant_t(value));
	OnPropertyChange((UINT_PTR)nullptr, (LONG_PTR)m_pPropOutput);

} // ChangeOutput

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
			CString csFolder = CString(varIn);
			CString csRelative = CHelper::ToRelative(csFolder);
			csRelative.TrimRight(L"\\");
			csRelative += L"\\";

			if (::PathFileExists(csRelative))
			{
				pDoc->ExportFolder = csRelative;
				m_pPropExportFolder->SetValue(_variant_t(csRelative));
			}
			else
			{
				CString csMessage;
				csMessage.Format
				(
					L"Pathname does not exist:\n%s",
					csRelative
				);
				AfxMessageBox(csMessage);
			}
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
	else if (csGroup == L"Render Properties")
	{
		if (csName == L"Output")
		{
			CString value = CString(varIn);
			pDoc->Output = value;
			value.MakeLower();
			if (value == L"plot")
			{
				m_pTrendGroup->Show();
				m_pGraphGroup->Show();
				m_pQueryGroup->Show(); 
			}
			else
			{
				m_pTrendGroup->Show(FALSE);
				m_pGraphGroup->Show(FALSE);
				m_pQueryGroup->Show(FALSE);
			}

			// there will eventually be multiple paths for 
			// different types: MD, MAP, and HTML
			if (value == L"image")
			{
				m_pPropImagePath->Show();
			}
			else
			{
				m_pPropImagePath->Show(FALSE);
			}
		}
		else if (csName == L"Content Title")
		{
			pDoc->ContentTitle = CString(varIn);
		}
		if (csName == L"Select an Image")
		{
			CString csPath = CString(varIn);
			if (csPath != pDoc->ImagePath)
			{
				CString csRelative = CHelper::ToRelative(csPath);

				if (::PathFileExists(csRelative))
				{
					pDoc->ImagePath = csRelative;
					m_pPropImagePath->SetValue(_variant_t(csRelative));
					CString csFile = CHelper::GetFileName(csPath);
					CString csTitle = m_pPropContentTitle->GetValue().bstrVal;
					if (csTitle.IsEmpty() || csTitle == L"Title")
					{
						m_pPropContentTitle->SetValue(_variant_t(csFile));
						pDoc->ContentTitle = csFile;
					}
				}
				else
				{
					CString csMessage;
					csMessage.Format
					(
						L"Pathname does not exist:\n%s",
						csRelative
					);
					AfxMessageBox(csMessage);
				}
			}
		}
		else if (csName == L"Layout")
		{
			pDoc->Layout = CString(varIn);
		}
		else if (csName == L"Placement")
		{
			pDoc->Placement = CString(varIn);
		}
		else if (csName == L"Units")
		{
			pDoc->Units = CString(varIn);
			PopulateThresholdForUnits();
		}
	}
	else if (csGroup == L"Query Properties")
	{
		if (csName == L"Pure")
		{
			pDoc->Pure = bool(varIn);
		}
		else if (csName == L"Scope")
		{
			CString scope = CString(varIn);
			pDoc->Scope = scope;

			PopulateStatesForScope(scope);

			if (scope.CompareNoCase(L"National") == 0)
			{
				m_pPropState->Enable();
				m_pPropLocation->Enable(FALSE);

				m_pPropState->Show(FALSE);
				m_pPropLocation->Show(FALSE);
			}
			else if (scope.CompareNoCase(L"State") == 0)
			{
				m_pPropState->Enable();
				m_pPropLocation->Enable(FALSE);

				m_pPropState->Show();
				m_pPropLocation->Show(FALSE);

			}
			else if (scope.CompareNoCase(L"Location") == 0)
			{
				m_pPropState->Enable();
				m_pPropLocation->Enable();

				m_pPropState->Show();
				m_pPropLocation->Show();

				CString csState = pDoc->State;
				PopulateLocationsForState(scope, csState);
				if (csState == L"All")
				{
					m_pPropLocation->Show(FALSE);
				}
			}

			CString csTitle = pDoc->Title;
			m_pPropContentTitle->SetValue(csTitle);

			m_wndPropList.RedrawWindow();
		}
		else if (csName == L"State")
		{
			HandleStateChanged(pProp);

			CString csTitle = pDoc->Title;
			m_pPropContentTitle->SetValue(csTitle);

			m_wndPropList.RedrawWindow();
		}
		else if (csName == L"Location")
		{
			pDoc->Location = CString(varIn);

			CString csTitle = pDoc->Title;
			m_pPropContentTitle->SetValue(csTitle);

			m_wndPropList.RedrawWindow();
		}
		else if (csName == L"Starting Year")
		{
			pDoc->YearStart = long(varIn);
		}
		else if (csName == L"Ending Year")
		{
			pDoc->YearEnd = long(varIn);
		}
		else if (csName == L"Subtype")
		{
			CString csSubtype = CString(varIn);
			pDoc->Subtype = csSubtype;

			if (csSubtype == L"Threshold")
			{
				m_pPropThreshold->Show();
				m_pPropUnits->Show();

				pDoc->LineColor = L"DarkRed";
				pDoc->RunningAvgColor = L"Red";

				m_pPropLineColor->SetValue(_variant_t(L"DarkRed"));
				m_pPropRunningAvgColor->SetValue(_variant_t(L"Red"));
				m_pPropState->SetValue(_variant_t(pDoc->State));
				m_pPropLocation->SetValue(_variant_t(pDoc->Location));
			}
			else
			{
				m_pPropThreshold->Show(FALSE);
			}

			if (csSubtype == L"Stations")
			{
				pDoc->LineColor = L"DarkGreen";
				pDoc->LineStyle = Gdiplus::DashStyleDash;

				m_pPropLineColor->SetValue(_variant_t(L"DarkGreen"));
				m_pPropLineStyle->SetValue(L"Dash");
				m_pPropUnits->Show(FALSE);
				m_pPropState->SetValue(_variant_t(pDoc->State));
				m_pPropLocation->SetValue(_variant_t(pDoc->Location));
			}
			else
			{
				m_pPropUnits->Show();
				pDoc->LineStyle = Gdiplus::DashStyleDot;
				m_pPropLineStyle->SetValue(L"Dot");
			}

			if (csSubtype == L"Maximum")
			{
				pDoc->LineColor = L"DarkRed";
				pDoc->RunningAvgColor = L"Red";

				const COleVariant oLC = m_pPropLineColor->GetValue();
				VARTYPE vt = oLC.vt;
				m_pPropLineColor->SetValue(_variant_t(L"DarkRed"));
				m_pPropRunningAvgColor->SetValue(_variant_t(L"Red"));
				m_pPropState->SetValue(_variant_t(pDoc->State));
				m_pPropLocation->SetValue(_variant_t(pDoc->Location));
			}
			else if (csSubtype == L"Minimum")
			{
				pDoc->LineColor = L"DarkBlue";
				pDoc->RunningAvgColor = L"Blue";

				m_pPropLineColor->SetValue(_variant_t(L"DarkBlue"));
				m_pPropRunningAvgColor->SetValue(_variant_t(L"Blue"));
				m_pPropState->SetValue(_variant_t(pDoc->State));
				m_pPropLocation->SetValue(_variant_t(pDoc->Location));
			}
			else if (csSubtype == L"Average")
			{
				pDoc->LineColor = L"DarkMagenta";
				pDoc->RunningAvgColor = L"Magenta";

				m_pPropLineColor->SetValue(_variant_t(L"DarkMagenta"));
				m_pPropRunningAvgColor->SetValue(_variant_t(L"Magenta"));
				m_pPropState->SetValue(_variant_t(pDoc->State));
				m_pPropLocation->SetValue(_variant_t(pDoc->Location));
			}

			CString csTitle = pDoc->Title;
			m_pPropContentTitle->SetValue(csTitle);

			m_wndPropList.RedrawWindow();
		}
		else if (csName == L"Threshold")
		{
			pDoc->ThresholdText = CString(varIn);

			CString csTitle = pDoc->Title;
			m_pPropContentTitle->SetValue(csTitle);

			m_wndPropList.RedrawWindow();
		}
	}
	else if (csGroup == L"Graph Properties")
	{
		if (csName == L"Axis Label X")
		{
			pDoc->AxisLabelX = CString(varIn);
		}
		else if (csName == L"Axis Label Y")
		{
			pDoc->AxisLabelY = CString(varIn);
		}
		else if (csName == L"Primary Line Color")
		{
			CString csName = CString(varIn);
			pDoc->LineColor = csName;
			m_wndPropList.EndEditItem();
		}
		else if (csName == L"Primary Line Style")
		{
			CString value = CString(varIn);
			pDoc->LineStyle = pDoc->LineStyleEnum[value];
		}
		else if (csName == L"Primary Line Thickness (in)")
		{
			pDoc->LineThicknessInches = double(varIn);
		}
		if (csName == L"Running Average Color")
		{
			CString csName = CString(varIn);
			pDoc->RunningAvgColor = csName;
			m_wndPropList.EndEditItem();
		}
		else if (csName == L"Running Average Style")
		{
			CString value = CString(varIn);
			pDoc->RunningAvgStyle = pDoc->LineStyleEnum[value];
		}
		else if (csName == L"Running Average Thickness (in)")
		{
			pDoc->RunningAvgThicknessInches = double(varIn);
		}
		if (csName == L"Grid Line Color")
		{
			CString csName = CString(varIn);
			pDoc->GridColor = csName;
			m_wndPropList.EndEditItem();
		}
		else if (csName == L"Grid Line Style")
		{
			CString value = CString(varIn);
			pDoc->GridLineStyle = pDoc->LineStyleEnum[value];
		}
		else if (csName == L"Grid Line Thickness (in)")
		{
			pDoc->GridLineThicknessInches = double(varIn);
		}
		else if (csName == L"Title Font Size (pt)")
		{
			pDoc->TitleFontSizePoints = int(varIn);
		}
		else if (csName == L"Axis Label Font Size (pt)")
		{
			pDoc->AxisLabelFontSizePoints = int(varIn);
		}
		else if (csName == L"Tick Label Font Size (pt)")
		{
			pDoc->TickLabelFontSizePoints = int(varIn);
		}
		else if (csName == L"Left Padding (in)")
		{
			pDoc->LeftPaddingInches = double(varIn);
		}
		else if (csName == L"Right Padding (in)")
		{
			pDoc->RightPaddingInches = double(varIn);
		}
		else if (csName == L"Top Padding (in)")
		{
			pDoc->TopPaddingInches = double(varIn);
		}
		else if (csName == L"Bottom Padding (in)")
		{
			pDoc->BottomPaddingInches = double(varIn);
		}
	}
	else if (csGroup == L"Trending Properties")
	{
		if (csName == L"Trend One Enable")
		{
			pDoc->TrendOneEnable = bool(varIn);
		}
		else if (csName == L"Trend One Color")
		{
			CString csName = CString(varIn);
			pDoc->TrendOneColor = csName;
			m_wndPropList.EndEditItem();
		}
		else if (csName == L"Trend One Style")
		{
			CString value = CString(varIn);
			pDoc->TrendOneStyle = pDoc->LineStyleEnum[value];
		}
		else if (csName == L"Trend One Thickness (in)")
		{
			pDoc->TrendOneThickness = double(varIn);
		}
		else if (csName == L"Trend One Year")
		{
			pDoc->TrendOneYear = long(varIn);
		}
		if (csName == L"Trend Two Enable")
		{
			pDoc->TrendTwoEnable = bool(varIn);
		}
		else if (csName == L"Trend Two Color")
		{
			CString csName = CString(varIn);
			pDoc->TrendTwoColor = csName;
			m_wndPropList.EndEditItem();
		}
		else if (csName == L"Trend Two Style")
		{
			CString value = CString(varIn);
			pDoc->TrendTwoStyle = pDoc->LineStyleEnum[value];
		}
		else if (csName == L"Trend Two Thickness (in)")
		{
			pDoc->TrendTwoThickness = double(varIn);
		}
		else if (csName == L"Trend Two Year")
		{
			pDoc->TrendTwoYear = long(varIn);
		}
		if (csName == L"Trend Three Enable")
		{
			pDoc->TrendThreeEnable = bool(varIn);
		}
		else if (csName == L"Trend Three Color")
		{
			CString csName = CString(varIn);
			pDoc->TrendThreeColor = csName;
			m_wndPropList.EndEditItem();
		}
		else if (csName == L"Trend Three Style")
		{
			CString value = CString(varIn);
			pDoc->TrendThreeStyle = pDoc->LineStyleEnum[value];
		}
		else if (csName == L"Trend Three Thickness (in)")
		{
			pDoc->TrendThreeThickness = double(varIn);
		}
		else if (csName == L"Trend Three Year")
		{
			pDoc->TrendThreeYear = long(varIn);
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
			else if (csName == L"Pure")
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
			else if (csName == L"Subtype")
			{
				CString value = pDoc->Subtype;
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
			else if (csName == L"Content Title")
			{
				CString value = pDoc->ContentTitle;
				pProp->SetValue(value);
			}
			else if (csName == L"Layout")
			{
				CString value = pDoc->Layout;
				pProp->SetValue(value);
			}
			else if (csName == L"Units")
			{
				CString value = pDoc->Units;
				pProp->SetValue(value);
			}
			else if (csName == L"Select an Image")
			{
				CString value = pDoc->ImagePath;
				pProp->SetValue(value);
			}
			else if (csName == L"Axis Label X")
			{
				CString value = pDoc->AxisLabelX;
				pProp->SetValue(value);
			}
			else if (csName == L"Axis Label Y")
			{
				CString value = pDoc->AxisLabelY;
				pProp->SetValue(value);
			}
			else if (csName == L"Primary Line Color")
			{
				CString value = pDoc->LineColor;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Primary Line Style")
			{
				Gdiplus::DashStyle eStyle = pDoc->LineStyle;
				CString value = pDoc->LineStyleText[eStyle];
				pProp->SetValue(value);
			}
			else if (csName == L"Primary Line Thickness (in)")
			{
				double value = pDoc->LineThicknessInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Running Average Color")
			{
				CString value = pDoc->RunningAvgColor;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Running Average Style")
			{
				Gdiplus::DashStyle eStyle = pDoc->RunningAvgStyle;
				CString value = pDoc->LineStyleText[eStyle];
				pProp->SetValue(value);
			}
			else if (csName == L"Running Average Thickness (in)")
			{
				double value = pDoc->RunningAvgThicknessInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Grid Line Color")
			{
				CString value = pDoc->GridColor;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Grid Line Style")
			{
				Gdiplus::DashStyle eStyle = pDoc->GridLineStyle;
				CString value = pDoc->LineStyleText[eStyle];
				pProp->SetValue(value);
			}
			else if (csName == L"Grid Line Thickness (in)")
			{
				double value = pDoc->GridLineThicknessInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Title Font Size (pt)")
			{
				long value = pDoc->TitleFontSizePoints;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Axis Label Font Size (pt)")
			{
				long value = pDoc->AxisLabelFontSizePoints;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Tick Label Font Size (pt)")
			{
				long value = pDoc->TickLabelFontSizePoints;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Left Padding (in)")
			{
				double value = pDoc->LeftPaddingInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Right Padding (in)")
			{
				double value = pDoc->RightPaddingInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Top Padding (in)")
			{
				double value = pDoc->TopPaddingInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Bottom Padding (in)")
			{
				double value = pDoc->BottomPaddingInches;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend One Enable")
			{
				bool value = pDoc->TrendOneEnable;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend One Color")
			{
				CString value = pDoc->TrendOneColor;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend One Style")
			{
				Gdiplus::DashStyle eStyle = pDoc->TrendOneStyle;
				CString value = pDoc->LineStyleText[eStyle];
				pProp->SetValue(value);
			}
			else if (csName == L"Trend One Thickness (in)")
			{
				double value = pDoc->TrendOneThickness;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend One Year")
			{
				long value = pDoc->TrendOneYear;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Two Enable")
			{
				bool value = pDoc->TrendTwoEnable;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Two Color")
			{
				CString value = pDoc->TrendTwoColor;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Two Style")
			{
				Gdiplus::DashStyle eStyle = pDoc->TrendTwoStyle;
				CString value = pDoc->LineStyleText[eStyle];
				pProp->SetValue(value);
			}
			else if (csName == L"Trend Two Thickness (in)")
			{
				double value = pDoc->TrendTwoThickness;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Two Year")
			{
				long value = pDoc->TrendTwoYear;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Three Enable")
			{
				bool value = pDoc->TrendThreeEnable;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Three Color")
			{
				CString value = pDoc->TrendThreeColor;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Three Style")
			{
				Gdiplus::DashStyle eStyle = pDoc->TrendThreeStyle;
				CString value = pDoc->LineStyleText[eStyle];
				pProp->SetValue(value);
			}
			else if (csName == L"Trend Three Thickness (in)")
			{
				double value = pDoc->TrendThreeThickness;
				pProp->SetValue(_variant_t(value));
			}
			else if (csName == L"Trend Three Year")
			{
				long value = pDoc->TrendThreeYear;
				pProp->SetValue(_variant_t(value));
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
void CPropertiesWnd::InitTitlePageProperties()
{
	CString csDescription;
	CString csCopyright;
	CString csPublisher;
	CString csSubtitle;

	csSubtitle = theApp.DatabaseVersion;
	csDescription = theApp.DatabaseCredits;
	csCopyright = theApp.Copyright;
	csPublisher = theApp.Publisher;

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

} // InitTitlePageProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitExportPageProperties()
{
	CMFCPropertyGridProperty* pExportGroup =
		new CMFCPropertyGridProperty(L"Export Properties");
	pExportGroup->SetDescription
	(
		L"Specifies the information on how and where the document "
		L"will be exported."
	);

	m_pPropExportFolder = new CMFCPropertyGridFileProperty
	(
		L"Export Folder",
		L".\\Books\\"
	);
	m_pPropExportFolder->SetDescription
	(
		L"The folder where exported documents are written to."
	);
	pExportGroup->AddSubItem(m_pPropExportFolder);

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

} // InitExportPageProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitRenderProperties()
{
	CMFCPropertyGridProperty* pRenderGroup =
		new CMFCPropertyGridProperty(L"Render Properties");

	pRenderGroup->SetDescription
	(
		L"Properties that define how the query is rendered."
	);

	m_wndPropList.AddProperty(pRenderGroup);

	// ---------------------------------------------------------------
	// Output (Plot, Image...)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropOutput =
		new CMFCPropertyGridProperty
		(
			L"Output",
			(_variant_t)L"Plot",
			L"Select the visual output type:\n"
			L"  • Plot  — Plots the result of a query.\n"
			L"  • Image — Displays an image the user selected.\n"
			L"  • Map   — Displays a map of stations based on location.\n"
			L"  • MD    — Displays Markdown file the user selected.\n"
			L"  • HTML  — Displays an HTML link the user selected."
		);

	// Add dropdown options
	pPropOutput->AddOption(L"Plot");
	pPropOutput->AddOption(L"Image");
	pPropOutput->AddOption(L"Map");
	pPropOutput->AddOption(L"MD");
	pPropOutput->AddOption(L"HTML");

	// Default selection
	pPropOutput->SetValue((_variant_t)L"Plot");

	// remember the property
	m_pPropOutput = pPropOutput;

	// Add to group
	pRenderGroup->AddSubItem(pPropOutput);

	// ---------------------------------------------------------------
	// Title of the content drawn above the content
	// ---------------------------------------------------------------
	m_pPropContentTitle =
		new CMFCPropertyGridProperty
		(
			L"Content Title",
			(_variant_t)L"Title",
			L"The title text placed at the top of the content."
		);

	// Add to group
	pRenderGroup->AddSubItem(m_pPropContentTitle);

	// CMFCPropertyGridProperty file filter
	static TCHAR BASED_CODE szFilter[] = 
		L"JPG Files(*.jpg)|*.jpg|"
		L"JPEG Files(*.jpeg)|*.jpeg|"
		L"PNG Files(*.png)|*.png|"
		L"All Files(*.*)| *.*||";

	m_pPropImagePath = new CMFCPropertyGridFileProperty
	(
		L"Select an Image", TRUE,
		L"", L"jpg", 0, szFilter,
		L"Relative path to the image to be used for Image outputs."
	);

	// hidden by default and enabled when the output is Image
	m_pPropImagePath->Show(FALSE);

	pRenderGroup->AddSubItem(m_pPropImagePath);
	
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
	pRenderGroup->AddSubItem(m_pPropLayout);

	// ---------------------------------------------------------------
	// Placement (append, insert, or replace)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropPlacement =
		new CMFCPropertyGridProperty
		(
			L"Placement",
			(_variant_t)L"Append",
			L"Select the placement of new query execution:\n"
			L"  • Append  — append to the end of the document.\n"
			L"  • Insert  — insert before a SINGLE selection.\n"
			L"  • Replace — replace a sINGLE selection.\n"
			L"NOTE: Execute button is disabled for Insert\n"
			L" and Replace if a single item is not selected."
		);

	// Add dropdown options
	pPropPlacement->AddOption(L"Append");
	pPropPlacement->AddOption(L"Insert");
	pPropPlacement->AddOption(L"Replace");

	// Default selection
	pPropPlacement->SetValue((_variant_t)L"Append");

	// remember the property
	m_pPropPlacement = pPropPlacement;

	// Add to group
	pRenderGroup->AddSubItem(pPropPlacement);

	// ---------------------------------------------------------------
	// Units (degF, degC, raw)
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
	pRenderGroup->AddSubItem(pPropUnits);

} // InitRenderProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitQueryProperties()
{
	m_pQueryGroup =
		new CMFCPropertyGridProperty(L"Query Properties");

	m_pQueryGroup->SetDescription
	(
		L"Properties that define how the climate query is constructed."
	);

	// Add the group to the property list
	m_wndPropList.AddProperty(m_pQueryGroup);

	// ---------------------------------------------------------------
	// Pure (boolean)
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
	m_pQueryGroup->AddSubItem(pPropPure);

	// ---------------------------------------------------------------
	// Scope (National, State, or Location)
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
	m_pQueryGroup->AddSubItem(pPropScope);

	// ---------------------------------------------------------------
	// State (dropdown, initially empty)
	// ---------------------------------------------------------------
	m_pPropState =
		new CMFCPropertyGridProperty
		(
			L"State",
			(_variant_t)L"All",
			L"Select a state when Scope is set to 'State'. "
			L"This list will be populated from the climate database."
		);

	// when all is selected for the scope of state, a graph for all
	// states will be generated
	m_pPropState->AddOption(L"All");

	CClimateDatabase* pDB = theApp.ClimateDatabase;
	vector<CString> arrStates = pDB->States;
	for (auto& csState : arrStates)
	{
		m_pPropState->AddOption(csState);
	}

	// intially the scope is national
	m_pPropState->Enable(FALSE);
	m_pPropState->Show(FALSE);

	// Add to group
	m_pQueryGroup->AddSubItem(m_pPropState);

	// ---------------------------------------------------------------
	// Location (dropdown, initially empty)
	// ---------------------------------------------------------------
	m_pPropLocation =
		new CMFCPropertyGridProperty
		(
			L"Location",
			(_variant_t)L"All",
			L"Select a location when Scope is set to 'Location'. "
			L"This list will be populated from the climate database."
		);

	m_pPropLocation->AddOption(L"All");

	// intially the scope is national
	m_pPropLocation->Enable(FALSE);
	m_pPropLocation->Show(FALSE);

	// Add to group
	m_pQueryGroup->AddSubItem(m_pPropLocation);

	// ---------------------------------------------------------------
	// Starting Year (integer)
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
	m_pQueryGroup->AddSubItem(pPropStartYear);

	// ---------------------------------------------------------------
	// Ending Year (integer)
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
	m_pQueryGroup->AddSubItem(pPropEndYear);

	// ---------------------------------------------------------------
	// Subtype (Maximum, Minimum, Average, Threshold, or Stations)
	// ---------------------------------------------------------------
	CMFCPropertyGridProperty* pPropType =
		new CMFCPropertyGridProperty
		(
			L"Subtype",
			(_variant_t)L"Maximum",
			L"Select the measurement type:\n"
			L"  • Maximum — daily/monthly high\n"
			L"  • Minimum — daily/monthly low\n"
			L"  • Average — mean temperature\n"
			L"  • Threshold — percent maximums over a threshold value.\n"
			L"  • Stations — number of active stations."
		);

	// Add dropdown options
	pPropType->AddOption(L"Maximum");
	pPropType->AddOption(L"Minimum");
	pPropType->AddOption(L"Average");
	pPropType->AddOption(L"Threshold");
	pPropType->AddOption(L"Stations");

	// Default selection
	pPropType->SetValue((_variant_t)L"Maximum");

	// remember the property
	m_pPropType = pPropType;

	// Add to group
	m_pQueryGroup->AddSubItem(pPropType);

	// ---------------------------------------------------------------
	// Threshold (dropdown)
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
	m_pPropThreshold->Show(FALSE);

	m_pQueryGroup->Show();

	// Add to group
	m_pQueryGroup->AddSubItem(pPropThreshold);

} // InitQueryProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitGraphProperties()
{
	m_pGraphGroup = new CMFCPropertyGridProperty(L"Graph Properties");

	m_pGraphGroup->SetDescription
	(
		L"Properties of the graph being plotted."
	);

	// -------------------------------------------------------------
	// Graph text properties
	// -------------------------------------------------------------
	m_pPropAxisLabelX =
		new CMFCPropertyGridProperty
		(
			L"X Axis Label",
			(_variant_t)L"Year",
			L"Label describing the values along the X axis."
		);

	m_pPropAxisLabelY =
		new CMFCPropertyGridProperty
		(
			L"Y Axis Label",
			(_variant_t)L"Value",
			L"Label describing the values along the Y axis."
		);

	// -------------------------------------------------------------
	// Line properties
	// -------------------------------------------------------------
	m_pPropLineColor =
		new CColorNameProperty(L"Primary Line Color", L"DarkRed");

	m_pPropLineColor->SetDescription(L"Line Color of the primary value being plotted.");

	m_pPropLineStyle =
		new CMFCPropertyGridProperty
		(
			L"Primary Line Style",
			(_variant_t)L"Dot",
			L"Pick the line style for the primary value being plotted."
		);

	PopulateLineStyles(m_pPropLineStyle);

	m_pPropLineWeight =
		new CMFCPropertyGridProperty
		(
			L"Primary Line Thickness (in)",
			(_variant_t)0.015,
			L"The line weight in inches of the primary value being plotted."
		);

	// -------------------------------------------------------------
	// Running Average line properties
	// -------------------------------------------------------------
	m_pPropRunningAvgColor =
		new CColorNameProperty(L"Running Average Color", L"Red");

	m_pPropRunningAvgColor->SetDescription(L"Color of the running average line being plotted.");

	m_pPropRunningAvgStyle =
		new CMFCPropertyGridProperty
		(
			L"Running Average Style",
			(_variant_t)L"Solid",
			L"Pick the line style for the running average line being plotted."
		);

	PopulateLineStyles(m_pPropRunningAvgStyle);

	m_pPropRunningAvgWeight =
		new CMFCPropertyGridProperty
		(
			L"Running Average Thickness (in)",
			(_variant_t)0.03,
			L"The line weight in inches of the trend line being plotted."
		);

	// -------------------------------------------------------------
	// Grid properties
	// -------------------------------------------------------------
	m_pPropGridColor =
		new CColorNameProperty(L"Grid Line Color", L"Silver");

	m_pPropGridColor->SetDescription(L"Line Color of the reference grid.");

	m_pPropGridStyle =
		new CMFCPropertyGridProperty
		(
			L"Grid Line Style",
			(_variant_t)L"Dot",
			L"Pick the line style for the reference grid."
		);

	PopulateLineStyles(m_pPropGridStyle);

	m_pPropGridWeight =
		new CMFCPropertyGridProperty
		(
			L"Grid Line Thickness (in)",
			(_variant_t)0.03,
			L"The line weight in inches of the reference grid."
		);

	// -------------------------------------------------------------
	// Text sizes (points)
	// -------------------------------------------------------------
	m_pPropTitlePoints =
		new CMFCPropertyGridProperty
		(
			L"Title Font Size (pt)",
			(_variant_t)long(14),
			L"Font size in points of the title text."
		);

	m_pPropLabelPoints =
		new CMFCPropertyGridProperty
		(
			L"Axis Label Font Size (pt)",
			(_variant_t)long(10),
			L"Font size in points of the axis label text."
		);

	m_pPropTickPoints =
		new CMFCPropertyGridProperty
		(
			L"Tick Label Font Size (pt)",
			(_variant_t)long(9),
			L"Font size in points of the tick label text."
		);

	// -------------------------------------------------------------
	// Padding (inches)
	// -------------------------------------------------------------
	m_pPropLeftPad =
		new CMFCPropertyGridProperty
		(
			L"Left Padding (in)",
			(_variant_t)0.50,
			L"Size of the blank space in inches to the left of the graph."
		);

	m_pPropRightPad =
		new CMFCPropertyGridProperty
		(
			L"Right Padding (in)",
			(_variant_t)0.50,
			L"Size of the blank space in inches to the right of the graph."
		);

	m_pPropTopPad =
		new CMFCPropertyGridProperty
		(
			L"Top Padding (in)",
			(_variant_t)0.50,
			L"Size of the blank space in inches above the graph."
		);

	m_pPropBottomPad =
		new CMFCPropertyGridProperty
		(
			L"Bottom Padding (in)",
			(_variant_t)0.50,
			L"Size of the blank space in inches below the graph."
		);

	// -------------------------------------------------------------
	// Add all properties to the group
	// -------------------------------------------------------------
	m_pGraphGroup->AddSubItem(m_pPropAxisLabelX);
	m_pGraphGroup->AddSubItem(m_pPropAxisLabelY);

	m_pGraphGroup->AddSubItem(m_pPropLineColor);
	m_pGraphGroup->AddSubItem(m_pPropLineStyle);
	m_pGraphGroup->AddSubItem(m_pPropLineWeight);

	m_pGraphGroup->AddSubItem(m_pPropRunningAvgColor);
	m_pGraphGroup->AddSubItem(m_pPropRunningAvgStyle);
	m_pGraphGroup->AddSubItem(m_pPropRunningAvgWeight);

	m_pGraphGroup->AddSubItem(m_pPropGridColor);
	m_pGraphGroup->AddSubItem(m_pPropGridStyle);
	m_pGraphGroup->AddSubItem(m_pPropGridWeight);

	m_pGraphGroup->AddSubItem(m_pPropTitlePoints);
	m_pGraphGroup->AddSubItem(m_pPropLabelPoints);
	m_pGraphGroup->AddSubItem(m_pPropTickPoints);

	m_pGraphGroup->AddSubItem(m_pPropLeftPad);
	m_pGraphGroup->AddSubItem(m_pPropRightPad);
	m_pGraphGroup->AddSubItem(m_pPropTopPad);
	m_pGraphGroup->AddSubItem(m_pPropBottomPad);

	m_pGraphGroup->Show();

	// -------------------------------------------------------------
	// Add group to property list
	// -------------------------------------------------------------
	m_wndPropList.AddProperty(m_pGraphGroup);
} // InitGraphProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitTrendProperties()
{
	m_pTrendGroup = new CMFCPropertyGridProperty(L"Trending Properties");

	m_pTrendGroup->SetDescription
	(
		L"Properties of the trending curves (T1, T2, and T3) being plotted."
	);

	// -------------------------------------------------------------
	// Trend One Curve properties
	// -------------------------------------------------------------
	CMFCPropertyGridProperty* pPropEnableT1 =
		new CMFCPropertyGridProperty
		(
			L"Trend One Enable",
			(_variant_t)true,
			L"Enable the display of the Trend One curve."
		);


	CMFCPropertyGridProperty* pPropColorT1 =
		new CColorNameProperty(L"Trend One Color", L"Gold");

	pPropColorT1->SetDescription(L"Line color of the Trend One curve.");

	CMFCPropertyGridProperty* pPropStyleT1 =
		new CMFCPropertyGridProperty
		(
			L"Trend One Style",
			(_variant_t)L"DashDotDot",
			L"The line style for the Trend One curve."
		);

	PopulateLineStyles(pPropStyleT1);

	CMFCPropertyGridProperty* pPropWeightT1 =
		new CMFCPropertyGridProperty
		(
			L"Trend One Thickness (in)",
			(_variant_t)0.03,
			L"The line weight in inches of the Trend One curve."
		);

	CMFCPropertyGridProperty* pPropYearT1 =
		new CMFCPropertyGridProperty
		(
			L"Trend One Year",
			(_variant_t)(long)1900,
			L"The year Trend One curve begins."
		);

	pPropYearT1->EnableSpinControl(TRUE, 1853, 3000);

	// add the properties to the group
	m_pTrendGroup->AddSubItem(pPropEnableT1);
	m_pTrendGroup->AddSubItem(pPropColorT1);
	m_pTrendGroup->AddSubItem(pPropStyleT1);
	m_pTrendGroup->AddSubItem(pPropWeightT1);
	m_pTrendGroup->AddSubItem(pPropYearT1);

	// -------------------------------------------------------------
	// Trend Two Curve properties
	// -------------------------------------------------------------
	CMFCPropertyGridProperty* pPropEnableT2 =
		new CMFCPropertyGridProperty
		(
			L"Trend Two Enable",
			(_variant_t)true,
			L"Enable the display of the Trend Two curve."
		);

	CMFCPropertyGridProperty* pPropColorT2 =
		new CColorNameProperty(L"Trend Two Color", L"Orange");

	pPropColorT2->SetDescription(L"Line color of the Trend Two curve.");

	CMFCPropertyGridProperty* pPropStyleT2 =
		new CMFCPropertyGridProperty
		(
			L"Trend Two Style",
			(_variant_t)L"DashDotDot",
			L"The line style for the Trend Two curve."
		);

	PopulateLineStyles(pPropStyleT2);

	CMFCPropertyGridProperty* pPropWeightT2 =
		new CMFCPropertyGridProperty
		(
			L"Trend Two Thickness (in)",
			(_variant_t)0.03,
			L"The line weight in inches of the Trend Two curve."
		);

	CMFCPropertyGridProperty* pPropYearT2 =
		new CMFCPropertyGridProperty
		(
			L"Trend Two Year",
			(_variant_t)(long)1950,
			L"The year Trend Two curve begins."
		);

	pPropYearT2->EnableSpinControl(TRUE, 1853, 3000);

	// add the properties to the group
	m_pTrendGroup->AddSubItem(pPropEnableT2);
	m_pTrendGroup->AddSubItem(pPropColorT2);
	m_pTrendGroup->AddSubItem(pPropStyleT2);
	m_pTrendGroup->AddSubItem(pPropWeightT2);
	m_pTrendGroup->AddSubItem(pPropYearT2);

	// -------------------------------------------------------------
	// Trend Three Curve properties
	// -------------------------------------------------------------
	CMFCPropertyGridProperty* pPropEnableT3 =
		new CMFCPropertyGridProperty
		(
			L"Trend Three Enable",
			(_variant_t)true,
			L"Enable the display of the Trend Three curve."
		);


	CMFCPropertyGridProperty* pPropColorT3 =
		new CColorNameProperty(L"Trend Three Color", L"Olive");

	pPropColorT3->SetDescription(L"Line color of the Trend Three curve.");

	CMFCPropertyGridProperty* pPropStyleT3 =
		new CMFCPropertyGridProperty
		(
			L"Trend Three Style",
			(_variant_t)L"DashDotDot",
			L"The line style for the Trend Three curve."
		);

	PopulateLineStyles(pPropStyleT3);

	CMFCPropertyGridProperty* pPropWeightT3 =
		new CMFCPropertyGridProperty
		(
			L"Trend Three Thickness (in)",
			(_variant_t)0.03,
			L"The line weight in inches of the Trend Three curve."
		);

	CMFCPropertyGridProperty* pPropYearT3 =
		new CMFCPropertyGridProperty
		(
			L"Trend Three Year",
			(_variant_t)(long)2000,
			L"The year Trend Three curve begins."
		);

	pPropYearT3->EnableSpinControl(TRUE, 1853, 3000);

	// add the properties to the group
	m_pTrendGroup->AddSubItem(pPropEnableT3);
	m_pTrendGroup->AddSubItem(pPropColorT3);
	m_pTrendGroup->AddSubItem(pPropStyleT3);
	m_pTrendGroup->AddSubItem(pPropWeightT3);
	m_pTrendGroup->AddSubItem(pPropYearT3);

	m_pTrendGroup->Show();
	m_pTrendGroup->Expand();
	m_pTrendGroup->Enable();

	// -------------------------------------------------------------
	// Add group to property list
	// -------------------------------------------------------------
	m_wndPropList.AddProperty(m_pTrendGroup);
} // InitTrendProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitImageProperties()
{
} // InitImageProperties

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::InitPropList()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	m_wndPropList.Host = this;

	SetPropListFont();

	// ===============================================
	// Title Page Properties 
	// ===============================================
	InitTitlePageProperties();

	// ===============================================
	// Export Properties 
	// ===============================================
	InitExportPageProperties();

	// ===============================================
	// Render Properties 
	// ===============================================
	InitRenderProperties();

	// ===============================================
	// Query Properties 
	// ===============================================
	InitQueryProperties();

	// ===============================================
	// Graphing Properties
	// ===============================================
	InitGraphProperties();

	// ===============================================
	// Trending Properties
	// ===============================================
	InitTrendProperties();

	// ===============================================
	// Image Properties 
	// ===============================================
	InitImageProperties();

} // InitPropList

/////////////////////////////////////////////////////////////////////////////
// the user select a different state postal code
void CPropertiesWnd::HandleStateChanged(CMFCPropertyGridProperty* pProp)
{
	CString state = pProp->GetValue().bstrVal;
	CString scope = m_pPropScope->GetValue().bstrVal;
	
	CClimateExplorerDoc* pDoc = ClimateExplorerDocument;

	if (!pDoc || !m_wndPropList.GetSafeHwnd())
		return;

	pDoc->Scope = scope;
	pDoc->State = state;

	PopulateLocationsForState(scope, state);

	if (scope.CompareNoCase(L"State") == 0)
		m_pPropLocation->SetValue(L"All");

	if (state == L"All")
	{
		m_pPropLocation->SetValue(L"All");
		m_pPropLocation->Show(FALSE);
		pDoc->Location = L"All";
	}
	else
	{
		m_pPropLocation->Show();
		m_pPropLocation->SetValue(L"None");
	}
} // HandleStateChanged

/////////////////////////////////////////////////////////////////////////////
void CPropertiesWnd::UpdateTableOfContents(CClimateExplorerDoc* pDoc)
{
	if (pDoc == nullptr)
	{
		pDoc = ClimateExplorerDocument;
	}

	if (!pDoc || !m_wndPropList.GetSafeHwnd())
		return;

	// Ensure the Table of Contents group exists
	if (!m_pTableOfContents)
	{
		m_pTableOfContents = new CMFCPropertyGridProperty(_T("Table of Contents"));
		m_pTableOfContents->SetDescription
		(
			L"A list of pages with their corresponding page numbers."
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
		pDoc->TitleTableOfContents;
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
