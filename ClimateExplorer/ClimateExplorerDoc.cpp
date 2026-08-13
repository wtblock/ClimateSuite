/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ClimateExplorer.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"
#include "ClimateStation.h"
#include "ThumbnailDialog.h"
#include "MainFrm.h"
#include <propkey.h>
#include <xmllite.h>
#include <set>
#include "Color.h"

/////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "xmllite.lib")

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CClimateExplorerDoc, CBaseDoc)

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CClimateExplorerDoc, CBaseDoc)
	ON_COMMAND(ID_FILE_SAVE, &CClimateExplorerDoc::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CClimateExplorerDoc::OnFileSaveAs)
	ON_COMMAND(ID_EXECUTE_QUERY, &CClimateExplorerDoc::OnExecuteQuery)
	ON_UPDATE_COMMAND_UI(ID_EXECUTE_QUERY, &CClimateExplorerDoc::OnUpdateExecuteQuery)
	ON_COMMAND(ID_EDIT_DELETE, &CClimateExplorerDoc::OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CClimateExplorerDoc::OnUpdateEditDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerDoc::CClimateExplorerDoc()
{
	InitializeProperties();

	// initialize GDI dash lookup table
	vector<pair<CString, Gdiplus::DashStyle> > arrDash =
	{
		{ L"Solid", Gdiplus::DashStyleSolid },
		{ L"Dash", Gdiplus::DashStyleDash },
		{ L"Dot", Gdiplus::DashStyleDot },
		{ L"DashDot", Gdiplus::DashStyleDashDot },
		{ L"DashDotDot", Gdiplus::DashStyleDashDotDot }
	};

	for (auto& node : arrDash)
	{
		CString csKey = node.first;

		shared_ptr<Gdiplus::DashStyle> pDash =
			shared_ptr<Gdiplus::DashStyle>
			(new Gdiplus::DashStyle(node.second));
		m_mapDash.add(csKey, pDash);
	}

	

} // CClimateExplorerDoc

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerDoc::~CClimateExplorerDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::InitializeProperties()
{
	HeightOfPage = 11.0;
	WidthOfPage = 8.5;

	Pages = 2;
	Page = 1;
	Images = 0;
	ExportFolder = L".\\Books\\";
	ExportDPI = 400;
	ExportQuality = 75;

	Title = L"Climate Explorer";
	ISBN = L"ISBN: ";

	CClimateExplorerApp* pApp = (CClimateExplorerApp*)AfxGetApp();
	if (pApp)
	{
		Subtitle = pApp->DatabaseVersion;
		Publisher = pApp->Publisher;
		Copyright = pApp->Copyright;
		Description = pApp->DatabaseCredits;
	}

	QueryType = L"Picker";
	NaturalLanguage = L"None";
	Pure = true;
	Scope = L"National";
	State = L"None";
	Location = L"None";
	YearStart = 1900;
	YearEnd = 2025;
	Subtype = L"Maximum";
	ThresholdText = L"90";

	Units = L"degF";
	Output = L"Plot";
	Layout = L"Half";

	// -------------------------------------------------------------
	// Plot Text
	// -------------------------------------------------------------
	GraphTitle = L"Title";
	AxisLabelX = L"Year";
	AxisLabelY = L"Value";

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	LineColor = CColor::darkred;
	LineStyle = Gdiplus::DashStyleDot;
	LineThicknessInches = 0.015; // ~6 px at 400 DPI

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	TrendLine = TRUE;
	TrendLineColor = CColor::red;
	TrendLineStyle = Gdiplus::DashStyleSolid;
	TrendLineThicknessInches = 0.03; // ~12 px at 400 DPI

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	GridColor = CColor::silver;
	GridLineStyle = Gdiplus::DashStyleDot;
	GridLineThicknessInches = 0.020; // ~8 px at 400 DPI

	// -------------------------------------------------------------
	// Text sizes (points)
	// -------------------------------------------------------------
	TitleFontSizePoints = 14.0; // prominent title
	AxisLabelFontSizePoints = 12.0; // "Year", "Value"
	TickLabelFontSizePoints = 10.0; // numeric labels

	// -------------------------------------------------------------
	// Padding (inches)
	// -------------------------------------------------------------
	//LeftPaddingInches = 0.50; // 200 px
	RightPaddingInches = 0.50; // 200 px
	TopPaddingInches = 0.375; // 150 px
	BottomPaddingInches = 0.50; // 200 px

	// -------------------------------------------------------------
	// Tick mark length (inches)
	// -------------------------------------------------------------
	TickLengthInches = 0.030; // 12 px

	Clear();

} // InitializeProperties

/////////////////////////////////////////////////////////////////////////////
// Main function to generate Google Maps link
CString CClimateExplorerDoc::GenerateMapLink(double dLat, double dLong, bool bBing)
{
	CString value;
	if (!CHelper::NearlyEqual(dLat + dLong, 0.0))
	{
		if (bBing)
		{
			value.Format
			(
				L"https://www.bing.com/maps?q=%.7f,%.7f",
				dLat, dLong
			);
		}
		else // Google map
		{
			value.Format
			(
				L"https://www.google.com/maps?q=%.7f,%.7f",
				dLat, dLong
			);
		}
	}

	return value;
} // GenerateMapLink

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::OnSaveDocument(CString& csPath)
{
	bool value = false;
	IStream* pFileStream = nullptr;
	HRESULT hr = SHCreateStreamOnFileW(
		csPath, STGM_CREATE | STGM_WRITE, &pFileStream);

	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create file stream.");
		return value;
	}

	IXmlWriter* pWriter = nullptr;
	hr = CreateXmlWriter(__uuidof(IXmlWriter),
		reinterpret_cast<void**>(&pWriter),
		nullptr);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create XML writer.");
		pFileStream->Release();
		return value;
	}

	hr = pWriter->SetOutput(pFileStream);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to set output for XML writer.");
		pWriter->Release();
		pFileStream->Release();
		return value;
	}

	// ⭐ Enable pretty-print formatting
	pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

	value = true;

	pWriter->WriteStartDocument(XmlStandalone_Yes);
	pWriter->WriteStartElement(nullptr, L"ClimateExplorer", nullptr);

	auto WriteStringProp = [&](LPCWSTR name, const CString& val)
	{
		pWriter->WriteStartElement(nullptr, name, nullptr);
		pWriter->WriteAttributeString(nullptr, L"value", nullptr, val);
		pWriter->WriteAttributeString(nullptr, L"type", nullptr, L"string");
		pWriter->WriteEndElement();
	};

	WriteStringProp(L"Title", Title);
	WriteStringProp(L"Subtitle", Subtitle);
	WriteStringProp(L"Publisher", Publisher);
	WriteStringProp(L"ISBN", ISBN);
	WriteStringProp(L"Copyright", Copyright);
	WriteStringProp(L"Description", Description);
	WriteStringProp(L"ExportFolder", ExportFolder);
	WriteStringProp(L"ExportPages", ExportPages);

	CString csDpi;
	csDpi.Format(L"%d", ExportDPI > 0 ? ExportDPI : 400);
	WriteStringProp(L"ExportDPI", csDpi);

	CString csQuality;
	csQuality.Format(L"%d", ExportQuality > 0 ? ExportQuality : 75);
	WriteStringProp(L"ExportQuality", csQuality);

	pWriter->WriteEndElement();   // </ClimateExplorer>
	pWriter->WriteEndDocument();

	pWriter->Release();
	pFileStream->Release();

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	CPropertyGridCtrl* pProps = pProperties->PropList;
	pProperties->UpdatePropertiesFromDocument(this);

	// Build flat list of editable properties
	vector<CMFCPropertyGridProperty*> flat;
	pProps->BuildFlatList(flat);

	// clear modified state on every property so they are no 
	// longer bolded
	for ( auto& pProp : flat)
	{
		if (pProp->IsModified())
		{
			COleVariant var = pProp->GetValue();
			pProp->SetOriginalValue(var);
			pProp->ResetOriginalValue();
		}
	}

	SetModifiedFlag(FALSE);
	pProps->Invalidate();

	return value;
} // OnSaveDocument

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::PromptForFileName(CString& strFilePath)
{
	CFileDialog fileDlg
	(
		FALSE, L"CEx", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Climate Explorer Files (*.CEx)|*.CEx|All Files (*.*)|*.*||"
	);

	if (fileDlg.DoModal() == IDOK)
	{
		strFilePath = fileDlg.GetPathName();
		return TRUE; // User selected a valid file name
	}

	return FALSE; // User canceled
} // PromptForFileName

