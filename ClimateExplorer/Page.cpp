/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Page.h"
#include "ClimateExplorerDoc.h"
#include "ClimateExplorerView.h"

/////////////////////////////////////////////////////////////////////////////
CPage::CPage
(
	UINT nPage, CString csLayout, 
	CClimateExplorerDoc* pDoc, 
	CPage::PAGE_TYPE eType
)
{
	Page = nPage;
	Layout = csLayout;
	PageType = eType;

	HeightOfPage = pDoc->HeightOfPage;
	WidthOfPage = pDoc->WidthOfPage;
	Map = pDoc->Map;
	TopMargin = pDoc->TopMargin;
	BottomMargin = pDoc->BottomMargin;
	InsideMargin = pDoc->InsideMargin;
	OutsideMargin = pDoc->OutsideMargin;
	Gutter = pDoc->Gutter;

	Rect = MarginRectangle;
}

/////////////////////////////////////////////////////////////////////////////
// add an image to the page
bool CPage::AddAnImage(shared_ptr<CGraphPlotter> pPlot)
{
	CString csImage = pPlot->GraphTitle;
	bool value = false;
	if (PageIsFull)
	{
		return value;
	}
	if (m_arrPlots.Exists[csImage])
	{
		return value;
	}

	value = true;

	m_arrPlots.add(csImage, pPlot);

	return value;
} // AddAnImage

/////////////////////////////////////////////////////////////////////////////
// after getting a new page number, rectangles
// may need adjusting
vector<CRect> CPage::GetRectangles()
{
	vector<CRect> value;

	Rect = MarginRectangle;
	CRect rect = Rect;
	int nHeight = rect.Height();
	int nWidth = rect.Width();
	CString csLayout = Layout;

	int nRect = 0;
	for (auto& node : m_arrPlots.Items)
	{
		// initialize the image rectangle to the margin rectangle which 
		// also applies to full page layout
		CRect rectPlot = rect;

		// based on the position in the collection and the layout
		// of the page, calculate the image rectangle's size
		if (csLayout == L"Half")
		{
			if (nRect == 0)
			{
				rectPlot.bottom = rectPlot.top + nHeight / 2;
			}
			else
			{
				rectPlot.top = rectPlot.bottom - nHeight / 2;
			}
		}
		else if (csLayout == L"Quarter")
		{
			switch (nRect)
			{
			case 0: // upper left quadrant
				rectPlot.right = rectPlot.left + nWidth / 2;
				rectPlot.bottom = rectPlot.top + nHeight / 2;
				break;
			case 1: // upper right quadrant
				rectPlot.left = rectPlot.right - nWidth / 2;
				rectPlot.bottom = rectPlot.top + nHeight / 2;
				break;
			case 2: // lower left quadrant
				rectPlot.right = rectPlot.left + nWidth / 2;
				rectPlot.top = rectPlot.bottom - nHeight / 2;
				break;
			case 3: // lower right quadrant
				rectPlot.left = rectPlot.right - nWidth / 2;
				rectPlot.top = rectPlot.bottom - nHeight / 2;
				break;
			}
		}

		// store the plot rectangle in the output vector
		value.push_back(rectPlot);

		// update the position in the collection
		nRect++;
	}

	return value;

} // ComputeRectangles

/////////////////////////////////////////////////////////////////////////////
