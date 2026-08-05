/////////////////////////////////////////////////////////////////////////////
// File    : Color.h
// Project : Color Management
// Purpose : CColor Declaration
// Date    : 10 Jan 1999
// Author  : Christian Rodemeyer
// Note    : © 1999 by Christian Rodemeyer
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <map>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CColor: Represents a color value
//
// WTB Note:
// If you will be performing multiple lookups from color names to COLORREF,
// construct with the bIndex flag set to true, or construct normally and call
// FindNamedColor(), which will automatically build the name index.
/////////////////////////////////////////////////////////////////////////////
class CColor
{
protected:
	// Mapping from color name → RGB value
	typedef pair<CString, COLORREF> PAIR_NAME_RGB;
	typedef map<CString, COLORREF> MAP_NAME_RGB;
	typedef MAP_NAME_RGB::iterator POS_NAME_RGB;

	// Named color index
	MAP_NAME_RGB m_mapColors;
	bool m_bNamedColorsIndexed;

public:
	// Number of indexed colors
	inline int IndexedColors() { return (int)m_mapColors.size(); }

	// Whether named colors have been indexed
	inline bool NamedColorsIndexed() { return m_bNamedColorsIndexed; }
	inline void NamedColorsIndexed(bool value) { m_bNamedColorsIndexed = value; }

protected:
	// Clear the named color index
	inline void clearIndex()
	{
		NamedColorsIndexed(false);
		m_mapColors.clear();
	}

	// Add a color to the index
	inline void addColor(PAIR_NAME_RGB pairColor)
	{
		const POS_NAME_RGB posEnd = m_mapColors.end();
		const POS_NAME_RGB pos = m_mapColors.find(pairColor.first);
		if (pos == posEnd)
		{
			m_mapColors.insert(pairColor);
		}
	}

public:
	// Build the named color index
	int IndexNamedColors();

	// Find a named color — returns "none" if not found
	COLORREF FindNamedColor(LPCTSTR pcszColor);

	// Convert a string to a COLORREF:
	// - If the string begins with "0x", treat it as a hex value
	// - Otherwise attempt a named color lookup
	// - If that fails, attempt a standard numeric hex conversion
	COLORREF ColorFromString(LPCTSTR pcszColor);

public:
	// Constructor:
	// If you will be doing multiple name → COLORREF lookups,
	// construct with bIndex = true.
	CColor(COLORREF cr = black, bool bIndex = false);
	operator COLORREF() const;

	// RGB routines
	void SetRed(int red);       // 0..255
	void SetGreen(int green);   // 0..255
	void SetBlue(int blue);     // 0..255
	void SetRGB(int red, int blue, int green);

	int GetRed() const;         // 0..255
	int GetGreen() const;       // 0..255
	int GetBlue() const;        // 0..255

	// HLS routines
	void SetHue(float hue);               // 0.0 .. 360.0
	void SetLuminance(float luminance);   // 0.0 .. 1.0
	void SetSaturation(float saturation); // 0.0 .. 1.0
	void SetHLS(float hue, float luminance, float saturation);

	float GetHue() const;        // 0.0 .. 360.0
	float GetLuminance() const;  // 0.0 .. 1.0
	float GetSaturation() const; // 0.0 .. 1.0

	// String conversion
	CString GetString() const;          // Returns "RRGGBB"
	bool    SetString(LPCTSTR pcColor); // Expects "RRGGBB"

	// Named web colors
	CString GetName(CString csPrefix = _T("#")) const;
	// Returns the named color or "#RRGGBB" if no name is known

	static CColor FromString(LPCTSTR pcColor);
	static CColor FromName(LPCTSTR pcColor, bool bConvertHex = false);
	static LPCTSTR GetNameFromIndex(int i);
	static CColor GetColorFromIndex(int i);
	static int GetNumNames() { return numNamedColors; }

