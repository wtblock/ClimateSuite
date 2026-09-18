/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MapRenderer.h"
#include "MapOSM.h"
#include "ImagePlus.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Convert latitude/longitude to pixel coordinates inside the stitched bitmap
bool CMapRenderer::LatLonToPixel(double dLat, double dLon, int& x, int& y)
{
	x = 0;
	y = 0;

	if (MapOSM == nullptr)
		return false;

	shared_ptr<CMapTileGrid> pGrid = m_pMapOSM->TileGrid;
	if (pGrid == nullptr)
		return false;

	const int nZoom = pGrid->Zoom;

	// clamp latitude to valid OSM range
	const double dLatMax = 85.05112878;
	if (dLat > dLatMax) dLat = dLatMax;
	if (dLat < -dLatMax) dLat = -dLatMax;

	const double dPi = 3.14159265358979323846;
	const double dRadLat = dLat * dPi / 180.0;
	const double dRadLon = dLon * dPi / 180.0;

	const double dZoomFactor = pow(2.0, (double)nZoom);
	const double dMapSize = dZoomFactor * 256.0;

	// global pixel coordinates
	const double globalX = (dLon + 180.0) / 360.0 * dMapSize;
	const double globalY =
		(1.0 - log(tan(dRadLat) + 1.0 / cos(dRadLat)) / dPi) / 2.0 * dMapSize;

	// stitched bitmap offset
	const int tileXMin = pGrid->TileXMin;
	const int tileYMin = pGrid->TileYMin;

	const double offsetX = tileXMin * 256.0;
	const double offsetY = tileYMin * 256.0;

	// final pixel coordinates inside stitched bitmap
	x = (int)(globalX - offsetX);
	y = (int)(globalY - offsetY);

	return true;
} // LatLonToPixel

/////////////////////////////////////////////////////////////////////////////
bool CMapRenderer::DrawSinglePin
(
	CDC* pDC, int x, int y, Gdiplus::Color Color
)
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

	CBrush brush(Color.ToCOLORREF());
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	CPen pen(PS_SOLID, 1, Color.ToCOLORREF());
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
// Draw pins
void CMapRenderer::DrawPins(CDC* pDC)
{
	if (!pDC || !MapOSM)
		return;

	for (auto& pin : *Pins)
	{
		int px = 0;
		int py = 0;

		if (LatLonToPixel(pin.Lat, pin.Lon, px, py))
		{
			DrawSinglePin(pDC, px, py, pin.Color);
		}
	}
} // DrawPins

/////////////////////////////////////////////////////////////////////////////
