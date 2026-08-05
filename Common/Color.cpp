/////////////////////////////////////////////////////////////////////////////
// File    : Color.cpp
// Project : Color Management
// Purpose : CColor Implementation
// Date    : 10 Jan 1999
// Author  : Christian Rodemeyer
// Note    : © 1999 by Christian Rodemeyer
//
//          Reference information for the HLS conversion functions:
//            - Foley & Van Dam: “Fundamentals of Interactive Computer Graphics”
//            - MSDN: “HLS Color Spaces”
//            - MSDN: “Converting Colors Between RGB and HLS”
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Color.h"

/////////////////////////////////////////////////////////////////////////////
// CColor: Implementation
/////////////////////////////////////////////////////////////////////////////
const CColor::DNamedColor CColor::m_namedColor[CColor::numNamedColors] =
{
  {aliceblue            , L"aliceblue"},
  {antiquewhite         , L"antiquewhite"},
  {aqua                 , L"aqua"},
  {aquamarine           , L"aquamarine"},
  {azure                , L"azure"},
  {beige                , L"beige"},
  {bisque               , L"bisque"},
  {black                , L"black"},
  {blanchedalmond       , L"blanchedalmond"},
  {blue                 , L"blue"},
  {blueviolet           , L"blueviolet"},
  {brown                , L"brown"},
  {burlywood            , L"burlywood"},
  {cadetblue            , L"cadetblue"},
  {chartreuse           , L"chartreuse"},
  {chocolate            , L"chocolate"},
  {coral                , L"coral"},
  {cornflower           , L"cornflower"},
  {cornsilk             , L"cornsilk"},
  {crimson              , L"crimson"},
  {cyan                 , L"cyan"},
  {darkblue             , L"darkblue"},
  {darkcyan             , L"darkcyan"},
  {darkgoldenrod        , L"darkgoldenrod"},
  {darkgray             , L"darkgray"},
  {darkgreen            , L"darkgreen"},
  {darkkhaki            , L"darkkhaki"},
  {darkmagenta          , L"darkmagenta"},
  {darkolivegreen       , L"darkolivegreen"},
  {darkorange           , L"darkorange"},
  {darkorchid           , L"darkorchid"},
  {darkred              , L"darkred"},
  {darksalmon           , L"darksalmon"},
  {darkseagreen         , L"darkseagreen"},
  {darkslateblue        , L"darkslateblue"},
  {darkslategray        , L"darkslategray"},
  {darkturquoise        , L"darkturquoise"},
  {darkviolet           , L"darkviolet"},
  {deeppink             , L"deeppink"},
  {deepskyblue          , L"deepskyblue"},
  {dimgray              , L"dimgray"},
  {dodgerblue           , L"dodgerblue"},
  {firebrick            , L"firebrick"},
  {floralwhite          , L"floralwhite"},
  {forestgreen          , L"forestgreen"},
  {fuchsia              , L"fuchsia"},
  {gainsboro            , L"gainsboro"},
  {ghostwhite           , L"ghostwhite"},
  {gold                 , L"gold"},
  {goldenrod            , L"goldenrod"},
  {gray                 , L"gray"},
  {green                , L"green"},
  {greenyellow          , L"greenyellow"},
  {honeydew             , L"honeydew"},
  {hotpink              , L"hotpink"},
  {indianred            , L"indianred"},
  {indigo               , L"indigo"},
  {ivory                , L"ivory"},
  {khaki                , L"khaki"},
  {lavender             , L"lavender"},
  {lavenderblush        , L"lavenderblush"},
  {lawngreen            , L"lawngreen"},
  {lemonchiffon         , L"lemonchiffon"},
  {lightblue            , L"lightblue"},
  {lightcoral           , L"lightcoral"},
  {lightcyan            , L"lightcyan"},
  {lightgoldenrodyellow , L"lightgoldenrodyellow"},
  {lightgreen           , L"lightgreen"},
  {lightgrey            , L"lightgrey"},
  {lightpink            , L"lightpink"},
  {lightsalmon          , L"lightsalmon"},
  {lightseagreen        , L"lightseagreen"},
  {lightskyblue         , L"lightskyblue"},
  {lightslategray       , L"lightslategray"},
  {lightsteelblue       , L"lightsteelblue"},
  {lightyellow          , L"lightyellow"},
  {lime                 , L"lime"},
  {limegreen            , L"limegreen"},
  {linen                , L"linen"},
  {magenta              , L"magenta"},
  {maroon               , L"maroon"},
  {mediumaquamarine     , L"mediumaquamarine"},
  {mediumblue           , L"mediumblue"},
  {mediumorchid         , L"mediumorchid"},
  {mediumpurple         , L"mediumpurple"},
  {mediumseagreen       , L"mediumseagreen"},
  {mediumslateblue      , L"mediumslateblue"},
  {mediumspringgreen    , L"mediumspringgreen"},
  {mediumturquoise      , L"mediumturquoise"},
  {mediumvioletred      , L"mediumvioletred"},
  {midnightblue         , L"midnightblue"},
  {mintcream            , L"mintcream"},
  {mistyrose            , L"mistyrose"},
  {moccasin             , L"moccasin"},
  {navajowhite          , L"navajowhite"},
  {navy                 , L"navy"},
  {oldlace              , L"oldlace"},
  {olive                , L"olive"},
  {olivedrab            , L"olivedrab"},
  {orange               , L"orange"},
  {orangered            , L"orangered"},
  {orchid               , L"orchid"},
  {palegoldenrod        , L"palegoldenrod"},
  {palegreen            , L"palegreen"},
  {paleturquoise        , L"paleturquoise"},
  {palevioletred        , L"palevioletred"},
  {papayawhip           , L"papayawhip"},
  {peachpuff            , L"peachpuff"},
  {peru                 , L"peru"},
  {pink                 , L"pink"},
  {plum                 , L"plum"},
  {powderblue           , L"powderblue"},
  {purple               , L"purple"},
  {red                  , L"red"},
  {rosybrown            , L"rosybrown"},
  {royalblue            , L"royalblue"},
  {saddlebrown          , L"saddlebrown"},
  {salmon               , L"salmon"},
  {sandybrown           , L"sandybrown"},
  {seagreen             , L"seagreen"},
  {seashell             , L"seashell"},
  {sienna               , L"sienna"},
  {silver               , L"silver"},
  {skyblue              , L"skyblue"},
  {slateblue            , L"slateblue"},
  {slategray            , L"slategray"},
  {snow                 , L"snow"},
  {springgreen          , L"springgreen"},
  {steelblue            , L"steelblue"},
  {tan                  , L"tan"},
  {teal                 , L"teal"},
  {thistle              , L"thistle"},
  {tomato               , L"tomato"},
  {turquoise            , L"turquoise"},
  {violet               , L"violet"},
  {wheat                , L"wheat"},
  {white                , L"white"},
  {whitesmoke           , L"whitesmoke"},
  {yellow               , L"yellow"},
  {yellowgreen          , L"yellowgreen"},
  {moneygreen           , L"moneygreen"},
  {spanishwhite         , L"spanishwhite"},
  {mediumgray           , L"mediumgray"},
  {midgray              , L"midgray"},
  {midcyan              , L"midcyan"},
  {grayishgreen         , L"grayishgreen"}
};

