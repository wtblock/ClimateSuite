/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ClimateStations.h"

/////////////////////////////////////////////////////////////////////////////
// read the station data into the stations collection from the original
// USHCN text file: "ushcn-v2.5-stations.txt"
bool CClimateStations::ReadStationData()
{
	// the stations text file pathname
	const CString csStationData = StationPath;

	// open the stations text file
	CStdioFile file;
	const bool value =
		file.Open
		( 
			csStationData, CFile::modeRead | CFile::shareDenyNone 
		);

	// if the open was successful, read each line of the file and 
	// collect the station data properties
	if ( value == true )
	{
		CString csLine;
		while ( file.ReadString( csLine ) )
		{
			shared_ptr<CClimateStation> pStation =
				shared_ptr<CClimateStation>( new CClimateStation( csLine ) );
			const CString csKey = pStation->Station;
			m_Stations.add( csKey, pStation );
		}
	}

	return value;
} // ReadStationData

/////////////////////////////////////////////////////////////////////////////
// add a station year to the collection of data
shared_ptr<CClimateStation> CClimateStations::AddStationYear(shared_ptr<CStationYear>& input)
{
	shared_ptr<CClimateStation> pStation;

	CClimateTemperature::MEASURE_TYPE eType = input->MeasurementType;
	CString csStation = input->Station;
	CString csYear = input->Year;
	pStation = ClimateStation[csStation];
	if (pStation == nullptr)
	{
		return pStation;
	}

	switch (eType)
	{
		case CClimateTemperature::MEASURE_TYPE::mtMinimum:
		{
			pStation->MinimumValue[csYear] = input;
			break;
		}
		case CClimateTemperature::MEASURE_TYPE::mtAverage:
		{
			pStation->AverageValue[csYear] = input;
			break;
		}
		case CClimateTemperature::MEASURE_TYPE::mtMaximum:
		{
			pStation->MaximumValue[csYear] = input;
			break;
		}
	}

	return pStation;
}

/////////////////////////////////////////////////////////////////////////////