/////////////////////////////////////////////////////////////////////////////
// open a data file
bool CClimateExplorerDoc::Open(LPCTSTR szFilename, bool bRead, LPCTSTR pcszFileID)
{
	bool value = false;

	IStream* pFileStream = nullptr;
	HRESULT hr = SHCreateStreamOnFileW(szFilename, STGM_READ, &pFileStream);
	if (FAILED(hr))
	{
		AfxMessageBox( L"Failed to create file stream." );
		return value;
	}

	IXmlReader* pReader = nullptr;
	hr = CreateXmlReader
	(
		__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), nullptr
	);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create XML reader." );
		pFileStream->Release();
		return value;
	}

	hr = pReader->SetInput(pFileStream);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to set input for XML reader." );
		pReader->Release();
		pFileStream->Release();
		return value;
	}

	XmlNodeType nodeType;
	while (S_OK == pReader->Read(&nodeType))
	{
		if (nodeType == XmlNodeType_Element)
		{
			const WCHAR* pwszLocalName = nullptr;
			pReader->GetLocalName(&pwszLocalName, nullptr);

			if
				(
					wcscmp(pwszLocalName, L"Title") == 0 ||
					wcscmp(pwszLocalName, L"Subtitle") == 0 ||
					wcscmp(pwszLocalName, L"Publisher") == 0 ||
					wcscmp(pwszLocalName, L"ISBN") == 0 ||
					wcscmp(pwszLocalName, L"Copyright") == 0 ||
					wcscmp(pwszLocalName, L"Description") == 0 ||
					wcscmp(pwszLocalName, L"StartFolder") == 0 ||
					wcscmp(pwszLocalName, L"EndFolder") == 0 ||
					wcscmp(pwszLocalName, L"Query") == 0 ||
					wcscmp(pwszLocalName, L"ExportFolder") == 0 ||
					wcscmp(pwszLocalName, L"ExportPages") == 0 ||
					wcscmp(pwszLocalName, L"ExportDPI") == 0 ||
					wcscmp(pwszLocalName, L"ExportQuality") == 0
				)
			{
				if (pReader->MoveToFirstAttribute() == S_OK)
				{
					do
					{
						const WCHAR* pwszAttrName = nullptr;
						const WCHAR* pwszValue = nullptr;

						pReader->GetLocalName(&pwszAttrName, nullptr);
						pReader->GetValue(&pwszValue, nullptr);

						if (wcscmp(pwszAttrName, L"value") == 0)
						{
							CString elementName(pwszLocalName);
							CString elementValue(pwszValue);

							if (elementName == L"Title")
							{
								Title = elementValue;

							}
							else if (elementName == L"Subtitle")
							{
								Subtitle = elementValue;

							}
							else if (elementName == L"Publisher")
							{
								Publisher = elementValue;

							}
							else if (elementName == L"ISBN")
							{
								ISBN = elementValue;

							}
							else if (elementName == L"Copyright")
							{
								Copyright = elementValue;

							}
							else if (elementName == L"Description")
							{
								Description = elementValue;

							}
							else if (elementName == L"ExportFolder")
							{
								ExportFolder = elementValue;
							}
							else if (elementName == L"ExportPages")
							{
								ExportPages = elementValue;
							}
							else if (elementName == L"ExportDPI")
							{
								if (elementValue.IsEmpty())
								{
									elementValue = L"400";
								}
								UINT nValue = (UINT)_tstol(elementValue);

								ExportDPI = nValue;
							}
							else if (elementName == L"ExportQuality")
							{
								if (elementValue.IsEmpty())
								{
									elementValue = L"75";
								}
								UINT nValue = (UINT)_tstol(elementValue);

								ExportQuality = nValue;
							}
						}
					} while (pReader->MoveToNextAttribute() == S_OK);
				}
			}
		}
	}
	value = true;
	pReader->Release();
	pFileStream->Release();
	return value;
} // Open