/////////////////////////////////////////////////////////////////////////////
CColor::CColor(COLORREF cr, bool bIndex)
	: m_bIsRGB(true), m_bIsHLS(false), m_colorref(cr)
{
	NamedColorsIndexed(false);
	if (!bIndex)
	{
		return;
	}
	IndexNamedColors();
}

/////////////////////////////////////////////////////////////////////////////
// find a named color -- returns none if not found
COLORREF CColor::FindNamedColor(LPCTSTR pcszColor)
{
	if (!NamedColorsIndexed())
	{
		IndexNamedColors();
	}
	CString csColor(pcszColor);
	csColor.Replace(_T(" "), _T(""));
	csColor.MakeLower();

	COLORREF rgb = none;
	const POS_NAME_RGB posEnd = m_mapColors.end();
	const POS_NAME_RGB pos = m_mapColors.find(csColor);
	if (pos != posEnd)
	{
		rgb = pos->second;
	}
	return rgb;
} // FindNamedColor

/////////////////////////////////////////////////////////////////////////////
// color from string will do a test for leading "0x" and do FromString
// conversion if true, otherwise and named lookup will attempt to 
// resolve the color, and finally if all else fails, a standard
// numeric hex conversion of the string will be attempted.
COLORREF CColor::ColorFromString(LPCTSTR pcszColor)
{
	CString csColor(pcszColor);

	COLORREF rgb = none;
	// leading "0x" indicates a format of "0xRRGGBB"
	if (csColor.Left(2) == _T("0x"))
	{
		rgb = CColor::FromString(pcszColor);
	}
	else
	{
		rgb = FindNamedColor(pcszColor);
		if (rgb == none)
		{	// try to convert the text as straight hex numeric
			// based on format of "00BBGGRR"
			TCHAR* pEnd = 0;
			rgb = _tcstol(csColor, &pEnd, 16);
		}
	}
	return rgb;
} // ColorFromString

