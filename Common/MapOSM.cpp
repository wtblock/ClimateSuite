/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MapOSM.h"
#include "MapTile.h"
#include <wininet.h>

/////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "wininet.lib")
/////////////////////////////////////////////////////////////////////////////

using namespace Gdiplus;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// DownloadSingleTile
//
// Downloads a single OSM tile using WinINet and saves it to csLocalPath.
/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::DownloadSingleTile
(
	int nZoom, int nTileX, int nTileY, CString& csLocalPath
)
{
	bool value = false;

	// ---------------------------------------------------------
	// Build URL
	// ---------------------------------------------------------
	CString csUrl;
	csUrl.Format
	(
		_T("https://tile.openstreetmap.org/%d/%d/%d.png"),
		nZoom,
		nTileX,
		nTileY
	);

	// ---------------------------------------------------------
	// WinINet session
	// ---------------------------------------------------------
	HINTERNET hSession = ::InternetOpen
	(
		_T("ClimateExplorer/1.0"),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0
	);

	if (hSession == NULL)
	{
		return value;
	}

	// ---------------------------------------------------------
	// Open URL
	// ---------------------------------------------------------
	HINTERNET hUrl = ::InternetOpenUrl
	(
		hSession,
		csUrl,
		NULL,
		0,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE,
		0
	);

	if (hUrl == NULL)
	{
		::InternetCloseHandle(hSession);
		return value;
	}

	// ---------------------------------------------------------
	// Read data
	// ---------------------------------------------------------
	BYTE buffer[4096];
	DWORD dwRead = 0;

	HANDLE hFile = ::CreateFile
	(
		csLocalPath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		::InternetCloseHandle(hUrl);
		::InternetCloseHandle(hSession);
		return value;
	}

	while (::InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead) && dwRead > 0)
	{
		DWORD dwWritten = 0;
		::WriteFile(hFile, buffer, dwRead, &dwWritten, NULL);
	}

	::CloseHandle(hFile);
	::InternetCloseHandle(hUrl);
	::InternetCloseHandle(hSession);

	value = true;
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
	double dLatCenter, double dLonCenter, int nZoom,
	int radiusX, int radiusY
)
{
	bool value = false;

	InitializeEmptyGrid();

	if (m_pTileGrid == nullptr)
	{
		return value;
	}

	m_pTileGrid->Zoom = nZoom;

	int nTileXCenter = 0;
	int nTileYCenter = 0;

	if (!ComputeTileCoordinates(dLatCenter, dLonCenter, nZoom,
		nTileXCenter, nTileYCenter))
	{
		return value;
	}

	// ---------------------------------------------------------
	// Start with horizontal bounds (radiusX)
	// ---------------------------------------------------------
	const bool bBoundsOK = m_pTileGrid->ComputeBounds
	(
		nTileXCenter,
		nTileYCenter,
		radiusX
	);
	if (!bBoundsOK)
	{
		return value;
	}

	// ---------------------------------------------------------
	// Adjust vertical bounds to radiusY (landscape aspect)
	// ---------------------------------------------------------
	int nYMin = nTileYCenter - radiusY;
	int nYMax = nTileYCenter + radiusY;

	// clamp to valid tile range
	const int nMaxTile = (1 << nZoom) - 1;

	if (nYMin < 0)       nYMin = 0;
	if (nYMax > nMaxTile) nYMax = nMaxTile;

	m_pTileGrid->TileYMin = nYMin;
	m_pTileGrid->TileYMax = nYMax;

	// also clamp X to valid range
	int nXMin = m_pTileGrid->TileXMin;
	int nXMax = m_pTileGrid->TileXMax;

	if (nXMin < 0)       nXMin = 0;
	if (nXMax > nMaxTile) nXMax = nMaxTile;

	m_pTileGrid->TileXMin = nXMin;
	m_pTileGrid->TileXMax = nXMax;

	// ---------------------------------------------------------
	// Populate the grid with empty tiles
	// ---------------------------------------------------------
	nXMin = m_pTileGrid->TileXMin;
	nXMax = m_pTileGrid->TileXMax;
	nYMin = m_pTileGrid->TileYMin;
	nYMax = m_pTileGrid->TileYMax;

	for (int nX = nXMin; nX <= nXMax; ++nX)
	{
		for (int nY = nYMin; nY <= nYMax; ++nY)
		{
			CString csLocalPath; // filled later in DownloadTiles
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
			_T("%s%d_%d_%d.png"),
			lpszCacheFolder,
			nZoom,
			nTileX,
			nTileY
		);

		// ---------------------------------------------------------
		// Check cache: if file already exists, skip download
		// ---------------------------------------------------------
		DWORD dwAttr = ::GetFileAttributes(csLocalPath);
		if (dwAttr != INVALID_FILE_ATTRIBUTES)
		{
			// file exists in cache
			pTile->LocalPath = csLocalPath;
			continue;
		}

		// ---------------------------------------------------------
		// Otherwise, download tile
		// ---------------------------------------------------------
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
// GenerateFinalBitmap
//
// Stitches all downloaded tiles into a single GDI+ Bitmap.
// Assumes each tile is 256x256 pixels (standard OSM).
/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::GenerateFinalBitmap()
{
	bool value = false;

	CMapTileGrid* pGrid = TileGrid.get();
	if (pGrid == nullptr)
	{
		return value;
	}

	vector<shared_ptr<CMapTile>>& vTiles = pGrid->Tiles;
	const size_t nCount = vTiles.size();
	if (nCount == 0)
	{
		return value;
	}

	// ---------------------------------------------------------
	// Compute grid bounds and dimensions
	// ---------------------------------------------------------
	const int nTileXMin = pGrid->TileXMin;
	const int nTileXMax = pGrid->TileXMax;
	const int nTileYMin = pGrid->TileYMin;
	const int nTileYMax = pGrid->TileYMax;

	const int nCols = (nTileXMax - nTileXMin + 1);
	const int nRows = (nTileYMax - nTileYMin + 1);

	if (nCols <= 0 || nRows <= 0)
	{
		return value;
	}

	const int nTileSize = 256;   // OSM tile size

	const int nWidth = nCols * nTileSize;
	const int nHeight = nRows * nTileSize;

#ifdef _DEBUG
#undef new
#endif
	std::unique_ptr<Gdiplus::Bitmap> pBmp
	(
		new Gdiplus::Bitmap(nWidth, nHeight, PixelFormat32bppARGB)
	);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	if (!pBmp)
	{
		return value;
	}

	Graphics g(pBmp.get());
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.Clear(Color(255, 255, 255, 255));   // white background

	// ---------------------------------------------------------
	// Stitch tiles
	// ---------------------------------------------------------
	for (size_t i = 0; i < nCount; ++i)
	{
		shared_ptr<CMapTile> pTile = vTiles[i];
		if (!pTile)
		{
			continue;
		}

		CString csLocalPath = pTile->LocalPath;
		if (csLocalPath.IsEmpty())
		{
			continue;
		}

#ifdef _DEBUG
#undef new
#endif
		// Load tile bitmap
		std::unique_ptr<Gdiplus::Bitmap> 
			pTileBmp(new Gdiplus::Bitmap(csLocalPath));
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

		if (!pTileBmp || pTileBmp->GetLastStatus() != Ok)
		{
			continue;
		}

		// Compute tile position within grid
		const int nCol = pTile->TileX - nTileXMin;
		const int nRow = pTile->TileY - nTileYMin;

		if (nCol < 0 || nCol >= nCols)
		{
			continue;
		}
		if (nRow < 0 || nRow >= nRows)
		{
			continue;
		}

		const int nDestX = nCol * nTileSize;
		const int nDestY = nRow * nTileSize;

		// Draw tile
		g.DrawImage(pTileBmp.get(), nDestX, nDestY, nTileSize, nTileSize);
	}

	// ---------------------------------------------------------
	// Assign final bitmap
	// ---------------------------------------------------------
	m_pFinalBitmap.reset(pBmp.release());

	value = true;
	return value;
} // GenerateFinalBitmap

/////////////////////////////////////////////////////////////////////////////
// PixelToLatLon
//
// Converts a pixel coordinate inside the stitched map bitmap
// back into a geographic latitude/longitude.
//
// This is the exact inverse of LatLonToPixel.
//
// Requirements:
//   • pGrid->Zoom
//   • pGrid->TileXMin / TileYMin
//   • Web Mercator math
/////////////////////////////////////////////////////////////////////////////
bool CMapOSM::PixelToLatLon(int px, int py, double& latDeg, double& lonDeg)
{
	shared_ptr<CMapTileGrid> pGrid = TileGrid;
	if (!pGrid)
		return false;

	const int z = pGrid->Zoom;
	const int minTileX = pGrid->TileXMin;
	const int minTileY = pGrid->TileYMin;

	const double dPi = 3.14159265358979323846;

	//
	// 1. Convert stitched-bitmap pixel → global pixel
	//
	const double globalX = px + (minTileX * 256);
	const double globalY = py + (minTileY * 256);

	//
	// 2. Convert global pixel → normalized Web Mercator coordinates
	//
	const double n = pow(2.0, z) * 256.0;

	const double xNorm = globalX / n;          // 0 → 1
	const double yNorm = globalY / n;          // 0 → 1

	//
	// 3. Convert normalized → lon/lat in radians
	//
	lonDeg = xNorm * 360.0 - 180.0;

	// Web Mercator inverse:
	const double merc = dPi * (1.0 - 2.0 * yNorm);
	const double latRad = atan(sinh(merc));

	latDeg = latRad * 180.0 / dPi;

	return true;
} // PixelToLatLon

/////////////////////////////////////////////////////////////////////////////
