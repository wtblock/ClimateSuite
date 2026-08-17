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
#include "Color.h"
#include "ZipWriter.h"

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
BOOL CClimateExplorerDoc::SaveCEx(CString& csPath)
{
	BOOL value = FALSE;
	IStream* pFileStream = nullptr;

	HRESULT hr = SHCreateStreamOnFileW(
		csPath, STGM_CREATE | STGM_WRITE, &pFileStream);

	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create file stream.");
		return FALSE;
	}

	IXmlWriter* pWriter = nullptr;
	hr = CreateXmlWriter(__uuidof(IXmlWriter),
		reinterpret_cast<void**>(&pWriter), nullptr);

	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create XML writer.");
		pFileStream->Release();
		return FALSE;
	}

	pWriter->SetOutput(pFileStream);
	pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

	auto WriteProp = [&](LPCWSTR name, const CString& val, LPCWSTR type)
	{
		pWriter->WriteStartElement(nullptr, name, nullptr);
		pWriter->WriteAttributeString(nullptr, L"value", nullptr, val);
		pWriter->WriteAttributeString(nullptr, L"type", nullptr, type);
		pWriter->WriteEndElement();
	};

	// ============================================================
	// START DOCUMENT
	// ============================================================
	pWriter->WriteStartDocument(XmlStandalone_Yes);
	pWriter->WriteStartElement(nullptr, L"ClimateExplorer", nullptr);

	// ============================================================
	// DOCUMENT NUMBER OF PAGES
	// ============================================================
	CString csPages; csPages.Format(L"%d", Pages);
	WriteProp(L"PageCount", csPages, L"int");

	// ============================================================
	// DOCUMENT METADATA
	// ============================================================
	WriteProp(L"Title", Title, L"string");
	WriteProp(L"Subtitle", Subtitle, L"string");
	WriteProp(L"Publisher", Publisher, L"string");
	WriteProp(L"ISBN", ISBN, L"string");
	WriteProp(L"Copyright", Copyright, L"string");
	WriteProp(L"Description", Description, L"string");

	// ============================================================
	// EXPORT SETTINGS
	// ============================================================
	WriteProp(L"ExportFolder", ExportFolder, L"string");
	WriteProp(L"ExportPages", ExportPages, L"string");

	CString csDpi; csDpi.Format(L"%d", ExportDPI);
	WriteProp(L"ExportDPI", csDpi, L"int");

	CString csQuality; csQuality.Format(L"%d", ExportQuality);
	WriteProp(L"ExportQuality", csQuality, L"int");

	// ============================================================
	// PAGE LAYOUT SETTINGS
	// ============================================================
	WriteProp(L"WidthOfPage", FormatDouble(WidthOfPage), L"double");
	WriteProp(L"HeightOfPage", FormatDouble(HeightOfPage), L"double");

	CString csMap; csMap.Format(L"%d", Map);
	WriteProp(L"LogicalDPI", csMap, L"int");

	WriteProp(L"TopMargin", FormatDouble(TopMargin), L"double");
	WriteProp(L"BottomMargin", FormatDouble(BottomMargin), L"double");
	WriteProp(L"InsideMargin", FormatDouble(InsideMargin), L"double");
	WriteProp(L"OutsideMargin", FormatDouble(OutsideMargin), L"double");
	WriteProp(L"Gutter", FormatDouble(Gutter), L"double");

	// ============================================================
	// PAGES
	// ============================================================
	pWriter->WriteStartElement(nullptr, L"Pages", nullptr);

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

		// ========================================================
		// PLOTS ON THIS PAGE
		// ========================================================
		pWriter->WriteStartElement(nullptr, L"Plots", nullptr);

		for (auto& plotNode : pPage->Plots.Items)
		{
			shared_ptr<CGraphPlotter> pPlot = plotNode.second;

			pWriter->WriteStartElement(nullptr, L"Plot", nullptr);

			// ---------------- QUERY PROPERTIES -------------------
			WriteProp(L"QueryType", pPlot->QueryType, L"string");
			WriteProp(L"Pure", pPlot->Pure ? L"true" : L"false", L"bool");
			WriteProp(L"Scope", pPlot->Scope, L"string");

			WriteProp(L"YearStart", FormatInt(pPlot->YearStart), L"int");
			WriteProp(L"YearEnd", FormatInt(pPlot->YearEnd), L"int");

			WriteProp(L"Subtype", pPlot->Subtype, L"string");
			WriteProp(L"Threshold", FormatInt(pPlot->Threshold), L"int");

			WriteProp(L"Units", pPlot->Units, L"string");
			WriteProp(L"Output", pPlot->Output, L"string");

			WriteProp(L"State", pPlot->State, L"string");
			WriteProp(L"Location", pPlot->Location, L"string");
			WriteProp(L"Station", pPlot->Station, L"string");

			WriteProp(L"Latitude", FormatDouble(pPlot->Latitude), L"double");
			WriteProp(L"Longitude", FormatDouble(pPlot->Longitude), L"double");

			WriteProp(L"SQL", pPlot->SQL, L"string");

			// ---------------- APPEARANCE PROPERTIES --------------
			WriteProp(L"GraphTitle", pPlot->GraphTitle, L"string");
			WriteProp(L"AxisLabelX", pPlot->AxisLabelX, L"string");
			WriteProp(L"AxisLabelY", pPlot->AxisLabelY, L"string");

			WriteProp(L"LineColor", FormatColor(pPlot->LineColor), L"color");
			WriteProp(L"LineStyle", DashStyleToString(pPlot->LineStyle), L"string");
			WriteProp(L"LineThicknessInches", FormatDouble(pPlot->LineThicknessInches), L"double");

			WriteProp(L"TrendLine", pPlot->TrendLine ? L"true" : L"false", L"bool");
			WriteProp(L"TrendLineColor", FormatColor(pPlot->TrendLineColor), L"color");
			WriteProp(L"TrendLineStyle", DashStyleToString(pPlot->TrendLineStyle), L"string");
			WriteProp(L"TrendLineThicknessInches", FormatDouble(pPlot->TrendLineThicknessInches), L"double");

			WriteProp(L"GridLineColor", FormatColor(pPlot->GridColor), L"color");
			WriteProp(L"GridLineStyle", DashStyleToString(pPlot->GridLineStyle), L"string");
			WriteProp(L"GridLineThicknessInches", FormatDouble(pPlot->GridLineThicknessInches), L"double");

			WriteProp(L"TitleFontSizePoints", FormatInt(pPlot->TitleFontSizePoints), L"int");
			WriteProp(L"AxisLabelFontSizePoints", FormatInt(pPlot->AxisLabelFontSizePoints), L"int");
			WriteProp(L"TickLabelFontSizePoints", FormatInt(pPlot->TickLabelFontSizePoints), L"int");

			WriteProp(L"LeftPaddingInches", FormatDouble(pPlot->LeftPaddingInches), L"double");
			WriteProp(L"RightPaddingInches", FormatDouble(pPlot->RightPaddingInches), L"double");
			WriteProp(L"TopPaddingInches", FormatDouble(pPlot->TopPaddingInches), L"double");
			WriteProp(L"BottomPaddingInches", FormatDouble(pPlot->BottomPaddingInches), L"double");

			WriteProp(L"TickLengthInches", FormatDouble(pPlot->TickLengthInches), L"double");
			WriteProp(L"Layout", pPlot->Layout, L"string");

			pWriter->WriteEndElement(); // </Plot>
		}

		pWriter->WriteEndElement(); // </Plots>
		pWriter->WriteEndElement(); // </Page>
	}

	pWriter->WriteEndElement(); // </Pages>
	pWriter->WriteEndElement(); // </ClimateExplorer>
	pWriter->WriteEndDocument();

	pWriter->Release();
	pFileStream->Release();

	SetModifiedFlag(FALSE);
	return TRUE;
} // SaveCEx

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::SaveCE(const CString& csPath)
{
	BOOL value = FALSE;

	// -------------------------------------------------------------
	// 1. Create the output stream
	// -------------------------------------------------------------
	IStream* pXmlStream = nullptr;
	HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pXmlStream);

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
		pXmlStream->Release();
		return FALSE;
	}

	pWriter->SetOutput(pXmlStream);
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

	// -------------------------------------------------------------
	// 4. Document-level properties (trimmed)
	// -------------------------------------------------------------
	CString csPages; csPages.Format(L"%u", Pages);
	WriteCEProp(L"PageCount", csPages);

	WriteCEProp(L"Title", Title);
	WriteCEProp(L"Subtitle", Subtitle);
	WriteCEProp(L"Publisher", Publisher);
	WriteCEProp(L"Copyright", Copyright);

	// -------------------------------------------------------------
	// 5. Pages
	// -------------------------------------------------------------
	pWriter->WriteStartElement(nullptr, L"Pages", nullptr);

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

		// ---------------------------------------------------------
		// 6. Plots (trimmed metadata only)
		// ---------------------------------------------------------
		pWriter->WriteStartElement(nullptr, L"Plots", nullptr);

		UINT plotIndex = 0;
		for (auto& plotNode : pPage->Plots.Items)
		{
			shared_ptr<CGraphPlotter> pPlot = plotNode.second;

			pWriter->WriteStartElement(nullptr, L"Plot", nullptr);

			WriteCEProp(L"GraphTitle", pPlot->GraphTitle);
			WriteCEProp(L"Scope", pPlot->Scope);
			WriteCEProp(L"Subtype", pPlot->Subtype);

			WriteCEProp(L"YearStart", FormatInt(pPlot->YearStart));
			WriteCEProp(L"YearEnd", FormatInt(pPlot->YearEnd));

			WriteCEProp(L"State", pPlot->State);
			WriteCEProp(L"Location", pPlot->Location);

			// -------------------------------------------------------------
			// Render PNG for this plot and write filename
			// -------------------------------------------------------------

			// 1. Compute rectangles for this page
			vector<CRect> arrRects = pPage->Rectangles;

			// 2. Find this plot's rectangle
			CRect rcLogical = arrRects[plotIndex];

			// 3. Convert logical → pixel rectangle (400 DPI)
			CRect rcPixels = ConvertLogicalToPixels(rcLogical);

			// 4. Render PNG bytes
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

			WriteCEProp(L"Layout", pPlot->Layout);

			pWriter->WriteEndElement(); // </Plot>

			plotIndex++;
		}

		pWriter->WriteEndElement(); // </Plots>
		pWriter->WriteEndElement(); // </Page>

	}

	pWriter->WriteEndElement(); // </Pages>
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
	pXmlStream->Stat(&stat, STATFLAG_NONAME);

	ULONG xmlSize = (ULONG)stat.cbSize.QuadPart;
	std::vector<BYTE> xmlBytes(xmlSize);

	LARGE_INTEGER liZero = {};
	pXmlStream->Seek(liZero, STREAM_SEEK_SET, NULL);

	ULONG bytesRead = 0;
	pXmlStream->Read(xmlBytes.data(), xmlSize, &bytesRead);

	// -------------------------------------------------------------
	// Create CE ZIP file using CZipWriter
	// -------------------------------------------------------------
	CZipWriter zip;
	if (!zip.Create(csPath))
	{
		pXmlStream->Release();
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
	pXmlStream->Release();

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
// open a data file
bool CClimateExplorerDoc::Open(LPCTSTR szFilename, bool bRead, LPCTSTR pcszFileID)
{
	IStream* pFileStream = nullptr;
	HRESULT hr = SHCreateStreamOnFileW(szFilename, STGM_READ, &pFileStream);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to open file.");
		return false;
	}

	CString csData = CHelper::GetDataName(szFilename);

	IXmlReader* pReader = nullptr;
	hr = CreateXmlReader(__uuidof(IXmlReader),
		reinterpret_cast<void**>(&pReader), nullptr);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Failed to create XML reader.");
		pFileStream->Release();
		return false;
	}

	pReader->SetInput(pFileStream);

	XmlNodeType nodeType;
	CString currentElement;

	shared_ptr<CPage>         currentPage;
	shared_ptr<CGraphPlotter> currentPlot;

	m_arrPages.clear();

	// prepare the progress dialog
	theApp.OnIdle(0);
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	// launch the progress dialog
	CThumbnailDialog dlg;
	dlg.Parent = pFrame;
	dlg.CreateDlg();
	dlg.ShowWindow(SW_SHOW);
	CString csDialogTitle;
	csDialogTitle.Format(L"Opening document %s", csData);

	dlg.SetWindowText(csDialogTitle);
	dlg.Objects = L"Pages";

	dlg.TotalImages = (int)0;
	bool bAbort = false;

	while (S_OK == pReader->Read(&nodeType))
	{
		// let the user cancel out
		if (dlg.Cancel)
		{
			bAbort = true;
			break;
		}

		if (nodeType == XmlNodeType_Element)
		{
			const WCHAR* pwszLocalName = nullptr;
			pReader->GetLocalName(&pwszLocalName, nullptr);
			currentElement = pwszLocalName;
			if (currentElement == L"PageCount")
			{
				CString val = ReadValueAttribute(pReader);
				dlg.TotalImages = _tstol(val);
			}

			// document-level properties
			if (IsDocProperty(currentElement))
			{
				CString val = ReadValueAttribute(pReader);
				AssignDocumentProperty(currentElement, val);
			}
			// page start
			else if (currentElement == L"Page")
			{
				CString csNumber = ReadAttribute(pReader, L"number");
				CString csType = ReadAttribute(pReader, L"type");
				CString csLayout = ReadAttribute(pReader, L"layout");

				UINT nPage = _tstol(csNumber);

				// update the progress dialog's status
				dlg.CurrentImage = nPage;


				CPage::PAGE_TYPE eType =
					csType == L"Cover" ? CPage::pageCover :
					csType == L"TOC" ? CPage::pageTOC :
					CPage::pageGraph;

				currentPage = make_shared<CPage>(nPage, csLayout, this, eType);
				m_arrPages.append(currentPage);
			}
			// plot start
			else if (currentElement == L"Plot")
			{
				currentPlot = make_shared<CGraphPlotter>(this);
			}
			// plot property
			else if (IsPlotProperty(currentElement))
			{
				CString val = ReadValueAttribute(pReader);
				AssignPlotProperty(currentPlot, currentElement, val);
			}
		}
		else if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* pwszLocalName = nullptr;
			pReader->GetLocalName(&pwszLocalName, nullptr);
			CString endElement = pwszLocalName;

			if (endElement == L"Plot")
			{
				// rebuild plot data from SQL
				ExecutePlotQuery(currentPlot);

				// use plot title as key
				CString csKey = currentPlot->GraphTitle;
				if (csKey.IsEmpty())
					csKey = L"Untitled Plot";

				// ensure uniqueness if needed
				int suffix = 1;
				CString csOrigKey = csKey;
				while (currentPage->Plots.Exists[csKey])
				{
					csKey.Format(L"%s (%d)", csOrigKey.GetString(), suffix++);
				}

				currentPage->Plots.add(csKey, currentPlot);
			}
		}

		// wait one milliseconds while letting normal 
		// window messaging to run
		pFrame->Wait(1);
	}

	// done with the progress dialog
	dlg.DestroyWindow();

	pReader->Release();
	pFileStream->Release();

	// adjust for possible TOC size increase
	AdjustForTOC();

	return true;
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
// ParseColor
//
// Converts a hex "RRGGBB" string into a COLORREF.
// Example: "8b0000" -> RGB(139, 0, 0)
//
// If the format is invalid, returns RGB(0,0,0).
//
/////////////////////////////////////////////////////////////////////////////
COLORREF CClimateExplorerDoc::ParseColor(const CString& csValue)
{
	COLORREF rgb = RGB(0, 0, 0);

	CString cs = csValue;
	cs.Trim();

	if (cs.IsEmpty())
	{
		return rgb;
	}

	// Expect exactly 6 hex digits: RRGGBB
	if (cs.GetLength() != 6)
	{
		return rgb;
	}

	// Parse as hex
	wchar_t* pEnd = nullptr;
	unsigned long n = wcstoul(cs, &pEnd, 16);

	if (pEnd == cs.GetString())
	{
		// No conversion performed
		return rgb;
	}

	int r = (n >> 16) & 0xFF;
	int g = (n >> 8) & 0xFF;
	int b = n & 0xFF;

	rgb = RGB(r, g, b);

	return rgb;

} // ParseColor

