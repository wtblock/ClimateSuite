/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "PlusGDI.h"
#include "GdiplusColor.h"
#include "KeyedCollection.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// 
// A class to easily translate from Gdiplus::Color defintions to their names.
// 
/////////////////////////////////////////////////////////////////////////////
class CColorPlus : public Color
{
// protected data
protected:
	CKeyedCollection<CString, Gdiplus::ARGB> m_mapColors;
	CKeyedCollection<Gdiplus::ARGB, CString> m_mapARGB;

// public properties
public:
	// get the Color by name
	Color GetColorByName(CString csColor)
	{
		Color value;
		if (m_mapColors.Exists[csColor])
		{
			value.SetValue(*m_mapColors.find(csColor));
		}
		return value;
	}
	// get the color by name
	__declspec(property(get = GetColorByName))
		Color ColorByName[];

	// get the RGB by name
	COLORREF GetRGBByName(CString csColor)
	{
		Color color(ColorByName[csColor]);
		COLORREF value = color.ToCOLORREF();
		return value;
	}
	// get the RGB by name
	__declspec(property(get = GetRGBByName))
		COLORREF RGBByName[];

	// get the ARGB by name
	Gdiplus::ARGB GetARGBByName(CString csColor)
	{
		Color color(ColorByName[csColor]);
		Gdiplus::ARGB value = color.GetValue();
		return value;
	}
	// get the ARGB by name
	__declspec(property(get = GetARGBByName))
		Gdiplus::ARGB ARGBByName[];

	// get the name of given color
	CString GetNameOfARGB(Gdiplus::ARGB argb)
	{
		CString value = L"Black";
		if (m_mapARGB.Exists[argb])
		{
			value = *m_mapARGB.find(argb);
		}
		return value;
	}
	// get the ARGB value by name
	__declspec(property(get = GetNameOfARGB))
		CString NameOfARGB[];

	// get the name of given color
	CString GetNameOfColor(Color color)
	{
		CString value = NameOfARGB[color.GetValue()];
		return value;
	}
	// get the ARGB value by name
	__declspec(property(get = GetNameOfColor))
		CString NameOfColor[];

	// get all color names
	vector<CString> GetAllColorNames()
	{
		vector<CString> value;
		for (auto& csColor : m_mapColors.Items)
		{
			value.push_back(csColor.first);
		}
		return value;
	}
	// get all color names
	__declspec(property(get = GetAllColorNames))
		vector<CString> AllColorNames;

// protected methods
protected:

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CColorPlus();
	~CColorPlus()
	{

	}
}; // class CColorPlus

/////////////////////////////////////////////////////////////////////////////