/////////////////////////////////////////////////////////////////////////////
// Save the document data to a file
// lpszPathName = path name where to save document file
// if lpszPathName is NULL then the user will be prompted (SaveAs)
// note: lpszPathName can be different than 'm_strPathName'
// if 'bReplace' is TRUE will change file name if successful (SaveAs)
// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
BOOL CClimateExplorerDoc::DoSave(CString& csPath, BOOL bReplace)
{
	CWaitCursor wait;
	if (csPath.IsEmpty())
	{
		// Prompt user for file name
		CString strNewPath;
		if (!PromptForFileName(strNewPath))
			return FALSE; // User canceled

		csPath = strNewPath;
	}

	// Perform the actual save operation
	if (!OnSaveDocument(csPath))
		return FALSE; // Save failed

	// If bReplace is TRUE, update the document's path
	if (bReplace)
		SetPathName(csPath);

	return TRUE;

} // DoSave

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnFileSave()
{
	// Get the current file path
	CString strFilePath = GetPathName();

	if (strFilePath.IsEmpty())
	{
		// If no file is set, prompt for "Save As"
		OnFileSaveAs();
		return;
	}

	// Save document data
	if (!DoSave(strFilePath))
	{
		AfxMessageBox(L"Error saving file.");
	}

} // OnFileSave

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::SaveModified()
{
	if (!IsModified())
		return TRUE;    // nothing to do, go ahead and close

	int res = AfxMessageBox(_T("Save changes?"), MB_YESNOCANCEL);

	if (res == IDCANCEL)
		return FALSE;   // ❌ cancel: do NOT close the document

	if (res == IDNO)
		return TRUE;    // ❌ no: close WITHOUT saving

	// ✅ yes: try to save
	OnFileSave();

	return TRUE;        // saved successfully → close
}

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnFileSaveAs()
{
	// Get the current file path
	CString strFilePath;

	// Save document data
	if (!DoSave(strFilePath, TRUE))
	{
		AfxMessageBox(L"Error saving file.");
	}

} // OnFileSaveAs

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::OnNewDocument()
{
	if (!CBaseDoc::OnNewDocument())
		return FALSE;

	InitializeProperties();

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr)
	{
		CPropertiesWnd* pProps = pFrame->PropertiesPane;
		pProps->UpdatePropertiesFromDocument(this);
		pProps->PopulateStatesForScope(Scope);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// the view associated with this document
CClimateExplorerView* CClimateExplorerDoc::GetClimateExplorerView()
{
	CClimateExplorerView* value = nullptr;
	CView* pView = nullptr;
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CClimateExplorerView)))
		{
			value = static_cast<CClimateExplorerView*>(pView);
			break;
		}
	}

	return value;

} // GetClimateExplorerView

/////////////////////////////////////////////////////////////////////////////
// get a vector corresponding to the document's table of contents
vector<pair<CString, int>>& CClimateExplorerDoc::GetTitleTableOfContents()
{
	m_arrTOC.clear();
	pair<CString, int> item;

	bool bTOC = false;
	for (auto& node : m_arrPages.Items)
	{
		CPage::PAGE_TYPE eType = node->PageType;
		if (bTOC && eType == CPage::pageTOC)
		{
			continue;
		}
		if (eType == CPage::pageTOC)
		{
			bTOC = true;
		}
		CString csTitle = node->Title;
		int nPage = node->Page;
		item.first = csTitle;
		item.second = nPage;
		m_arrTOC.push_back(item);
	}

	return m_arrTOC;

} // GetTitleTableOfContents

// CClimateExplorerDoc diagnostics

