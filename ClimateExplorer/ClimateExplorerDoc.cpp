/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ClimateExplorer.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"
#include "ClimateStation.h"
#include "ImagePlus.h"
#include "ThumbnailDialog.h"
#include "MainFrm.h"
#include <propkey.h>
#include <set>
#include "ZipWriter.h"
#include "ZipReader.h"
#include "PageGraph.h"

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
	Map = 1000;
	TopMargin = 0.5;
	BottomMargin = 0.5;
	InsideMargin = 0.6;
	OutsideMargin = 0.4;
	Gutter = 0.2;

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
	Placement = L"Append";

	// -------------------------------------------------------------
	// Plot Text
	// -------------------------------------------------------------
	GraphTitle = L"Title";
	AxisLabelX = L"Year";
	AxisLabelY = L"Value";

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	LineColor = L"DarkRed";
	LineStyle = Gdiplus::DashStyleDot;
	LineThicknessInches = 0.015; // ~6 px at 400 DPI

	// -------------------------------------------------------------
	// Running Average line appearance
	// -------------------------------------------------------------
	RunningAvgColor = L"Red";
	RunningAvgStyle = Gdiplus::DashStyleSolid;
	RunningAvgThicknessInches = 0.03; // ~12 px at 400 DPI

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	GridColor = L"Silver";
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

	// -------------------------------------------------------------
	// Trend Curve appearance
	// -------------------------------------------------------------
	TrendOneEnable = true;
	TrendOneColor = L"Gold";
	TrendOneStyle = Gdiplus::DashStyleDashDotDot;
	TrendOneThickness = 0.03;
	TrendOneYear = 1900;

	TrendTwoEnable = true;
	TrendTwoColor = L"Orange";
	TrendTwoStyle = Gdiplus::DashStyleDashDotDot;
	TrendTwoThickness = 0.03;
	TrendTwoYear = 1950;

	TrendThreeEnable = true;
	TrendThreeColor = L"Olive";
	TrendThreeStyle = Gdiplus::DashStyleDashDotDot;
	TrendThreeThickness = 0.03;
	TrendThreeYear = 2000;

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
BOOL CClimateExplorerDoc::SaveCEx(CString& csPath)
{
	HRESULT hr = S_OK;
	CComPtr<IXmlWriter> pWriter;
	CComPtr<IStream> pStream;

	// Create file-backed stream directly
	hr = SHCreateStreamOnFileEx
	(
		csPath,
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,
		FILE_ATTRIBUTE_NORMAL,
		TRUE,      // fCreate
		nullptr,
		&pStream
	);
	if (FAILED(hr)) return FALSE;


	// Create XML writer
	hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, nullptr);
	if (FAILED(hr)) return FALSE;

	hr = pWriter->SetOutput(pStream);
	if (FAILED(hr)) return FALSE;

	pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);
	pWriter->WriteStartDocument(XmlStandalone_Omit);

	// <ClimateExplorer>
	pWriter->WriteStartElement(nullptr, L"ClimateExplorer", nullptr);

	pWriter->WriteStartElement(nullptr, L"Document", nullptr);

	WriteHeaderElement(pWriter, L"PageCount", (int)Pages);
	WriteHeaderElement(pWriter, L"Title", Title);
	WriteHeaderElement(pWriter, L"Subtitle", Subtitle);
	WriteHeaderElement(pWriter, L"Publisher", Publisher);
	WriteHeaderElement(pWriter, L"ISBN", ISBN);
	WriteHeaderElement(pWriter, L"Copyright", Copyright);
	WriteHeaderElement(pWriter, L"Description", Description);
	WriteHeaderElement(pWriter, L"ExportFolder", ExportFolder);
	WriteHeaderElement(pWriter, L"ExportPages", ExportPages);
	WriteHeaderElement(pWriter, L"ExportDPI", (int)ExportDPI);
	WriteHeaderElement(pWriter, L"ExportQuality", (int)ExportQuality);
	WriteHeaderElement(pWriter, L"WidthOfPage", WidthOfPage);
	WriteHeaderElement(pWriter, L"HeightOfPage", HeightOfPage);
	WriteHeaderElement(pWriter, L"LogicalDPI", (int)Map);
	WriteHeaderElement(pWriter, L"TopMargin", TopMargin);
	WriteHeaderElement(pWriter, L"BottomMargin", BottomMargin);
	WriteHeaderElement(pWriter, L"InsideMargin", InsideMargin);
	WriteHeaderElement(pWriter, L"OutsideMargin", OutsideMargin);
	WriteHeaderElement(pWriter, L"Gutter", Gutter);

	pWriter->WriteEndElement(); // </Document>

	// <PageEx>
	pWriter->WriteStartElement(nullptr, L"PageEx", nullptr);

	// Iterate CSmartArray<CPage>
	for (auto& page : m_arrPages.Items)
	{
		pWriter->WriteStartElement(nullptr, L"Page", nullptr);

		// number
		CString csNum;
		csNum.Format(L"%u", page->Page);
		pWriter->WriteAttributeString(nullptr, L"number", nullptr, csNum);

		// type
		CString csType;
		switch (page->PageType)
		{
		case CPage::pageCover: csType = L"Cover"; break;
		case CPage::pageTOC:   csType = L"TOC";   break;
		case CPage::pageGraph: csType = L"Graph"; break;
		default:               csType = L"Graph"; break;
		}
		pWriter->WriteAttributeString(nullptr, L"type", nullptr, csType);

		// layout
		pWriter->WriteAttributeString(nullptr, L"layout", nullptr, page->Layout);

		// each plot → <Graph>
		auto& plots = page->Plots.Items;
		for (auto& kv : plots)
		{
			auto pPlot = kv.second;
			CPageGraph wrapper(pPlot, this);
			wrapper.WriteXml(pWriter);
		}

		pWriter->WriteEndElement(); // </Page>
	}

	pWriter->WriteEndElement(); // </PageEx>
	pWriter->WriteEndElement(); // </ClimateExplorer>
	pWriter->WriteEndDocument();

	// SHCreateStreamOnFileEx flushes on release
	return TRUE;
} // SaveCEx

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::SaveCE(const CString& csPath)
{
	BOOL value = FALSE;
	m_arrCEPNGs.clear();

	// -------------------------------------------------------------
	// 1. Create the output stream
	// -------------------------------------------------------------
	IStream* pStream = nullptr;
	HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);

	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create CE file stream.");
		return FALSE;
	}

	// -------------------------------------------------------------
	// 2. Create the XmlLite writer
	// -------------------------------------------------------------
	IXmlWriter* pWriter = nullptr;
	hr = CreateXmlWriter(__uuidof(IXmlWriter),
		reinterpret_cast<void**>(&pWriter), nullptr);

	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create CE XML writer.");
		pStream->Release();
		return FALSE;
	}

	pWriter->SetOutput(pStream);
	pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

	// Helper lambda for CE properties (trimmed set)
	auto WriteCEProp = [&](LPCWSTR name, const CString& val)
	{
		pWriter->WriteStartElement(nullptr, name, nullptr);
		pWriter->WriteAttributeString(nullptr, L"value", nullptr, val);
		pWriter->WriteEndElement();
	};

	// -------------------------------------------------------------
	// 3. Begin CE XML document
	// -------------------------------------------------------------
	pWriter->WriteStartDocument(XmlStandalone_Yes);
	pWriter->WriteStartElement(nullptr, L"ClimateExplorer", nullptr);

	pWriter->WriteStartElement(nullptr, L"Document", nullptr);

	WriteHeaderElement(pWriter, L"PageCount", (int)Pages);
	WriteHeaderElement(pWriter, L"Title", Title);
	WriteHeaderElement(pWriter, L"Subtitle", Subtitle);
	WriteHeaderElement(pWriter, L"Publisher", Publisher);
	WriteHeaderElement(pWriter, L"ISBN", ISBN);
	WriteHeaderElement(pWriter, L"Copyright", Copyright);
	WriteHeaderElement(pWriter, L"Description", Description);
	WriteHeaderElement(pWriter, L"ExportFolder", ExportFolder);
	WriteHeaderElement(pWriter, L"ExportPages", ExportPages);
	WriteHeaderElement(pWriter, L"ExportDPI", (int)ExportDPI);
	WriteHeaderElement(pWriter, L"ExportQuality", (int)ExportQuality);
	WriteHeaderElement(pWriter, L"WidthOfPage", WidthOfPage);
	WriteHeaderElement(pWriter, L"HeightOfPage", HeightOfPage);
	WriteHeaderElement(pWriter, L"LogicalDPI", (int)Map);
	WriteHeaderElement(pWriter, L"TopMargin", TopMargin);
	WriteHeaderElement(pWriter, L"BottomMargin", BottomMargin);
	WriteHeaderElement(pWriter, L"InsideMargin", InsideMargin);
	WriteHeaderElement(pWriter, L"OutsideMargin", OutsideMargin);
	WriteHeaderElement(pWriter, L"Gutter", Gutter);

	pWriter->WriteEndElement(); // </Document>

	// <PageEx>
	pWriter->WriteStartElement(nullptr, L"PageEx", nullptr);

	for (auto& pPage : m_arrPages.Items)
	{
		pWriter->WriteStartElement(nullptr, L"Page", nullptr);

		CString csPageNum; csPageNum.Format(L"%u", pPage->Page);
		CString csType;

		switch (pPage->PageType)
		{
		case CPage::pageCover: csType = L"Cover"; break;
		case CPage::pageTOC:   csType = L"TOC"; break;
		case CPage::pageGraph: csType = L"Graph"; break;
		}

		pWriter->WriteAttributeString(nullptr, L"number", nullptr, csPageNum);
		pWriter->WriteAttributeString(nullptr, L"type", nullptr, csType);
		pWriter->WriteAttributeString(nullptr, L"layout", nullptr, pPage->Layout);

		UINT plotIndex = 0;
		// each plot → <Graph>
		auto& plots = pPage->Plots.Items;
		for (auto& kv : plots)
		{
			auto pPlot = kv.second;
			CPageGraph wrapper(pPlot, this);
			wrapper.WriteXml(pWriter);

			// -------------------------------------------------------------
			// Render PNG for this plot and write filename
			// -------------------------------------------------------------

			// Convert logical → pixel rectangle (already rotated to landscape)
			CRect rcPixels = ImageRectangle;

			// Render PNG bytes
			std::vector<BYTE> pngBytes;
			RenderPlotToPNG(pPlot, rcPixels, pngBytes);

			// 5. Assign CE filename
			CString csFilename;
			csFilename.Format
			(
				L"Plots/Page_%04u_Plot_%02u.png",
				pPage->Page,
				plotIndex + 1
			);

			// 6. Write filename into CE XML
			WriteCEProp(L"Image", csFilename);

			// 7. Store PNG bytes for ZIP packaging (Step 5)
			CEPNG item;
			item.filename = csFilename;
			item.bytes = std::move(pngBytes);
			m_arrCEPNGs.push_back(std::move(item));

			plotIndex++;
		}

		pWriter->WriteEndElement(); // </Page>
	}

	pWriter->WriteEndElement(); // </PageEx>
	pWriter->WriteEndElement(); // </ClimateExplorer>
	pWriter->WriteEndDocument();

	// -------------------------------------------------------------
	// 7. Cleanup
	// -------------------------------------------------------------
	pWriter->Release();

	// -------------------------------------------------------------
	// Extract XML bytes from the memory stream
	// -------------------------------------------------------------
	STATSTG stat;
	pStream->Stat(&stat, STATFLAG_NONAME);

	ULONG xmlSize = (ULONG)stat.cbSize.QuadPart;
	std::vector<BYTE> xmlBytes(xmlSize);

	LARGE_INTEGER liZero = {};
	pStream->Seek(liZero, STREAM_SEEK_SET, NULL);

	ULONG bytesRead = 0;
	pStream->Read(xmlBytes.data(), xmlSize, &bytesRead);

	// -------------------------------------------------------------
	// Create CE ZIP file using CZipWriter
	// -------------------------------------------------------------
	CZipWriter zip;
	if (!zip.Create(csPath))
	{
		pStream->Release();
		return FALSE;
	}

	// Add XML file
	zip.AddFile(L"ClimateExplorer.xml", xmlBytes.data(), xmlBytes.size());

	// Add PNGs
	for (const auto& item : m_arrCEPNGs)
	{
		zip.AddFile(item.filename, item.bytes.data(), item.bytes.size());
	}

	// Finalize ZIP
	zip.Close();

	// Cleanup
	pStream->Release();

	value = TRUE;
	return value;
} // SaveCE

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::OnSaveDocument(CString& csPath)
{
	BOOL value = FALSE;

	CString csExt = CHelper::GetExtension(csPath);
	csExt.MakeLower();

	if (csExt == L".cex")
	{
		value = SaveCEx(csPath);
	}
	else if (csExt == L".ce")
	{
		value = SaveCE(csPath);
	}
	else
	{
		AfxMessageBox(L"Unknown file type.");
	}

	SetModifiedFlag(FALSE);

	return value;
} // OnSaveDocument

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::PromptForFileName(CString& strFilePath)
{
	const CString csFilter =
		_T("All Possible|*.CEx;*.CE|")
		_T("Climate Explorer XML (*.CEx)|*.CEx|")
		_T("Climate Explorer (*.CE)|*.CE|")
		_T("All Files (*.*)|*.*||");

	CFileDialog fileDlg
	(
		FALSE, L"CE", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		csFilter
	);

	if (fileDlg.DoModal() == IDOK)
	{
		strFilePath = fileDlg.GetPathName();
		return TRUE; // User selected a valid file name
	}

	return FALSE; // User canceled
} // PromptForFileName

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
		AdjustForTOC();
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
void CClimateExplorerDoc::CopyPlotPropertiesToDocument(CGraphPlotter* pPlot)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	Pure = pPlot->Pure;
	Scope = pPlot->Scope;
	YearStart = pPlot->YearStart;
	YearEnd = pPlot->YearEnd;
	Subtype = pPlot->Subtype;
	Threshold = pPlot->Threshold;
	Units = pPlot->Units;
	Output = pPlot->Output;
	State = pPlot->State;
	Location = pPlot->Location;

	// -------------------------------------------------------------
	// Appearance: Titles and labels
	// -------------------------------------------------------------
	GraphTitle = pPlot->GraphTitle;
	AxisLabelX = pPlot->AxisLabelX;
	AxisLabelY = pPlot->AxisLabelY;

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	LineColor = pPlot->LineColor;
	LineStyle = pPlot->LineStyle;
	LineThicknessInches = pPlot->LineThicknessInches;

	// -------------------------------------------------------------
	// Running Average line appearance
	// -------------------------------------------------------------
	RunningAvgColor = pPlot->RunningAvgColor;
	RunningAvgStyle = pPlot->RunningAvgStyle;
	RunningAvgThicknessInches = pPlot->RunningAvgThicknessInches;

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	GridColor = pPlot->GridColor;
	GridLineStyle = pPlot->GridLineStyle;
	GridLineThicknessInches = pPlot->GridLineThicknessInches;

	// -------------------------------------------------------------
	// Font sizes
	// -------------------------------------------------------------
	TitleFontSizePoints = pPlot->TitleFontSizePoints;
	AxisLabelFontSizePoints = pPlot->AxisLabelFontSizePoints;
	TickLabelFontSizePoints = pPlot->TickLabelFontSizePoints;

	// -------------------------------------------------------------
	// Padding
	// -------------------------------------------------------------
	RightPaddingInches = pPlot->RightPaddingInches;
	BottomPaddingInches = pPlot->BottomPaddingInches;

	// -------------------------------------------------------------
	// Tick length
	// -------------------------------------------------------------
	TickLengthInches = pPlot->TickLengthInches;

	// -------------------------------------------------------------
	// Layout
	// -------------------------------------------------------------
	Layout = pPlot->Layout;

	TrendOneEnable = pPlot->TrendOneEnable;
	TrendOneColor = pPlot->TrendOneColor;
	TrendOneStyle = pPlot->TrendOneStyle;
	TrendOneThickness = pPlot->TrendOneThickness;
	TrendOneYear = pPlot->TrendOneYear;

	TrendTwoEnable = pPlot->TrendTwoEnable;
	TrendTwoColor = pPlot->TrendTwoColor;
	TrendTwoStyle =pPlot->TrendTwoStyle;
	TrendTwoThickness = pPlot->TrendTwoThickness;
	TrendTwoYear = pPlot->TrendTwoYear;

	TrendThreeEnable = pPlot->TrendThreeEnable;
	TrendThreeColor = pPlot->TrendThreeColor;
	TrendThreeStyle = pPlot->TrendThreeStyle;
	TrendThreeThickness = pPlot->TrendThreeThickness;
	TrendThreeYear = pPlot->TrendThreeYear;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	pProperties->UpdatePropertiesFromDocument(this);

} // CopyPlotPropertiesToDocument

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::LoadCE(const CString& csPath)
{
	BOOL value = FALSE;

	// -------------------------------------------------------------
	// 1. Open CE ZIP file
	// -------------------------------------------------------------
	CZipReader zip;
	if (!zip.Open(csPath))
	{
		AfxMessageBox(L"Failed to open CE file.");
		return value;
	}

	// -------------------------------------------------------------
	// 2. Extract ClimateExplorer.xml
	// -------------------------------------------------------------
	std::vector<uint8_t> xmlBytes;
	if (!zip.ExtractFile(L"ClimateExplorer.xml", xmlBytes))
	{
		AfxMessageBox(L"CE file missing ClimateExplorer.xml.");
		zip.Close();
		return value;
	}

	// Create stream from XML bytes
	CComPtr<IStream> pStream;
	HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	if (FAILED(hr))
	{
		zip.Close();
		return value;
	}

	ULONG written = 0;
	pStream->Write(xmlBytes.data(), (ULONG)xmlBytes.size(), &written);

	LARGE_INTEGER liZero = {};
	pStream->Seek(liZero, STREAM_SEEK_SET, NULL);

	// -------------------------------------------------------------
	// 3. Create XmlLite reader
	// -------------------------------------------------------------
	CComPtr<IXmlReader> pReader;
	hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, nullptr);
	if (FAILED(hr))
	{
		zip.Close();
		return value;
	}

	hr = pReader->SetInput(pStream);
	if (FAILED(hr))
	{
		zip.Close();
		return value;
	}

	XmlNodeType nodeType = XmlNodeType_None;
	const WCHAR* name = nullptr;
	shared_ptr<CGraphPlotter> pPlot;

	// -------------------------------------------------------------
	// 5. XML reading loop
	// -------------------------------------------------------------
	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_Element)
		{
			pReader->GetLocalName(&name, nullptr);
			if (!name) continue;

			// <Document>
			if (wcscmp(name, L"Document") == 0)
			{
				while (pReader->Read(&nodeType) == S_OK)
				{
					if (nodeType == XmlNodeType_EndElement)
					{
						const WCHAR* endName = nullptr;
						pReader->GetLocalName(&endName, nullptr);
						if (endName && wcscmp(endName, L"Document") == 0)
							break;     // finished reading header
					}

					if (nodeType != XmlNodeType_Element)
						continue;

					const WCHAR* propName = nullptr;
					pReader->GetLocalName(&propName, nullptr);
					if (!propName)
						continue;

					// Read attributes: value + type
					const WCHAR* attrName = nullptr;
					const WCHAR* attrValue = nullptr;
					CString csValue;
					CString csType;

					while (pReader->MoveToNextAttribute() == S_OK)
					{
						pReader->GetLocalName(&attrName, nullptr);
						pReader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"value") == 0)
							csValue = attrValue;
						else if (wcscmp(attrName, L"type") == 0)
							csType = attrValue;
					}

					// Assign based on element name
					if (wcscmp(propName, L"PageCount") == 0)
						Pages = _wtoi(csValue);

					else if (wcscmp(propName, L"Title") == 0)
						Title = csValue;

					else if (wcscmp(propName, L"Subtitle") == 0)
						Subtitle = csValue;

					else if (wcscmp(propName, L"Publisher") == 0)
						Publisher = csValue;

					else if (wcscmp(propName, L"ISBN") == 0)
						ISBN = csValue;

					else if (wcscmp(propName, L"Copyright") == 0)
						Copyright = csValue;

					else if (wcscmp(propName, L"Description") == 0)
						Description = csValue;

					else if (wcscmp(propName, L"ExportFolder") == 0)
						ExportFolder = csValue;

					else if (wcscmp(propName, L"ExportPages") == 0)
						ExportPages = csValue;

					else if (wcscmp(propName, L"ExportDPI") == 0)
						ExportDPI = _wtoi(csValue);

					else if (wcscmp(propName, L"ExportQuality") == 0)
						ExportQuality = _wtoi(csValue);

					else if (wcscmp(propName, L"WidthOfPage") == 0)
						WidthOfPage = _wtof(csValue);

					else if (wcscmp(propName, L"HeightOfPage") == 0)
						HeightOfPage = _wtof(csValue);

					else if (wcscmp(propName, L"LogicalDPI") == 0)
						Map = _wtoi(csValue);

					else if (wcscmp(propName, L"TopMargin") == 0)
						TopMargin = _wtof(csValue);

					else if (wcscmp(propName, L"BottomMargin") == 0)
						BottomMargin = _wtof(csValue);

					else if (wcscmp(propName, L"InsideMargin") == 0)
						InsideMargin = _wtof(csValue);

					else if (wcscmp(propName, L"OutsideMargin") == 0)
						OutsideMargin = _wtof(csValue);

					else if (wcscmp(propName, L"Gutter") == 0)
						Gutter = _wtof(csValue);
				}

				continue;   // finished <Document>, return to main loop
			}

			// <PageEx>
			if (wcscmp(name, L"PageEx") == 0)
			{
				continue;   // descend into PageEx and keep reading
			}

			// <Page>
			if (wcscmp(name, L"Page") == 0)
			{
				CString csLayout;
				CString csType;
				CPage::PAGE_TYPE eType = CPage::pageGraph;

				const WCHAR* attrName = nullptr;
				const WCHAR* attrValue = nullptr;

				while (pReader->MoveToNextAttribute() == S_OK)
				{
					pReader->GetLocalName(&attrName, nullptr);
					pReader->GetValue(&attrValue, nullptr);

					if (wcscmp(attrName, L"number") == 0)
					{
						int nPage = _wtoi(attrValue);
					}
					else if (wcscmp(attrName, L"type") == 0)
					{
						if (wcscmp(attrValue, L"Cover") == 0)
						{
							csType = attrValue;
						}
						else if (wcscmp(attrValue, L"TOC") == 0)
						{
							csType = attrValue;
						}
						else
						{
							eType = CPage::pageGraph;
							csType = attrValue;
						}
					}
					else if (wcscmp(attrName, L"layout") == 0)
					{
						csLayout = attrValue;
					}
				}

				// Ignore Cover and TOC pages (constructor already created them)
				if (csType == L"Cover" || csType == L"TOC")
				{
					continue;
				}

				continue;
			}

			// <Graph>
			if (wcscmp(name, L"Graph") == 0)
			{
				// Create a real plot object
				pPlot = make_shared<CGraphPlotter>(this);

				// Wrap it in CPageGraph – m_pPlot is now valid
				CPageGraph wrapper(pPlot, this);

				// Let CPageGraph read XML into pPlot 
				// (picker + appearance + metadata)
				wrapper.ReadXml(pReader);

				// the following commented out code is the 
				// time consuming part that is replaced
				// by the next section labeled <Image>
				// where the image is stored in the zip file.
				// 
				//// Build SQL from picker properties
				//CString csSQL = pPlot->BuildPickerSQL();

				//// Store SQL in the plot
				//pPlot->SQL = csSQL;

				//// execute the query to build the table of values
				//pPlot->ExecutePickerQuery();

				//// restore some data gathered from the query
				//pPlot->GetDefaults(this);

				Pages = (UINT)m_arrPages.Count;
				int nPages = Pages;

				// page index is zero based
				int nPage = nPages - 1;

				shared_ptr<CPage> pPage = m_arrPages.get(nPage);

				// critical: reorganzes rectangle and margins
				pPage->Page = nPages;

				if (pPage->PageIsFull)
				{
					pPage = shared_ptr<CPage>
						(new CPage(nPages + 1, Layout, this, CPage::pageGraph));
					m_arrPages.append(pPage);
					Pages = (UINT)m_arrPages.Count;
					nPages = Pages;
					nPage = nPages - 1;

					// critical: reorganzes rectangle and margins
					pPage->Page = nPages;
				}

				Page = pPage->Page;

				// Add the plot to the current page
				pPage->AddAnImage(pPlot);

				continue;
			}

			// <Image>
			if (wcscmp(name, L"Image") == 0)
			{
				const WCHAR* attrName = nullptr;
				const WCHAR* attrValue = nullptr;

				CString csImagePath;

				// Read all attributes on <Image ... />
				while (pReader->MoveToNextAttribute() == S_OK)
				{
					pReader->GetLocalName(&attrName, nullptr);
					pReader->GetValue(&attrValue, nullptr);

					if (wcscmp(attrName, L"value") == 0)
					{
						csImagePath = attrValue;

						// Load PNG bytes from ZIP
						std::vector<uint8_t> pngBytes;
						if (zip.ExtractFile(csImagePath, pngBytes))
						{
							pPlot->BytesPNG = pngBytes;
						}

					}
				}
				continue;
			}
		}

		// End of <Page>
		if (nodeType == XmlNodeType_EndElement)
		{
			pReader->GetLocalName(&name, nullptr);
			CString csName(name);
		}
	}

	zip.Close();

	/////////////////////////////////////////////////////////////////////////////
	// New architecture (disabled for now)
	/////////////////////////////////////////////////////////////////////////////
	// {
	//     std::shared_ptr<CPage> pNewPage = std::make_shared<CPage>();
	//     pNewPage->ReadXml(pReader);
	//     m_arrPagesEx.push_back(pNewPage);
	// }

	return TRUE;
} // LoadCE

