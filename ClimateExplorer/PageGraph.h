/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "PageContent.h"
#include "GraphPlotter.h"

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
// CPageGraph
//
// Shell wrapper for CGraphPlotter. This class will eventually handle:
//
//     • XML serialization of graph properties
//     • XML deserialization of graph properties
//     • Integration with CPageContent polymorphism
//     • CE (.CE) minimal PNG export
//     • CEx (.CEx) full‑fidelity export
//
// For now, it simply wraps a CGraphPlotter instance and compiles cleanly.
//
/////////////////////////////////////////////////////////////////////////////
class CPageGraph : public CPageContent
{
protected:
	///////////////////////////////////////////////////////////////////////////
	// m_pPlot
	///////////////////////////////////////////////////////////////////////////
	std::shared_ptr<CGraphPlotter> m_pPlot;

	CClimateExplorerDoc* m_pDoc;

public:
	///////////////////////////////////////////////////////////////////////////
	// Plot property
	///////////////////////////////////////////////////////////////////////////
	std::shared_ptr<CGraphPlotter> GetPlot()
	{
		return m_pPlot;
	}

	void SetPlot(std::shared_ptr<CGraphPlotter> value)
	{
		m_pPlot = value;
	}

	__declspec(property(get = GetPlot, put = SetPlot))
		std::shared_ptr<CGraphPlotter> Plot;

// XML helper methods
protected:
	void ReadPickerXml(IXmlReader* pReader);
	void ReadAppearanceXml(IXmlReader* pReader);
	void ReadAxisXml(IXmlReader* pReader);
	void ReadLineXml(IXmlReader* pReader);
	void ReadRunningAverageXml(IXmlReader* pReader);
	void ReadGridXml(IXmlReader* pReader);
	void ReadFontsXml(IXmlReader* pReader);
	void ReadPaddingXml(IXmlReader* pReader);
	void ReadTrendsXml(IXmlReader* pReader);

	// convert a string name into a Gdiplus DashStyle enumeration
	Gdiplus::DashStyle StringToDashStyle(const CString& s)
	{
		if (s == L"Solid") return Gdiplus::DashStyleSolid;
		if (s == L"Dash") return Gdiplus::DashStyleDash;
		if (s == L"Dot") return Gdiplus::DashStyleDot;
		if (s == L"DashDot") return Gdiplus::DashStyleDashDot;
		if (s == L"DashDotDot") return Gdiplus::DashStyleDashDotDot;
		return Gdiplus::DashStyleSolid;
	}

	// convert a Gdiplus DashStyle enumeration into a string name
	CString DashStyleToString(Gdiplus::DashStyle style)
	{
		switch (style)
		{
		case Gdiplus::DashStyleSolid:      return L"Solid";
		case Gdiplus::DashStyleDash:       return L"Dash";
		case Gdiplus::DashStyleDot:        return L"Dot";
		case Gdiplus::DashStyleDashDot:    return L"DashDot";
		case Gdiplus::DashStyleDashDotDot: return L"DashDotDot";
		default:                           return L"Solid";
		}
	}

	void ReadSingleTrendXml
	(
		IXmlReader* pReader,
		bool& enable,
		CString& color,
		Gdiplus::DashStyle& style,
		double& thickness,
		int& year
	);

// XML write helper methods
protected:
	void WritePickerXml(IXmlWriter* pWriter);
	void WriteAppearanceXml(IXmlWriter* pWriter);
	void WriteAxisXml(IXmlWriter* pWriter);
	void WriteLineXml(IXmlWriter* pWriter);
	void WriteRunningAverageXml(IXmlWriter* pWriter);
	void WriteGridXml(IXmlWriter* pWriter);
	void WriteFontsXml(IXmlWriter* pWriter);
	void WritePaddingXml(IXmlWriter* pWriter);
	void WriteTrendsXml(IXmlWriter* pWriter);

	void WriteSingleTrendXml
	(
		IXmlWriter* pWriter,
		LPCWSTR trendName,
		bool enable,
		const CString& color,
		Gdiplus::DashStyle style,
		double thickness,
		int year
	);

	static CString FormatDouble(double value)
	{
		CString cs;
		cs.Format(L"%.6f", value);
		return cs;
	}


public:
	///////////////////////////////////////////////////////////////////////////
	// WriteXml
	//
	// Shell only — does nothing yet.
	// Will be implemented during the serialization refactor.
	///////////////////////////////////////////////////////////////////////////
	virtual void WriteXml(IXmlWriter* pWriter) override;

	///////////////////////////////////////////////////////////////////////////
	// ReadXml
	//
	// Shell only — does nothing yet.
	// Will be implemented during the serialization refactor.
	///////////////////////////////////////////////////////////////////////////
	virtual void ReadXml(IXmlReader* pReader) override;

public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor / Destructor
	///////////////////////////////////////////////////////////////////////////
	CPageGraph();

	CPageGraph(CClimateExplorerDoc* pDoc);

	CPageGraph(std::shared_ptr<CGraphPlotter> pPlot, CClimateExplorerDoc* pDoc);

	virtual ~CPageGraph()
	{
	}
};