#ifdef _DEBUG
void CClimateExplorerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClimateExplorerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// collection of page numbers to be exported
CKeyedCollection<UINT, UINT>& CClimateExplorerDoc::GetExportPageNumbers()
{
	m_keyExportPages.clear();

	CString csPages = ExportPages;
	csPages.Trim();
	CString csToken;
	int nStart = 0;
	bool bDone = false;
	do
	{
		csToken = csPages.Tokenize(L",", nStart);
		csToken.Trim();
		bDone = csToken.IsEmpty();
		if (!bDone)
		{
			int nBegin = 0;
			CString csBegin = csToken.Tokenize(L"-", nBegin);
			csBegin.Trim();
			UINT nPage1 = (int)_tstol(csBegin);
			UINT nPage2 = 0;
			if (csBegin == csToken)
			{
				if (!m_keyExportPages.Exists[nPage1])
				{
					shared_ptr<UINT> pRight = shared_ptr<UINT>(new UINT(0));
					m_keyExportPages.add(nPage1, pRight);
				}
			}
			else
			{
				CString csEnd = csToken.Tokenize(L"-", nBegin);
				csEnd.Trim();
				nPage2 = (int)_tstol(csEnd);
				if (nPage1 < nPage2)
				{
					for (UINT nPage = nPage1; nPage <= nPage2; nPage++)
					{
						if (!m_keyExportPages.Exists[nPage])
						{
							shared_ptr<UINT> pRight = shared_ptr<UINT>(new UINT(0));
							m_keyExportPages.add(nPage, pRight);
						}
					}
				}
			}
		}

	} while (!bDone);

	// if empty, all pages are implied
	if (m_keyExportPages.Count == 0)
	{
		UINT nPages = Pages;
		for (UINT nPage = 1; nPage <= nPages; nPage++)
		{
			if (!m_keyExportPages.Exists[nPage])
			{
				shared_ptr<UINT> pRight = shared_ptr<UINT>(new UINT(0));
				m_keyExportPages.add(nPage, pRight);
			}
		}
	}

	return m_keyExportPages;
} // GetExportPageNumbers

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	BOOL value = FALSE;
	if (!CBaseDoc::OnOpenDocument(lpszPathName))
		return value;

	if (Open(lpszPathName))
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CPropertiesWnd* pProperties = pFrame->PropertiesPane;
		pProperties->UpdatePropertiesFromDocument( this );
		pProperties->PropList->ResetOriginalValues();
		InitDocument();
		CClimateExplorerView* pView = ClimateExplorerView;
		pView->Invalidate();
		value = TRUE;
	}

	return value;
} // OnOpenDocument

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnCloseDocument()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	Title = L"";
	Subtitle = L"";
	Publisher = L"";
	ISBN = L"";
	Description = L"";
	Copyright = L"";
	ExportFolder = L"";
	pProperties->UpdatePropertiesFromDocument(this);
	Clear();
	pProperties->UpdateTableOfContents();

	// Prevent MFC from doing a second save with empty contents
	SetModifiedFlag(FALSE);

	CBaseDoc::OnCloseDocument();
} // OnCloseDocument

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnExecuteQuery()
{
	CClimateExplorerView* pView = ClimateExplorerView;
	if (pView == nullptr)
	{
		return;
	}

	pView->Station = L"";
	pView->Latitude = 0.0f;
	pView->Longitude = 0.0f;

	// remove selection
	SelectLimit[-1] = -1;


	CClimateDatabase* pDB =
		((CClimateExplorerApp*)AfxGetApp())->ClimateDatabase;
	pDB->PopulateStations();

	double dPageHeight = HeightOfPage;
	CString csScope = Scope;
	CString csState = State;
	CString csCity = Location;
	long lYearStart = YearStart;
	long lYearEnd = YearEnd;
	CString csSubtype = Subtype;

	bool bAllState = csState == L"All";
	bool bAllCities = csCity == L"All";

	vector<CString> arrLocations;
	if (csScope == L"National")
	{
		arrLocations.push_back(csScope);
	}
	else if (csScope == L"State")
	{
		if (bAllState)
		{
			arrLocations = pDB->States;
		}
		else 
		{
			arrLocations.push_back(csState);
		}
	}
	else if (csScope == L"Location")
	{
		if (bAllCities)
		{
			if (bAllState)
			{
				vector<CString> arrStates = pDB->States;
				for (auto& csState : arrStates)
				{
					vector<CString> arrCities = pDB->Cities[csState];
					for (auto& csCity : arrCities)
					{
						CString csLoc;
						csLoc.Format(L"%s|%s", csState, csCity);
						arrLocations.push_back(csLoc);
					}
				}
			}
			else
			{
				arrLocations = pDB->Cities[csState];
			}
		}
		else
		{
			arrLocations.push_back(csCity);
		}
	}

	size_t nLocations = arrLocations.size();
	int nLocation = 1;

	// prepare the progress dialog
	theApp.OnIdle(0);
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	// launch the progress dialog
	CThumbnailDialog dlg;
	dlg.Parent = pFrame;
	dlg.CreateDlg();
	dlg.ShowWindow(SW_SHOW);
	CString csDialogTitle;
	csDialogTitle.Format(L"Rendering %s Locations", csScope);

	dlg.SetWindowText(csDialogTitle);
	dlg.Objects = L"Locations";

	dlg.TotalImages = (int)nLocations;

	bool bAbort = false;

	for (auto& csLocation : arrLocations)
	{
		if (nLocations > 1)
		{
			if (csScope == L"State")
			{
				State = csLocation;
			}
			else if (csScope == L"Location")
			{
				if (bAllState && bAllCities)
				{
					int nStart = 0;
					State = csLocation.Tokenize(L"|", nStart);
					Location = csLocation.Tokenize(L"|", nStart);
				}
				else
				{
					Location = csLocation;
				}
			}
		}

		// let the user cancel out
		if (dlg.Cancel)
		{
			bAbort = true;
			break;
		}

		// update the progress dialog's status
		dlg.CurrentImage = nLocation++;

		// 1. Build SQL from picker properties
		CString csSQL = BuildPickerSQL();

		// 2. Store SQL on the document
		SQL = csSQL;

		// 3. Write SQL to the Output window (SQL tab)
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame != nullptr && pFrame->OutputPane != nullptr)
		{
			pFrame->OutputPane->SQLText = csSQL;
		}

		// 4. Execute the query to generate output
		ExecutePickerQuery();

		int nPages = Pages;

		// page index is zero based
		int nPage = nPages - 1;
		shared_ptr<CPage> pPage = m_arrPages.get(nPage);
		if (pPage->PageIsFull)
		{
			pPage = shared_ptr<CPage>
				(new CPage(nPages + 1, Layout, this, CPage::pageGraph));
			m_arrPages.append(pPage);
			Pages = (UINT)m_arrPages.Count;
		}

		nPages = Pages;
		Page = pPage->Page;
		double dTop = dPageHeight * (nPages - 1);
		pView->TopOfView = dTop;
		pView->SetupScrollBars();
		pView->Invalidate();

		CRect rect = MarginRectangle;
		pPage->Rect = rect;

		CString csTitle, csState, csCity, csStation;
		float fLatitude = 0, fLongitude = 0;
		if (csScope == L"National")
		{
			csTitle.Format
			(
				L"%s, %s, %d, %d", csScope, csSubtype, lYearStart, lYearEnd
			);
		}
		else if (csScope == L"State")
		{
			csTitle.Format
			(
				L"%s, %s, %s, %d, %d", csScope, csSubtype, State, lYearStart, lYearEnd
			);
		}
		else if (csScope == L"Location")
		{
			csState = State;
			csCity = Location;
			csCity.TrimRight();
			CString csKey;
			csKey.Format(L"%s, %s", csState, csCity);
			shared_ptr<CClimateStation> pStation = pDB->StationByLocation[csKey];
			if (pStation != nullptr)
			{
				csStation = pStation->Station;
				fLatitude = pStation->Latitude;
				fLongitude = pStation->Longitude;
			}

			csTitle.Format
			(
				L"%s, %s, %s, %s, %d, %d", csScope, csSubtype, csState, csCity, lYearStart, lYearEnd
			);
		}

		// -------------------------------------------------------------
		// Generate the graph bitmap
		// -------------------------------------------------------------
		shared_ptr<CGraphPlotter> pPlot = 
			shared_ptr<CGraphPlotter>( new CGraphPlotter(this, Years, Values));
		pPlot->Station = csStation;
		pPlot->State = csState;
		pPlot->Location = csCity;
		pPlot->Latitude = fLatitude;
		pPlot->Longitude = fLongitude;

		pPage->AddAnImage(pPlot);

		// 5. Mark document modified (optional)
		SetModifiedFlag(TRUE);

		// wait ten milliseconds while letting normal 
		// window messaging to run
		pFrame->Wait(10);
	}

	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	pProperties->UpdateTableOfContents();

	// done with the progress dialog
	dlg.DestroyWindow();

} // OnExecuteQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnUpdateExecuteQuery(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

} // OnUpdateExecuteQuery