/////////////////////////////////////////////////////////////////////////////
// index the named colors
int CColor::IndexNamedColors()
{
	if (NamedColorsIndexed())
	{
		return IndexedColors();
	}
	NamedColorsIndexed(true);

	// add the named colors to an index
	for (int nColor = 0; nColor < numNamedColors; nColor++)
	{
		PAIR_NAME_RGB pairColor
		(m_namedColor[nColor].name,
			m_namedColor[nColor].color
		);
		addColor(pairColor);
	}
	return IndexedColors();
} // IndexNamedColors

/////////////////////////////////////////////////////////////////////////////
LPCTSTR CColor::GetNameFromIndex(int i)
{
	ASSERT(0 <= i && i < numNamedColors);
	return m_namedColor[i].name;
}

/////////////////////////////////////////////////////////////////////////////
CColor CColor::GetColorFromIndex(int i)
{
	ASSERT(0 <= i && i < numNamedColors);
	return m_namedColor[i].color;
}

/////////////////////////////////////////////////////////////////////////////
CColor CColor::FromString(LPCTSTR pcColor)
{
	CColor t;
	if (!t.SetString(pcColor))
	{
		return CColor(RGB(1, 1, 1));
	}
	return t;
}

/////////////////////////////////////////////////////////////////////////////
CColor CColor::FromName(LPCTSTR pcColor, bool bConvertHex)
{
	CString csColor(pcColor);

	// remove all the white space inside the string
	int iS = 0;
	for (int iC = 0; iC < csColor.GetLength(); iC++)
	{
		csColor.SetAt(iC, ' ');
		if (pcColor[iC] == ' ') continue;
		csColor.SetAt(iS++, pcColor[iC]);
	}
	csColor.TrimRight();

	csColor.MakeLower();
	CColor t(RGB(1, 1, 1));
	int nColors = GetNumNames();
	for (int nColor = 0; nColor < nColors; nColor++)
	{
		if (csColor == m_namedColor[nColor].name)
		{
			return GetColorFromIndex(nColor);
		}
	}
	if (bConvertHex)
	{
		return CColor::FromString(pcColor);
	}
	return t;
}

/////////////////////////////////////////////////////////////////////////////
CColor::operator COLORREF() const
{
	const_cast<CColor*>(this)->ToRGB();
	return m_colorref;
}

/////////////////////////////////////////////////////////////////////////////
// RGB
/////////////////////////////////////////////////////////////////////////////
void CColor::SetRed(int red)
{
	ASSERT(0 <= red && red <= 255);
	ToRGB();
	m_color[c_red] = static_cast<unsigned char>(red);
	m_bIsHLS = false;
}

