/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "ClimateStations.h"

// collection of climate stations
CClimateStations m_Stations;

// processed count
int m_nCount;

// has version been collected
bool m_bVersion;

// USHCN Version
CString m_csVersion;

// USHCN release date
CString m_csReleaseDate;

// has dataset been collected
bool m_bDataset;

// dataset: raw, tob, or FLs.52j
CString m_csDataset;

// data source credit
CString m_csCredit;

CString m_csCreditFormat;
