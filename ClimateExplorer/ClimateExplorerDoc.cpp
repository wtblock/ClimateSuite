/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ClimateExplorer.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"
#include "MainFrm.h"
#include <propkey.h>
#include <xmllite.h>
#include <set>

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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerDoc::CClimateExplorerDoc()
{
	HeightOfPage = 11.0;
	WidthOfPage = 8.5;

	Pages = 1;
	Page = 1;
	Images = 0;
	ExportDPI = 400;
	ExportQuality = 75;

	CClimateExplorerApp* pApp = (CClimateExplorerApp*)AfxGetApp();
	if (pApp)
	{
		Publisher = pApp->Publisher;
		Copyright = pApp->Copyright;
		Description = pApp->DatabaseCredits;
	}

	QueryType = L"Picker";
	NaturalLanguage = L"None";
	Units = L"degF";
	Pure = true;
	Active = true;
	Scope = L"National";
	State = L"None";
	Location = L"None";
	YearStart = 1900;
	YearEnd = 2025;
	MeasurementText = L"Maximum";
	ThresholdText = L"90";
	Output = L"Plot";
	Layout = L"Full";

} // CClimateExplorerDoc

/////////////////////////////////////////////////////////////////////////////
CClimateExplorerDoc::~CClimateExplorerDoc()
{
}

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
vector<pair<CString, int>>& CClimateExplorerDoc::GetAlbumTableOfContents()
{
	m_arrTOC.clear();
	pair<CString, int> item;

	// account for the title page
	item.first = L"Title Page";
	item.second = 1;
	m_arrTOC.push_back(item);

	// account for the title page
	item.first = L"Table of Contents";
	item.second = 2;
	m_arrTOC.push_back(item);

	CString csCurrentAlbum;
	for (auto& node : m_arrPages.Items)
	{
		CString csFolder = node->Folder;
		CString csAlbum = CHelper::GetDataName(csFolder);
		if (csAlbum != csCurrentAlbum)
		{
			csCurrentAlbum = csAlbum;
			int nPage = node->Page;
			item.first = csAlbum;
			item.second = nPage;
			m_arrTOC.push_back(item);
		}
	}

	return m_arrTOC;

} // GetAlbumTableOfContents

/////////////////////////////////////////////////////////////////////////////
shared_ptr<Image> CClimateExplorerDoc::FindImage
(
	CString csFolder, CString csImage
)
{
	shared_ptr<Image> value;
	shared_ptr<MAP_IMAGES> pImages = m_mapAlbums.find(csFolder);
	if (pImages != nullptr)
	{
		value = pImages->find(csImage);
	}
	return value;
} // FindImage

/////////////////////////////////////////////////////////////////////////////
// CClimateExplorerDoc serialization
void CClimateExplorerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

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
	// TODO: Add your command handler code here

} // OnExecuteQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnUpdateExecuteQuery(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

} // OnUpdateExecuteQuery

/////////////////////////////////////////////////////////////////////////////