/////////////////////////////////////////////////////////////////////////////
// AssignDocumentProperty
//
// Maps a single XML document-level property into the document.
// Used by LoadCEx() before OnExecuteQuery() regenerates pages and plots.
//
// All property names match those written by SaveCEx().
//
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::AssignDocumentProperty
(
	const CString& csName,
	const CString& csValue
)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	if (csName == L"QueryType")
	{
		QueryType = csValue;
	}
	else if (csName == L"Pure")
	{
		Pure = (csValue == L"true");
	}
	else if (csName == L"Scope")
	{
		Scope = csValue;
	}
	else if (csName == L"YearStart")
	{
		YearStart = _tstol(csValue);
	}
	else if (csName == L"YearEnd")
	{
		YearEnd = _tstol(csValue);
	}
	else if (csName == L"Subtype")
	{
		Subtype = csValue;
	}
	else if (csName == L"Threshold")
	{
		Threshold = _tstol(csValue);
	}
	else if (csName == L"Units")
	{
		Units = csValue;
	}
	else if (csName == L"Output")
	{
		Output = csValue;
	}

	// -------------------------------------------------------------
	// Location / Station selection
	// -------------------------------------------------------------
	else if (csName == L"State")
	{
		State = csValue;
	}
	else if (csName == L"Location")
	{
		Location = csValue;
	}

	// -------------------------------------------------------------
	// Page geometry
	// -------------------------------------------------------------
	else if (csName == L"WidthOfPage")
	{
		WidthOfPage = _tstof(csValue);
	}
	else if (csName == L"HeightOfPage")
	{
		HeightOfPage = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Margins
	// -------------------------------------------------------------
	else if (csName == L"Gutter")
	{
		Gutter = _tstof(csValue);
	}
	else if (csName == L"InsideMargin")
	{
		InsideMargin = _tstof(csValue);
	}
	else if (csName == L"OutsideMargin")
	{
		OutsideMargin = _tstof(csValue);
	}
	else if (csName == L"TopMargin")
	{
		TopMargin = _tstof(csValue);
	}
	else if (csName == L"BottomMargin")
	{
		BottomMargin = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Layout (document-level)
	// -------------------------------------------------------------
	else if (csName == L"Layout")
	{
		Layout = csValue;
	}

	// -------------------------------------------------------------
	// Export settings
	// -------------------------------------------------------------
	else if (csName == L"ExportDPI")
	{
		ExportDPI = _tstol(csValue);
	}
	else if (csName == L"ExportQuality")
	{
		ExportQuality = _tstol(csValue);
	}

	// -------------------------------------------------------------
	// PageCount (informational only)
	// -------------------------------------------------------------
	else if (csName == L"PageCount")
	{
		// PageCount is informational; loader does not use it.
	}

} // AssignDocumentProperty

/////////////////////////////////////////////////////////////////////////////
// IsDocProperty
//
// Returns TRUE if the given XML element name corresponds to a document-level
// property written by SaveCEx(). Used by LoadCEx() to route XML values into
// AssignDocumentProperty().
//
// These properties apply to the entire document, not individual plots.
//
/////////////////////////////////////////////////////////////////////////////
bool CClimateExplorerDoc::IsDocProperty(const CString& csName)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	if (csName == L"QueryType") return true;
	if (csName == L"Pure") return true;
	if (csName == L"Scope") return true;
	if (csName == L"YearStart") return true;
	if (csName == L"YearEnd") return true;
	if (csName == L"Subtype") return true;
	if (csName == L"Threshold") return true;
	if (csName == L"Units") return true;
	if (csName == L"Output") return true;

	// -------------------------------------------------------------
	// Location / Station selection
	// -------------------------------------------------------------
	if (csName == L"State") return true;
	if (csName == L"Location") return true;

	// -------------------------------------------------------------
	// Page geometry
	// -------------------------------------------------------------
	if (csName == L"HeightOfPage") return true;
	if (csName == L"WidthOfPage") return true;

	// -------------------------------------------------------------
	// Margins
	// -------------------------------------------------------------
	if (csName == L"Gutter") return true;
	if (csName == L"InsideMargin") return true;
	if (csName == L"OutsideMargin") return true;
	if (csName == L"TopMargin") return true;
	if (csName == L"BottomMargin") return true;

	// -------------------------------------------------------------
	// Layout (document-level)
	// -------------------------------------------------------------
	if (csName == L"Layout") return true;

	// -------------------------------------------------------------
	// Export settings
	// -------------------------------------------------------------
	if (csName == L"ExportDPI") return true;
	if (csName == L"ExportQuality") return true;

	// -------------------------------------------------------------
	// Page count (informational)
	// -------------------------------------------------------------
	if (csName == L"PageCount") return true;

	return false;

} // IsDocProperty

