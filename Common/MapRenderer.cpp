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
// Prepare renderer
bool CMapRenderer::PrepareRenderer()
{
	// ---------------------------------------------------------
	// Validate OSM map object
	// ---------------------------------------------------------
	if (MapOSM == nullptr)
		return false;

	// ---------------------------------------------------------
	// Validate stitched bitmap
	// ---------------------------------------------------------
	if (MapOSM->FinalBitmap == nullptr)
		return false;

	// ---------------------------------------------------------
	// Validate target rectangle
	// ---------------------------------------------------------
	if (TargetRect.IsRectEmpty())
		return false;

	return true;
} // PrepareRenderer

/////////////////////////////////////////////////////////////////////////////
// Draw stitched OSM map
void CMapRenderer::DrawMap(CDC* pDC)
{
	if (!pDC || !MapOSM || !MapOSM->FinalBitmap)
		return;

	Graphics g(pDC->m_hDC);

	const int nSrcWidth = MapOSM->FinalBitmap->GetWidth();
	const int nSrcHeight = MapOSM->FinalBitmap->GetHeight();

	Rect destRect
	(
		TargetRect.left,
		TargetRect.top,
		TargetRect.Width(),
		TargetRect.Height()
	);

	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.SetSmoothingMode(SmoothingModeHighQuality);

	g.DrawImage
	(
		MapOSM->FinalBitmap.get(),
		destRect,
		0, 0, nSrcWidth, nSrcHeight,
		UnitPixel
	);
} // DrawMap

/////////////////////////////////////////////////////////////////////////////
// Draw pins (placeholder)
void CMapRenderer::DrawPins(CDC* pDC)
{
	if (pDC == nullptr)
		return;

	// ---------------------------------------------------------
	// No pins yet — placeholder for future overlays
	// ---------------------------------------------------------
} // DrawPins

/////////////////////////////////////////////////////////////////////////////
