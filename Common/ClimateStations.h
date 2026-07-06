/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ClimateStation.h"
#include "StationYear.h"
#include "KeyedCollection.h"

/////////////////////////////////////////////////////////////////////////////
// a collection of climate stations as defined by the 
// U.S. Historical Climatology Network (USHCN)
class CClimateStations
{
// public definitions
public:

// protected data
protected:
	// the path to the station data text file
	CString m_csStationPath;

	// rapid station lookup of climate stations
	CKeyedCollection<CString, CClimateStation> m_Stations;

// public properties
public:
	// the path to the station data text file
	CString GetStationPath()
	{
		return m_csStationPath;

	}
	// the path to the station data text file
	void SetStationPath(CString value)
	{
		m_csStationPath = value;

	} 
	// the path to the station data text file
	__declspec( property( get = GetStationPath, put = SetStationPath ) )
		CString StationPath;

	// return the station for the given key
	shared_ptr<CClimateStation> GetClimateStation(CString csKey)
	{
		shared_ptr<CClimateStation> value;
		if (m_Stations.Exists[csKey])
		{
			value = m_Stations.find(csKey);
		}
		return value;
	}
	// return the station for the given key
	__declspec( property( get = GetClimateStation) )
		shared_ptr<CClimateStation> ClimateStation[];

// protected methods
protected:

// public methods
public:
	// read the station data into the stations collection from the original
	// USHCN text file "ushcn-v2.5-stations.txt" 
	bool ReadStationData();

// protected overrides
protected:

// public overrides
public:
	// add a station year to the collection of data and return a pointer to 
	// the climate station
	shared_ptr<CClimateStation> AddStationYear(shared_ptr<CStationYear>& input);

// public construction / destruction
public:
	// constructor
	CClimateStations()
	{

	}

	// destructor
	virtual ~CClimateStations()
	{
		//m_spStationList.reset();
	}
};

/////////////////////////////////////////////////////////////////////////////
