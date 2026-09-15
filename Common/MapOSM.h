/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////
#include "ImagePlus.h"
#include "MapTileGrid.h"
#include <memory>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CMapOSM
// Top-level OSM map object.
// Hosts the tile grid and the final stitched bitmap.
// Consumed by CMapRenderer and wrapped by CPageMap.
/////////////////////////////////////////////////////////////////////////////
class CMapOSM
{
// public types
public:

// protected data
protected:
	shared_ptr<CMapTileGrid> m_pTileGrid;
	shared_ptr<Bitmap>       m_pFinalBitmap;
	CString                  m_csTitle;
	CString                  m_csDescription;

// public properties
public:
	shared_ptr<CMapTileGrid> GetTileGrid()
	{
		return m_pTileGrid;
	}

	void SetTileGrid(shared_ptr<CMapTileGrid> value)
	{
		m_pTileGrid = value;
	}

	__declspec(property(get = GetTileGrid, put = SetTileGrid))
		shared_ptr<CMapTileGrid> TileGrid;

	shared_ptr<Bitmap> GetFinalBitmap()
	{
		return m_pFinalBitmap;
	}

	void SetFinalBitmap(shared_ptr<Bitmap> value)
	{
		m_pFinalBitmap = value;
	}

	__declspec(property(get = GetFinalBitmap, put = SetFinalBitmap))
		shared_ptr<Bitmap> FinalBitmap;

	CString GetTitle()
	{
		return m_csTitle;
	}

	void SetTitle(CString value)
	{
		m_csTitle = value;
	}

	__declspec(property(get = GetTitle, put = SetTitle))
		CString Title;

	CString GetDescription()
	{
		return m_csDescription;
	}

	void SetDescription(CString value)
	{
		m_csDescription = value;
	}

	__declspec(property(get = GetDescription, put = SetDescription))
		CString Description;

// protected methods
protected:
	bool DownloadSingleTile
	(
		int nZoom, int nTileX, int nTileY, CString& csLocalPath
	);
	bool LoadTileIntoGrid
	(
		int nZoom, int nTileX, int nTileY, CString& csLocalPath
	);

// public methods
public:
	void InitializeEmptyGrid()
	{
		m_pTileGrid = make_shared<CMapTileGrid>();
	}
	bool ComputeTileCoordinates
	(
		double dLat, double dLon, int nZoom, int& nTileX, int& nTileY
	);
	bool BuildTileGrid
	(
		double dLatCenter, double dLonCenter, int nZoom, int nTileRadius
	);
	bool DownloadTiles(LPCTSTR lpszCacheFolder);
	bool StitchTiles();
	bool GenerateFinalBitmap();

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CMapOSM()
		: m_pTileGrid(nullptr)
		, m_pFinalBitmap(nullptr)
		, m_csTitle(L"")
		, m_csDescription(L"")
	{
	}

	~CMapOSM()
	{
	}
}; // class CMapOSM

/////////////////////////////////////////////////////////////////////////////
