/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseDoc.h"
#include "ClimateExplorer.h"
#include "ClimateExplorerView.h"
#include "ClimateDatabase.h"
#include "ClimateTemperature.h"
#include "Page.h"
#include "SmartArray.h"

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc : public CBaseDoc
{
	// public definitions
public:
	typedef CKeyedCollection<CString, Image> MAP_IMAGES;
	typedef CKeyedCollection<CString, MAP_IMAGES> MAP_ALBUM;
	typedef CKeyedCollection<CString, CString> MAP_INDEX;

// protected data
protected:
	DECLARE_DYNCREATE(CClimateExplorerDoc)

	// collection of albums (folders) where each album is a collection
	// of bitmaps
	MAP_ALBUM m_mapAlbums;

	// sorted folders 
	CKeyedCollection< CString, int > m_keyFolders;

	// collection of pages containing image names and rectangles
	CSmartArray<CPage> m_arrPages;

	// table of contents lines for the entire document where
	// the string is the album title and the int is the page
	// number where the album begins. Return value is the
	// number of lines.
	vector<pair<CString, int>> m_arrTOC;

	// collection of page numbers to be exported
	CKeyedCollection<UINT, UINT> m_keyExportPages;

	// number of pages in the document
	UINT m_nPages;

	// current page
	UINT m_nPage;

	// number of images in the document
	UINT m_nImages;

	// height of document in inches
	double m_dHeight;

	// height of page in inches
	double m_dHeightOfPage;

	// width of page in inches
	double m_dWidthOfPage;

	// document export folder
	CString m_csExportFolder;

	// document export pages: blank for all, 
	// example: 5-9, 11, 15 mean pages 5 through 9 and pages 11 and 15
	CString m_csExportPages;

	// Dots Per Inch when exporting the document
	UINT m_nExportDPI;

	// Image quality in percent between 1 and 100
	// document title
	UINT m_nExportQuality;

	// document title
	CString m_csTitle;

	// document subtitle
	CString m_csSubtitle;

	// document publisher
	CString m_csPublisher;

	// document ISBN
	CString m_csISBN;

	// document copyright
	CString m_csCopyright;

	// document description
	CString m_csDescription;

	// working folder
	CString m_csWorkingFolder;

	// ===============================================
	// Query Properties (new Climate Explorer section)
	// ===============================================

	// 1. Type of Query
	CString m_csQueryType;

	// 2. Natural Language Query
	CString m_csNaturalLanguage;

	// 3. SQL query
	CString m_csSQL;

	// 4B. Pure meaning no estimated data
	bool m_bPure;

	// 4D. Scope (National, State, or Location)
	CString m_csScope;

	// 4E. State postal code (AL, TX, CA, etc.) location of station
	CString m_csState;

	// 4F. Location is the name of the cite the station is in
	CString m_csLocation;

	// 4G. Starting Year
	long m_lYearStart; 

	// 4H. Ending Year
	long m_lYearEnd; 

	// 4I. Type (Maximum, Minimum, Average)
	CClimateTemperature::MEASURE_TYPE m_eMeasurementType;

	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	int m_nThreshold;

	// 4K. Threshold versus Temperature query
	int m_bTemperature;

	// 5A. Units (degF, degC, raw)
	CString m_csUnits;

	// 5B. Output (Plot, Table, Map + Plot)
	CString m_csOutput;

	// 5C. Layout (Full, Half, Quarter)
	CString m_csLayout;

	// 5D. Placement (append, insert, or replace)
	CString m_csPlacement;


// public properties
public:
	// the climate database is a relational database of USHCN data
	CClimateDatabase* GetClimateDatabase()
	{
		CClimateDatabase* value = nullptr;
		CClimateExplorerApp* pApp = (CClimateExplorerApp*)AfxGetApp();
		if (pApp != nullptr)
		{
			value = pApp->ClimateDatabase;
		}
		return value;
	}
	// the climate database is a relational database of USHCN data
	__declspec(property(get = GetClimateDatabase))
		CClimateDatabase* ClimateDatabase;

	// number of albums in the document
	UINT GetAlbumCount()
	{
		UINT value = m_mapAlbums.Count;
		return value;
	}
	// number of albums in the document
	__declspec(property(get = GetAlbumCount))
		UINT AlbumCount;

	// number of pages in the table of contents
	UINT GetTableOfContentsPages()
	{
		// take into account the Title Page and Table of Contents lines that are at
		// the beginning of the first page of the table of contents.
		UINT nAlbums = 2;

		// add the actual number of albums which represents lines int th
		// table of contents
		nAlbums += AlbumCount;

		// place 55 lines on each page
		UINT value = nAlbums / 55;

		// if there is a fraction of a page, add one more page to account for it.
		UINT nMod = nAlbums % 55;
		if (nMod > 0)
		{
			value++;
		}
		return value;
	}
	// number of pages in the table of contents
	__declspec(property(get = GetTableOfContentsPages))
		UINT TableOfContentsPages;

	// number of pages in the document
	virtual UINT GetPages()
	{
		return m_nPages;
	}
	// number of pages in the document
	void SetPages(UINT value)
	{
		m_nPages = value;
	}
	// number of pages in the document
	__declspec(property(get = GetPages, put = SetPages))
		UINT Pages;

	// current page number of the entire document
	// including the overhead pages of title and
	// table of contents.
	virtual UINT GetPage()
	{
		return m_nPage;
	}
	// current page number of the entire document
	// including the overhead pages of title and
	// table of contents.
	void SetPage(UINT value)
	{
		m_nPage = value;
	}
	// current page number of the entire document
	// including the overhead pages of title and
	// table of contents.
	__declspec(property(get = GetPage, put = SetPage))
		UINT Page;

	// current page object where page objects contain
	// the rendered images and the indexing is zero
	// based compared to document pages that are one based 
	// and include overhead pages for the title page and 
	// the table of contents page.
	shared_ptr<CPage> GetCurrentPage()
	{
		shared_ptr<CPage> value;

		// this is a one based page number: 1..n
		UINT nPage = Page;
		UINT nPagesTOC = TableOfContentsPages;
		UINT nOverhead = nPagesTOC + 1;
		if (nPage > nOverhead)
		{
			// this is a zero based index accounting for 
			// overhead pages, so we need to decrement
			// by overhead + 1 to make zero based
			nPage -= (nOverhead + 1);
			const int nPages = m_arrPages.Count;
			value = m_arrPages.get((long)nPage);
		}
		return value;
	}
	// current page object where page objects contain
	// the rendered images and the indexing is zero
	// based compared to document pages that are one based 
	// and include overhead pages for the title page and 
	// the table of contents page.
	__declspec(property(get = GetCurrentPage))
		shared_ptr<CPage> CurrentPage;

	// height of page in inches
	virtual double GetHeightOfPage()
	{
		return m_dHeightOfPage;
	}
	// height of page in inches
	void SetHeightOfPage(double value)
	{
		m_dHeightOfPage = value;
	}
	// height of page in inches
	__declspec(property(get = GetHeightOfPage, put = SetHeightOfPage))
		double HeightOfPage;

	// width of page in inches
	virtual double GetWidthOfPage()
	{
		return m_dWidthOfPage;
	}
	// width of page in inches
	void SetWidthOfPage(double value)
	{
		m_dWidthOfPage = value;
	}
	// width of page in inches
	__declspec(property(get = GetWidthOfPage, put = SetWidthOfPage))
		double WidthOfPage;

	// top of page in inches
	double GetTopOfPage()
	{
		double value = 0;
		const UINT nPage = Page;
		const double dHeight = HeightOfPage;
		value = dHeight * (nPage - 1);
		return value;
	}
	// top of page in inches
	__declspec(property(get = GetTopOfPage))
		double TopOfPage;

	// bottom of page in inches
	double GetBottomOfPage()
	{
		double value = TopOfPage;
		const double dHeight = HeightOfPage;
		value += dHeight;
		return value;
	}
	// bottom of page in inches
	__declspec(property(get = GetBottomOfPage))
		double BottomOfPage;

	// height of document in inches
	virtual double GetHeight()
	{
		m_dHeight = HeightOfPage * Pages;
		return m_dHeight;
	}
	// height of document in inches
	void SetHeight(double value)
	{
		m_dHeight = value;
	}
	// height of document in inches
	__declspec(property(get = GetHeight, put = SetHeight))
		double Height;

	// width of document in inches
	virtual double GetWidth()
	{
		return WidthOfPage;
	}
	// width of document in inches
	void SetWidth(double value)
	{
		WidthOfPage = value;
	}
	// width of document in inches
	__declspec(property(get = GetWidth, put = SetWidth))
		double Width;

	// top margin in inches
	double GetTopMargin()
	{
		return 0.5;
	}
	// top margin in inches
	__declspec(property(get = GetTopMargin))
		double TopMargin;

	// bottom margin in inches
	double GetBottomMargin()
	{
		return 0.5;
	}
	// bottom margin in inches
	__declspec(property(get = GetBottomMargin))
		double BottomMargin;

	// inside margin in inches
	double GetInsideMargin()
	{
		return 0.6;
	}
	// inside margin in inches
	__declspec(property(get = GetInsideMargin))
		double InsideMargin;

	// outside margin in inches
	double GetOutsideMargin()
	{
		return 0.4;
	}
	// outside margin in inches
	__declspec(property(get = GetOutsideMargin))
		double OutsideMargin;

	// gutter in inches
	double GetGutter()
	{
		return 0.2;
	}
	// gutter in inches
	__declspec(property(get = GetGutter))
		double Gutter;

	// left margin of the current page
	double GetLeftMargin()
	{
		double value = InsideMargin + Gutter;
		const UINT nPage = Page;
		const bool bEven = CHelper::GetEven(nPage);
		if (bEven)
		{
			value = OutsideMargin;
		}
		return value;
	}
	// left margin of the current page
	__declspec(property(get = GetLeftMargin))
		double LeftMargin;

	// right margin of the current page
	double GetRightMargin()
	{
		double value = InsideMargin + Gutter;
		const UINT nPage = Page;
		const bool bOdd = CHelper::GetOdd(nPage);
		if (bOdd)
		{
			value = OutsideMargin;
		}
		return value;
	}
	// right margin of the current page
	__declspec(property(get = GetRightMargin))
		double RightMargin;

	// The margin rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left side of the printed book and odd pages
	// are on the right side of the printed book
	CRect GetMarginRectangle()
	{
		const double dTopOfPage = TopOfPage;
		const double dBottomOfPage = BottomOfPage;
		const double dRightOfPage = Width;

		const double dTopMargin = TopMargin;
		const double dBottomMargin = BottomMargin;
		const double dLeftMargin = LeftMargin;
		const double dRightMargin = RightMargin;

		const int nTop = InchesToLogical(dTopOfPage + dTopMargin);
		const int nBottom = InchesToLogical(dBottomOfPage - dBottomMargin);
		const int nLeft = InchesToLogical(dLeftMargin);
		const int nRight = InchesToLogical(dRightOfPage - dRightMargin);

		CRect value(nLeft, nTop, nRight, nBottom);

		return value;
	}
	// The margin rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left side of the printed book and odd pages
	// are on the right side of the printed book
	__declspec(property(get = GetMarginRectangle))
		CRect MarginRectangle;

	// table of contents lines for the entire document where
	// the string is the album title and the int is the page
	// number where the album begins. Return value is the
	// number of lines.
	vector<pair<CString, int>>& GetAlbumTableOfContents();
	// table of contents lines for the entire document where
	// the string is the album title and the int is the page
	// number where the album begins. Return value is the
	// number of lines.
	__declspec(property(get = GetAlbumTableOfContents))
		vector<pair<CString, int>>& AlbumTableOfContents;

	// document export folder
	CString GetExportFolder()
	{
		return m_csExportFolder;
	}
	// document export folder
	void SetExportFolder(CString value)
	{
		if (!value.IsEmpty())
		{
			if (value.Right(1) != L"\\")
			{
				value += L"\\";
			}
		}
		m_csExportFolder = value;
	}
	// document export folder
	__declspec(property(get = GetExportFolder, put = SetExportFolder))
		CString ExportFolder;

	// document export pages: blank for all, 
	// example: 5-9, 11, 15 mean pages 5 through 9 and pages 11 and 15
	CString GetExportPages()
	{
		return m_csExportPages;
	}
	// document export pages: blank for all, 
	// example: 5-9, 11, 15 mean pages 5 through 9 and pages 11 and 15
	void SetExportPages(CString value)
	{
		m_csExportPages = value;
	}
	// document export pages: blank for all, 
	// example: 5-9, 11, 15 mean pages 5 through 9 and pages 11 and 15
	__declspec(property(get = GetExportPages, put = SetExportPages))
		CString ExportPages;

	// collection of page numbers to be exported
	CKeyedCollection<UINT, UINT>& GetExportPageNumbers();
	// collection of page numbers to be exported
	__declspec(property(get = GetExportPageNumbers))
		CKeyedCollection<UINT, UINT>& ExportPageNumbers;

	// Dots Per Inch when exporting the document
	UINT GetExportDPI()
	{
		return m_nExportDPI;
	}
	// Dots Per Inch when exporting the document
	void SetExportDPI(UINT value)
	{
		m_nExportDPI = value;
	}
	// Dots Per Inch when exporting the document
	__declspec(property(get = GetExportDPI, put = SetExportDPI))
		UINT ExportDPI;

	// Image quality in percent between 1 and 100
	UINT GetExportQuality()
	{
		return m_nExportQuality;
	}
	// Image quality in percent between 1 and 100
	void SetExportQuality(UINT value)
	{
		if (value < 1)
		{
			value = 75;
		}
		else if (value > 100)
		{
			value = 100;
		}
		m_nExportQuality = value;
	}
	// Image quality in percent between 1 and 100
	__declspec(property(get = GetExportQuality, put = SetExportQuality))
		UINT ExportQuality;

	// document title
	CString GetTitle()
	{
		return m_csTitle;
	}
	// document title
	void SetTitle(CString value)
	{
		m_csTitle = value;
	}
	// document title
	__declspec(property(get = GetTitle, put = SetTitle))
		CString Title;

	// document subtitle
	CString GetSubtitle()
	{
		return m_csSubtitle;
	}
	// document subtitle
	void SetSubtitle(CString value)
	{
		m_csSubtitle = value;
	}
	// document subtitle
	__declspec(property(get = GetSubtitle, put = SetSubtitle))
		CString Subtitle;

	// document publisher
	CString GetPublisher()
	{
		return m_csPublisher;
	}
	// document publisher
	void SetPublisher(CString value)
	{
		m_csPublisher = value;
	}
	// document publisher
	__declspec(property(get = GetPublisher, put = SetPublisher))
		CString Publisher;

	// document ISBN
	CString GetISBN()
	{
		return m_csISBN;
	}
	// document ISBN
	void SetISBN(CString value)
	{
		m_csISBN = value;
	}
	// document ISBN
	__declspec(property(get = GetISBN, put = SetISBN))
		CString ISBN;

	// document copyright
	CString GetCopyright()
	{
		return m_csCopyright;
	}
	// document copyright
	void SetCopyright(CString value)
	{
		m_csCopyright = value;
	}
	// document copyright
	__declspec(property(get = GetCopyright, put = SetCopyright))
		CString Copyright;

	// document description
	CString GetDescription()
	{
		return m_csDescription;
	}
	// document description
	void SetDescription(CString value)
	{
		m_csDescription = value;
	}
	// document description
	__declspec(property(get = GetDescription, put = SetDescription))
		CString Description;

	// the view associated with this document
	CClimateExplorerView* GetClimateExplorerView();
	// the view associated with this document
	__declspec(property(get = GetClimateExplorerView))
		CClimateExplorerView* ClimateExplorerView;

	// number of images in the document
	virtual UINT GetImages()
	{
		return m_nImages;
	}
	// number of images in the document
	void SetImages(UINT value)
	{
		m_nImages = value;
	}
	// number of images in the document
	__declspec(property(get = GetImages, put = SetImages))
		UINT Images;

	// optional working folder 
	inline CString GetWorkingFolder()
	{
		CClimateExplorerApp* pApp = (CClimateExplorerApp*)AfxGetApp();
		m_csWorkingFolder = pApp->WorkingFolder;
		return m_csWorkingFolder;
	}
	// optional working folder 
	__declspec(property(get = GetWorkingFolder))
		CString WorkingFolder;

	// ===============================================
	// Query Properties (new Climate Explorer section)
	// ===============================================

	// 1. Type of Query
	CString GetQueryType()
	{
		return m_csQueryType;
	}
	// 1. Type of Query
	void SetQueryType(CString value)
	{
		m_csQueryType = value;
	}
	// 1. Type of Query
	__declspec(property(get = GetQueryType, put = SetQueryType))
		CString QueryType;

	// 2. Natural Language Query
	CString GetNaturalLanguage()
	{
		return m_csNaturalLanguage;
	}
	// 2. Natural Language Query
	void SetNaturalLanguage(CString value)
	{
		m_csNaturalLanguage = value;
	}
	// 2. Natural Language Query
	__declspec(property(get = GetNaturalLanguage, put = SetNaturalLanguage))
		CString NaturalLanguage;

	// 3. SQL query
	CString GetSQL()
	{
		return m_csSQL;
	}
	// 3. SQL query
	void SetSQL(CString value)
	{
		m_csSQL = value;
	}
	// 3. SQL query
	__declspec(property(get = GetSQL, put = SetSQL))
		CString SQL;

	// 4B. Pure meaning no estimated data
	bool GetPure()
	{
		return m_bPure;
	}
	// 4B. Pure meaning no estimated data
	void SetPure(bool value)
	{
		m_bPure = value;
	}
	// 4B. Pure meaning no estimated data
	__declspec(property(get = GetPure, put = SetPure))
		bool Pure;

	// 4D. Scope (National, State, or Location)
	CString GetScope()
	{
		return m_csScope;
	}
	// 4D. Scope (National, State, or Location)
	void SetScope(CString value)
	{
		m_csScope = value;
	}
	// 4D. Scope (National, State, or Location)
	__declspec(property(get = GetScope, put = SetScope))
		CString Scope;

	// 4E. State postal code (AL, TX, CA, etc.) location of station
	CString GetState()
	{
		return m_csState;
	}
	// 4E. State postal code (AL, TX, CA, etc.) location of station
	void SetState(CString value)
	{
		m_csState = value;

		// when the state changes, for location to None because
		// the user may want all locations in the state
		Location = L"None";
	}
	// 4E. State postal code (AL, TX, CA, etc.) location of station
	__declspec(property(get = GetState, put = SetState))
		CString State;

	// 4F. Location is the name of the cite the station is in
	CString GetLocation()
	{
		return m_csLocation;
	}
	// 4F. Location is the name of the cite the station is in
	void SetLocation(CString value)
	{
		m_csLocation = value;
	}
	// 4F. Location is the name of the cite the station is in
	__declspec(property(get = GetLocation, put = SetLocation))
		CString Location;

	// 4G. Starting Year
	long GetYearStart()
	{
		return m_lYearStart;
	}
	// 4G. Starting Year
	void SetYearStart(long value)
	{
		m_lYearStart = value;
	}
	// 4G. Starting Year
	__declspec(property(get = GetYearStart, put = SetYearStart))
		long YearStart;

	// 4H. Ending Year
	long GetYearEnd()
	{
		return m_lYearEnd;
	}
	// 4H. Ending Year
	void SetYearEnd(long value)
	{
		m_lYearEnd = value;
	}
	// 4H. Ending Year
	__declspec(property(get = GetYearEnd, put = SetYearEnd))
		long YearEnd;

	// 4I. Type ("Maximum", "Minimum", "Average", and "Threshold"
	// where "Threshold" sets the type to mtMaximum and enables 
	// Threshold boolean
	CString GetMeasurementText()
	{
		CString value(L"Maximum");
		const bool bTemperature = Temperature;
		if (bTemperature)
		{
			switch (MeasurementType)
			{
			case CClimateTemperature::mtAverage:
				value = L"Average";
				break;
			case CClimateTemperature::mtMinimum:
				value = L"Minimum";
			}
		}
		else
		{
			value = L"Threshold";
		}
		return value;
	}
	// 4I. Type ("Maximum", "Minimum", "Average", and "Threshold"
	// where "Threshold" sets the type to mtMaximum and enables 
	// Threshold boolean
	void SetMeasurementText(CString value)
	{
		value.MakeLower();
		Temperature = true;
		MeasurementType = CClimateTemperature::mtMaximum;
		if (value == L"average")
		{
			MeasurementType = CClimateTemperature::mtAverage;
		}
		else if (value == L"minimm")
		{
			MeasurementType = CClimateTemperature::mtMinimum;
		}
		else if (value == L"threshold")
		{
			Temperature = false;
		}
	}
	// 4I. Type ("Maximum", "Minimum", "Average", and "Threshold"
	// where "Threshold" sets the type to mtMaximum and enables 
	// Threshold boolean
	__declspec(property(get = GetMeasurementText, put = SetMeasurementText))
		CString MeasurementText;

	// 4I. Type (Maximum, Minimum, Average)
	CClimateTemperature::MEASURE_TYPE GetMeasurementType()
	{
		return m_eMeasurementType;
	}
	// 4I. Type (Maximum, Minimum, Average)
	void SetMeasurementType(CClimateTemperature::MEASURE_TYPE value)
	{
		m_eMeasurementType = value;
	}
	// 4I. Type (Maximum, Minimum, Average)
	__declspec(property(get = GetMeasurementType, put = SetMeasurementType))
		CClimateTemperature::MEASURE_TYPE MeasurementType;

	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	CString GetThresholdText()
	{
		CString value = L"90";
		const int nThreshold = Threshold;
		value.Format(L"%d", nThreshold);
		return value;
	}
	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	void SetThresholdText(CString value)
	{
		const int nThreshold = (int)_tstol(value);
		Threshold = nThreshold;
	}
	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	__declspec(property(get = GetThresholdText, put = SetThresholdText))
		CString ThresholdText;

	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	int GetThreshold()
	{
		return m_nThreshold;
	}
	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	void SetThreshold(int value)
	{
		m_nThreshold = value;
	}
	// 4J. Threshold (90F, 95F, ... 125F) temperatures to count
	__declspec(property(get = GetThreshold, put = SetThreshold))
		int Threshold;

	// 4K. Threshold versus Temperature query
	bool GetTemperature()
	{
		return m_bTemperature;
	}
	// 4K. Threshold versus Temperature query
	void SetTemperature(bool value)
	{
		m_bTemperature = value;
	}
	// 4K. Threshold versus Temperature query
	__declspec(property(get = GetTemperature, put = SetTemperature))
		bool Temperature;


	// ===============================================
	// Render Properties 
	// ===============================================

	// 5A. Units (degF, degC, raw)
	CString GetUnits()
	{
		return m_csUnits;
	}
	// 5A. Units (degF, degC, raw)
	void SetUnits(CString value)
	{
		m_csUnits = value;
	}
	// 5A. Units (degF, degC, raw)
	__declspec(property(get = GetUnits, put = SetUnits))
		CString Units;

	// 5B. Output (Plot, Table, Map + Plot)
	CString GetOutput()
	{
		return m_csOutput;
	}
	// 5B. Output (Plot, Table, Map + Plot)
	void SetOutput(CString value)
	{
		m_csOutput = value;
	}
	// 5B. Output (Plot, Table, Map + Plot)
	__declspec(property(get = GetOutput, put = SetOutput))
		CString Output;

	// 5C. Layout (Full, Half, Quarter)
	CString GetLayout()
	{
		return m_csLayout;
	}
	// 5C. Layout (Full, Half, Quarter)
	void SetLayout(CString value)
	{
		m_csLayout = value;
	}
	// 5C. Layout (Full, Half, Quarter)
	__declspec(property(get = GetLayout, put = SetLayout))
		CString Layout;

	// 5D. Placement (append, insert, or replace)
	CString GetPlacement()
	{
		return m_csPlacement;
	}
	// 5D. Placement (append, insert, or replace)
	void SetPlacement(CString value)
	{
		m_csPlacement = value;
	}
	// 5D. Placement (append, insert, or replace)
	__declspec(property(get = GetPlacement, put = SetPlacement))
		CString Placement;


// protected methods
protected:
	// clear the document data
	void Clear()
	{
		m_mapAlbums.clear();
		m_keyFolders.clear();
		m_arrPages.clear();
		m_nPages = 2;
		Height = HeightOfPage;
	}

	void InitializeProperties();

	BOOL PromptForFileName(CString& strFilePath);

	CString BuildPickerSQL();

// public methods
public:
	bool IsNationalComposite()
	{
		return Scope == L"National" && State == L"None";
	}

	bool IsNationalMultiState()
	{
		return Scope == L"National" && State == L"All";
	}

	bool IsStateComposite()
	{
		return Scope == L"State" && Location == L"None";
	}

	bool IsStateMultiLocation()
	{
		return Scope == L"State" && Location == L"All";
	}

	bool IsSingleLocation()
	{
		return Scope == L"Location";
	}

	shared_ptr<Image> FindImage(CString csFolder, CString csImage);

	// initialize the document
	void InitDocument()
	{
		Clear();
	}

	// correct for working folder
	inline CString CorrectForWorkingFolder(CString csInput)
	{
		CString value;
		if (csInput.Left(1) == L".")
		{
			csInput.TrimLeft(L".");
			CString csWork = WorkingFolder;
			value.Format(L"%s%s", csWork, csInput);
		}
		else
		{
			value = csInput;
		}
		return value;
	}

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// protected overrides
protected:

// public overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL SaveModified();
	virtual BOOL OnSaveDocument(CString& csPath);
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
	virtual BOOL DoSave(CString& csPath, BOOL bReplace = TRUE);
	// open a data file
	virtual bool Open
	(
		LPCTSTR szFilename, // name of data file to open
		bool bRead = true, // do an initial read after open
		LPCTSTR pcszFileID = _T("F1")
	);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();

// public constructor/destructor
public:
	CClimateExplorerDoc();
	virtual ~CClimateExplorerDoc();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();

public:
	afx_msg void OnExecuteQuery();
	afx_msg void OnUpdateExecuteQuery(CCmdUI* pCmdUI);
}; // CClimateExplorerDoc

/////////////////////////////////////////////////////////////////////////////
