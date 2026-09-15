/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MapOSM.h"
#include "MapTile.h"

using namespace Gdiplus;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::DownloadSingleTile
(
	int nZoom, int nTileX, int nTileY, CString& csLocalPath
)
{
	bool value = false;

	// build the OSM tile URL
	CString csUrl;
	csUrl.Format
	(
		_T("https://tile.openstreetmap.org/%d/%d/%d.png"),
		nZoom,
		nTileX,
		nTileY
	);

	// TODO: perform HTTP GET from csUrl and save to csLocalPath
	// This placeholder preserves structure; actual download will use
	// your chosen HTTP stack (WinINet, CInternetSession, etc.).

	// for now, just report failure (no download implemented yet)
	value = false;

	return value;
} // DownloadSingleTile

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::LoadTileIntoGrid
(
	int nZoom, int nTileX, int nTileY, CString& csLocalPath
)
{
	bool value = false;

	if (m_pTileGrid == nullptr)
	{
		return value;
	}

	shared_ptr<CMapTile> pTile = make_shared<CMapTile>();
	pTile->Zoom = nZoom;
	pTile->TileX = nTileX;
	pTile->TileY = nTileY;
	pTile->LocalPath = csLocalPath;

	m_pTileGrid->AddTile(pTile);
	value = true;

	return value;
} // LoadTileIntoGrid

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::ComputeTileCoordinates
(
	double dLat, double dLon, int nZoom, int& nTileX, int& nTileY
)
{
	bool value = false;

	// clamp latitude to valid OSM range
	const double dLatMax = 85.05112878;
	if (dLat > dLatMax)
	{
		dLat = dLatMax;
	}
	if (dLat < -dLatMax)
	{
		dLat = -dLatMax;
	}

	const double dPi = 3.14159265358979323846;
	const double dRadLat = dLat * dPi / 180.0;
	const double dRadLon = dLon * dPi / 180.0;

	const double dZoomFactor = pow(2.0, (double)nZoom);

	// OSM tile X/Y computation
	const double dX = (dLon + 180.0) / 360.0 * dZoomFactor;
	const double dY = (1.0 - log(tan(dRadLat) + 1.0 / cos(dRadLat)) / dPi) / 2.0 * dZoomFactor;

	nTileX = (int)floor(dX);
	nTileY = (int)floor(dY);

	value = true;

	return value;
} // ComputeTileCoordinates

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::BuildTileGrid
(
	double dLatCenter, double dLonCenter, int nZoom, int nTileRadius
)
{
	bool value = false;

	InitializeEmptyGrid();

	if (m_pTileGrid == nullptr)
	{
		return value;
	}

	int nTileXCenter = 0;
	int nTileYCenter = 0;

	if (!ComputeTileCoordinates(dLatCenter, dLonCenter, nZoom, nTileXCenter, nTileYCenter))
	{
		return value;
	}

	const bool bBoundsOK = m_pTileGrid->ComputeBounds
	(
		nTileXCenter,
		nTileYCenter,
		nTileRadius
	);
	if (!bBoundsOK)
	{
		return value;
	}

	// populate the grid with empty tiles (paths will be filled during download)
	const int nXMin = m_pTileGrid->TileXMin;
	const int nXMax = m_pTileGrid->TileXMax;
	const int nYMin = m_pTileGrid->TileYMin;
	const int nYMax = m_pTileGrid->TileYMax;

	for (int nX = nXMin; nX <= nXMax; ++nX)
	{
		for (int nY = nYMin; nY <= nYMax; ++nY)
		{
			CString csLocalPath; // will be set later in DownloadTiles
			LoadTileIntoGrid(nZoom, nX, nY, csLocalPath);
		}
	}

	value = true;

	return value;
} // BuildTileGrid

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::DownloadTiles(LPCTSTR lpszCacheFolder)
{
	bool value = false;

	if (m_pTileGrid == nullptr)
	{
		return value;
	}

	vector<shared_ptr<CMapTile>>& vTiles = m_pTileGrid->Tiles;
	const size_t nCount = vTiles.size();

	for (size_t i = 0; i < nCount; ++i)
	{
		shared_ptr<CMapTile> pTile = vTiles[i];
		if (!pTile)
		{
			continue;
		}

		const int nZoom = pTile->Zoom;
		const int nTileX = pTile->TileX;
		const int nTileY = pTile->TileY;

		CString csLocalPath;
		csLocalPath.Format
		(
			_T("%s\\%d_%d_%d.png"),
			lpszCacheFolder,
			nZoom,
			nTileX,
			nTileY
		);

		if (!DownloadSingleTile(nZoom, nTileX, nTileY, csLocalPath))
		{
			continue;
		}

		pTile->LocalPath = csLocalPath;
	}

	// for now, report success if we have at least one tile with a path
	bool bAnyTile = false;
	for (size_t i = 0; i < nCount; ++i)
	{
		shared_ptr<CMapTile> pTile = vTiles[i];
		if (!pTile)
		{
			continue;
		}
		if (!pTile->LocalPath.IsEmpty())
		{
			bAnyTile = true;
			break;
		}
	}

	value = bAnyTile;

	return value;
} // DownloadTiles

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::StitchTiles()
{
	bool value = false;

	if (m_pTileGrid == nullptr)
	{
		return value;
	}

	vector<shared_ptr<CMapTile>>& vTiles = m_pTileGrid->Tiles;
	const size_t nCount = vTiles.size();

	if (nCount == 0)
	{
		return value;
	}

	// assume standard OSM tile size
	const int nTileSize = 256;

	const int nXMin = m_pTileGrid->TileXMin;
	const int nXMax = m_pTileGrid->TileXMax;
	const int nYMin = m_pTileGrid->TileYMin;
	const int nYMax = m_pTileGrid->TileYMax;

	const int nTilesX = (nXMax - nXMin + 1);
	const int nTilesY = (nYMax - nYMin + 1);

	const int nWidth = nTilesX * nTileSize;
	const int nHeight = nTilesY * nTileSize;

	shared_ptr<Bitmap> pBitmap = make_shared<Bitmap>
		(
			nWidth,
			nHeight,
			PixelFormat32bppARGB
			);

	Graphics graphics(pBitmap.get());
	graphics.Clear(Color::White);

	for (size_t i = 0; i < nCount; ++i)
	{
		shared_ptr<CMapTile> pTile = vTiles[i];
		if (!pTile)
		{
			continue;
		}
		if (pTile->LocalPath.IsEmpty())
		{
			continue;
		}

		USES_CONVERSION;
		const WCHAR* pPath = T2CW(pTile->LocalPath);
		unique_ptr<Bitmap> pTileBitmap(new Bitmap(pPath));
		if (pTileBitmap->GetLastStatus() != Ok)
		{
			continue;
		}

		const int nTileX = pTile->TileX;
		const int nTileY = pTile->TileY;

		const int nOffsetX = (nTileX - nXMin) * nTileSize;
		const int nOffsetY = (nTileY - nYMin) * nTileSize;

		Rect rectDest(nOffsetX, nOffsetY, nTileSize, nTileSize);
		graphics.DrawImage(pTileBitmap.get(), rectDest);
	}

	m_pFinalBitmap = pBitmap;
	value = (m_pFinalBitmap != nullptr);

	return value;
} // StitchTiles

/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::GenerateFinalBitmap()
{
	bool value = false;

	const bool bOK = StitchTiles();
	if (!bOK)
	{
		return value;
	}

	value = (m_pFinalBitmap != nullptr);

	return value;
} // GenerateFinalBitmap

/////////////////////////////////////////////////////////////////////////////
