/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "KeyedCollection.h"
#include "GraphPlotter.h"
#include "CHelper.h"

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
// protected data
protected:
	// the page number
	UINT m_nPage;

	// height of page in inches
	double m_dHeightOfPage;

	// width of page in inches
	double m_dWidthOfPage;

	// page boundary rectangle varies depending on the page
	// number being an even or odd number. Even pages are
	// on the left and odd pages are on the right of the
	// printed document.
	CRect m_Rect;

	// Layout (Full, Half, Quarter)
	CString m_csLayout;

	// title of the page
	CString m_csTitle;

	// the key is the image name associated with the given rectangle the
	// image will be drawn in
	CKeyedCollection<CString, CRect> m_arrRectangles;

	// the key is the image title associated with the given plot
	CKeyedCollection<CString, CGraphPlotter> m_arrPlots;

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
		ComputeRectangles();
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

	// logical pixels per inch
	static int GetMap()
	{
		return 1000;
	}
	// logical pixels per inch
	__declspec(property(get = GetMap))
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
		return (UINT)m_arrRectangles.Count;
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
		if (m_csTitle.IsEmpty())
		{
			if (Page == 1)
			{
				Title = L"Cover Page";
			}
			else if (Page == 2)
			{
				Title = L"Table of Contents";
			}
			else if (!Plots.Count == 0)
			{
				for (auto& plot : Plots.Items)
				{
					Title = plot.second->GraphTitle;
					break;
				}
			}
		}
		return m_csTitle;
	}
	// title of the page
	void SetTitle(CString value)
	{
		m_csTitle = value;
	}
	// title of the page
	__declspec(property(get = GetTitle, put = SetTitle))
		CString Title;

	// image names and rectangles
	CKeyedCollection<CString, CRect>& GetRectangles()
	{
		return m_arrRectangles;
	}
	// image names and rectangles
	__declspec(property(get = GetRectangles))
		CKeyedCollection<CString, CRect>& Rectangles;

	// the key is the image title associated with the given plot
	CKeyedCollection<CString, CGraphPlotter>& GetPlots()
	{
		return m_arrPlots;
	}
	// the key is the image title associated with the given plot
	__declspec(property(get = GetPlots))
		CKeyedCollection<CString, CGraphPlotter>& Plots;

	// is the page full of images
	bool GetPageIsFull()
	{
		CString csTitle = Title;
		if (csTitle == L"Cover Page" || csTitle == L"Table of Contents")
		{
			return true;
		}

		bool value = false;
		int nImages = ImageCount;
		CString csLayout = Layout;
		if (csLayout == L"Full")
		{
			value = nImages == 1;
		}
		else if (csLayout == L"Half")
		{
			value = nImages == 2;
		}
		else if (csLayout == L"Quarter")
		{
			value = nImages == 4;
		}

		return value;
	}
	// is the page full of images
	__declspec(property(get = GetPageIsFull))
		bool PageIsFull;

// protected methods
protected:
	// after getting a new page number, rectangles
	// may need adjusting
	void ComputeRectangles();

// public methods
public:
	// add an image to the page
	bool AddAnImage(shared_ptr<CGraphPlotter> pPlot);

	// render the image rectangles
	void RenderImageRectangles(CDC* pDC)
	{
		for (auto& image : m_arrRectangles.Items)
		{
			shared_ptr<CRect> pRect = image.second;
			pDC->Rectangle(pRect.get());
		}
	}

	// offset rectangles
	void OffsetRectangles(int nX, int nY);

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CPage()
	{
		Page = 0;
		m_Rect.SetRectEmpty();
		Layout = L"Half";
		HeightOfPage = 11.0;
		WidthOfPage = 8.5;
		
	}
	CPage( UINT nPage, CString csLayout )
	{
		m_Rect.SetRectEmpty();
		HeightOfPage = 11.0;
		WidthOfPage = 8.5;
		Layout = csLayout;

		// page must be set after the above because the page number
		// calculates the rectangle of the page
		Page = nPage;
	}
	~CPage()
	{

	}
};

/////////////////////////////////////////////////////////////////////////////
