/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Page.h"

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

	CRect rect = Rect;
	int nHeight = rect.Height();
	int nWidth = rect.Width();

	int nImages = ImageCount;
	CString csLayout = pPlot->Layout;
	if (csLayout == L"Half")
	{
		if (nImages == 0)
		{
			rect.bottom = rect.top + nHeight / 2;
		}
		else
		{
			rect.top = rect.bottom - nHeight / 2;
		}
	}
	else if (csLayout == L"Quarter")
	{
		switch (nImages)
		{
		case 0: // upper left quadrant
			rect.right = rect.left + nWidth / 2;
			rect.bottom = rect.top + nHeight / 2;
			break;
		case 1: // upper right quadrant
			rect.left = rect.right - nWidth / 2;
			rect.bottom = rect.top + nHeight / 2;
			break;
		case 2: // lower left quadrant
			rect.right = rect.left + nWidth / 2;
			rect.top = rect.bottom - nHeight / 2;
			break;
		case 3: // lower right quadrant
			rect.left = rect.right - nWidth / 2;
			rect.top = rect.bottom - nHeight / 2;
			break;
		}
	}
	shared_ptr<CRect> pRect = shared_ptr<CRect>(new CRect( &rect ));
	m_arrRectangles.add(csImage, pRect);
	m_arrPlots.add(csImage, pPlot);

	return value;
} // AddAnImage

/////////////////////////////////////////////////////////////////////////////
// offset rectangles
void CPage::OffsetRectangles(int nX, int nY)
{
	m_Rect.OffsetRect(nX, nY);

	for (auto& node : m_arrRectangles.Items)
	{
		node.second->OffsetRect( nX, nY );
	}

} // OffsetRectangles

/////////////////////////////////////////////////////////////////////////////
// after getting a new page number, rectangles
// may need adjusting
void CPage::ComputeRectangles()
{
	Rect = MarginRectangle;
	CRect rect = Rect;
	int nHeight = rect.Height();
	int nWidth = rect.Width();
	CString csLayout = Layout;

	int nRect = 0;
	for (auto& node : m_arrRectangles.Items)
	{
		shared_ptr<CRect> pRect = node.second;

		// initialize the image rectangle to the margin rectangle which 
		// also applies to full page layout
		*pRect = rect;

		// based on the position in the collection and the layout
		// of the page, calculate the image rectangle's size
		if (csLayout == L"Half")
		{
			if (nRect == 0)
			{
				pRect->bottom = pRect->top + nHeight / 2;
			}
			else
			{
				pRect->top = pRect->bottom - nHeight / 2;
			}
		}
		else if (csLayout == L"Quarter")
		{
			switch (nRect)
			{
			case 0: // upper left quadrant
				pRect->right = pRect->left + nWidth / 2;
				pRect->bottom = pRect->top + nHeight / 2;
				break;
			case 1: // upper right quadrant
				pRect->left = pRect->right - nWidth / 2;
				pRect->bottom = pRect->top + nHeight / 2;
				break;
			case 2: // lower left quadrant
				pRect->right = pRect->left + nWidth / 2;
				pRect->top = pRect->bottom - nHeight / 2;
				break;
			case 3: // lower right quadrant
				pRect->left = pRect->right - nWidth / 2;
				pRect->top = pRect->bottom - nHeight / 2;
				break;
			}
		}

		// update the position in the collection
		nRect++;
	}

} // ComputeRectangles

/////////////////////////////////////////////////////////////////////////////
