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

			WriteProp(L"LineColor", pPlot->LineColor, L"string");
			WriteProp(L"LineStyle", DashStyleToString(pPlot->LineStyle), L"string");
			WriteProp(L"LineThicknessInches", FormatDouble(pPlot->LineThicknessInches), L"double");

			WriteProp(L"RunningAvgColor", pPlot->RunningAvgColor, L"string");
			WriteProp(L"RunningAvgStyle", DashStyleToString(pPlot->RunningAvgStyle), L"string");
			WriteProp(L"RunningAvgThicknessInches", FormatDouble(pPlot->RunningAvgThicknessInches), L"double");

			WriteProp(L"GridLineColor", pPlot->GridColor, L"string");
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

			WriteProp(L"TrendOneEnable", pPlot->TrendOneEnable ? L"true" : L"false", L"bool");
			WriteProp(L"TrendOneColor", pPlot->TrendOneColor, L"string");
			WriteProp(L"TrendOneStyle", DashStyleToString(pPlot->TrendOneStyle), L"string");
			WriteProp(L"TrendOneThickness", FormatDouble(pPlot->TrendOneThickness), L"double");
			WriteProp(L"TrendOneYear", FormatInt(pPlot->TrendOneYear), L"int");

			WriteProp(L"TrendTwoEnable", pPlot->TrendTwoEnable ? L"true" : L"false", L"bool");
			WriteProp(L"TrendTwoColor", pPlot->TrendTwoColor, L"string");
			WriteProp(L"TrendTwoStyle", DashStyleToString(pPlot->TrendTwoStyle), L"string");
			WriteProp(L"TrendTwoThickness", FormatDouble(pPlot->TrendTwoThickness), L"double");
			WriteProp(L"TrendTwoYear", FormatInt(pPlot->TrendTwoYear), L"int");

			WriteProp(L"TrendThreeEnable", pPlot->TrendThreeEnable ? L"true" : L"false", L"bool");
			WriteProp(L"TrendThreeColor", pPlot->TrendThreeColor, L"string");
			WriteProp(L"TrendThreeStyle", DashStyleToString(pPlot->TrendThreeStyle), L"string");
			WriteProp(L"TrendThreeThickness", FormatDouble(pPlot->TrendThreeThickness), L"double");
			WriteProp(L"TrendThreeYear", FormatInt(pPlot->TrendThreeYear), L"int");

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
	m_arrCEPNGs.clear();

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

			// ---------------- QUERY PROPERTIES -------------------
			WriteCEProp(L"QueryType", pPlot->QueryType);
			WriteCEProp(L"Pure", pPlot->Pure ? L"true" : L"false");
			WriteCEProp(L"Scope", pPlot->Scope);

			WriteCEProp(L"YearStart", FormatInt(pPlot->YearStart));
			WriteCEProp(L"YearEnd", FormatInt(pPlot->YearEnd));

			WriteCEProp(L"Subtype", pPlot->Subtype);
			WriteCEProp(L"Threshold", FormatInt(pPlot->Threshold));

			WriteCEProp(L"Units", pPlot->Units);
			WriteCEProp(L"Output", pPlot->Output);

			WriteCEProp(L"State", pPlot->State);
			WriteCEProp(L"Location", pPlot->Location);
			WriteCEProp(L"Station", pPlot->Station);

			WriteCEProp(L"Latitude", FormatDouble(pPlot->Latitude));
			WriteCEProp(L"Longitude", FormatDouble(pPlot->Longitude));

			WriteCEProp(L"SQL", pPlot->SQL);

			// ---------------- APPEARANCE PROPERTIES --------------
			WriteCEProp(L"GraphTitle", pPlot->GraphTitle);
			WriteCEProp(L"AxisLabelX", pPlot->AxisLabelX);
			WriteCEProp(L"AxisLabelY", pPlot->AxisLabelY);

			WriteCEProp(L"LineColor", pPlot->LineColor);
			WriteCEProp(L"LineStyle", DashStyleToString(pPlot->LineStyle));
			WriteCEProp(L"LineThicknessInches", FormatDouble(pPlot->LineThicknessInches));

			WriteCEProp(L"RunningAvgColor", pPlot->RunningAvgColor);
			WriteCEProp(L"RunningAvgStyle", DashStyleToString(pPlot->RunningAvgStyle));
			WriteCEProp(L"RunningAvgThicknessInches", FormatDouble(pPlot->RunningAvgThicknessInches));

			WriteCEProp(L"GridLineColor", pPlot->GridColor);
			WriteCEProp(L"GridLineStyle", DashStyleToString(pPlot->GridLineStyle));
			WriteCEProp(L"GridLineThicknessInches", FormatDouble(pPlot->GridLineThicknessInches));

			WriteCEProp(L"TitleFontSizePoints", FormatInt(pPlot->TitleFontSizePoints));
			WriteCEProp(L"AxisLabelFontSizePoints", FormatInt(pPlot->AxisLabelFontSizePoints));
			WriteCEProp(L"TickLabelFontSizePoints", FormatInt(pPlot->TickLabelFontSizePoints));

			WriteCEProp(L"LeftPaddingInches", FormatDouble(pPlot->LeftPaddingInches));
			WriteCEProp(L"RightPaddingInches", FormatDouble(pPlot->RightPaddingInches));
			WriteCEProp(L"TopPaddingInches", FormatDouble(pPlot->TopPaddingInches));
			WriteCEProp(L"BottomPaddingInches", FormatDouble(pPlot->BottomPaddingInches));

			WriteCEProp(L"TickLengthInches", FormatDouble(pPlot->TickLengthInches));
			WriteCEProp(L"Layout", pPlot->Layout);

			WriteCEProp(L"TrendOneEnable", pPlot->TrendOneEnable ? L"true" : L"false");
			WriteCEProp(L"TrendOneColor", pPlot->TrendOneColor);
			WriteCEProp(L"TrendOneStyle", DashStyleToString(pPlot->TrendOneStyle));
			WriteCEProp(L"TrendOneThickness", FormatDouble(pPlot->TrendOneThickness));
			WriteCEProp(L"TrendOneYear", FormatInt(pPlot->TrendOneYear));

			WriteCEProp(L"TrendTwoEnable", pPlot->TrendTwoEnable ? L"true" : L"false");
			WriteCEProp(L"TrendTwoColor", pPlot->TrendTwoColor);
			WriteCEProp(L"TrendTwoStyle", DashStyleToString(pPlot->TrendTwoStyle));
			WriteCEProp(L"TrendTwoThickness", FormatDouble(pPlot->TrendTwoThickness));
			WriteCEProp(L"TrendTwoYear", FormatInt(pPlot->TrendTwoYear));

			WriteCEProp(L"TrendThreeEnable", pPlot->TrendThreeEnable ? L"true" : L"false");
			WriteCEProp(L"TrendThreeColor", pPlot->TrendThreeColor);
			WriteCEProp(L"TrendThreeStyle", DashStyleToString(pPlot->TrendThreeStyle));
			WriteCEProp(L"TrendThreeThickness", FormatDouble(pPlot->TrendThreeThickness));
			WriteCEProp(L"TrendThreeYear", FormatInt(pPlot->TrendThreeYear));

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
	// Running Average line appearance
	// -------------------------------------------------------------
	if (csName == L"RunningAvgColor") return true;
	if (csName == L"RunningAvgStyle") return true;
	if (csName == L"RunningAvgThicknessInches") return true;

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
		temp.LineColor = csValue;
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
	// Running Average line appearance
	// -------------------------------------------------------------
	else if (csName == L"RunningAvgColor")
	{
		temp.RunningAvgColor = csValue;
	}
	else if (csName == L"RunningAvgStyle")
	{
		temp.RunningAvgStyle = csValue;
	}
	else if (csName == L"RunningAvgThicknessInches")
	{
		temp.RunningAvgThicknessInches = _tstof(csValue);
	}

	// -------------------------------------------------------------
	// Grid appearance
	// -------------------------------------------------------------
	else if (csName == L"GridLineColor")
	{
		temp.GridColor = csValue;
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

	// -------------------------------------------------------------
	// Trend One line appearance
	// -------------------------------------------------------------
	else if (csName == L"TrendOneEnable")
	{
		temp.TrendOneEnable = (csValue == L"true");
	}
	else if (csName == L"TrendOneColor")
	{
		temp.TrendOneColor = csValue;
	}
	else if (csName == L"TrendOneStyle")
	{
		temp.TrendOneStyle = csValue;
	}
	else if (csName == L"TrendOneThickness")
	{
		temp.TrendOneThickness = _tstof(csValue);
	}
	else if (csName == L"TrendOneYear")
	{
		temp.TrendOneYear = _tstol(csValue);
	}

	// -------------------------------------------------------------
	// Trend Two line appearance
	// -------------------------------------------------------------
	else if (csName == L"TrendTwoEnable")
	{
		temp.TrendTwoEnable = (csValue == L"true");
	}
	else if (csName == L"TrendTwoColor")
	{
		temp.TrendTwoColor = csValue;
	}
	else if (csName == L"TrendTwoStyle")
	{
		temp.TrendTwoStyle = csValue;
	}
	else if (csName == L"TrendTwoThickness")
	{
		temp.TrendTwoThickness = _tstof(csValue);
	}
	else if (csName == L"TrendTwoYear")
	{
		temp.TrendTwoYear = _tstol(csValue);
	}

	// -------------------------------------------------------------
	// Trend Three line appearance
	// -------------------------------------------------------------
	else if (csName == L"TrendThreeEnable")
	{
		temp.TrendThreeEnable = (csValue == L"true");
	}
	else if (csName == L"TrendThreeColor")
	{
		temp.TrendThreeColor = csValue;
	}
	else if (csName == L"TrendThreeStyle")
	{
		temp.TrendThreeStyle = csValue;
	}
	else if (csName == L"TrendThreeThickness")
	{
		temp.TrendThreeThickness = _tstof(csValue);
	}
	else if (csName == L"TrendThreeYear")
	{
		temp.TrendThreeYear = _tstol(csValue);
	}

} // AssignPlotPropertyToTemp

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::CopyPlotPropertiesToDocument(CGraphPlotter* pPlot)
{
	// -------------------------------------------------------------
	// Query properties
	// -------------------------------------------------------------
	QueryType = pPlot->QueryType;
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
	// Running Average line appearance
	// -------------------------------------------------------------
	RunningAvgColor = temp.RunningAvgColor;
	RunningAvgStyle = LineStyleEnum[temp.RunningAvgStyle];
	RunningAvgThicknessInches = temp.RunningAvgThicknessInches;

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

	TrendOneEnable = temp.TrendOneEnable;
	TrendOneColor = temp.TrendOneColor;
	TrendOneStyle = LineStyleEnum[temp.TrendOneStyle];
	TrendOneThickness = temp.TrendOneThickness;
	TrendOneYear = temp.TrendOneYear;

	TrendTwoEnable = temp.TrendTwoEnable;
	TrendTwoColor = temp.TrendTwoColor;
	TrendTwoStyle = LineStyleEnum[temp.TrendTwoStyle];
	TrendTwoThickness = temp.TrendTwoThickness;
	TrendTwoYear = temp.TrendTwoYear;

	TrendThreeEnable = temp.TrendThreeEnable;
	TrendThreeColor = temp.TrendThreeColor;
	TrendThreeStyle = LineStyleEnum[temp.TrendThreeStyle];
	TrendThreeThickness = temp.TrendThreeThickness;
	TrendThreeYear = temp.TrendThreeYear;

} // ApplyPlotPropsToDocument