/////////////////////////////////////////////////////////////////////////////
void CColor::SetGreen(int green)
{
	ASSERT(0 <= green && green <= 255);
	ToRGB();
	m_color[c_green] = static_cast<unsigned char>(green);
	m_bIsHLS = false;
}

/////////////////////////////////////////////////////////////////////////////
void CColor::SetBlue(int blue)
{
	ASSERT(0 <= blue && blue <= 255);
	ToRGB();
	m_color[c_blue] = static_cast<unsigned char>(blue);
	m_bIsHLS = false;
}

/////////////////////////////////////////////////////////////////////////////
void CColor::SetRGB(int red, int blue, int green)
{
	ASSERT(0 <= red && red <= 255);
	ASSERT(0 <= green && green <= 255);
	ASSERT(0 <= blue && blue <= 255);

	m_color[c_red] = static_cast<unsigned char>(red);
	m_color[c_green] = static_cast<unsigned char>(green);
	m_color[c_blue] = static_cast<unsigned char>(blue);
	m_bIsHLS = false;
	m_bIsRGB = true;
}

/////////////////////////////////////////////////////////////////////////////
int CColor::GetRed() const
{
	const_cast<CColor*>(this)->ToRGB();
	return m_color[c_red];
}

/////////////////////////////////////////////////////////////////////////////
int CColor::GetGreen() const
{
	const_cast<CColor*>(this)->ToRGB();
	return m_color[c_green];
}

/////////////////////////////////////////////////////////////////////////////
int CColor::GetBlue() const
{
	const_cast<CColor*>(this)->ToRGB();
	return m_color[c_blue];
}

/////////////////////////////////////////////////////////////////////////////
// HSL
/////////////////////////////////////////////////////////////////////////////
void CColor::SetHue(float hue)
{
	ASSERT(hue >= 0.0 && hue <= 360.0);

	ToHLS();
	m_hue = hue;
	m_bIsRGB = false;
}

/////////////////////////////////////////////////////////////////////////////
void CColor::SetSaturation(float saturation)
{
	ASSERT(saturation >= 0.0 && saturation <= 1.0); // 0.0 ist undefiniert

	ToHLS();
	m_saturation = saturation;
	m_bIsRGB = false;
}

/////////////////////////////////////////////////////////////////////////////
void CColor::SetLuminance(float luminance)
{
	ASSERT(luminance >= 0.0 && luminance <= 1.0);

	ToHLS();
	m_luminance = luminance;
	m_bIsRGB = false;
}

/////////////////////////////////////////////////////////////////////////////
void CColor::SetHLS(float hue, float luminance, float saturation)
{
	ASSERT(hue >= 0.0 && hue <= 360.0);
	ASSERT(luminance >= 0.0 && luminance <= 1.0);
	ASSERT(saturation >= 0.0 && saturation <= 1.0); // 0.0 ist undefiniert

	m_hue = hue;
	m_luminance = luminance;
	m_saturation = saturation;
	m_bIsRGB = false;
	m_bIsHLS = true;
}

/////////////////////////////////////////////////////////////////////////////
float CColor::GetHue() const
{
	const_cast<CColor*>(this)->ToHLS();
	return m_hue;
}

/////////////////////////////////////////////////////////////////////////////
float CColor::GetSaturation() const
{
	const_cast<CColor*>(this)->ToHLS();
	return m_saturation;
}

/////////////////////////////////////////////////////////////////////////////
float CColor::GetLuminance() const
{
	const_cast<CColor*>(this)->ToHLS();
	return m_luminance;
}

