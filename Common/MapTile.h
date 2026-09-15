/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMapTile
// Represents a single OSM tile at a specific zoom/X/Y coordinate.
// Stores tile metadata and the local file path after download.
// Rendering and stitching are handled by higher-level classes.
/////////////////////////////////////////////////////////////////////////////
class CMapTile
{
// public types
public:

// protected data
protected:
	int     m_nZoom;
	int     m_nTileX;
	int     m_nTileY;
	CString m_csLocalPath;

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

	int GetTileX()
	{
		return m_nTileX;
	}

	void SetTileX(int value)
	{
		m_nTileX = value;
	}

	__declspec(property(get = GetTileX, put = SetTileX))
		int TileX;

	int GetTileY()
	{
		return m_nTileY;
	}

	void SetTileY(int value)
	{
		m_nTileY = value;
	}

	__declspec(property(get = GetTileY, put = SetTileY))
		int TileY;

	CString GetLocalPath()
	{
		return m_csLocalPath;
	}

	void SetLocalPath(CString value)
	{
		m_csLocalPath = value;
	}

	__declspec(property(get = GetLocalPath, put = SetLocalPath))
		CString LocalPath;

// protected methods
protected:

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CMapTile()
		: m_nZoom(0)
		, m_nTileX(0)
		, m_nTileY(0)
		, m_csLocalPath(L"")
	{
	}

	~CMapTile()
	{
	}
}; // class CMapTile

/////////////////////////////////////////////////////////////////////////////