/////////////////////////////////////////////////////////////////////////////
void CClimateExplorerDoc::ApplyPlotPropsToPlotter
(
	CGraphPlotter& plot, const PlotProps& props
)
{
	// ---------------- QUERY PROPERTIES -------------------
	plot.QueryType = props.QueryType;
	plot.Pure = props.Pure;
	plot.Scope = props.Scope;

	plot.YearStart = props.YearStart;
	plot.YearEnd = props.YearEnd;

	plot.Subtype = props.Subtype;
	plot.Threshold = props.Threshold;

	plot.Units = props.Units;
	plot.Output = props.Output;

	plot.State = props.State;
	plot.Location = props.Location;
	plot.Station = props.Station;

	plot.Latitude = props.Latitude;
	plot.Longitude = props.Longitude;

	// ---------------- APPEARANCE PROPERTIES --------------
	plot.GraphTitle = props.GraphTitle;
	plot.AxisLabelX = props.AxisLabelX;
	plot.AxisLabelY = props.AxisLabelY;

	plot.LineColor = props.LineColor;
	plot.LineStyle = LineStyleEnum[props.LineStyle];
	plot.LineThicknessInches = props.LineThicknessInches;

	plot.RunningAvgColor = props.RunningAvgColor;
	plot.RunningAvgStyle = LineStyleEnum[props.RunningAvgStyle];
	plot.RunningAvgThicknessInches = props.RunningAvgThicknessInches;

	plot.GridColor = props.GridColor;
	plot.GridLineStyle = LineStyleEnum[props.GridLineStyle];
	plot.GridLineThicknessInches = props.GridLineThicknessInches;

	plot.TitleFontSizePoints = props.TitleFontSizePoints;
	plot.AxisLabelFontSizePoints = props.AxisLabelFontSizePoints;
	plot.TickLabelFontSizePoints = props.TickLabelFontSizePoints;

	plot.LeftPaddingInches = props.LeftPaddingInches;
	plot.RightPaddingInches = props.RightPaddingInches;
	plot.TopPaddingInches = props.TopPaddingInches;
	plot.BottomPaddingInches = props.BottomPaddingInches;

	plot.TickLengthInches = props.TickLengthInches;

	// Layout is stored twice in SaveCE — we honor the last one
	plot.Layout = props.Layout;

	// -------------------------------------------------------------
	// Trending properties
	// -------------------------------------------------------------
	plot.TrendOneEnable = props.TrendOneEnable;
	plot.TrendOneColor = props.TrendOneColor;
	plot.TrendOneStyle = LineStyleEnum[props.TrendOneStyle];
	plot.TrendOneThickness = props.TrendOneThickness;
	plot.TrendOneYear = props.TrendOneYear;

	plot.TrendTwoEnable = props.TrendTwoEnable;
	plot.TrendTwoColor = props.TrendTwoColor;
	plot.TrendTwoStyle = LineStyleEnum[props.TrendTwoStyle];
	plot.TrendTwoThickness = props.TrendTwoThickness;
	plot.TrendTwoYear = props.TrendTwoYear;

	plot.TrendThreeEnable = props.TrendThreeEnable;
	plot.TrendThreeColor = props.TrendThreeColor;
	plot.TrendThreeStyle = LineStyleEnum[props.TrendThreeStyle];
	plot.TrendThreeThickness = props.TrendThreeThickness;
	plot.TrendThreeYear = props.TrendThreeYear;

	// ---------------- CE-Specific -------------------------
	// PNGBytes is assigned in LoadCE after ExtractFile()
	// so nothing to do here.
}

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

	// -------------------------------------------------------------
	// 4. Clear existing document
	// -------------------------------------------------------------
	m_arrPages.clear();
	Pages = 0;

	XmlNodeType eType;
	CString csElement;

	shared_ptr<CPage> pCurrentPage;

	// -------------------------------------------------------------
	// 5. XML reading loop
	// -------------------------------------------------------------
	while (pReader->Read(&eType) == S_OK)
	{
		if (eType != XmlNodeType_Element)
			continue;

		const WCHAR* pwszName = nullptr;
		pReader->GetLocalName(&pwszName, nullptr);
		csElement = pwszName;

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
			CString csNumber, csType, csLayout;

			if (pReader->MoveToFirstAttribute() == S_OK)
			{
				do
				{
					const WCHAR* pwszAttr = nullptr;
					const WCHAR* pwszVal = nullptr;
					pReader->GetLocalName(&pwszAttr, nullptr);
					pReader->GetValue(&pwszVal, nullptr);

					if (wcscmp(pwszAttr, L"number") == 0)
						csNumber = pwszVal;
					else if (wcscmp(pwszAttr, L"type") == 0)
						csType = pwszVal;
					else if (wcscmp(pwszAttr, L"layout") == 0)
						csLayout = pwszVal;

				} while (pReader->MoveToNextAttribute() == S_OK);
			}

			UINT nPage = _wtoi(csNumber);

			CPage::PAGE_TYPE eTypePage = CPage::pageGraph;
			if (csType == L"Cover")  eTypePage = CPage::pageCover;
			else if (csType == L"TOC") eTypePage = CPage::pageTOC;

			// Construct page using your actual constructor
			pCurrentPage = std::make_shared<CPage>(nPage, csLayout, this, eTypePage);

			// Append to CSmartArray
			m_arrPages.append(pCurrentPage);
			Pages = (UINT)m_arrPages.Count;

			continue;
		}

		// ---------------------------------------------------------
		// Plot element
		// ---------------------------------------------------------
		if (csElement == L"Plot")
		{
			PlotProps temp;
			CString csImageFilename;

			// Read plot properties
			while (pReader->Read(&eType) == S_OK)
			{
				if (eType == XmlNodeType_Element)
				{
					const WCHAR* pwszPlotName = nullptr;
					pReader->GetLocalName(&pwszPlotName, nullptr);
					CString csPlotName = pwszPlotName;

					CString csValue = ReadValueAttribute(pReader);

					if (csPlotName == L"Image")
					{
						csImageFilename = csValue;
					}
					else if (IsPlotProperty(csPlotName))
					{
						AssignPlotPropertyToTemp(temp, csPlotName, csValue);
					}
				}
				else if (eType == XmlNodeType_EndElement)
				{
					const WCHAR* pwszEnd = nullptr;
					pReader->GetLocalName(&pwszEnd, nullptr);
					if (wcscmp(pwszEnd, L"Plot") == 0)
						break;
				}
			}

			// Create plotter
			auto pPlot = std::make_shared<CGraphPlotter>();
			ApplyPlotPropsToPlotter(*pPlot, temp);

			// Load PNG bytes from ZIP
			std::vector<uint8_t> pngBytes;
			if (zip.ExtractFile(csImageFilename, pngBytes))
			{
				pPlot->BytesPNG = pngBytes; // your member for CE images
			}

			// Add plot to page using GraphTitle as key
			CString csKey = pPlot->GraphTitle;
			pCurrentPage->Plots.add(csKey, pPlot);

			continue;
		}
	}

	zip.Close();
	return TRUE;
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