	// Named colors (usable as COLORREF)
	enum ENamedColor
	{
		none = 0x7FFFFFFF,
		aliceblue = 0x00FFF8F0,
		antiquewhite = 0x00D7EBFA,
		aqua = 0x00FFFF00,
		aquamarine = 0x00D4FF7F,
		azure = 0x00FFFFF0,
		beige = 0x00DCF5F5,
		bisque = 0x00C4E4FF,
		black = 0x00000000,
		blanchedalmond = 0x00CDEBFF,
		blue = 0x00FF0000,
		blueviolet = 0x00E22B8A,
		brown = 0x002A2AA5,
		burlywood = 0x0087B8DE,
		cadetblue = 0x00A09E5F,
		chartreuse = 0x0000FF7F,
		chocolate = 0x001E69D2,
		coral = 0x00507FFF,
		cornflower = 0x00ED9564,
		cornsilk = 0x00DCF8FF,
		crimson = 0x003C14DC,
		cyan = 0x00FFFF00,
		darkblue = 0x008B0000,
		darkcyan = 0x008B8B00,
		darkgoldenrod = 0x000B86B8,
		darkgray = 0x00A9A9A9,
		darkgreen = 0x00006400,
		darkkhaki = 0x006BB7BD,
		darkmagenta = 0x008B008B,
		darkolivegreen = 0x002F6B55,
		darkorange = 0x00008CFF,
		darkorchid = 0x00CC3299,
		darkred = 0x0000008B,
		darksalmon = 0x007A96E9,
		darkseagreen = 0x008BBC8F,
		darkslateblue = 0x008B3D48,
		darkslategray = 0x004F4F2F,
		darkturquoise = 0x00D1CE00,
		darkviolet = 0x00D30094,
		deeppink = 0x009314FF,
		deepskyblue = 0x00FFBF00,
		dimgray = 0x00696969,
		dodgerblue = 0x00FF901E,
		firebrick = 0x002222B2,
		floralwhite = 0x00F0FAFF,
		forestgreen = 0x00228B22,
		fuchsia = 0x00FF00FF,
		gainsboro = 0x00DCDCDC,
		ghostwhite = 0x00FFF8F8,
		gold = 0x0000D7FF,
		goldenrod = 0x0020A5DA,
		gray = 0x00808080,
		green = 0x00008000,
		greenyellow = 0x002FFFAD,
		honeydew = 0x00F0FFF0,
		hotpink = 0x00B469FF,
		indianred = 0x005C5CCD,
		indigo = 0x0082004B,
		ivory = 0x00F0FFFF,
		khaki = 0x008CE6F0,
		lavender = 0x00FAE6E6,
		lavenderblush = 0x00F5F0FF,
		lawngreen = 0x0000FC7C,
		lemonchiffon = 0x00CDFAFF,
		lightblue = 0x00E6D8AD,
		lightcoral = 0x008080F0,
		lightcyan = 0x00FFFFE0,
		lightgoldenrodyellow = 0x00D2FAFA,
		lightgreen = 0x0090EE90,
		lightgrey = 0x00D3D3D3,
		lightpink = 0x00C1B6FF,
		lightsalmon = 0x007AA0FF,
		lightseagreen = 0x00AAB220,
		lightskyblue = 0x00FACE87,
		lightslategray = 0x00998877,
		lightsteelblue = 0x00DEC4B0,
		lightyellow = 0x00E0FFFF,
		lime = 0x0000FF00,
		limegreen = 0x0032CD32,
		linen = 0x00E6F0FA,
		magenta = 0x00FF00FF,
		maroon = 0x00000080,
		mediumaquamarine = 0x00AACD66,
		mediumblue = 0x00CD0000,
		mediumorchid = 0x00D355BA,
		mediumpurple = 0x00DB7093,
		mediumseagreen = 0x0071B33C,
		mediumslateblue = 0x00EE687B,
		mediumspringgreen = 0x009AFA00,
		mediumturquoise = 0x00CCD148,
		mediumvioletred = 0x008515C7,
		midnightblue = 0x00701919,
		mintcream = 0x00FAFFF5,
		mistyrose = 0x00E1E4FF,
		moccasin = 0x00B5E4FF,
		navajowhite = 0x00ADDEFF,
		navy = 0x00800000,
		oldlace = 0x00E6F5FD,
		olive = 0x00008080,
		olivedrab = 0x00238E6B,
		orange = 0x0000A5FF,
		orangered = 0x000045FF,
		orchid = 0x00D670DA,
		palegoldenrod = 0x00AAE8EE,
		palegreen = 0x0098FB98,
		paleturquoise = 0x00EEEEAF,
		palevioletred = 0x009370DB,
		papayawhip = 0x00D5EFFF,
		peachpuff = 0x00B9DAFF,
		peru = 0x003F85CD,
		pink = 0x00CBC0FF,
		plum = 0x00DDA0DD,
		powderblue = 0x00E6E0B0,
		purple = 0x00800080,
		red = 0x000000FF,
		rosybrown = 0x008F8FBC,
		royalblue = 0x00E16941,
		saddlebrown = 0x0013458B,
		salmon = 0x007280FA,
		sandybrown = 0x0060A4F4,
		seagreen = 0x00578B2E,
		seashell = 0x00EEF5FF,
		sienna = 0x002D52A0,
		silver = 0x00C0C0C0,
		skyblue = 0x00EBCE87,
		slateblue = 0x00CD5A6A,
		slategray = 0x00908070,
		snow = 0x00FAFAFF,
		springgreen = 0x007FFF00,
		steelblue = 0x00B48246,
		tan = 0x008CB4D2,
		teal = 0x00808000,
		thistle = 0x00D8BFD8,
		tomato = 0x004763FF,
		turquoise = 0x00D0E040,
		violet = 0x00EE82EE,
		wheat = 0x00B3DEF5,
		white = 0x00FFFFFF,
		whitesmoke = 0x00F5F5F5,
		yellow = 0x0000FFFF,
		yellowgreen = 0x0032CD9A,
		yellowselect = 0x00CCFFFF,
		moneygreen = 0x00DCC008,
		spanishwhite = 0x00F0FBFF,
		mediumgray = 0x00A4A0A0,
		midgray = 0x009B9B9B,
		midcyan = 0x00F0CAA6,
		grayishgreen = 0x00C0DCC0,

		numNamedColors
	};

private:
	// Conversion helpers
	void ToRGB(); // logically constant
	void ToHLS(); // logically constant
	static unsigned char ToRGB1(float rm1, float rm2, float rh);

	// Data storage (byte-level access to COLORREF)
	union
	{
		COLORREF      m_colorref;
		unsigned char m_color[4];
	};
	enum { c_red = 0, c_green = 1, c_blue = 2, c_null = 3 };

	float m_hue;         // 0.0 .. 360.0
	float m_saturation;  // 0.0 .. 1.0
	float m_luminance;   // 0.0 .. 1.0

	// Lazy evaluation flags
	bool m_bIsRGB;
	bool m_bIsHLS;

	// Static named color table
	struct DNamedColor
	{
		COLORREF color;
		LPCTSTR  name;
	};
	static const DNamedColor m_namedColor[numNamedColors];
};
