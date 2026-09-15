/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <memory>
using namespace std;

class CMapTile;

/////////////////////////////////////////////////////////////////////////////
// CMapTileGrid
// Represents a rectangular grid of OSM tiles at a specific zoom level.
// Hosts CMapTile objects and defines the tile coordinate bounds.
// Stitching and rendering are handled by higher-level classes.
/////////////////////////////////////////////////////////////////////////////
class CMapTileGrid
{
// public types
public:

// protected data
protected:
	int     m_nZoom;
	int     m_nTileXMin;
	int     m_nTileXMax;
	int     m_nTileYMin;
	int     m_nTileYMax;

	vector<shared_ptr<CMapTile>> m_vTiles;

// public properties
public:
	int GetZoom()
	{
		return m_nZoom;
	}

	void SetZoom(int value)
	{
		m_nZoom = value;
	}

	__declspec(property(get = GetZoom, put = SetZoom))
		int Zoom;

	int GetTileXMin()
	{
		return m_nTileXMin;
	}

	void SetTileXMin(int value)
	{
		m_nTileXMin = value;
	}

	__declspec(property(get = GetTileXMin, put = SetTileXMin))
		int TileXMin;

	int GetTileXMax()
	{
		return m_nTileXMax;
	}

	void SetTileXMax(int value)
	{
		m_nTileXMax = value;
	}

	__declspec(property(get = GetTileXMax, put = SetTileXMax))
		int TileXMax;

	int GetTileYMin()
	{
		return m_nTileYMin;
	}

	void SetTileYMin(int value)
	{
		m_nTileYMin = value;
	}

	__declspec(property(get = GetTileYMin, put = SetTileYMin))
		int TileYMin;

	int GetTileYMax()
	{
		return m_nTileYMax;
	}

	void SetTileYMax(int value)
	{
		m_nTileYMax = value;
	}

	__declspec(property(get = GetTileYMax, put = SetTileYMax))
		int TileYMax;

	vector<shared_ptr<CMapTile>>& GetTiles()
	{
		return m_vTiles;
	}

	void SetTiles(vector<shared_ptr<CMapTile>> value)
	{
		m_vTiles = value;
	}

	__declspec(property(get = GetTiles, put = SetTiles))
		vector<shared_ptr<CMapTile>>& Tiles;

// protected methods
protected:

// public methods
public:
	bool ComputeBounds(int nTileXCenter, int nTileYCenter, int nTileRadius);
	bool AddTile(shared_ptr<CMapTile> pTile);
	void Clear();
	bool Contains(int nTileX, int nTileY);
	shared_ptr<CMapTile> FindTile(int nTileX, int nTileY);

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CMapTileGrid()
		: m_nZoom(0)
		, m_nTileXMin(0)
		, m_nTileXMax(0)
		, m_nTileYMin(0)
		, m_nTileYMax(0)
		, m_vTiles()
	{
	}

	~CMapTileGrid()
	{
	}
}; // class CMapTileGrid

/////////////////////////////////////////////////////////////////////////////
