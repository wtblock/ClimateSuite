/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageMap.h"
#include "ImagePlus.h"
#include "ClimateExplorerDoc.h"
#include "MainFrm.h"
#include "CHelper.h"
#include "MapOSM.h"
#include "MapRenderer.h"
#include "ClimateStation.h"
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CPageMap::CPageMap(CClimateExplorerDoc* pDoc)
{
	ContentType = ContentMap;
	m_pDoc = pDoc;

	// document metadata
	Scope = pDoc->Scope;
	State = pDoc->State;
	Location = pDoc->Location;

	// default metadata
	CenterLat = 0.0;
	CenterLon = 0.0;
	Zoom = 0;

}

/////////////////////////////////////////////////////////////////////////////
// Add a station pin to the map
void CPageMap::AddStationPin(const CString& stationID, double lat, double lon)
{
	MAP_PIN pin;

	pin.StationID = stationID;
	pin.Lat = lat;
	pin.Lon = lon;

	CString locationKey = theApp.ClimateDatabase->ActiveLocation[stationID];
	pin.LocationKey = locationKey;

	const bool active = !locationKey.IsEmpty();
	pin.Color = active ? 
		Color::DarkRed :
		Color::DarkBlue;

	m_arrPins.push_back(pin);
} // AddStationPin

/////////////////////////////////////////////////////////////////////////////
bool CPageMap::LatLonToPixel
(
	CMapOSM* pMapOSM, double latDeg, double lonDeg, int& px, int& py
)
{
	if (!pMapOSM)
		return false;

	shared_ptr<CMapTileGrid> pGrid = pMapOSM->TileGrid;
	if (pGrid == nullptr)
		return false;

	const int z = pGrid->Zoom;
	const int minTileX = pGrid->TileXMin;
	const int minTileY = pGrid->TileYMin;

	// Convert degrees → radians
	const double dPi = 3.14159265358979323846;
	const double lat = latDeg * dPi / 180.0;
	const double lon = lonDeg * dPi / 180.0;

	const double n = pow(2.0, z) * 256.0;

	// Global pixel coordinates
	const double globalX = (lonDeg + 180.0) / 360.0 * n;
	const double globalY =
		(1.0 - log(tan(lat) + 1.0 / cos(lat)) / dPi) / 2.0 * n;

	// Convert to stitched bitmap pixel coordinates
	px = static_cast<int>(globalX - (minTileX * 256));
	py = static_cast<int>(globalY - (minTileY * 256));

	return true;
} // LatLonToPixel

/////////////////////////////////////////////////////////////////////////////
// ResolveCenterFromQuery
//
// Placeholder — will be implemented once map metadata is finalized.
// This method will use CClimateDatabase to compute:
//     • center latitude
//     • center longitude
//     • zoom level
/////////////////////////////////////////////////////////////////////////////
void CPageMap::ResolveCenterFromQuery()
{
	// ---------------------------------------------------------
	// Ensure document pointer is valid
	// ---------------------------------------------------------
	if (m_pDoc == nullptr)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		m_pDoc = pFrame->ClimateExplorerDocument;
		if (m_pDoc == nullptr)
			return;
	}

	CClimateDatabase* pDB = theApp.ClimateDatabase;
	if (pDB == nullptr)
		return;

	CString csScope = Scope;
	csScope.Trim();

	// ---------------------------------------------------------
	// NATIONAL SCOPE
	// Use ALL stations in the database
	// ---------------------------------------------------------
	if (csScope.CompareNoCase(L"National") == 0)
	{
		CClimateDatabase::GPS_COORDINATE gps = pDB->CenterNational;
		
		// center of bounding box
		CenterLat = gps.first;
		CenterLon = gps.second;

		// national zoom (fits entire US)
		if (Zoom == 0)
		{
			Zoom = 6;
		}
		return;
	}

	// ---------------------------------------------------------
	// STATE SCOPE
	// Use all stations whose postal code matches State
	// ---------------------------------------------------------
	if (csScope.CompareNoCase(L"State") == 0)
	{
		CString csState = State;
		csState.Trim();

		CClimateDatabase::GPS_COORDINATE gps = pDB->CenterState[csState];

		// center of bounding box
		CenterLat = gps.first;
		CenterLon = gps.second;

		if (!CHelper::NearlyEqual(gps.first, 0.0f))
		{
			// state-level zoom (fits a single US state)
			if (Zoom == 0)
			{
				Zoom = 8;
			}
		}
		else
		{
			// fallback
			if (Zoom == 0)
			{
				Zoom = 4;
			}
		}

		return;
	}

	// ---------------------------------------------------------
	// LOCATION SCOPE
	// Use exactly one station: "<State>, <City>"
	// ---------------------------------------------------------
	if (csScope.CompareNoCase(L"Location") == 0)
	{
		CString csState = State;
		CString csCity = Location;

		CClimateDatabase::GPS_COORDINATE gps = pDB->Coordinates[csState][csCity];

		// center of bounding box
		CenterLat = gps.first;
		CenterLon = gps.second;

		if (!CHelper::NearlyEqual(gps.first, 0.0f))
		{
			// location-level zoom (close-up)
			if (Zoom == 0)
			{
				Zoom = 15;
			}
		}
		else
		{
			// fallback
			if (Zoom == 0)
			{
				Zoom = 4;
			}
		}

		return;
	}

	// ---------------------------------------------------------
	// Unknown scope — fallback
	// ---------------------------------------------------------
	CenterLat = 0.0;
	CenterLon = 0.0;
	Zoom = 4;
} // ResolveCenterFromQuery

