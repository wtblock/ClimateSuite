/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageGraph.h"
#include "ClimateExplorerDoc.h"

/////////////////////////////////////////////////////////////////////////////
CPageGraph::CPageGraph()
{
	ContentType = ContentGraph;
	m_pDoc = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
CPageGraph::CPageGraph(CClimateExplorerDoc* pDoc)
{
	ContentType = ContentGraph;
	m_pDoc = pDoc;
}

/////////////////////////////////////////////////////////////////////////////
CPageGraph::CPageGraph(std::shared_ptr<CGraphPlotter> pPlot, CClimateExplorerDoc* pDoc)
{
	ContentType = ContentGraph;
	m_pPlot = pPlot;
	m_pDoc = pDoc;
}

/////////////////////////////////////////////////////////////////////////////
// CPageGraph::WriteXml  (Unified CE + CEx)
// Writes <Graph> with Picker + Appearance.
// If ZipWriter->IsOpen == true → embeds <Image> and writes PNG to ZIP.
/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteXml
(
	IXmlWriter* pWriter, int nPage, int nItem
)
{
	// <Graph>
	pWriter->WriteStartElement(nullptr, L"Graph", nullptr);

	// Always write full Picker
	WritePickerXml(pWriter);

	// Always write full Appearance
	WriteAppearanceXml(pWriter);

	// -------------------------------------------------------------
	// CE-only behavior: embed <Image> and write PNG to ZIP
	// -------------------------------------------------------------
	auto pZip = m_pDoc->ZipWriter;
	if (pZip && pZip->IsOpen)
	{
		// Compute CE filename
		CString csFilename;
		csFilename.Format
		(
			L"Plots/Page_%04u_Plot_%02u.png",
			nPage, 
			nItem  
		);

		// <Image value="Plots/Page_XXXX_Plot_YY.png"/>
		pWriter->WriteStartElement(nullptr, L"Image", nullptr);
		pWriter->WriteAttributeString(nullptr, L"value", nullptr, csFilename);
		pWriter->WriteEndElement(); // </Image>

		// Render PNG bytes
		std::vector<BYTE> pngBytes;
		CRect rcPixels = m_pDoc->ImageRectangle;
		if (m_pDoc->RenderPlotToPNG(m_pPlot, rcPixels, pngBytes))
		{
			// Add PNG to ZIP
			pZip->AddFile(csFilename, pngBytes.data(), pngBytes.size());
		}
	}

	// </Graph>
	pWriter->WriteEndElement();
} // WriteXml

/////////////////////////////////////////////////////////////////////////////
// CPageGraph::WriteXmlWithImage  (CE)
// Writes <Graph> with Picker, Appearance, and embedded <Image>.
/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteXmlWithImage(IXmlWriter* pWriter, const CString& csFilename)
{
	// <Graph>
	pWriter->WriteStartElement(nullptr, L"Graph", nullptr);

	// Full Picker
	WritePickerXml(pWriter);

	// Full Appearance
	WriteAppearanceXml(pWriter);

	// CE-only: embedded PNG reference
	pWriter->WriteStartElement(nullptr, L"Image", nullptr);
	pWriter->WriteAttributeString(nullptr, L"value", nullptr, csFilename);
	pWriter->WriteEndElement(); // </Image>

	pWriter->WriteEndElement(); // </Graph>
} // WriteXmlWithImage

/////////////////////////////////////////////////////////////////////////////
// CPageGraph::ReadXml
/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadXml(IXmlReader* pReader)
{
	// Ensure we have a plot when called from CPage::ReadXml
	if (m_pPlot == nullptr && m_pDoc != nullptr)
	{
		m_pPlot = std::make_shared<CGraphPlotter>(m_pDoc);
	}

	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		// End of <Graph>
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);
			if (name && wcscmp(name, L"Graph") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		// Unified PageEx structure
		if (wcscmp(name, L"Picker") == 0)
		{
			ReadPickerXml(pReader);
			continue;
		}

		if (wcscmp(name, L"Appearance") == 0)
		{
			ReadAppearanceXml(pReader);
			continue;
		}

		// CE-only: <Image value="Plots/Page_XXXX_Plot_YY.png"/>
		if (wcscmp(name, L"Image") == 0)
		{
			const WCHAR* attrName = nullptr;
			const WCHAR* attrValue = nullptr;

			CString csImagePath;

			while (pReader->MoveToNextAttribute() == S_OK)
			{
				pReader->GetLocalName(&attrName, nullptr);
				pReader->GetValue(&attrValue, nullptr);

				if (wcscmp(attrName, L"value") == 0)
					csImagePath = attrValue;
			}

			if (!csImagePath.IsEmpty())
			{
				shared_ptr<CZipReader> pZip = m_pDoc->ZipReader;
				if (pZip != nullptr)
				{
					if (pZip->IsOpen)
					{
						std::vector<uint8_t> pngBytes;
						if (pZip->ExtractFile(csImagePath, pngBytes))
						{
							m_pPlot->BytesPNG = pngBytes;
						}
					}
				}
			}

			continue;
		}
	}

	// CE: If PNG bytes were already loaded in LoadCE, skip SQL entirely.
	if (m_pPlot != nullptr && !m_pPlot->BytesPNG.empty())
	{
		return;
	}

	// CEx: No PNG → regenerate plot from SQL
	if (m_pPlot != nullptr)
	{
		CString csSQL = m_pPlot->BuildPickerSQL();
		m_pPlot->SQL = csSQL;
		m_pPlot->ExecutePickerQuery();
		m_pPlot->GetDefaults(m_pDoc);
	}
} // ReadXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadPickerXml(IXmlReader* pReader)
{
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* endName = nullptr;
			pReader->GetLocalName(&endName, nullptr);

			if (endName && wcscmp(endName, L"Picker") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign picker values to DOC
		// -----------------------------

		if (wcscmp(name, L"Pure") == 0)
			m_pPlot->Pure = (csValue == L"true");

		else if (wcscmp(name, L"Scope") == 0)
			m_pPlot->Scope = csValue;

		else if (wcscmp(name, L"Subtype") == 0)
			m_pPlot->Subtype = csValue;

		else if (wcscmp(name, L"YearStart") == 0)
			m_pPlot->YearStart = _wtoi(csValue);

		else if (wcscmp(name, L"YearEnd") == 0)
			m_pPlot->YearEnd = _wtoi(csValue);

		else if (wcscmp(name, L"Units") == 0)
			m_pPlot->Units = csValue;

		else if (wcscmp(name, L"Output") == 0)
			m_pPlot->Output = csValue;

		else if (wcscmp(name, L"Threshold") == 0)
			m_pPlot->Threshold = _wtoi(csValue);

		else if (wcscmp(name, L"State") == 0)
			m_pPlot->State = csValue;

		else if (wcscmp(name, L"Location") == 0)
			m_pPlot->Location = csValue;

		else if (wcscmp(name, L"Station") == 0)
			m_pPlot->Station = csValue;

		else if (wcscmp(name, L"Latitude") == 0)
			m_pPlot->Latitude = _wtof(csValue);

		else if (wcscmp(name, L"Longitude") == 0)
			m_pPlot->Longitude = _wtof(csValue);

		//else if (wcscmp(name, L"Title") == 0)
		//	m_pPlot->Title = csValue;
	}
} // ReadPickerXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadAppearanceXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);
			if (name && wcscmp(name, L"Appearance") == 0)
				break;
			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name) continue;

		// Axis, Line, RunningAverage, Grid, Fonts, Padding, 
		// TickLengthInches, Trends
		if (wcscmp(name, L"Axis") == 0)
		{
			ReadAxisXml(pReader);
		}
		else if (wcscmp(name, L"Line") == 0)
		{
			ReadLineXml(pReader);
		}
		else if (wcscmp(name, L"RunningAverage") == 0)
		{
			ReadRunningAverageXml(pReader);
		}
		else if (wcscmp(name, L"Grid") == 0)
		{
			ReadGridXml(pReader);
		}
		else if (wcscmp(name, L"Fonts") == 0)
		{
			ReadFontsXml(pReader);
		}
		else if (wcscmp(name, L"Padding") == 0)
		{
			ReadPaddingXml(pReader);
		}
		else if (wcscmp(name, L"Trends") == 0)
		{
			ReadTrendsXml(pReader);
		}
		else if (wcscmp(name, L"TickLengthInches") == 0)
		{
			CString csValue;
			if (ReadElementString(pReader, csValue))
				m_pPlot->TickLengthInches = _wtof(csValue);
		}
	}
} // ReadAppearanceXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadAxisXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"Axis") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		if (wcscmp(name, L"XLabel") == 0)
			m_pPlot->AxisLabelX = csValue;
		else if (wcscmp(name, L"YLabel") == 0)
			m_pPlot->AxisLabelY = csValue;
	}
} // ReadAxisXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadLineXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"Line") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign to m_pPlot
		// -----------------------------
		if (wcscmp(name, L"Color") == 0)
		{
			m_pPlot->LineColor = csValue;
		}
		else if (wcscmp(name, L"Style") == 0)
		{
			m_pPlot->LineStyle = StringToDashStyle(csValue);
		}
		else if (wcscmp(name, L"ThicknessInches") == 0)
		{
			m_pPlot->LineThicknessInches = _wtof(csValue);
		}
	}
} // ReadLineXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadRunningAverageXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"RunningAverage") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign to m_pPlot
		// -----------------------------
		if (wcscmp(name, L"Color") == 0)
		{
			m_pPlot->RunningAvgColor = csValue;
		}
		else if (wcscmp(name, L"Style") == 0)
		{
			m_pPlot->RunningAvgStyle = StringToDashStyle(csValue);
		}
		else if (wcscmp(name, L"ThicknessInches") == 0)
		{
			m_pPlot->RunningAvgThicknessInches = _wtof(csValue);
		}
	}
} // ReadRunningAverageXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadGridXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"Grid") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign to m_pPlot
		// -----------------------------
		if (wcscmp(name, L"Color") == 0)
		{
			m_pPlot->GridColor = csValue;
		}
		else if (wcscmp(name, L"Style") == 0)
		{
			m_pPlot->GridLineStyle = StringToDashStyle(csValue);
		}
		else if (wcscmp(name, L"ThicknessInches") == 0)
		{
			m_pPlot->GridLineThicknessInches = _wtof(csValue);
		}
	}
} // ReadGridXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadFontsXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"Fonts") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign to m_pPlot
		// -----------------------------
		if (wcscmp(name, L"TitleSizePoints") == 0)
		{
			m_pPlot->TitleFontSizePoints = _wtoi(csValue);
		}
		else if (wcscmp(name, L"AxisLabelSizePoints") == 0)
		{
			m_pPlot->AxisLabelFontSizePoints = _wtoi(csValue);
		}
		else if (wcscmp(name, L"TickLabelSizePoints") == 0)
		{
			m_pPlot->TickLabelFontSizePoints = _wtoi(csValue);
		}
	}
} // ReadFontsXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadPaddingXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"Padding") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign to m_pPlot
		// -----------------------------
		if (wcscmp(name, L"LeftInches") == 0)
		{
			m_pPlot->LeftPaddingInches = _wtof(csValue);
		}
		else if (wcscmp(name, L"RightInches") == 0)
		{
			m_pPlot->RightPaddingInches = _wtof(csValue);
		}
		else if (wcscmp(name, L"TopInches") == 0)
		{
			m_pPlot->TopPaddingInches = _wtof(csValue);
		}
		else if (wcscmp(name, L"BottomInches") == 0)
		{
			m_pPlot->BottomPaddingInches = _wtof(csValue);
		}
	}
} // ReadPaddingXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadSingleTrendXml
(
	IXmlReader* pReader,
	bool& enable,
	CString& color,
	Gdiplus::DashStyle& style,
	double& thickness,
	int& year
)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			// End of TrendOne / TrendTwo / TrendThree
			if 
			(
				name &&
				(
					wcscmp(name, L"TrendOne") == 0 ||
					wcscmp(name, L"TrendTwo") == 0 ||
					wcscmp(name, L"TrendThree") == 0
				)
			)
			{
				break;
			}

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* name = nullptr;
		pReader->GetLocalName(&name, nullptr);
		if (!name)
			continue;

		CString csValue;
		if (!ReadElementString(pReader, csValue))
			continue;

		// -----------------------------
		// Assign to referenced members
		// -----------------------------
		if (wcscmp(name, L"Enable") == 0)
		{
			enable = (csValue.CompareNoCase(L"true") == 0);
		}
		else if (wcscmp(name, L"Color") == 0)
		{
			color = csValue;
		}
		else if (wcscmp(name, L"Style") == 0)
		{
			style = StringToDashStyle(csValue);
		}
		else if (wcscmp(name, L"ThicknessInches") == 0)
		{
			thickness = _wtof(csValue);
		}
		else if (wcscmp(name, L"Year") == 0)
		{
			year = _wtoi(csValue);
		}
	}
} // ReadSingleTrendXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::ReadTrendsXml(IXmlReader* pReader)
{
	HRESULT hr = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;

	while (pReader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_EndElement)
		{
			const WCHAR* name = nullptr;
			pReader->GetLocalName(&name, nullptr);

			if (name && wcscmp(name, L"Trends") == 0)
				break;

			continue;
		}

		if (nodeType != XmlNodeType_Element)
			continue;

		const WCHAR* trendName = nullptr;
		pReader->GetLocalName(&trendName, nullptr);
		if (!trendName)
			continue;

		bool bEnable;
		CString csColor;
		Gdiplus::DashStyle eStyle;
		double dThick;
		int nYear;

		// ---------------------------------------------------------
		// Handle TrendOne, TrendTwo, TrendThree
		// ---------------------------------------------------------
		if (wcscmp(trendName, L"TrendOne") == 0)
		{
			ReadSingleTrendXml(pReader, bEnable, csColor, eStyle, dThick, nYear);
			m_pPlot->TrendOneEnable = bEnable;
			m_pPlot->TrendOneColor = csColor;
			m_pPlot->TrendOneStyle = eStyle;
			m_pPlot->TrendOneThickness = dThick;
			m_pPlot->TrendOneYear = nYear;
		}
		else if (wcscmp(trendName, L"TrendTwo") == 0)
		{
			ReadSingleTrendXml(pReader, bEnable, csColor, eStyle, dThick, nYear);
			m_pPlot->TrendTwoEnable = bEnable;
			m_pPlot->TrendTwoColor = csColor;
			m_pPlot->TrendTwoStyle = eStyle;
			m_pPlot->TrendTwoThickness = dThick;
			m_pPlot->TrendTwoYear = nYear;
		}
		else if (wcscmp(trendName, L"TrendThree") == 0)
		{
			ReadSingleTrendXml(pReader, bEnable, csColor, eStyle, dThick, nYear);
			m_pPlot->TrendThreeEnable = bEnable;
			m_pPlot->TrendThreeColor = csColor;
			m_pPlot->TrendThreeStyle = eStyle;
			m_pPlot->TrendThreeThickness = dThick;
			m_pPlot->TrendThreeYear = nYear;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WritePickerXml(IXmlWriter* pWriter)
{
	HRESULT hr = S_OK;

	hr = pWriter->WriteStartElement(nullptr, L"Picker", nullptr);

	auto W = [&](LPCWSTR name, const CString& val)
	{
		pWriter->WriteElementString(nullptr, name, nullptr, val);
	};
	auto WInt = [&](LPCWSTR name, int v)
	{
		CString cs; cs.Format(L"%d", v);
		pWriter->WriteElementString(nullptr, name, nullptr, cs);
	};

	W(L"Pure", m_pPlot->Pure ? L"true" : L"false");
	W(L"Scope", m_pPlot->Scope);
	W(L"Subtype", m_pPlot->Subtype);
	WInt(L"YearStart", m_pPlot->YearStart);
	WInt(L"YearEnd", m_pPlot->YearEnd);
	W(L"Units", m_pPlot->Units);
	W(L"Output", m_pPlot->Output);
	WInt(L"Threshold", m_pPlot->Threshold);
	W(L"State", m_pPlot->State);
	W(L"Location", m_pPlot->Location);
	W(L"Station", m_pPlot->Station);
	W(L"Latitude", FormatDouble(m_pPlot->Latitude));
	W(L"Longitude", FormatDouble(m_pPlot->Longitude));
	W(L"Title", m_pPlot->GraphTitle);

	pWriter->WriteEndElement(); // </Picker>
} // WritePickerXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteAppearanceXml(IXmlWriter* pWriter)
{
	HRESULT hr = S_OK;

	hr = pWriter->WriteStartElement(nullptr, L"Appearance", nullptr);

	WriteAxisXml(pWriter);
	WriteLineXml(pWriter);
	WriteRunningAverageXml(pWriter);
	WriteGridXml(pWriter);
	WriteFontsXml(pWriter);
	WritePaddingXml(pWriter);
	WriteTrendsXml(pWriter);

	pWriter->WriteEndElement(); // </Appearance>
} // WriteAppearanceXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteAxisXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"Axis", nullptr);

	pWriter->WriteElementString(nullptr, L"XLabel", nullptr, m_pPlot->AxisLabelX);
	pWriter->WriteElementString(nullptr, L"YLabel", nullptr, m_pPlot->AxisLabelY);

	pWriter->WriteEndElement(); // </Axis>
} // WriteAxisXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteLineXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"Line", nullptr);

	pWriter->WriteElementString(nullptr, L"Color", nullptr, m_pPlot->LineColor);
	pWriter->WriteElementString(nullptr, L"Style", nullptr, DashStyleToString(m_pPlot->LineStyle));

	CString cs;
	cs.Format(L"%f", m_pPlot->LineThicknessInches);
	pWriter->WriteElementString(nullptr, L"ThicknessInches", nullptr, cs);

	pWriter->WriteEndElement(); // </Line>
} // WriteLineXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteRunningAverageXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"RunningAverage", nullptr);

	pWriter->WriteElementString(nullptr, L"Color", nullptr, m_pPlot->RunningAvgColor);
	pWriter->WriteElementString(nullptr, L"Style", nullptr, DashStyleToString(m_pPlot->RunningAvgStyle));

	CString cs;
	cs.Format(L"%f", m_pPlot->RunningAvgThicknessInches);
	pWriter->WriteElementString(nullptr, L"ThicknessInches", nullptr, cs);

	pWriter->WriteEndElement(); // </RunningAverage>
} // WriteRunningAverageXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteGridXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"Grid", nullptr);

	pWriter->WriteElementString(nullptr, L"Color", nullptr, m_pPlot->GridColor);
	pWriter->WriteElementString(nullptr, L"Style", nullptr, DashStyleToString(m_pPlot->GridLineStyle));

	CString cs;
	cs.Format(L"%f", m_pPlot->GridLineThicknessInches);
	pWriter->WriteElementString(nullptr, L"ThicknessInches", nullptr, cs);

	pWriter->WriteEndElement(); // </Grid>
} // WriteGridXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteFontsXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"Fonts", nullptr);

	CString cs;

	cs.Format(L"%d", m_pPlot->TitleFontSizePoints);
	pWriter->WriteElementString(nullptr, L"TitleSizePoints", nullptr, cs);

	cs.Format(L"%d", m_pPlot->AxisLabelFontSizePoints);
	pWriter->WriteElementString(nullptr, L"AxisLabelSizePoints", nullptr, cs);

	cs.Format(L"%d", m_pPlot->TickLabelFontSizePoints);
	pWriter->WriteElementString(nullptr, L"TickLabelSizePoints", nullptr, cs);

	pWriter->WriteEndElement(); // </Fonts>
} // WriteFontsXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WritePaddingXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"Padding", nullptr);

	CString cs;

	cs.Format(L"%f", m_pPlot->LeftPaddingInches);
	pWriter->WriteElementString(nullptr, L"LeftInches", nullptr, cs);

	cs.Format(L"%f", m_pPlot->RightPaddingInches);
	pWriter->WriteElementString(nullptr, L"RightInches", nullptr, cs);

	cs.Format(L"%f", m_pPlot->TopPaddingInches);
	pWriter->WriteElementString(nullptr, L"TopInches", nullptr, cs);

	cs.Format(L"%f", m_pPlot->BottomPaddingInches);
	pWriter->WriteElementString(nullptr, L"BottomInches", nullptr, cs);

	pWriter->WriteEndElement(); // </Padding>
} // WritePaddingXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteTrendsXml(IXmlWriter* pWriter)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, L"Trends", nullptr);

	WriteSingleTrendXml
	(
		pWriter,
		L"TrendOne",
		m_pPlot->TrendOneEnable,
		m_pPlot->TrendOneColor,
		m_pPlot->TrendOneStyle,
		m_pPlot->TrendOneThickness,
		m_pPlot->TrendOneYear
	);

	WriteSingleTrendXml
	(
		pWriter,
		L"TrendTwo",
		m_pPlot->TrendTwoEnable,
		m_pPlot->TrendTwoColor,
		m_pPlot->TrendTwoStyle,
		m_pPlot->TrendTwoThickness,
		m_pPlot->TrendTwoYear
	);

	WriteSingleTrendXml
	(
		pWriter,
		L"TrendThree",
		m_pPlot->TrendThreeEnable,
		m_pPlot->TrendThreeColor,
		m_pPlot->TrendThreeStyle,
		m_pPlot->TrendThreeThickness,
		m_pPlot->TrendThreeYear
	);

	pWriter->WriteEndElement(); // </Trends>
} // WriteTrendsXml

/////////////////////////////////////////////////////////////////////////////
void CPageGraph::WriteSingleTrendXml
(
	IXmlWriter* pWriter,
	LPCWSTR trendName,
	bool enable,
	const CString& color,
	Gdiplus::DashStyle style,
	double thickness,
	int year
)
{
	HRESULT hr = pWriter->WriteStartElement(nullptr, trendName, nullptr);

	pWriter->WriteElementString(nullptr, L"Enable", nullptr, enable ? L"true" : L"false");
	pWriter->WriteElementString(nullptr, L"Color", nullptr, color);
	pWriter->WriteElementString(nullptr, L"Style", nullptr, DashStyleToString(style));

	CString cs;
	cs.Format(L"%f", thickness);
	pWriter->WriteElementString(nullptr, L"ThicknessInches", nullptr, cs);

	cs.Format(L"%d", year);
	pWriter->WriteElementString(nullptr, L"Year", nullptr, cs);

	pWriter->WriteEndElement(); // </TrendX>
} // WriteSingleTrendXml

/////////////////////////////////////////////////////////////////////////////
