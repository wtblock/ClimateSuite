/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageMap.h"
#include "MapRenderer.h"
#include "MapOSM.h"

/////////////////////////////////////////////////////////////////////////////
bool CPageMap::Initialize(shared_ptr<CMapOSM> pMapOSM)
{
	bool value = false;

	if (!pMapOSM)
	{
		return value;
	}

	// assign map object
	m_pMapOSM = pMapOSM;

	// create renderer
	m_pRenderer = make_shared<CMapRenderer>();
	if (!m_pRenderer)
	{
		return value;
	}

	// assign map to renderer
	m_pRenderer->MapOSM = m_pMapOSM;

	// prepare renderer (validates final bitmap)
	const bool bOK = m_pRenderer->PrepareRenderer();
	if (!bOK)
	{
		return value;
	}

	value = true;

	return value;
} // Initialize

/////////////////////////////////////////////////////////////////////////////
bool CPageMap::Layout(CRect rectPage)
{
	bool value = false;

	// store the target rectangle
	m_rectTarget = rectPage;

	// assign to renderer
	if (m_pRenderer != nullptr)
	{
		m_pRenderer->TargetRect = rectPage;
		value = true;
	}

	return value;
} // Layout

/////////////////////////////////////////////////////////////////////////////