/////////////////////////////////////////////////////////////////////////////
// IsPlotProperty
//
// Returns TRUE if the given XML element name corresponds to a plot-level
// property written by SaveCEx(). Used by LoadCEx() to route XML values
// into AssignPlotPropertyToTemp().
//
/////////////////////////////////////////////////////////////////////////////
bool CClimateExplorerDoc::IsPlotProperty(const CString& csName)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	if (csName == L"QueryType") return true;
	if (csName == L"Pure") return true;
	if (csName == L"Scope") return true;
	if (csName == L"YearStart") return true;
	if (csName == L"YearEnd") return true;
	if (csName == L"Subtype") return true;
	if (csName == L"Threshold") return true;
	if (csName == L"Units") return true;
	if (csName == L"Output") return true;
	if (csName == L"State") return true;
	if (csName == L"Location") return true;
	if (csName == L"Station") return true;
	if (csName == L"Latitude") return true;
	if (csName == L"Longitude") return true;
	if (csName == L"SQL") return true; // debug-only, ignored but recognized

	// -------------------------------------------------------------
	// Appearance: Titles and labels
	// -------------------------------------------------------------
	if (csName == L"GraphTitle") return true;
	if (csName == L"AxisLabelX") return true;
	if (csName == L"AxisLabelY") return true;

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	if (csName == L"LineColor") return true;
	if (csName == L"LineStyle") return true;
	if (csName == L"LineThicknessInches") return true;

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	if (csName == L"TrendLine") return true;
	if (csName == L"TrendLineColor") return true;
	if (csName == L"TrendLineStyle") return true;
	if (csName == L"TrendLineThicknessInches") return true;

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	if (csName == L"GridLineColor") return true;
	if (csName == L"GridLineStyle") return true;
	if (csName == L"GridLineThicknessInches") return true;

	// -------------------------------------------------------------
	// Font sizes
	// -------------------------------------------------------------
	if (csName == L"TitleFontSizePoints") return true;
	if (csName == L"AxisLabelFontSizePoints") return true;
	if (csName == L"TickLabelFontSizePoints") return true;

	// -------------------------------------------------------------
	// Padding
	// -------------------------------------------------------------
	if (csName == L"LeftPaddingInches") return true;
	if (csName == L"RightPaddingInches") return true;
	if (csName == L"TopPaddingInches") return true;
	if (csName == L"BottomPaddingInches") return true;

	// -------------------------------------------------------------
	// Tick length
	// -------------------------------------------------------------
	if (csName == L"TickLengthInches") return true;

	// -------------------------------------------------------------
	// Layout
	// -------------------------------------------------------------
	if (csName == L"Layout") return true;

	return false;

} // IsPlotProperty

