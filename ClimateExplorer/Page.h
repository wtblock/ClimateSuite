/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "KeyedCollection.h"
#include "GraphPlotter.h"
#include "CHelper.h"
#include <xmllite.h>
#include "PageContent.h"

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
// CPage
//
// Represents a single page in a ClimateExplorer document. Each page contains
// a boundary rectangle (which varies for even/odd pages) and a collection of image
// rectangles describing where each image will be placed.
//
// Purpose:
//   • Provide a lightweight container for all layout information needed to
//     render a single page of the climate book.
//   • Store the page number, page geometry, and the set of
//     image rectangles assigned to that page.
//   • Allow ClimateExplorerView and the PDF export system to render pages
//     consistently and device‑independently.
//
// Why this class exists:
//   ClimateExplorer builds pages dynamically based on the the Layout:
//   • Full page - the image fills the margin rectangle rotated based
//     on the page number being even or odd.
//   • Half page - the images are not rotated but are either on the top 
//     half or bottom half of the page.
//   • Quarter page - the images are rotated similar to Full page, but
//     there are up to fore images on the page.
//
// Responsibilities:
//   • Store the page number (even pages on the left, odd pages on the right).
//   • Store the page boundary rectangle (in logical units).
//   • Store the title associated with the images on the page.
//   • Store a keyed collection of image rectangles (image name → CRect).
//   • Provide AddAnImage() to attach an image and its layout rectangle.
//   • Provide RenderImageRectangles() for debugging or preview rendering.
//
// Interaction with other components:
//   • CClimateExplorerDoc creates and populates CPage objects during
//     GeneratePages().
//   • CClimateExplorerView uses CPage to draw images, margins, and page
//     geometry on screen.
//   • PDF export uses CPage rectangles to place images at high DPI.
//   • The Properties pane uses page numbers to navigate the document.
//
// This class is the fundamental building block of ClimateExplorer’s page
// layout system, encapsulating all geometry and placement information
// needed to render a complete page.
/////////////////////////////////////////////////////////////////////////////
class CPage
{
// public typedefs
public:
	typedef enum PAGE_TYPE
	{
		pageCover,
		pageTOC,
		pageGraph,
		pageImage,
		pageMD,
		pageHTML,
		pageMap
	} PAGE_TYPE;

// protected data
protected:
	// the page number
	UINT m_nPage;

	// Layout (Full, Half, Quarter)
	CString m_csLayout;

	// page type
	PAGE_TYPE m_ePageType;

	// logical pixels per inch
	int m_nMap;

	// height of page in inches
	double m_dHeightOfPage;

	// width of page in inches
	double m_dWidthOfPage;

	// top margin in inches
	double m_dTopMargin;

	// bottom margin in inches
	double m_dBottomMargin;

	// inside margin in inches
	double m_dInsideMargin;

	// outside margin in inches
	double m_dOutsideMargin;

	// gutter in inches
	double m_dGutter;

	// page boundary rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left and odd pages are on the right of the
	// printed document.
	CRect m_Rect;

	// title of the page
	CString m_csTitle;

	// pointer to our document
	CClimateExplorerDoc* m_pDoc;

	// the key is the content title
	CKeyedCollection<CString, CPageContent> m_arrContent;

// public properties
public:
	// current page
	UINT GetPage()
	{
		return m_nPage;
	}
	// current page
	void SetPage(UINT value)
	{
		m_nPage = value;
	}
	// current page
	__declspec( property( get = GetPage, put = SetPage ) )
		UINT Page;

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

	// top margin in inches
	double GetTopMargin()
	{
		return m_dTopMargin;
	}
	// top margin in inches
	void SetTopMargin(double value)
	{
		m_dTopMargin = value;
	}
	// top margin in inches
	__declspec(property(get = GetTopMargin, put = SetTopMargin))
		double TopMargin;

	// bottom margin in inches
	double GetBottomMargin()
	{
		return m_dBottomMargin;
	}
	// bottom margin in inches
	void SetBottomMargin(double value)
	{
		m_dBottomMargin = value;
	}
	// bottom margin in inches
	__declspec(property(get = GetBottomMargin, put = SetBottomMargin))
		double BottomMargin;

	// inside margin in inches
	double GetInsideMargin()
	{
		return m_dInsideMargin;
	}
	// inside margin in inches
	void SetInsideMargin(double value)
	{
		m_dInsideMargin = value;
	}
	// inside margin in inches
	__declspec(property(get = GetInsideMargin, put = SetInsideMargin))
		double InsideMargin;

	// outside margin in inches
	double GetOutsideMargin()
	{
		return m_dOutsideMargin;
	}
	// outside margin in inches
	void SetOutsideMargin(double value)
	{
		m_dOutsideMargin = value;
	}
	// outside margin in inches
	__declspec(property(get = GetOutsideMargin, put = SetOutsideMargin))
		double OutsideMargin;

	// gutter in inches
	double GetGutter()
	{
		return m_dGutter;
	}
	// gutter in inches
	void SetGutter(double value)
	{
		m_dGutter = value;
	}
	// gutter in inches
	__declspec(property(get = GetGutter, put = SetGutter))
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