/////////////////////////////////////////////////////////////////////////////
// Conversion
/////////////////////////////////////////////////////////////////////////////
void CColor::ToHLS()
{
	if (!m_bIsHLS)
	{
		// Conversion
		unsigned char minval = min(m_color[c_red], min(m_color[c_green], m_color[c_blue]));
		unsigned char maxval = max(m_color[c_red], max(m_color[c_green], m_color[c_blue]));
		float mdiff = float(maxval) - float(minval);
		float msum = float(maxval) + float(minval);

		m_luminance = msum / 510.0f;

		if (maxval == minval)
		{
			m_saturation = 0.0f;
			m_hue = 0.0f;
		}
		else
		{
			float rnorm = (maxval - m_color[c_red]) / mdiff;
			float gnorm = (maxval - m_color[c_green]) / mdiff;
			float bnorm = (maxval - m_color[c_blue]) / mdiff;

			m_saturation = (m_luminance <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

			if (m_color[c_red] == maxval) m_hue = 60.0f * (6.0f + bnorm - gnorm);
			if (m_color[c_green] == maxval) m_hue = 60.0f * (2.0f + rnorm - bnorm);
			if (m_color[c_blue] == maxval) m_hue = 60.0f * (4.0f + gnorm - rnorm);
			if (m_hue > 360.0f) m_hue = m_hue - 360.0f;
		}
		m_bIsHLS = true;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColor::ToRGB()
{
	if (!m_bIsRGB)
	{
		if (m_saturation == 0.0) // Grauton, einfacher Fall
		{
			m_color[c_red] = m_color[c_green] = m_color[c_blue] = unsigned char(m_luminance * 255.0);
		}
		else
		{
			float rm1, rm2;

			if (m_luminance <= 0.5f) rm2 = m_luminance + m_luminance * m_saturation;
			else                     rm2 = m_luminance + m_saturation - m_luminance * m_saturation;
			rm1 = 2.0f * m_luminance - rm2;
			m_color[c_red] = ToRGB1(rm1, rm2, m_hue + 120.0f);
			m_color[c_green] = ToRGB1(rm1, rm2, m_hue);
			m_color[c_blue] = ToRGB1(rm1, rm2, m_hue - 120.0f);
		}
		m_bIsRGB = true;
	}
}

/////////////////////////////////////////////////////////////////////////////
unsigned char CColor::ToRGB1(float rm1, float rm2, float rh)
{
	if (rh > 360.0f) rh -= 360.0f;
	else if (rh < 0.0f) rh += 360.0f;

	if (rh < 60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
	else if (rh < 180.0f) rm1 = rm2;
	else if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

	return static_cast<unsigned char>(rm1 * 255);
}

/////////////////////////////////////////////////////////////////////////////
// String conversion using the RRGGBB hexadecimal color format
/////////////////////////////////////////////////////////////////////////////
CString CColor::GetString() const
{
	CString color;
	color.Format(_T("%02X%02X%02X"), GetRed(), GetGreen(), GetBlue());
	return color;
}

/////////////////////////////////////////////////////////////////////////////
bool CColor::SetString(LPCTSTR pcColor)
{
	ASSERT(pcColor);
	CString strText = pcColor;

	strText.MakeLower();
	int nP = strText.Find(_T("#"));
	if (nP == 0) strText = strText.Mid(nP + 1);
	nP = strText.Find(_T("x"));
	if (nP == 1) strText = strText.Mid(nP + 1);

	int r, g, b;
	if (_stscanf(strText, _T("%2x%2x%2x"), &r, &g, &b) != 3)
	{
		m_color[c_red] = m_color[c_green] = m_color[c_blue] = 0;
		return false;
	}
	else
	{
		m_color[c_red] = static_cast<unsigned char>(r);
		m_color[c_green] = static_cast<unsigned char>(g);
		m_color[c_blue] = static_cast<unsigned char>(b);
		m_bIsRGB = true;
		m_bIsHLS = false;
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CColor::GetName(CString prefix) const
{
	const_cast<CColor*>(this)->ToRGB();
	int i = numNamedColors;
	while (i-- && m_colorref != m_namedColor[i].color);
	if (i < 0)
	{
		return prefix + GetString();
	}
	else return m_namedColor[i].name;
}

/////////////////////////////////////////////////////////////////////////////
