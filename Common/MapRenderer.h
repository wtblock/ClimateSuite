/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////
#include "ImagePlus.h"
#include <memory>
using namespace std;

class CMapOSM;
class CPageMap;

/////////////////////////////////////////////////////////////////////////////
// CMapRenderer
// Responsible for drawing the final stitched OSM map bitmap into a page.
// Consumed by CPageMap and driven by document layout rules.
// Parallel to CGraphPlotter in the plot subsystem.
/////////////////////////////////////////////////////////////////////////////
class CMapRenderer
{
// public types
public:
	struct MAP_PIN
	{
		double Lat;
		double Lon;
		CString StationID;
		CString LocationKey;
		Gdiplus::Color Color;
	};

// protected data
protected:
	shared_ptr<CMapOSM> m_pMapOSM;
	shared_ptr<Bitmap>  m_pBitmap;
	CRect               m_rectTarget;
	vector<MAP_PIN>*    m_pPins = nullptr;

// public properties
public:
	shared_ptr<CMapOSM> GetMapOSM()
	{
		return m_pMapOSM;
	}

	void SetMapOSM(shared_ptr<CMapOSM> value)
	{
		m_pMapOSM = value;
	}

	__declspec(property(get = GetMapOSM, put = SetMapOSM))
		shared_ptr<CMapOSM> MapOSM;

	shared_ptr<Bitmap> GetBitmap()
	{
		return m_pBitmap;
	}

	void SetBitmap(shared_ptr<Bitmap> value)
	{
		m_pBitmap = value;
	}

	__declspec(property(get = GetBitmap, put = SetBitmap))
		shared_ptr<Bitmap> Bitmap;

	CRect GetTargetRect()
	{
		return m_rectTarget;
	}

	void SetTargetRect(CRect value)
	{
		m_rectTarget = value;
	}

	__declspec(property(get = GetTargetRect, put = SetTargetRect))
		CRect TargetRect;

	vector<MAP_PIN>* GetPins()
	{
		return m_pPins;
	}
	void SetPins(vector<MAP_PIN>* pPins)
	{
		m_pPins = pPins;
	}
	__declspec(property(get = GetPins, put = SetPins))
		vector<MAP_PIN>* Pins;

// protected methods
protected:
	bool DrawSinglePin(CDC* pDC, int x, int y, Gdiplus::Color Color);
	bool LatLonToPixel(double dLat, double dLon, int& x, int& y);

// public methods
public:
/////////////////////////////////////////////////////////////////////////////
// Render
//
// Draws the final stitched map bitmap into the target rectangle.
// Implementation will mirror CGraphPlotter::Render() but for maps.
/////////////////////////////////////////////////////////////////////////////
	void Render(CDC* pDC)
	{
		if (m_pBitmap != nullptr)
		{
			CImagePlus plus(m_pBitmap);
			plus.Draw(pDC, m_rectTarget);
		}
	}
	bool PrepareRenderer();
	void DrawPins(CDC* pDC);
	void DrawMap(CDC* pDC);

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CMapRenderer()
		: m_pMapOSM(nullptr)
		, m_pBitmap(nullptr)
		, m_rectTarget(0, 0, 0, 0)
	{
	}

	~CMapRenderer()
	{
	}
}; // class CMapRenderer

/////////////////////////////////////////////////////////////////////////////