/////////////////////////////////////////////////////////////////////////////
// CPageMap::WriteXml  (Unified CE + CEx)
/////////////////////////////////////////////////////////////////////////////
void CPageMap::WriteXml(IXmlWriter* pWriter, int nPage, int nItem)
{
	HRESULT hr = S_OK;

	if (m_pDoc == nullptr)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		m_pDoc = pFrame->ClimateExplorerDocument;
	}

	// <Map>
	hr = pWriter->WriteStartElement(nullptr, L"Map", nullptr);
	if (FAILED(hr))
		return;

	// ---------------------------------------------------------
	// Title (required for page identity)
	// ---------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"Title", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(ContentTitle);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteEndElement(); // </Title>

	// ---------------------------------------------------------
	// Scope
	// ---------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"Scope", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(Scope);
	hr = pWriter->WriteEndElement();

	// ---------------------------------------------------------
	// State
	// ---------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"State", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(State);
	hr = pWriter->WriteEndElement();

	// ---------------------------------------------------------
	// Location
	// ---------------------------------------------------------
	hr = pWriter->WriteStartElement(nullptr, L"Location", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(Location);
	hr = pWriter->WriteEndElement();

	// ---------------------------------------------------------
	// Zoom
	// ---------------------------------------------------------
	CString csZoom;
	csZoom.Format(L"%d", Zoom);

	hr = pWriter->WriteStartElement(nullptr, L"Zoom", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(csZoom);
	hr = pWriter->WriteEndElement();

	// ---------------------------------------------------------
	// CenterLat
	// ---------------------------------------------------------
	CString csLat;
	csLat.Format(L"%.7f", CenterLat);

	hr = pWriter->WriteStartElement(nullptr, L"CenterLat", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(csLat);
	hr = pWriter->WriteEndElement(); // </CenterLat>

	// ---------------------------------------------------------
	// CenterLon
	// ---------------------------------------------------------
	CString csLon;
	csLon.Format(L"%.7f", CenterLon);

	hr = pWriter->WriteStartElement(nullptr, L"CenterLon", nullptr);
	if (FAILED(hr))
		return;

	hr = pWriter->WriteString(csLon);
	hr = pWriter->WriteEndElement(); // </CenterLon>

	// ---------------------------------------------------------
	// CE-only: embed map snapshot
	// ---------------------------------------------------------
	auto pZip = m_pDoc->ZipWriter;
	if (pZip && pZip->IsOpen)
	{
		CString csFilename;
		csFilename.Format
		(
			L"Maps/Page_%04u_Map_%02u.png",
			nPage,
			nItem
		);

		hr = pWriter->WriteStartElement(nullptr, L"MapImage", nullptr);
		if (FAILED(hr))
			return;

		hr = pWriter->WriteAttributeString(nullptr, L"value", nullptr, csFilename);
		hr = pWriter->WriteEndElement();

		if (ImageContent != nullptr)
		{
			std::vector<BYTE> pngBytes;
			bool bOK = CHelper::EncodeBitmapToMemory(
				static_cast<Gdiplus::Bitmap*>(ImageContent.get()),
				L"image/png",
				pngBytes
			);

			if (bOK)
			{
				pZip->AddFile(csFilename, pngBytes.data(), pngBytes.size());
			}
		}
	}

	// </Map>
	hr = pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageMap::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageMap::ReadXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	CString csTitle;
	CString csScope;
	CString csState;
	CString csLocation;
	CString csZoom;
	CString csZipPath;
	CString csCenterLat;
	CString csCenterLon;

	while (pReader->Read(&nodeType) == S_OK)
	{
		// End of <Map>
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);
			if (name && wcscmp(name, L"Map") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		// Helper
		auto ReadSimpleText = [&](CString& out)
		{
			XmlNodeType ntText;
			hr = pReader->Read(&ntText);

			if (SUCCEEDED(hr) && ntText == XmlNodeType_Text)
			{
				const WCHAR* pwszText = nullptr;
				pReader->GetValue(&pwszText, nullptr);

				if (pwszText)
					out = pwszText;
			}
		};

		// ---------------------------------------------------------
		// <Title>
		// ---------------------------------------------------------
		if (wcscmp(name, L"Title") == 0)
		{
			ReadSimpleText(csTitle);
			continue;
		}

		// ---------------------------------------------------------
		// <Scope>
		// ---------------------------------------------------------
		if (wcscmp(name, L"Scope") == 0)
		{
			ReadSimpleText(csScope);
			continue;
		}

		// ---------------------------------------------------------
		// <State>
		// ---------------------------------------------------------
		if (wcscmp(name, L"State") == 0)
		{
			ReadSimpleText(csState);
			continue;
		}

		// ---------------------------------------------------------
		// <Location>
		// ---------------------------------------------------------
		if (wcscmp(name, L"Location") == 0)
		{
			ReadSimpleText(csLocation);
			continue;
		}

		// ---------------------------------------------------------
		// <Zoom>
		// ---------------------------------------------------------
		if (wcscmp(name, L"Zoom") == 0)
		{
			ReadSimpleText(csZoom);
			continue;
		}

		// ---------------------------------------------------------
		// <CenterLat>
		// ---------------------------------------------------------
		if (wcscmp(name, L"CenterLat") == 0)
		{
			ReadSimpleText(csCenterLat);
			continue;
		}

		// ---------------------------------------------------------
		// <CenterLon>
		// ---------------------------------------------------------
		if (wcscmp(name, L"CenterLon") == 0)
		{
			ReadSimpleText(csCenterLon);
			continue;
		}

		// ---------------------------------------------------------
		// CE-only: <MapImage value="Maps/...png"/>
		// ---------------------------------------------------------
		if (wcscmp(name, L"MapImage") == 0)
		{
			const WCHAR* attrName = nullptr;
			const WCHAR* attrValue = nullptr;

			while (pReader->MoveToNextAttribute() == S_OK)
			{
				pReader->GetLocalName(&attrName, nullptr);
				pReader->GetValue(&attrValue, nullptr);

				if (wcscmp(attrName, L"value") == 0 && attrValue)
				{
					csZipPath = attrValue;
				}
			}

			pReader->MoveToElement();
			continue;
		}
	}

	// ---------------------------------------------------------
	// Store values
	// ---------------------------------------------------------
	ContentTitle = csTitle;
	Scope = csScope;
	State = csState;
	Location = csLocation;
	Zoom = _wtoi(csZoom);
	CenterLat = _wtof(csCenterLat);
	CenterLon = _wtof(csCenterLon);

	// ---------------------------------------------------------
	// CE: load snapshot (optional)
	// ---------------------------------------------------------
	if (!csZipPath.IsEmpty() && m_pDoc && m_pDoc->ZipReader)
	{
		auto pZip = m_pDoc->ZipReader;

		if (pZip->IsOpen)
		{
			std::vector<uint8_t> bytes;

			if (pZip->ExtractFile(csZipPath, bytes))
			{
				IStream* pStream = SHCreateMemStream(bytes.data(),
					(UINT)bytes.size());
				if (pStream)
				{
					Gdiplus::Image* pImg = Gdiplus::Image::FromStream(pStream);
					pStream->Release();

					if (pImg)
					{
						ImageContent = shared_ptr<Gdiplus::Image>(pImg);
						return;
					}
				}
			}
		}
	}

	// If no snapshot, map will be regenerated
} // ReadXml

/////////////////////////////////////////////////////////////////////////////
// image of the content
shared_ptr<Gdiplus::Image> CPageMap::GetImageContent()
{
	shared_ptr<Gdiplus::Image> value;

	// ---------------------------------------------------------
	// Ensure document pointer is valid
	// ---------------------------------------------------------
	if (m_pDoc == nullptr)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		m_pDoc = pFrame->ClimateExplorerDocument;
		if (m_pDoc == nullptr)
			return value;
	}

	// ---------------------------------------------------------
	// Lazy rendering: return cached image if available
	// ---------------------------------------------------------
	if (m_pImageContent != nullptr)
	{
		return m_pImageContent;
	}

	// ---------------------------------------------------------
	// Resolve center coordinates and zoom level
	// ---------------------------------------------------------
	ResolveCenterFromQuery();

	// ---------------------------------------------------------
	// Create OSM map object
	// ---------------------------------------------------------
	shared_ptr<CMapOSM> pMapOSM = MapOSM;

	// ---------------------------------------------------------
	// Build tile grid around center
	// ---------------------------------------------------------
	const int radiusX = 5;
	const int radiusY = 3;

	if (!pMapOSM->BuildTileGrid(CenterLat, CenterLon, Zoom, radiusX, radiusY))
		return value;

	// ---------------------------------------------------------
	// Ensure cache folder exists
	// ---------------------------------------------------------
	CString csCacheFolder = L".\\cache\\";
	{
		DWORD dwAttr = GetFileAttributes(csCacheFolder);
		if (dwAttr == INVALID_FILE_ATTRIBUTES || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
			::CreateDirectory(csCacheFolder, nullptr);
		}
	}

	// ---------------------------------------------------------
	// Download tiles
	// ---------------------------------------------------------
	if (!pMapOSM->DownloadTiles(csCacheFolder))
	{
		return value;
	}

	// ---------------------------------------------------------
	// Stitch tiles into final bitmap
	// ---------------------------------------------------------
	if (!pMapOSM->GenerateFinalBitmap())
	{
		return value;
	}

	// -------------------------------------------------------------
	// Populate pins based on the current map's latitude/longitude
	// -------------------------------------------------------------
	m_arrPins.clear();

	CClimateDatabase* pDB = theApp.ClimateDatabase;
	CString csScope = Scope;

	// -------------------------------------------------------------
	// NATIONAL SCOPE → all stations
	// -------------------------------------------------------------
	if (csScope == L"National")
	{
		for (auto& node : pDB->Stations->Items)
		{
			CClimateStation* pStation = node.second.get();

			if (pStation == nullptr)
			{
				continue;
			}

			CString csID = node.first;
			float fLatitude = pStation->Latitude;
			float fLongitude = pStation->Longitude;

			AddStationPin(csID, fLatitude, fLongitude);
		}
	}

	// -------------------------------------------------------------
	// STATE SCOPE → only stations in that state
	// -------------------------------------------------------------
	else if (csScope == L"State")
	{
		CString csState = State;
		vector<CString> arrCities = pDB->Cities[csState];

		for (auto& node : arrCities)
		{
			CString csLocation = node;
			csLocation.Trim();
			CString csKey;
			csKey.Format(L"%s, %s", csState, csLocation);

			shared_ptr<CClimateStation> pStation = 
				pDB->StationByLocation[csKey];

			if (pStation == nullptr)
			{
				continue;
			}

			CString csID = pStation->Station;
			float fLatitude = pStation->Latitude;
			float fLongitude = pStation->Longitude;

			AddStationPin(csID, fLatitude, fLongitude);
		}
	}

	// -------------------------------------------------------------
	// SINGLE STATION SCOPE → only one station
	// -------------------------------------------------------------
	else if (csScope == L"Location")
	{
		CString csState = State;
		CString csLocation = Location;
		csLocation.Trim();
		CString csKey;
		csKey.Format(L"%s, %s", csState, csLocation);

		shared_ptr<CClimateStation> pStation =
			pDB->StationByLocation[csKey];

		if (pStation != nullptr)
		{
			CString csID = pStation->Station;
			float fLatitude = pStation->Latitude;
			float fLongitude = pStation->Longitude;

			AddStationPin(csID, fLatitude, fLongitude);
		}
	}

	// Draw pins on top of the stitched map
	{
		shared_ptr<Bitmap> pBitmap = pMapOSM->FinalBitmap;
		Gdiplus::Graphics g(pBitmap.get());

		for (const auto& pin : *Pins)
		{
			int px = 0;
			int py = 0;

			if (LatLonToPixel(pMapOSM.get(), pin.Lat, pin.Lon, px, py))
			{
				const int radius = 10;
				Gdiplus::SolidBrush brush(pin.Color);

				g.FillEllipse
				(
					&brush, px - radius, py - radius, radius * 2, radius * 2
				);
			}
		}
	}

	//// ---------------------------------------------------------
	//// DEBUG: Save stitched map to PNG for inspection
	//// ---------------------------------------------------------
	//CString csDebugPath = L".\\debug_map.png";
	//CLSID pngClsid;
	//CImagePlus::GetEncoderClsid(L"image/png", &pngClsid);   // your existing helper
	//pMapOSM->FinalBitmap->Save(csDebugPath, &pngClsid, NULL);

	// ---------------------------------------------------------
	// Use stitched map bitmap directly as page image
	// ---------------------------------------------------------
	m_pImageContent = pMapOSM->FinalBitmap;
	value = m_pImageContent;

	return value;
} // GetImageContent

/////////////////////////////////////////////////////////////////////////////