/////////////////////////////////////////////////////////////////////////////
// BuildPickerSQL
//
// Constructs a SQL query based on the picker settings in ClimateExplorer.
// The method supports three distinct modes:
//
//   1. Threshold mode   (Subtype == "Threshold")
//   2. Stations mode    (Subtype == "Stations")
//   3. Temperature mode (Subtype == "Max", "Min", "Avg" via MeasurementType)
//
// Each mode returns a completely different SQL structure and result type.
//
/////////////////////////////////////////////////////////////////////////////
CString CClimateExplorerDoc::BuildPickerSQL()
{
	CString sql;

	CString csSubtype = Subtype;

	/////////////////////////////////////////////////////////////////////////////
	// 1. THRESHOLD MODE
	//
	// Computes the percentage of *monthly* temperature values that exceed
	// a user‑defined threshold. This mode always uses the Months table and
	// always uses MeasurementType = 1 (maximum temperature).
	//
	// The threshold is converted into raw hundredths‑of‑°C because
	// Months.CentigradeRaw stores values in that format.
	//
	/////////////////////////////////////////////////////////////////////////////
	if (csSubtype == L"Threshold")
	{
		// Convert threshold to raw hundredths‑of‑°C
		double dThreshold = Threshold;
		double dRaw = dThreshold;
		CString csUnits = Units;

		if (csUnits == L"degF")
		{
			// Convert Fahrenheit → Celsius → hundredths‑C
			dRaw = (dThreshold - 32.0) * 5.0 / 9.0;
			dRaw *= 100;
		}
		else if (csUnits == L"degC")
		{
			// Convert Celsius → hundredths‑C
			dRaw *= 100;
		}

		int nRaw = (int)dRaw;

		// Build SQL: percent of monthly values >= threshold
		sql.Format
		(
			L"SELECT m.Year,\n"
			L"       100.0 * SUM(CASE WHEN m.CentigradeRaw >= %d THEN 1 ELSE 0 END)\n"
			L"             / SUM(CASE WHEN m.CentigradeRaw > -9000 THEN 1 ELSE 0 END) AS Percent\n"
			L"FROM Months m\n"
			L"JOIN Stations s ON m.StationID = s.StationID\n"
			L"WHERE m.MeasurementType = 1\n"
			L"  AND m.CentigradeRaw > -9000\n",
			nRaw
		);

		sql.AppendFormat(L"  AND m.Year >= %d\n", YearStart);
		sql.AppendFormat(L"  AND m.Year <= %d\n", YearEnd);

		/////////////////////////////////////////////////////////////////////////////
		// PURE MODE
		//
		// Pure mode filters out based on quality flags.
		//
		/////////////////////////////////////////////////////////////////////////////
		if (Pure)
		{
			sql += L"AND m.DMFLAG != 'E' \n";
			sql += L"AND m.QCFLAG = ' ' \n";
		}

		/////////////////////////////////////////////////////////////////////////////
		// SCOPE FILTERING
		//
		// National  → no additional filtering
		// State     → restrict to a specific state
		// Location  → restrict to a specific station
		//
		/////////////////////////////////////////////////////////////////////////////
		if (Scope == L"State" && State != L"None")
		{
			sql.AppendFormat(L"  AND s.State = '%s'\n", State.GetString());
		}

		if (Scope == L"Location" && Location != L"None")
		{
			sql.AppendFormat(L"  AND s.Location = '%s'\n", Location.GetString());
		}

		/////////////////////////////////////////////////////////////////////////////
		// Final grouping and ordering
		/////////////////////////////////////////////////////////////////////////////
		sql +=
			L"GROUP BY m.Year\n"
			L"ORDER BY m.Year;\n";

		return sql;
	}

	/////////////////////////////////////////////////////////////////////////////
	// 2. STATIONS MODE
	//
	// Counts how many stations were active in each year. A station is considered
	// active if it has at least one valid monthly reading (CentigradeRaw > -9000).
	//
	// This mode uses the Months table and ignores MeasurementType.
	//
	/////////////////////////////////////////////////////////////////////////////
	if (csSubtype == L"Stations")
	{
		sql.Format
		(
			L"SELECT m.Year,\n"
			L"       COUNT(DISTINCT m.StationID) AS ActiveStations\n"
			L"FROM Months m\n"
			L"WHERE m.CentigradeRaw > -9000\n"
			L"  AND m.Year >= %d\n"
			L"  AND m.Year <= %d\n"
			L"GROUP BY m.Year\n"
			L"ORDER BY m.Year;\n",
			YearStart,
			YearEnd
		);

		return sql;
	}

	/////////////////////////////////////////////////////////////////////////////
	// 3. TEMPERATURE MODE (Max / Min / Avg)
	//
	// This mode uses the Years table, which is *constructed by ImportUSHCN*
	// from the monthly USHCN station files (.tmax, .tmin, .tavg). The Years table
	// is not a NOAA annual product; it is an annual aggregation created entirely
	// from the monthly values parsed by ImportUSHCN.
	//
	// What the monthly USHCN files contain:
	//   - .tmax = monthly average of daily maximum temperatures
	//   - .tmin = monthly average of daily minimum temperatures
	//   - .tavg = monthly average of daily mean temperatures
	//   (all stored in hundredths of °C)
	//
	// What ImportUSHCN stores in the Years table:
	//   MaxValue = the maximum of the 12 monthly values for the year
	//   MinValue = the minimum of the 12 monthly values for the year
	//   AvgValue = the average of the non‑missing monthly values for the year
	//   ValidReadings = the number of non‑missing monthly values (0–12)
	//
	// These are *annual aggregates of monthly data*, stored in raw hundredths‑°C.
	// They are NOT daily maxima, NOT daily minima, and NOT daily means. They are
	// exactly the values used by ClimateHistory to generate its CSV output.
	//
	// MeasurementType selects which annual statistic is returned:
	//
	//   1 → MaxValue : max of the 12 monthly TMAX values
	//   2 → MinValue : min of the 12 monthly TMIN values
	//   3 → AvgValue : average of the 12 monthly TAVG values
	//
	// This method returns raw values (hundredths‑°C). Downstream plotting logic
	// converts these raw values into Fahrenheit for display.
	//
	/////////////////////////////////////////////////////////////////////////////
	int nMeasureType = (int)MeasurementType;

	CString column;

	// Map MeasurementType → Years table column
	switch (nMeasureType)
	{
	case 1: column = L"MaxValue"; break;
	case 2: column = L"MinValue"; break;
	case 3: column = L"AvgValue"; break;
	default: return sql; // invalid type
	}

	// Base SQL for annual temperature values
	sql.Format
	(
		L"SELECT y.Year, AVG(y.%s) AS RawValue\n"
		L"FROM Years y\n"
		L"JOIN Stations s ON y.StationID = s.StationID\n"
		L"WHERE y.MeasurementType = %d\n"
		L"  AND y.%s <> -9999\n",
		column.GetString(),
		nMeasureType,
		column.GetString()
	);

	sql.AppendFormat(L"  AND y.Year >= %d\n", YearStart);
	sql.AppendFormat(L"  AND y.Year <= %d\n", YearEnd);

	/////////////////////////////////////////////////////////////////////////////
	// PURE MODE
	//
	// For monthly USHCN data, a "complete" station‑year contains all 12 months.
	// Years.ValidReadings stores the number of months present.
	//
	// Pure mode filters out incomplete years by requiring ValidReadings = 12.
	//
	/////////////////////////////////////////////////////////////////////////////
	if (Pure)
	{
		sql += L"  AND y.ValidReadings = 12\n";
	}

	/////////////////////////////////////////////////////////////////////////////
	// SCOPE FILTERING
	//
	// National  → no additional filtering
	// State     → restrict to a specific state
	// Location  → restrict to a specific station
	//
	/////////////////////////////////////////////////////////////////////////////
	if (Scope == L"State" && State != L"None")
	{
		sql.AppendFormat(L"  AND s.State = '%s'\n", State.GetString());
	}

	if (Scope == L"Location" && Location != L"None")
	{
		sql.AppendFormat(L"  AND s.Location = '%s'\n", Location.GetString());
	}

	/////////////////////////////////////////////////////////////////////////////
	// Final grouping and ordering
	/////////////////////////////////////////////////////////////////////////////
	sql +=
		L"GROUP BY y.Year\n"
		L"ORDER BY y.Year;\n";

	return sql;
} // BuildPickerSQL

