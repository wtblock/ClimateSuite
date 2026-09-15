/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MapRenderer.h"
#include "MapOSM.h"
#include "ImagePlus.h"

/////////////////////////////////////////////////////////////////////////////
bool CMapRenderer::DrawSinglePin(CDC* pDC, int x, int y, COLORREF rgbColor)
{
	bool value = false;

	if (!pDC)
	{
		return value;
	}

	// simple filled circle for now
	const int nRadius = 5;
	const int nLeft = x - nRadius;
	const int nTop = y - nRadius;
	const int nRight = x + nRadius;
	const int nBottom = y + nRadius;

	CBrush brush(rgbColor);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	CPen pen(PS_SOLID, 1, rgbColor);
	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->Ellipse(nLeft, nTop, nRight, nBottom);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	value = true;

	return value;
} // DrawSinglePin

/////////////////////////////////////////////////////////////////////////////
bool CMapRenderer::PrepareRenderer()
{
	bool value = false;

	// ensure we have a map object
	if (m_pMapOSM == nullptr)
	{
		return value;
	}

	// ensure the map has a final bitmap
	if (m_pMapOSM->FinalBitmap == nullptr)
	{
		return value;
	}

	// assign bitmap for rendering
	m_pBitmap = m_pMapOSM->FinalBitmap;

	value = true;

	return value;
} // PrepareRenderer

/////////////////////////////////////////////////////////////////////////////
bool CMapRenderer::DrawPins(CDC* pDC)
{
	bool value = false;

	if (!pDC)
	{
		return value;
	}

	// placeholder: no pin model defined yet
	// this loop will be replaced once you define pin storage

	// example: draw a single test pin at center of target rect
	const int nCenterX = (m_rectTarget.left + m_rectTarget.right) / 2;
	const int nCenterY = (m_rectTarget.top + m_rectTarget.bottom) / 2;

	DrawSinglePin(pDC, nCenterX, nCenterY, RGB(255, 0, 0));

	value = true;

	return value;
} // DrawPins

/////////////////////////////////////////////////////////////////////////////
bool CMapRenderer::DrawMap(CDC* pDC)
{
	bool value = false;

	if (!pDC)
	{
		return value;
	}

	if (m_pBitmap == nullptr)
	{
		return value;
	}

	// use your CImagePlus wrapper for mapping‑mode‑aware drawing
	CImagePlus plus(m_pBitmap);

	CRect rectSrc(0, 0, plus.Width, plus.Height);
	plus.Draw(pDC, m_rectTarget, rectSrc);

	value = true;

	return value;
} // DrawMap

/////////////////////////////////////////////////////////////////////////////
