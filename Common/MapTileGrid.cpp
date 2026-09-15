/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MapTileGrid.h"
#include "MapTile.h"

/////////////////////////////////////////////////////////////////////////////
bool CMapTileGrid::ComputeBounds
(
	int nTileXCenter, int nTileYCenter, int nTileRadius
)
{
	bool value = false;

	// compute min/max tile coordinates
	m_nTileXMin = nTileXCenter - nTileRadius;
	m_nTileXMax = nTileXCenter + nTileRadius;
	m_nTileYMin = nTileYCenter - nTileRadius;
	m_nTileYMax = nTileYCenter + nTileRadius;

	// basic validation
	if (m_nTileXMin <= m_nTileXMax &&
		m_nTileYMin <= m_nTileYMax)
	{
		value = true;
	}

	return value;
} // ComputeBounds

/////////////////////////////////////////////////////////////////////////////
void CMapTileGrid::Clear()
{
	// clear all tiles
	m_vTiles.clear();

	// reset bounds
	m_nTileXMin = 0;
	m_nTileXMax = 0;
	m_nTileYMin = 0;
	m_nTileYMax = 0;
} // Clear

/////////////////////////////////////////////////////////////////////////////
bool CMapTileGrid::Contains(int nTileX, int nTileY)
{
	bool value = false;

	const int nXMin = m_nTileXMin;
	const int nXMax = m_nTileXMax;
	const int nYMin = m_nTileYMin;
	const int nYMax = m_nTileYMax;

	if (nTileX >= nXMin &&
		nTileX <= nXMax &&
		nTileY >= nYMin &&
		nTileY <= nYMax)
	{
		value = true;
	}

	return value;
} // Contains

/////////////////////////////////////////////////////////////////////////////
shared_ptr<CMapTile> CMapTileGrid::FindTile(int nTileX, int nTileY)
{
	shared_ptr<CMapTile> value = nullptr;

	const size_t nCount = m_vTiles.size();

	for (size_t i = 0; i < nCount; ++i)
	{
		shared_ptr<CMapTile> pTile = m_vTiles[i];
		if (!pTile)
		{
			continue;
		}

		if (pTile->TileX == nTileX &&
			pTile->TileY == nTileY)
		{
			value = pTile;
			break;
		}
	}

	return value;
} // FindTile

/////////////////////////////////////////////////////////////////////////////
bool CMapTileGrid::AddTile(shared_ptr<CMapTile> pTile)
{
	bool value = false;

	if (!pTile)
	{
		return value;
	}

	// ensure tile is within bounds
	if (!Contains(pTile->TileX, pTile->TileY))
	{
		return value;
	}

	m_vTiles.push_back(pTile);
	value = true;

	return value;
} // AddTile

/////////////////////////////////////////////////////////////////////////////