/////////////////////////////////////////////////////////////////////////////
// FormatTemperatureText
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::FormatTemperatureText()
{
	CString cs;
	CString csUnits = Units;
	CString csUnitLine;
	csUnitLine.Format(L"               % 8s\n", csUnits);

	cs += L"Year   Temperature\n";
	cs += csUnitLine;
	cs += L"---------------------------\n";

	for (const auto& r : m_arrTemperatureRows)
	{
		CString line;
		double dUnit = ConvertUnits[r.dTemperature];
		CString csTemp = FormatValue[dUnit];
		line.Format(L"%4d      %s\n", r.nYear, csTemp);
		cs += line;

		// populate the graph data arrays
		double dYear = double(r.nYear);
		Years.push_back(double(dYear));

		// the value is the converted temperture
		Values.push_back(dUnit);
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->FormattedText = cs;
	}

} // FormatTemperatureText

/////////////////////////////////////////////////////////////////////////////
// FormatThresholdText
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::FormatThresholdText()
{
	CString cs;

	cs += L"Year   Percent\n";
	cs += L"----------------\n";

	for (const auto& r : m_arrThresholdRows)
	{
		CString line;
		line.Format(L"%4d   %.2f\n", r.nYear, r.dPercent);
		cs += line;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->FormattedText = cs;
	}

} // FormatThresholdText

/////////////////////////////////////////////////////////////////////////////
// FormatStationText
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::FormatStationText()
{
	CString cs;

	cs += L"Year   Count\n";
	cs += L"-------------\n";

	for (const auto& r : m_arrStationRows)
	{
		CString line;
		line.Format(L"%4d   %d\n", r.nYear, r.nCount);
		cs += line;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->FormattedText = cs;
	}

} // FormatStationText

/////////////////////////////////////////////////////////////////////////////
// FormatTemperatureCSV
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::FormatTemperatureCSV()
{
	CString cs;

	cs += L"Year,Temperature\n";

	for (const auto& r : m_arrTemperatureRows)
	{
		CString line;
		double dUnit = ConvertUnits[r.dTemperature];
		CString csTemp = FormatValue[dUnit];
		csTemp.TrimLeft();
		line.Format(L"%d,%s\n", r.nYear, csTemp.GetString());
		cs += line;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->CSVText = cs;
	}

} // FormatTemperatureCSV

//void CClimateExplorerDoc::FormatTemperatureCSV()
//{
//	CString cs;
//
//	cs += L"Year,Month,Temperature\n";
//
//	for (const auto& r : m_arrTemperatureRows)
//	{
//		CString line;
//		double dUnit = ConvertUnits[r.dTemperature];
//		CString csTemp = FormatValue[dUnit];
//		csTemp.TrimLeft();
//		line.Format(L"%d,%d,%s\n", r.nYear, r.nMonth, csTemp.GetString());
//		cs += line;
//	}
//
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
//	{
//		pFrame->OutputPane->CSVText = cs;
//	}
//
//} // FormatTemperatureCSV
//
/////////////////////////////////////////////////////////////////////////////
// FormatThresholdCSV
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::FormatThresholdCSV()
{
	CString cs;

	cs += L"Year,Percent\n";

	for (const auto& r : m_arrThresholdRows)
	{
		CString line;
		line.Format(L"%d,%.2f\n", r.nYear, r.dPercent);
		cs += line;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->CSVText = cs;
	}

} // FormatThresholdCSV

/////////////////////////////////////////////////////////////////////////////
// FormatStationCSV
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::FormatStationCSV()
{
	CString cs;

	cs += L"Year,Count\n";

	for (const auto& r : m_arrStationRows)
	{
		CString line;
		line.Format(L"%d,%d\n", r.nYear, r.nCount);
		cs += line;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->CSVText = cs;
	}

} // FormatStationCSV

