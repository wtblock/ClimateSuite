/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////
#include "ImagePlus.h"
#include "MapRenderer.h"
#include "MapOSM.h"
#include <memory>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CPageMap
// Page-level wrapper for OSM map content.
// Parallel to CPageGraph, CPageImage, CPageMD, and CPageHTML.
// Holds the map content and renderer, and provides the target rectangle
// used by CClimateExplorerDoc during page layout.
/////////////////////////////////////////////////////////////////////////////
class CPageMap
{
// public types
public:

// protected data
protected:
	shared_ptr<CMapOSM>      m_pMapOSM;
	shared_ptr<CMapRenderer> m_pRenderer;
	CRect                    m_rectTarget;

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

	shared_ptr<CMapRenderer> GetRenderer()
	{
		return m_pRenderer;
	}

	void SetRenderer(shared_ptr<CMapRenderer> value)
	{
		m_pRenderer = value;
	}

	__declspec(property(get = GetRenderer, put = SetRenderer))
		shared_ptr<CMapRenderer> Renderer;

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

// protected methods
protected:

// public methods
public:
/////////////////////////////////////////////////////////////////////////////
// Render
//
// Draws the map using the renderer into the target rectangle.
// Mirrors CPageGraph::Render() but for OSM maps.
/////////////////////////////////////////////////////////////////////////////
	void Render(CDC* pDC)
	{
		if (m_pRenderer != nullptr)
		{
			m_pRenderer->Render(pDC);
		}
	}
	bool Initialize(shared_ptr<CMapOSM> pMapOSM);
	bool Layout(CRect rectPage);

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CPageMap()
		: m_pMapOSM(nullptr)
		, m_pRenderer(nullptr)
		, m_rectTarget(0, 0, 0, 0)
	{
	}

	~CPageMap()
	{
	}
}; // class CPageMap

/////////////////////////////////////////////////////////////////////////////