/////////////////////////////////////////////////////////////////////////////
// LoadCEx
//
// Loads a full‑fidelity Climate Explorer (.CEx) document.
// Restores all document and plot properties from XML,
// then regenerates the document using OnExecuteQuery().
//
// Pagination, page creation, and plot creation are handled
// entirely by OnExecuteQuery(), exactly as during live execution.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::LoadCEx(const CString& csPath)
{
	HRESULT hr = S_OK;
	CComPtr<IXmlReader> pReader;
	CComPtr<IStream> pStream;

	// Load file into stream
	hr = SHCreateStreamOnFileEx
	(
		csPath,
		STGM_READ | STGM_SHARE_DENY_WRITE,
		FILE_ATTRIBUTE_NORMAL,
		FALSE,
		nullptr,
		&pStream
	);

	if (FAILED(hr)) return FALSE;

	// Create XML reader
	hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, nullptr);
	if (FAILED(hr)) return FALSE;

	hr = pReader->SetInput(pStream);
	if (FAILED(hr)) return FALSE;

	XmlNodeType nodeType = XmlNodeType_None;
	const WCHAR* name = nullptr;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_Element)
		{
			pReader->GetLocalName(&name, nullptr);
			if (!name) continue;

			// <Document>
			if (wcscmp(name, L"Document") == 0)
			{
				while (pReader->Read(&nodeType) == S_OK)
				{
					if (nodeType == XmlNodeType_EndElement)
					{
						const WCHAR* endName = nullptr;
						pReader->GetLocalName(&endName, nullptr);
						if (endName && wcscmp(endName, L"Document") == 0)
							break;     // finished reading header
					}

					if (nodeType != XmlNodeType_Element)
						continue;

					const WCHAR* propName = nullptr;
					pReader->GetLocalName(&propName, nullptr);
					if (!propName)
						continue;

					// Read attributes: value + type
					const WCHAR* attrName = nullptr;
					const WCHAR* attrValue = nullptr;
					CString csValue;
					CString csType;

					while (pReader->MoveToNextAttribute() == S_OK)
					{
						pReader->GetLocalName(&attrName, nullptr);
						pReader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"value") == 0)
							csValue = attrValue;
						else if (wcscmp(attrName, L"type") == 0)
							csType = attrValue;
					}

					// Assign based on element name
					if (wcscmp(propName, L"PageCount") == 0)
						Pages = _wtoi(csValue);

					else if (wcscmp(propName, L"Title") == 0)
						Title = csValue;

					else if (wcscmp(propName, L"Subtitle") == 0)
						Subtitle = csValue;

					else if (wcscmp(propName, L"Publisher") == 0)
						Publisher = csValue;

					else if (wcscmp(propName, L"ISBN") == 0)
						ISBN = csValue;

					else if (wcscmp(propName, L"Copyright") == 0)
						Copyright = csValue;

					else if (wcscmp(propName, L"Description") == 0)
						Description = csValue;

					else if (wcscmp(propName, L"ExportFolder") == 0)
						ExportFolder = csValue;

					else if (wcscmp(propName, L"ExportPages") == 0)
						ExportPages = csValue;

					else if (wcscmp(propName, L"ExportDPI") == 0)
						ExportDPI = _wtoi(csValue);

					else if (wcscmp(propName, L"ExportQuality") == 0)
						ExportQuality = _wtoi(csValue);

					else if (wcscmp(propName, L"WidthOfPage") == 0)
						WidthOfPage = _wtof(csValue);

					else if (wcscmp(propName, L"HeightOfPage") == 0)
						HeightOfPage = _wtof(csValue);

					else if (wcscmp(propName, L"LogicalDPI") == 0)
						Map = _wtoi(csValue);

					else if (wcscmp(propName, L"TopMargin") == 0)
						TopMargin = _wtof(csValue);

					else if (wcscmp(propName, L"BottomMargin") == 0)
						BottomMargin = _wtof(csValue);

					else if (wcscmp(propName, L"InsideMargin") == 0)
						InsideMargin = _wtof(csValue);

					else if (wcscmp(propName, L"OutsideMargin") == 0)
						OutsideMargin = _wtof(csValue);

					else if (wcscmp(propName, L"Gutter") == 0)
						Gutter = _wtof(csValue);
				}

				continue;   // finished <Document>, return to main loop
			}

			// <PageEx>
			if (wcscmp(name, L"PageEx") == 0)
			{
				continue;   // descend into PageEx and keep reading
			}

			// <Page>
			if (wcscmp(name, L"Page") == 0)
			{
				CString csLayout;
				CString csType;
				CPage::PAGE_TYPE eType = CPage::pageGraph;

				const WCHAR* attrName = nullptr;
				const WCHAR* attrValue = nullptr;

				while (pReader->MoveToNextAttribute() == S_OK)
				{
					pReader->GetLocalName(&attrName, nullptr);
					pReader->GetValue(&attrValue, nullptr);

					if (wcscmp(attrName, L"number") == 0)
					{
						int nPage = _wtoi(attrValue);
					}
					else if (wcscmp(attrName, L"type") == 0)
					{
						if (wcscmp(attrValue, L"Cover") == 0)
						{
							csType = attrValue;
						}
						else if (wcscmp(attrValue, L"TOC") == 0)
						{
							csType = attrValue;
						}
						else
						{
							eType = CPage::pageGraph;
							csType = attrValue;
						}
					}
					else if (wcscmp(attrName, L"layout") == 0)
					{
						csLayout = attrValue;
					}
				}

				// Ignore Cover and TOC pages (constructor already created them)
				if (csType == L"Cover" || csType == L"TOC")
				{
					continue;
				}

				continue;
			}

			// <Graph>
			if (wcscmp(name, L"Graph") == 0)
			{
				// Create a real plot object
				auto pPlot = make_shared<CGraphPlotter>(this);

				// Wrap it in CPageGraph – m_pPlot is now valid
				CPageGraph wrapper(pPlot, this);

				// Let CPageGraph read XML into pPlot 
				// (picker + appearance + metadata)
				wrapper.ReadXml(pReader);

				// Build SQL from picker properties
				CString csSQL = pPlot->BuildPickerSQL();

				// Store SQL in the plot
				pPlot->SQL = csSQL;

				// execute the query to build the table of values
				pPlot->ExecutePickerQuery();

				// restore some data gathered from the query
				pPlot->GetDefaults(this);

				Pages = (UINT)m_arrPages.Count;
				int nPages = Pages;

				// page index is zero based
				int nPage = nPages - 1;

				shared_ptr<CPage> pPage = m_arrPages.get(nPage);

				// critical: reorganzes rectangle and margins
				pPage->Page = nPages;

				if (pPage->PageIsFull)
				{
					pPage = shared_ptr<CPage>
						(new CPage(nPages + 1, Layout, this, CPage::pageGraph));
					m_arrPages.append(pPage);
					Pages = (UINT)m_arrPages.Count;
					nPages = Pages;
					nPage = nPages - 1;

					// critical: reorganzes rectangle and margins
					pPage->Page = nPages;
				}

				Page = pPage->Page;

				// Add the plot to the current page
				pPage->AddAnImage(pPlot);

				continue;
			}
		}

		// End of <Page>
		if (nodeType == XmlNodeType_EndElement)
		{
			pReader->GetLocalName(&name, nullptr);
			CString csName(name);
		}
	}

	return TRUE;
} // LoadCEx

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CBaseDoc::OnOpenDocument(lpszPathName))
		return FALSE;

	CString csExt = CHelper::GetExtension(lpszPathName);
	csExt.MakeLower();

	BOOL value = FALSE;

	if (csExt == L".cex")
	{
		value = LoadCEx(lpszPathName); 
	}
	else if (csExt == L".ce")
	{
		value = LoadCE(lpszPathName);
	}
	else
	{
		AfxMessageBox(L"Unknown file type.");
		return FALSE;
	}

	if (value)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CPropertiesWnd* pProperties = pFrame->PropertiesPane;
		pProperties->UpdatePropertiesFromDocument(this);
		pProperties->PropList->ResetOriginalValues();

		SetPathName(lpszPathName, FALSE);

		CClimateExplorerView* pView = ClimateExplorerView;
		pView->Invalidate();
	}

	// -------------------------------------------------------------
	// Adjust TOC
	// -------------------------------------------------------------
	AdjustForTOC();

	// -------------------------------------------------------------
	// Mark document clean
	// -------------------------------------------------------------
	SetModifiedFlag(FALSE);

	// -------------------------------------------------------------
	// Notify views (required for full main menu)
	// -------------------------------------------------------------
	UpdateAllViews(nullptr);

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
void CClimateExplorerDoc::ExecuteQuery(bool bProgress/* = true*/)
{
	CClimateExplorerView* pView = ClimateExplorerView;
	if (pView == nullptr)
	{
		return;
	}

	// selection if any
	pair < pair<int, int>, pair<int, int> > pairSel = SelectedPairs;
	pair<int, int> pairFirst = pairSel.first;

	CString csPlacement = Placement;
	const bool bSingleSelection = SingleSelection;

	// opening a document sets bProgress to false and under those
	// conditions, we always want to append
	if (bProgress == false)
	{
		csPlacement = L"Append";
		Placement = csPlacement;
	}

	pView->Station = L"";
	pView->Latitude = 0.0f;
	pView->Longitude = 0.0f;

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

	// launch the progress dialog
	CThumbnailDialog dlg;

	// did the caller disable this?
	if (bProgress == true)
	{
		// prepare the progress dialog
		theApp.OnIdle(0);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		dlg.Parent = pFrame;
		dlg.CreateDlg();
		dlg.ShowWindow(SW_SHOW);
		CString csDialogTitle;
		csDialogTitle.Format(L"Rendering %s Locations", csScope);

		dlg.SetWindowText(csDialogTitle);
		dlg.Objects = L"Locations";

		dlg.TotalImages = (int)nLocations;
	}

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

		// did the caller disable this?
		if (bProgress == true)
		{
			// let the user cancel out
			if (dlg.Cancel)
			{
				bAbort = true;
				break;
			}

			// update the progress dialog's status
			dlg.CurrentImage = nLocation++;
		}

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

		CString csTitle, csState, csCity, csStation;
		float fLatitude = 0, fLongitude = 0;
		if (csScope == L"Location")
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
		}

		// -------------------------------------------------------------
		// Generate the graph bitmap
		// -------------------------------------------------------------
		shared_ptr<CGraphPlotter> pPlot =
			shared_ptr<CGraphPlotter>(new CGraphPlotter(this, Years, Values));

		csTitle = pPlot->GraphTitle;
		pPlot->Station = csStation;
		pPlot->Latitude = fLatitude;
		pPlot->Longitude = fLongitude;

		int nPages = Pages;
		
		// page index is zero based
		int nPage = nPages - 1;

		// replace a matching selection if it exists
		if (csPlacement == L"Replace" && bSingleSelection == true)
		{
			nPage = pairFirst.first - 1;
		}
		else if (csPlacement == L"Insert" && bSingleSelection == true)
		{
			nPage = pairFirst.first - 1;
			ShiftPlotsDown();
		}

		shared_ptr<CPage> pPage = m_arrPages.get(nPage);

		// critical: reorganzes rectangle and margins
		pPage->Page = nPage + 1;

		// a title cannot be duplicated on a page, so if it exists
		// on the page, replace it by first removing the existing
		// plot on the page
		if (pPage->Plots.Exists[csTitle])
		{
			pPage->Plots.remove(csTitle);
		}

		if (pPage->PageIsFull)
		{
			pPage = shared_ptr<CPage>
				(new CPage(nPages + 1, Layout, this, CPage::pageGraph));
			m_arrPages.append(pPage);
			Pages = (UINT)m_arrPages.Count;
			nPages = Pages;
			nPage = nPages - 1;
		}

		Page = pPage->Page;
		double dTop = dPageHeight * nPage;
		pView->TopOfView = dTop;
		pView->SetupScrollBars();
		pView->Invalidate();

		CRect rect = MarginRectangle;
		pPage->Rect = rect;

		pPage->AddAnImage(pPlot);

		// 5. Mark document modified (optional)
		SetModifiedFlag(TRUE);

		if (bProgress == true)
		{
			// wait ten milliseconds while letting normal 
			// window messaging to run
			pFrame->Wait(10);
		}

	}

	// did the caller disable this?
	if (bProgress == true && dlg.m_hWnd != nullptr)
	{
		// done with the progress dialog
		dlg.DestroyWindow();
	}

} // ExecuteQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnExecuteQuery()
{
	ExecuteQuery();
	AdjustForTOC();

} // OnExecuteQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnUpdateExecuteQuery(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	const bool bSingleSelection = SingleSelection;
	CString csPlacement = Placement;
	if (csPlacement == L"Append")
	{
		pCmdUI->Enable();
	}
	else // inserting or replacing require a single selection
	{
		if (bSingleSelection)
		{
			pCmdUI->Enable();
		}
	}

} // OnUpdateExecuteQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::AdjustForTOC()
{
	// if the table of contents grew, we need to add a page or pages
	// and update the page numbers of the graphs that follow
	UINT uiPagesTOC1 = ExistingPagesForTOC;
	UINT uiPagesTOC2 = TableOfContentsPages;
	UINT uiDeltaTOC = uiPagesTOC2 - uiPagesTOC1;
	if (uiDeltaTOC != 0)
	{
		CSmartArray<CPage> arrPages = m_arrPages;
		m_arrPages.clear();
		bool bTOC = false;
		for (auto& page : arrPages.Items)
		{
			CPage::PAGE_TYPE eType = page->PageType;
			switch (eType)
			{
			case CPage::pageCover:
				m_arrPages.append(page);
				continue;
			case CPage::pageGraph:
			{
				UINT uiPage = page->Page;
				if (bTOC)
				{
					for (UINT uiTOC = 0; uiTOC < uiDeltaTOC; uiTOC++)
					{
						shared_ptr<CPage> pTOC =
							make_shared<CPage>(uiPage++, L"Full", this, CPage::pageTOC);
						m_arrPages.append(pTOC);
					}
					page->Page = uiPage;
					m_arrPages.append(page);
					bTOC = false;
				}
				else
				{
					uiPage += uiDeltaTOC;
					page->Page = uiPage;
					m_arrPages.append(page);
				}
				break;
			}
			case CPage::pageTOC:
				bTOC = true;
				m_arrPages.append(page);
			}
		}
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	pProperties->UpdateTableOfContents(this);

	// adjust the count
	UINT uiPages = m_arrPages.Count;
	Pages = uiPages;

	// adjust the current page and the view
	UINT nPage = uiPages;
	CString csPlacement = Placement;
	pair < pair<int, int>, pair<int, int> > pairSel = SelectedPairs;
	pair<int, int> pairFirst = pairSel.first;
	if (Selection && csPlacement == L"Replace")
	{
		nPage = (UINT)pairFirst.first;
	}
	else if (Selection && csPlacement == L"Insert")
	{
		nPage = (UINT)pairFirst.first;
	}

	Page = uiPages;
	double dPageHeight = HeightOfPage;
	double dTop = dPageHeight * (nPage - 1);

	CClimateExplorerView* pView = ClimateExplorerView;
	if (pView == nullptr)
	{
		return;
	}
	pView->TopOfView = dTop;
	pView->SetupScrollBars();
	pView->Invalidate();

}// AdjustForTOC

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
void CClimateExplorerDoc::ConvertTemperatureRows
(
	const CSmartArray<CSmartArray<CString>>& arrRaw,
	vector<double>& outYears,
	vector<double>& outValues
)
{
	outYears.clear();
	outValues.clear();

	for (auto& pRow : arrRaw.Items)
	{
		int nCol = 0;
		int year = 0;
		double temp = 0.0;

		for (auto& pCol : pRow->Items)
		{
			switch (nCol)
			{
			case 0:
				year = _ttoi(*pCol);
				break;
			case 1:
				temp = _ttof(*pCol);
				break;
			}
			nCol++;
		}

		outYears.push_back((double)year);
		outValues.push_back(temp);
	}
}

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
void CClimateExplorerDoc::ConvertThresholdRows
(
	const CSmartArray<CSmartArray<CString>>& arrRaw,
	vector<double>& outYears,
	vector<double>& outValues
)
{
	outYears.clear();
	outValues.clear();

	for (auto& pRow : arrRaw.Items)
	{
		int nCol = 0;
		int year = 0;
		double percent = 0.0;

		for (auto& pCol : pRow->Items)
		{
			switch (nCol)
			{
			case 0:
				year = _ttoi(*pCol);
				break;
			case 1:
				percent = _ttof(*pCol);
				break;
			}
			nCol++;
		}

		outYears.push_back((double)year);
		outValues.push_back(percent);
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
void CClimateExplorerDoc::ConvertStationRows
(
	const CSmartArray<CSmartArray<CString>>& arrRaw,
	vector<double>& outYears,
	vector<double>& outValues
)
{
	outYears.clear();
	outValues.clear();

	for (auto& pRow : arrRaw.Items)
	{
		int nCol = 0;
		int year = 0;
		int count = 0;

		for (auto& pCol : pRow->Items)
		{
			switch (nCol)
			{
			case 0:
				year = _ttoi(*pCol);
				break;
			case 1:
				count = _ttoi(*pCol);
				break;
			}
			nCol++;
		}

		outYears.push_back((double)year);
		outValues.push_back((double)count);
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

		// critical: reorganzes rectangle and margins
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
// Render a plot to PNG bytes in memory
/////////////////////////////////////////////////////////////////////////////
bool CClimateExplorerDoc::RenderPlotToPNG
(
	std::shared_ptr<CGraphPlotter> pPlot,
	const CRect& rcPixels,
	std::vector<BYTE>& outBytes
)
{
	outBytes.clear();

	// -------------------------------------------------------------
	// 1. Render the plot to a GDI+ Bitmap
	// -------------------------------------------------------------
	std::unique_ptr<Gdiplus::Bitmap> pBmp = pPlot->RenderPlot(rcPixels);
	if (!pBmp)
		return false;

	// -------------------------------------------------------------
	// 2. Wrap Bitmap in CImagePlus
	// -------------------------------------------------------------
	std::shared_ptr<Gdiplus::Bitmap> spBmp(std::move(pBmp));
	CImagePlus img(spBmp);

	// -------------------------------------------------------------
	// 3. Save to PNG in memory
	// -------------------------------------------------------------
	if (!img.SaveToStreamAsPNG(outBytes))
		return false;

	return true;
} // RenderPlotToPNG

/////////////////////////////////////////////////////////////////////////////
// plots are shifted toward the end of the document on position
// starting at the selection
void CClimateExplorerDoc::ShiftPlotsDown()
{
	if (!Selection)
	{
		return;
	}

	pair < pair<int, int>, pair<int, int> > pairSel = SelectedPairs;
	pair<int, int> pairFirst = pairSel.first;

	// zero based index of the page
	int nFirstPage = pairFirst.first - 1;
	int nFirstPlot = pairFirst.second;

	int nPages = m_arrPages.Count;
	int nLastPage = nPages - 1;
	shared_ptr<CPage> pFirstPage = m_arrPages.get(nFirstPage);
	shared_ptr<CPage> pLastPage = m_arrPages.get(nLastPage);

	// if the last page is full, we need to add a page for the current
	// last page plot to move down
	if (pLastPage->PageIsFull)
	{
		nLastPage++;
		CString csLayout = pLastPage->Layout;
		CPage::PAGE_TYPE eType = pLastPage->PageType;
		pLastPage =
			shared_ptr<CPage>(new CPage(nLastPage + 1, csLayout, this, eType));
		m_arrPages.append(pLastPage);
		nPages = m_arrPages.Count;
		Pages = nPages;
	}

	for (int nPage = nLastPage - 1; nPage >= nFirstPage; nPage--)
	{
		shared_ptr<CPage> pPageSrc = m_arrPages.get(nPage);

		shared_ptr<CPage> pPageDst = m_arrPages.get(nPage + 1);

		// the source plot to be shifted down
		CString csKey = pPageSrc->Plots.LastKey;
		shared_ptr<CGraphPlotter> pPlot = pPageSrc->Plots.find(csKey);

		// add the plot to the destionation page
		pPageDst->Plots.add(csKey, pPlot);

		// remove the plot from the source page
		pPageSrc->Plots.remove(csKey);

		// critical: reorganzes rectangle and margins
		pPageDst->Page = nPage + 2;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CPropertiesWnd* pProperties = pFrame->PropertiesPane;
	pProperties->UpdateTableOfContents();

} // ShiftPlotsDown

/////////////////////////////////////////////////////////////////////////////