/////////////////////////////////////////////////////////////////////////////
// ConvertTemperatureRows
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ConvertTemperatureRows
(
	const CSmartArray<CSmartArray<CString>>& arrRaw
)
{
	m_arrTemperatureRows.clear();

	for (auto& pRow : arrRaw.Items)
	{
		CClimateTempRow r;

		int nCol = 0;
		for (auto& pCol : pRow->Items)
		{
			switch (nCol)
			{
			case 0: 
				r.nYear = _ttoi(*pCol); 
				break;
			case 1: 
				r.dTemperature = _ttof(*pCol);
			//case 1: 
			//	r.nMonth = _ttoi(*pCol); 
			//	break;
			//case 2: 
			//	r.dTemperature = _ttof(*pCol);
			}
			nCol++;
		}

		// data for the output window
		m_arrTemperatureRows.push_back(r);
	}

} // ConvertTemperatureRows

/////////////////////////////////////////////////////////////////////////////
// ConvertThresholdRows
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ConvertThresholdRows
(
	const CSmartArray<CSmartArray<CString>>& arrRaw
)
{
	m_arrThresholdRows.clear();

	for (auto& pRow : arrRaw.Items)
	{
		CClimateThresholdRow r;

		int nCol = 0;
		for (auto& pCol : pRow->Items)
		{
			switch (nCol)
			{
			case 0: 
				r.nYear = _ttoi(*pCol); 
				break;
			case 1: 
				r.dPercent = _ttof(*pCol);
			}
			nCol++;
		}

		// data to create the graph
		const double dYear = double(r.nYear);
		const double dValue = r.dPercent;
		Years.push_back(dYear);
		Values.push_back(dValue);

		// data for the output window
		m_arrThresholdRows.push_back(r);
	}

} // ConvertThresholdRows

/////////////////////////////////////////////////////////////////////////////
// ConvertStationRows
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ConvertStationRows
(
	const CSmartArray<CSmartArray<CString>>& arrRaw
)
{
	m_arrStationRows.clear();

	for (auto& pRow : arrRaw.Items)
	{
		CClimateStationRow r;

		int nCol = 0;
		for (auto& pCol : pRow->Items)
		{
			switch (nCol)
			{
			case 0: 
				r.nYear = _ttoi(*pCol); 
				break;
			case 1: 
				r.nCount = _ttoi(*pCol);
			}
			nCol++;
		}

		// data to create the graph
		const double dYear = double(r.nYear);
		const double dValue = double(r.nCount);
		Years.push_back(dYear);
		Values.push_back(dValue);

		// data for the output window
		m_arrStationRows.push_back(r);
	}

} // ConvertStationRows

