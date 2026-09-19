/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "PageContent.h"
#include "ClimateExplorerDoc.h"
#include <afxstr.h>
#include <vector>
#include <gdipluscolor.h>
#include "MapOSM.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMap
/////////////////////////////////////////////////////////////////////////////
class CPageMap : public CPageContent
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
	// owning document
	CClimateExplorerDoc* m_pDoc;

	// map metadata
	CString   m_csScope;        // National / State / Location
	CString   m_csState;       // "TX"
	CString   m_csLocation;    // "TX, Weatherford"

	double    m_dCenterLat;    // resolved center latitude
	double    m_dCenterLon;    // resolved center longitude
	int       m_nZoom;         // OSM zoom level

	// collection of pins for the current map
	std::vector<MAP_PIN> m_arrPins;
	shared_ptr<CMapOSM> m_pMapOSM;

// public properties
public:
	// document pointer
	CClimateExplorerDoc* GetDoc()
	{
		return m_pDoc;
	}

	void SetDoc(CClimateExplorerDoc* value)
	{
		m_pDoc = value;
	}

	__declspec(property(get = GetDoc, put = SetDoc))
		CClimateExplorerDoc* Doc;

	// pointer to CMapOSM class
	shared_ptr<CMapOSM> GetMapOSM()
	{
		if (m_pMapOSM == nullptr)
		{
			m_pMapOSM = make_shared<CMapOSM>();
		}
		m_pMapOSM->Title = ContentTitle;
		m_pMapOSM->Description = ContentPath;

		return m_pMapOSM;
	}

	// pointer to CMapOSM class
	__declspec(property(get = GetMapOSM))
		shared_ptr<CMapOSM> MapOSM;

	// collection of pins for the current map
	std::vector<MAP_PIN>* GetPins()
	{
		return &m_arrPins;
	}

	// collection of pins for the current map
	__declspec(property(get = GetPins))
		std::vector<MAP_PIN>* Pins;

	// map scope
	CString GetScope()
	{
		return m_csScope;
	}

	void SetScope(CString value)
	{
		m_csScope = value;
	}

	__declspec(property(get = GetScope, put = SetScope))
		CString Scope;

	// state
	CString GetState()
	{
		return m_csState;
	}

	void SetState(CString value)
	{
		m_csState = value;
	}

	__declspec(property(get = GetState, put = SetState))
		CString State;

	// location ("TX, Weatherford")
	CString GetLocation()
	{
		return m_csLocation;
	}

	void SetLocation(CString value)
	{
		m_csLocation = value;
	}

	__declspec(property(get = GetLocation, put = SetLocation))
		CString Location;

	// center latitude
	double GetCenterLat()
	{
		return m_dCenterLat;
	}

	void SetCenterLat(double value)
	{
		m_dCenterLat = value;
	}

	__declspec(property(get = GetCenterLat, put = SetCenterLat))
		double CenterLat;

	// center longitude
	double GetCenterLon()
	{
		return m_dCenterLon;
	}

	void SetCenterLon(double value)
	{
		m_dCenterLon = value;
	}

	__declspec(property(get = GetCenterLon, put = SetCenterLon))
		double CenterLon;

	// zoom level
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

// protected methods
protected:
	// resolve center lat/lon + zoom from Scope/State/Location
	void ResolveCenterFromQuery();

	void AddStationPin(const CString& stationID, double lat, double lon);


// public methods
public:
	bool LatLonToPixel
	(
		CMapOSM* pMapOSM, double dLat, double dLon, int& x, int& y
	);

// protected overrides
protected:

// public overrides
public:
	virtual shared_ptr<Gdiplus::Image> GetImageContent() override;

	virtual void WriteXml
	(
		IXmlWriter* pWriter,
		int nPage = 0,
		int nItem = 0
	) override;

	virtual void ReadXml(IXmlReader* pReader) override;

// public constructor/destructor
public:
	// REQUIRED: matches CPageContent pattern
	CPageMap()
	{
		ContentType = ContentMap;
		m_pDoc = nullptr;

		Scope = L"National";
		State = L"";
		Location = L"";

		CenterLat = 0.0;
		CenterLon = 0.0;
		Zoom = 4;
	}

	// REQUIRED: matches all other content types
	CPageMap(CClimateExplorerDoc* pDoc);

	virtual ~CPageMap()
	{
	}
}; // CPageMap

/////////////////////////////////////////////////////////////////////////////