/////////////////////////////////////////////////////////////////////////////
// AssignPlotPropertyToTemp
//
// Maps a single XML plot property into the temporary PlotProps structure.
// This function is used only during LoadCEx() before OnExecuteQuery()
// regenerates the document.
//
// All property names match those written by SaveCEx().
//
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::AssignPlotPropertyToTemp
(
	PlotProps& temp,
	const CString& csName,
	const CString& csValue
)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	if (csName == L"QueryType")
	{
		temp.QueryType = csValue;
	}
	else if (csName == L"Pure")
	{
		temp.Pure = (csValue == L"true");
	}
	else if (csName == L"Scope")
	{
		temp.Scope = csValue;
	}
	else if (csName == L"YearStart")
	{
		temp.YearStart = _tstol(csValue);
	}
	else if (csName == L"YearEnd")
	{
		temp.YearEnd = _tstol(csValue);
	}
	else if (csName == L"Subtype")
	{
		temp.Subtype = csValue;
	}
	else if (csName == L"Threshold")
	{
		temp.Threshold = _tstol(csValue);
	}
	else if (csName == L"Units")
	{
		temp.Units = csValue;
	}
	else if (csName == L"Output")
	{
		temp.Output = csValue;
	}
	else if (csName == L"State")
	{
		temp.State = csValue;
	}
	else if (csName == L"Location")
	{
		temp.Location = csValue;
	}
	else if (csName == L"Station")
	{
		temp.Station = csValue;
	}
	else if (csName == L"Latitude")
	{
		temp.Latitude = _tstof(csValue);
	}
	else if (csName == L"Longitude")
	{
		temp.Longitude = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Appearance: Titles and labels
	// -------------------------------------------------------------
	else if (csName == L"GraphTitle")
	{
		temp.GraphTitle = csValue;
	}
	else if (csName == L"AxisLabelX")
	{
		temp.AxisLabelX = csValue;
	}
	else if (csName == L"AxisLabelY")
	{
		temp.AxisLabelY = csValue;
	}

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	else if (csName == L"LineColor")
	{
		temp.LineColor = ParseColor(csValue);
	}
	else if (csName == L"LineStyle")
	{
		temp.LineStyle = csValue;
	}
	else if (csName == L"LineThicknessInches")
	{
		temp.LineThicknessInches = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	else if (csName == L"TrendLine")
	{
		temp.TrendLine = (csValue == L"true");
	}
	else if (csName == L"TrendLineColor")
	{
		temp.TrendLineColor = ParseColor(csValue);
	}
	else if (csName == L"TrendLineStyle")
	{
		temp.TrendLineStyle = csValue;
	}
	else if (csName == L"TrendLineThicknessInches")
	{
		temp.TrendLineThicknessInches = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	else if (csName == L"GridLineColor")
	{
		temp.GridColor = ParseColor(csValue);
	}
	else if (csName == L"GridLineStyle")
	{
		temp.GridLineStyle = csValue;
	}
	else if (csName == L"GridLineThicknessInches")
	{
		temp.GridLineThicknessInches = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Font sizes
	// -------------------------------------------------------------
	else if (csName == L"TitleFontSizePoints")
	{
		temp.TitleFontSizePoints = _tstol(csValue);
	}
	else if (csName == L"AxisLabelFontSizePoints")
	{
		temp.AxisLabelFontSizePoints = _tstol(csValue);
	}
	else if (csName == L"TickLabelFontSizePoints")
	{
		temp.TickLabelFontSizePoints = _tstol(csValue);
	}

	// -------------------------------------------------------------
	// Padding
	// -------------------------------------------------------------
	else if (csName == L"LeftPaddingInches")
	{
		temp.LeftPaddingInches = _tstof(csValue);
	}
	else if (csName == L"RightPaddingInches")
	{
		temp.RightPaddingInches = _tstof(csValue);
	}
	else if (csName == L"TopPaddingInches")
	{
		temp.TopPaddingInches = _tstof(csValue);
	}
	else if (csName == L"BottomPaddingInches")
	{
		temp.BottomPaddingInches = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Tick length
	// -------------------------------------------------------------
	else if (csName == L"TickLengthInches")
	{
		temp.TickLengthInches = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Layout
	// -------------------------------------------------------------
	else if (csName == L"Layout")
	{
		temp.Layout = csValue;
	}

} // AssignPlotPropertyToTemp

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// ApplyPlotPropsToDocument
//
// Copies all fields from the temporary PlotProps structure into the
// document's live properties so ExecuteQuery() can generate pages
// exactly as they were saved.
//
/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ApplyPlotPropsToDocument(const PlotProps& temp)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	QueryType = temp.QueryType;
	Pure = temp.Pure;
	Scope = temp.Scope;
	YearStart = temp.YearStart;
	YearEnd = temp.YearEnd;
	Subtype = temp.Subtype;
	Threshold = temp.Threshold;
	Units = temp.Units;
	Output = temp.Output;
	State = temp.State;
	Location = temp.Location;

	// -------------------------------------------------------------
	// Appearance: Titles and labels
	// -------------------------------------------------------------
	GraphTitle = temp.GraphTitle;
	AxisLabelX = temp.AxisLabelX;
	AxisLabelY = temp.AxisLabelY;

	// -------------------------------------------------------------
	// Curve line appearance
	// -------------------------------------------------------------
	LineColor = temp.LineColor;
	LineStyle = LineStyleEnum[temp.LineStyle];
	LineThicknessInches = temp.LineThicknessInches;

	// -------------------------------------------------------------
	// Trend line appearance
	// -------------------------------------------------------------
	TrendLine = temp.TrendLine;
	TrendLineColor = temp.TrendLineColor;
	TrendLineStyle = LineStyleEnum[temp.TrendLineStyle];
	TrendLineThicknessInches = temp.TrendLineThicknessInches;

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	GridColor = temp.GridColor;
	GridLineStyle = LineStyleEnum[temp.GridLineStyle];
	GridLineThicknessInches = temp.GridLineThicknessInches;

	// -------------------------------------------------------------
	// Font sizes
	// -------------------------------------------------------------
	TitleFontSizePoints = temp.TitleFontSizePoints;
	AxisLabelFontSizePoints = temp.AxisLabelFontSizePoints;
	TickLabelFontSizePoints = temp.TickLabelFontSizePoints;

	// -------------------------------------------------------------
	// Padding
	// -------------------------------------------------------------
	//LeftPaddingInches = temp.LeftPaddingInches;
	RightPaddingInches = temp.RightPaddingInches;
	//TopPaddingInches = temp.TopPaddingInches;
	BottomPaddingInches = temp.BottomPaddingInches;

	// -------------------------------------------------------------
	// Tick length
	// -------------------------------------------------------------
	TickLengthInches = temp.TickLengthInches;

	// -------------------------------------------------------------
	// Layout
	// -------------------------------------------------------------
	Layout = temp.Layout;

} // ApplyPlotPropsToDocument

/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::LoadCE(const CString& csPath)
{
	AfxMessageBox(L"LoadCE() not implemented yet.");
	return FALSE;
} // LoadCE

/////////////////////////////////////////////////////////////////////////////
// LoadCEx
//
// Loads a full‑fidelity Climate Explorer (.CEx) document.
// Restores all document and plot properties from XML,
// then regenerates the document using OnExecuteQuery().
//
// SQL stored in the CEx file is debug‑only and is NOT executed.
// The authoritative source is the restored properties.
//
// Pagination, page creation, and plot creation are handled
// entirely by OnExecuteQuery(), exactly as during live execution.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CClimateExplorerDoc::LoadCEx(const CString& csFilename)
{
	BOOL value = FALSE;

	// -------------------------------------------------------------
	// Open the XML file
	// -------------------------------------------------------------
	CComPtr<IStream> pStream;
	HRESULT hr = SHCreateStreamOnFileEx
	(
		csFilename,
		STGM_READ | STGM_SHARE_DENY_WRITE,
		FILE_ATTRIBUTE_NORMAL,
		FALSE,
		nullptr,
		&pStream
	);

	if (FAILED(hr) || pStream == nullptr)
	{
		return value;
	}

	CComPtr<IXmlReader> pReader;
	hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, nullptr);
	if (FAILED(hr) || pReader == nullptr)
	{
		return value;
	}

	hr = pReader->SetInput(pStream);
	if (FAILED(hr))
	{
		return value;
	}

	// -------------------------------------------------------------
	// Suppress progress UI during load
	// -------------------------------------------------------------
	bool bProgress = false;

	Pages = (UINT)m_arrPages.Count;

	// -------------------------------------------------------------
	// XML reading loop
	// -------------------------------------------------------------
	XmlNodeType eType;
	CString csElement;

	while (pReader->Read(&eType) == S_OK)
	{
		if (eType == XmlNodeType_Element)
		{
			const WCHAR* pwszName = nullptr;
			pReader->GetLocalName(&pwszName, nullptr);
			csElement = pwszName;

			bool bEmpty = pReader->IsEmptyElement() == S_OK;

			// ---------------------------------------------------------
			// Document-level properties
			// ---------------------------------------------------------
			if (IsDocProperty(csElement))
			{
				CString csValue = ReadValueAttribute(pReader);
				AssignDocumentProperty(csElement, csValue);
				continue;
			}

			// ---------------------------------------------------------
			// Page element
			// ---------------------------------------------------------
			if (csElement == L"Page")
			{
				CString csType;
				CString csLayout;

				if (pReader->MoveToFirstAttribute() == S_OK)
				{
					do
					{
						const WCHAR* pwszAttr = nullptr;
						const WCHAR* pwszVal = nullptr;

						pReader->GetLocalName(&pwszAttr, nullptr);
						pReader->GetValue(&pwszVal, nullptr);

						if (wcscmp(pwszAttr, L"type") == 0)
						{
							csType = pwszVal;
						}
						else if (wcscmp(pwszAttr, L"layout") == 0)
						{
							csLayout = pwszVal;
						}

					} while (pReader->MoveToNextAttribute() == S_OK);
				}

				// Ignore Cover and TOC pages (constructor already created them)
				if (csType == L"Cover" || csType == L"TOC")
				{
					continue;
				}

				// Only Graph pages contain plots
				if (csType == L"Graph")
				{
					// descend into <Plots>
					continue;
				}
			}

			// ---------------------------------------------------------
			// Plot element
			// ---------------------------------------------------------
			if (csElement == L"Plot")
			{
				PlotProps temp;

				// read all plot properties
				while (pReader->Read(&eType) == S_OK)
				{
					if (eType == XmlNodeType_Element)
					{
						const WCHAR* pwszPlotName = nullptr;
						pReader->GetLocalName(&pwszPlotName, nullptr);
						CString csPlotName = pwszPlotName;

						if (IsPlotProperty(csPlotName))
						{
							CString csValue = ReadValueAttribute(pReader);
							AssignPlotPropertyToTemp(temp, csPlotName, csValue);
						}
					}
					else if (eType == XmlNodeType_EndElement)
					{
						const WCHAR* pwszEnd = nullptr;
						pReader->GetLocalName(&pwszEnd, nullptr);

						if (wcscmp(pwszEnd, L"Plot") == 0)
						{
							break;
						}
					}
				}

				ApplyPlotPropsToDocument(temp);
				ExecuteQuery(false);

				continue;
			}
		}
	}

	// -------------------------------------------------------------
	// Adjust TOC once at the end
	// -------------------------------------------------------------
	AdjustForTOC();

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

		CClimateExplorerView* pView = ClimateExplorerView;
		pView->Invalidate();
	}

	return value;
}

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
void CClimateExplorerDoc::ExecutePlotQuery(shared_ptr<CGraphPlotter>& pPlot)
{
	if (!pPlot) return;

	// -------------------------------------------------------------
	// 1. Clear previous results
	// -------------------------------------------------------------
	pPlot->Years.clear();
	pPlot->Values.clear();

	// -------------------------------------------------------------
	// 2. Execute SQL
	// -------------------------------------------------------------
	CSmartArray<CSmartArray<CString>> arrRawRows;
	CString csSQL = pPlot->SQL;

	ClimateDatabase->ExecuteTable(csSQL, arrRawRows);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame != nullptr && pFrame->OutputPane != nullptr)
	{
		pFrame->OutputPane->SQLText = csSQL;
	}

	// -------------------------------------------------------------
	// 3. Branch based on Subtype
	// -------------------------------------------------------------
	CString csSubtype = pPlot->Subtype;

	if (csSubtype == L"Maximum" ||
		csSubtype == L"Minimum" ||
		csSubtype == L"Average")
	{
		ConvertTemperatureRows(arrRawRows, pPlot->Years, pPlot->Values);
		return;
	}

	if (csSubtype == L"Threshold")
	{
		ConvertThresholdRows(arrRawRows, pPlot->Years, pPlot->Values);
		return;
	}

	if (csSubtype == L"Stations")
	{
		ConvertStationRows(arrRawRows, pPlot->Years, pPlot->Values);
		return;
	}
} // ExecutePlotQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ExecuteQuery(bool bProgress/* = true*/)
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
			shared_ptr<CGraphPlotter>(new CGraphPlotter(this, Years, Values));
		pPlot->Station = csStation;
		pPlot->Latitude = fLatitude;
		pPlot->Longitude = fLongitude;

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

	// adjust for possible TOC size increase
	AdjustForTOC();
} // OnExecuteQuery

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::OnUpdateExecuteQuery(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

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
	Page = uiPages;
	double dPageHeight = HeightOfPage;
	double dTop = dPageHeight * (uiPages - 1);

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
// Convert a logical rectangle (Map units) to a pixel rectangle at 400 DPI
/////////////////////////////////////////////////////////////////////////////
CRect CClimateExplorerDoc::ConvertLogicalToPixels(const CRect& rcLogical)
{
	CRect value(0, 0, 0, 0);

	const int nMap = Map;          // logical units per inch
	const double dpi = 400.0;      // graph rendering DPI (fixed)

	const double leftInches = double(rcLogical.left) / double(nMap);
	const double topInches = double(rcLogical.top) / double(nMap);
	const double rightInches = double(rcLogical.right) / double(nMap);
	const double bottomInches = double(rcLogical.bottom) / double(nMap);

	const int widthPixels = int((rightInches - leftInches) * dpi + 0.5);
	const int heightPixels = int((bottomInches - topInches) * dpi + 0.5);

	value.left = 0;
	value.top = 0;
	value.right = widthPixels;
	value.bottom = heightPixels;

	return value;
} // ConvertLogicalToPixels

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