/////////////////////////////////////////////////////////////////////////////
// ExecutePickerQuery
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ExecutePickerQuery()
{
	// -------------------------------------------------------------
	// 1. Clear previous results
	// -------------------------------------------------------------
	m_arrTemperatureRows.clear();
	m_arrThresholdRows.clear();
	m_arrStationRows.clear();

	Years.clear();
	Values.clear();

	// -------------------------------------------------------------
	// 2. Execute SQL
	// -------------------------------------------------------------
	CSmartArray<CSmartArray<CString>> arrRawRows;
	CString csSQL = SQL;
	ClimateDatabase->ExecuteTable(csSQL, arrRawRows);

	// -------------------------------------------------------------
	// 3. Branch based on Subtype
	// -------------------------------------------------------------
	CString csSubtype = Subtype;

	if 
	(
		csSubtype == L"Maximum" ||
		csSubtype == L"Minimum" ||
		csSubtype == L"Average"
	)
	{
		ConvertTemperatureRows(arrRawRows);
		FormatTemperatureText();
		FormatTemperatureCSV();
		return;
	}

	if (csSubtype == L"Threshold")
	{
		ConvertThresholdRows(arrRawRows);
		FormatThresholdText();
		FormatThresholdCSV();
		return;
	}

	if (csSubtype == L"Stations")
	{
		ConvertStationRows(arrRawRows);
		FormatStationText();
		FormatStationCSV();
		return;
	}

} // ExecutePickerQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnEditDelete()
{
	CClimateExplorerView* pView = ClimateExplorerView;

	// if there is no selection, then delete the current page
	if (!Selection)
	{
		UINT uiPage = Page - 1;
		m_arrPages.remove(uiPage);
		UINT uiPages = (UINT)m_arrPages.Count;
		Pages = uiPages;

		int nDelta = 1;
		for (; uiPage < uiPages; uiPage++)
		{
			shared_ptr<CPage> pPage = m_arrPages.get(uiPage);
			UINT uiPage = pPage->Page;
			pPage->Page = uiPage - nDelta;
		}

		pView->Invalidate();

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CPropertiesWnd* pProperties = pFrame->PropertiesPane;
		pProperties->UpdateTableOfContents();
		return;
	}

	pair < pair<int, int>, pair<int, int> >& pairSel = SelectedPairs;
	pair<int, int> pairStart = pairSel.first;
	pair<int, int> pairEnd = pairSel.second;

	// the first page number of the selection where page numbers are 
	// one based as they appear in the document (1..n)
	int nPageStart = pairStart.first;

	// page array index is zero based (0..n-1)
	int nStartIndex = nPageStart - 1;

	// the last page number of the selection which can be the same as the first
	int nPageEnd = pairEnd.first;
	int nEndIndex = nPageEnd - 1;

	// the first plot on the first page is zero based and depending on the layout,
	// there can be up to four plots on a page (0..3). If zero, the whole page
	// is selected.
	int nPlotStart = pairStart.second;

	// the last plot on the last page which can be the same as the first. If the 
	// plot is the last plot on a different page number than the first plot, 
	// the whole page is selected. If the Start and End pages are the same, the 
	// entire page is only selected if all of the plots are selected. 
	// 
	// For example: 
	//   1.	If there are four plots (0..3), but the first plot is 1 and the last plot 
	//		is 2, then the whole page is not selected.
	//   2.	Different start and end and the plot on the end is not the last plot, the
	//		whole page is not selected.
	//   3.	Different start and end and the plot on the start is not the first, the
	//		whole first page is not selected.
	int nPlotEnd = pairEnd.second;

	// number of selected pages
	int nPages = SelectedPages;

	bool bDeleteFirstPage = false;
	bool bDeleteLastPage = false;

	// the following can be duplications if start and end are the same page
	shared_ptr<CPage> pageStart = m_arrPages.get(nStartIndex);
	shared_ptr<CPage> pageEnd = m_arrPages.get(nEndIndex);
	int nStartPlots = pageStart->ImageCount;
	int nEndPlots = pageEnd->ImageCount;

	int nFirstDeletedPage = nStartIndex;
	int nLastDeletedPage = nEndIndex;
	bool bDeletePages = false;
	bool bDeleteStartPlots = false;
	bool bDeleteEndPlots = false;
	if (nPages > 1)
	{
		bDeleteFirstPage = nPlotStart == 0;
		bDeleteLastPage = nPlotEnd == nEndPlots - 1;
		if (!bDeleteLastPage)
		{
			nLastDeletedPage--;
			bDeleteEndPlots = true;
			CKeyedCollection<CString, CGraphPlotter> pPlots;
			int nPlot = 0;
			for (auto& pPlot : pageEnd->Plots.Items)
			{
				if (nPlot++ > nPlotEnd)
				{
					pPlots.add(pPlot.first, pPlot.second);
				}
			}
			pageEnd->Plots = pPlots;
		}
		if (!bDeleteFirstPage)
		{
			nFirstDeletedPage++;
			bDeleteStartPlots = true;
			CKeyedCollection<CString, CGraphPlotter> pPlots;
			int nPlot = 0;
			for (auto& pPlot : pageStart->Plots.Items)
			{
				if (nPlot++ < nPlotStart)
				{
					pPlots.add(pPlot.first, pPlot.second);
				}
			}
			pageStart->Plots = pPlots;
		}
		if (nFirstDeletedPage <= nLastDeletedPage)
		{
			bDeletePages = true;
		}
	}
	else // single page
	{
		bDeleteFirstPage = nPlotStart == 0 && nPlotEnd == nEndPlots - 1;
		if (bDeleteFirstPage)
		{
			bDeletePages = true;
		}
		else
		{
			bDeleteStartPlots = true;
			CKeyedCollection<CString, CGraphPlotter> pPlots;
			int nPlot = 0;
			for (auto& pPlot : pageStart->Plots.Items)
			{
				if (nPlot < nPlotStart || nPlot > nPlotEnd)
				{
					pPlots.add(pPlot.first, pPlot.second);
				}
				nPlot++;
			}
			pageStart->Plots = pPlots;
		}
	}

	if (bDeletePages)
	{
		const double dPageHeight = HeightOfPage;
		const double dTopOfPage = pView->TopOfView;

		long lPage = long(nStartIndex);
		Page = lPage + 1;
		long lPages = m_arrPages.Count;
		for (int nPage = nLastDeletedPage; nPage >= nFirstDeletedPage; nPage--)
		{
			m_arrPages.remove(nPage);
			lPages--;
			Pages = lPages;
		}

		if (lPage == lPages - 1)
		{
			Page = lPage + 1;
			const double dTop = dPageHeight * lPage;
			pView->TopOfView = dTop;
			pView->SetupScrollBars();
			pView->Invalidate();
		} 
		else
		{
			int nDelta = nLastDeletedPage - nFirstDeletedPage + 1;
			for (; lPage < lPages; lPage++)
			{
				shared_ptr<CPage> pPage = m_arrPages.get(lPage);
				UINT uiPage = pPage->Page;
				pPage->Page = uiPage - nDelta;
			}
		}
	}

	// shift images if necessary
	long lPages = Pages;
	ASSERT(lPages == m_arrPages.Count);
	long lPage = 0;
	for (auto& pPage : m_arrPages.Items)
	{
		if (lPage == lPages - 1)
		{
			break;
		}
		if (pPage->PageIsFull)
		{
			lPage++;
			continue;
		}
		
		UINT uiImages = pPage->ImageCount;
		UINT uiMax = pPage->MaximumImages;
		UINT uiDelta = uiMax - uiImages;

		long lNext = lPage + 1;
		shared_ptr<CPage> pNext = m_arrPages.get(lNext);
		vector<CString> arrKeys;
		UINT uiNext = pNext->ImageCount;

		for (auto& Plot : pNext->Plots.Items)
		{
			CString csKey = Plot.first;
			shared_ptr<CGraphPlotter> pGraph = Plot.second;
			pPage->Plots.add(csKey, pGraph);
			//pNext->Plots.remove(csKey);
			arrKeys.push_back(csKey);

			uiDelta--;
			if (uiDelta == 0)
			{
				break;
			}
			if (--uiNext == 0)
			{
				break;
			}
		}

		// remove the plots we shifted up to the previous page
		for (auto& csKey : arrKeys)
		{
			pNext->Plots.remove(csKey);
		}
		lPage++;
	}

	// if there are no images on the last page and that
	// page is a graph page, then delete it
	shared_ptr<CPage> pPage = m_arrPages.get(lPages - 1);
	long lImages = pPage->ImageCount;
	if (lImages == 0 && pPage->PageType == CPage::pageGraph)
	{
		m_arrPages.remove(lPages - 1);
		Pages = m_arrPages.Count;
	}

	UINT uiPage = Page;
	UINT uiPages = Pages;
	if (uiPage >= uiPages)
	{
		uiPage = uiPages - 1;
		Page = uiPage;
	}

	// deselect
	SelectLimit[-1] = -1;

	double dPageHeight = HeightOfPage;
	double dTop = dPageHeight * (nPages - 1);
	pView->TopOfView = dTop;
	pView->SetupScrollBars();
	pView->Invalidate();

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	pProperties->UpdateTableOfContents();

} // OnEditDelete

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);

	// if there is a selection, then enable delete
	if (Selection)
	{
		pCmdUI->Enable();
	}
	else
	{
		CClimateExplorerView* pView = ClimateExplorerView;
		const double dPageHeight = HeightOfPage;
		const double dTopOfPage = pView->TopOfView;
		double dPage = dTopOfPage / dPageHeight;
		UINT uiPage = UINT(dPage) + 1;
		Page = uiPage;
		UINT uiPagesTOC = TableOfContentsPages;
		if (uiPage > uiPagesTOC + 1)
		{
			pCmdUI->Enable();
		}
	}

} // OnUpdateEditDelete

/////////////////////////////////////////////////////////////////////////////