	// logical pixels per inch
	int GetMap()
	{
		return m_nMap;
	}
	// logical pixels per inch
	void SetMap(int value)
	{
		m_nMap = value;
	}
	// logical pixels per inch
	__declspec(property(get = GetMap, put = SetMap))
		int Map;

	// convert logical co-ordinate value to inches
	double LogicalToInches( int nValue )
	{
		const int nMap = Map;
		return ( double( nValue ) / nMap );
	}

	// convert inches to logical co-ordinate value
	int InchesToLogical( double dValue )
	{
		const int nMap = Map;
		return int( dValue * nMap );
	}

	// The margin rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left side of the printed book and odd pages
	// are on the right side of the printed book
	CRect GetMarginRectangle()
	{
		const double dTopOfPage = TopOfPage;
		const double dBottomOfPage = BottomOfPage;
		const double dRightOfPage = WidthOfPage;

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

	// number of images on the page
	UINT GetImageCount()
	{
		return (UINT)m_arrContent.Count;
	}
	// number of images on the page
	__declspec( property( get = GetImageCount) )
		UINT ImageCount;

	// Layout (Full, Half, Quarter)
	CString GetLayout()
	{
		return m_csLayout;
	}
	// Layout (Full, Half, Quarter)
	void SetLayout(CString value)
	{
		m_csLayout = value;
	}
	// Layout (Full, Half, Quarter)
	__declspec(property(get = GetLayout, put = SetLayout))
		CString Layout;

	// page type
	PAGE_TYPE GetPageType()
	{
		return m_ePageType;
	}
	// page type
	void SetPageType(PAGE_TYPE value)
	{
		m_ePageType = value;
	}
	// page type
	__declspec(property(get = GetPageType, put = SetPageType))
		PAGE_TYPE PageType;

	// maximum image count
	UINT GetMaximumImages()
	{
		UINT value = 1;
		CString csLayout = Layout;
		if (csLayout == L"Half")
		{
			value = 2;
		}
		else if (csLayout == L"Quarter")
		{
			value = 4;
		}

		return value;
	}
	// maximum image count
	__declspec( property( get = GetMaximumImages) )
		UINT MaximumImages;

	// is the page full of images
	bool GetPageIsFull()
	{
		CString csTitle = Title;
		if (csTitle == L"Cover Page" || csTitle == L"Table of Contents")
		{
			return true;
		}

		bool value = false;
		UINT nImages = ImageCount;
		UINT nMax = MaximumImages;
		value = nImages == nMax;
		return value;
	}
	// is the page full of images
	__declspec(property(get = GetPageIsFull))
		bool PageIsFull;

	// page boundary rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left and odd pages are on the right of the
	// printed document.
	CRect GetRect()
	{
		return m_Rect;
	}
	// page boundary rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left and odd pages are on the right of the
	// printed document.
	void SetRect(CRect value)
	{
		m_Rect = value;
	}
	// page boundary rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left and odd pages are on the right of the
	// printed document.
	__declspec( property( get = GetRect, put = SetRect ) )
		CRect Rect;

	// title of the page
	CString GetTitle()
	{
		CString value;
		CPage::PAGE_TYPE eType = PageType;

		switch (eType)
		{
		case CPage::pageCover:
			value = L"Cover Page";
			break;
		case CPage::pageTOC:
			value = L"Table of Contents";
			break;
		case CPage::pageGraph:
			if (m_arrContent.Count != 0)
			{
				shared_ptr<CPageContent> pContent = 
					m_arrContent.Items.begin()->second;
				value = pContent->Title;
			}
		}
		return value;
	}
	// title of the page
	__declspec(property(get = GetTitle))
		CString Title;

	// the key is the image title associated with the given plot
	CKeyedCollection<CString, CPageContent>& GetContent()
	{
		return m_arrContent;
	}
	// the key is the image title associated with the given plot
	void SetContent(CKeyedCollection<CString, CPageContent>& pContent)
	{
		m_arrContent = pContent;
	}
	// the key is the image title associated with the given plot
	__declspec(property(get = GetContent, put = SetContent))
		CKeyedCollection<CString, CPageContent>& Content;

	// rectangles corresponding to the images based on
	// the current page number and layout
	vector<CRect> GetRectangles();
	// rectangles corresponding to the images based on
	// the current page number and layout
	__declspec(property(get = GetRectangles))
		vector<CRect> Rectangles;

// protected methods
protected:

// public methods
public:
	void WriteXml(IXmlWriter* pWriter);
	void ReadXml(IXmlReader* pReader);

	// add an image to the page
	bool AddAnImage(shared_ptr<CGraphPlotter> pPlot);

	// render the image rectangles
	void RenderImageRectangles(CDC* pDC)
	{
		vector<CRect> arrRect = Rectangles;

		for (auto& rectPlot : arrRect )
		{
			pDC->Rectangle(rectPlot);
		}
	}

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CPage
	(
		UINT nPage, CString csLayout, 
		CClimateExplorerDoc* pDoc,
		CPage::PAGE_TYPE
	);
	~CPage()
	{

	}
};

/////////////////////////////////////////////////////////////////////////////
