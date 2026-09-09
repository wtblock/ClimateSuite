/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gdiplus.h>

/////////////////////////////////////////////////////////////////////////////
// CMarkdownRenderer
//
// Receives Markdown parse events from CMarkdownParser.
// The derived class will convert these events into a rendered bitmap.
// This class defines the full callback surface required by md4c.
/////////////////////////////////////////////////////////////////////////////
class CMarkdownRenderer
{
	// public type definitions
public:
	enum CellFontStyle
	{
		CellFontStyle_Normal = 0,
		CellFontStyle_Bold = 1,
		CellFontStyle_Italic = 2,
		CellFontStyle_Monospace = 4
	};

	struct CellPadding
	{
		double Left;
		double Right;
		double Top;
		double Bottom;

		CellPadding()
			: Left(0.0), Right(0.0), Top(0.0), Bottom(0.0)
		{
		}
	};

	struct CellBorder
	{
		bool DrawTop;
		bool DrawBottom;
		bool DrawLeft;
		bool DrawRight;

		Gdiplus::Color Color;
		float Thickness;

		CellBorder()
			: DrawTop(true),
			DrawBottom(true),
			DrawLeft(false),
			DrawRight(false),
			Color(0xD0, 0xD7, 0xDE),
			Thickness(1.0f)
		{
		}
	};

	struct CellCornerRadius
	{
		float TL; // top-left
		float TR; // top-right
		float BR; // bottom-right
		float BL; // bottom-left

		CellCornerRadius()
			: TL(0.0f), TR(0.0f), BR(0.0f), BL(0.0f)
		{
		}
	};

	struct CellShadow
	{
		bool Enabled;
		float OffsetX;
		float OffsetY;
		float Blur;      // not true Gaussian blur, but simulated
		Gdiplus::Color Color;

		CellShadow()
			: Enabled(false),
			OffsetX(2.0f),
			OffsetY(2.0f),
			Blur(3.0f),
			Color(0x80, 0x00, 0x00, 0x00) // semi-transparent black
		{
		}
	};

	enum CellGradientType
	{
		Gradient_None = 0,
		Gradient_Linear = 1,
		Gradient_Radial = 2
	};

	struct CellGradient
	{
		CellGradientType Type;

		Gdiplus::Color StartColor;
		Gdiplus::Color EndColor;

		float Angle;   // linear gradient angle in degrees
		float Radius;  // radial gradient radius multiplier

		CellGradient()
			: Type(Gradient_None),
			StartColor(Gdiplus::Color::Transparent),
			EndColor(Gdiplus::Color::Transparent),
			Angle(90.0f),
			Radius(1.0f)
		{
		}
	};

	enum CellImageMode
	{
		Image_None = 0,
		Image_Stretch = 1,
		Image_Center = 2,
		Image_Tile = 3
	};

	struct CellImageBackground
	{
		CellImageMode Mode;
		Gdiplus::Image* pImage;  // caller owns lifetime
		float Opacity;           // 0.0 to 1.0

		CellImageBackground()
			: Mode(Image_None),
			pImage(nullptr),
			Opacity(1.0f)
		{
		}
	};

	enum CellOverlayMode
	{
		Overlay_None = 0,
		Overlay_Color = 1,   // solid color overlay
		Overlay_Image = 2,   // image overlay
		Overlay_Glass = 3    // frosted-glass simulation
	};

	struct CellOverlay
	{
		CellOverlayMode Mode;

		// color overlay
		Gdiplus::Color Color;
		float Opacity;

		// image overlay
		Gdiplus::Image* pImage;
		float ImageOpacity;

		// glass overlay
		float GlassOpacity;

		CellOverlay()
			: Mode(Overlay_None),
			Color(Gdiplus::Color::Transparent),
			Opacity(0.0f),
			pImage(nullptr),
			ImageOpacity(1.0f),
			GlassOpacity(0.3f)
		{
		}
	};

	enum CellBlendMode
	{
		Blend_None = 0,
		Blend_Multiply,
		Blend_Screen,
		Blend_Overlay,
		Blend_Lighten,
		Blend_Darken
	};

	struct CellBlend
	{
		CellBlendMode Mode;
		Gdiplus::Color Color;  // blend color
		float Opacity;         // 0.0 to 1.0

		CellBlend()
			: Mode(Blend_None),
			Color(Gdiplus::Color::Transparent),
			Opacity(1.0f)
		{
		}
	};



// protected data
protected:

// public properties
public:

// protected methods
protected:

// public methods
public:
	virtual void OnHorizontalRule() {}

	virtual void OnParagraphStart() {}
	virtual void OnParagraphEnd() {}

	virtual void OnHeadingStart(int /*level*/) {}
	virtual void OnHeadingEnd() {}

	virtual void OnUnorderedListStart() {}
	virtual void OnUnorderedListEnd() {}

	virtual void OnOrderedListStart() {}
	virtual void OnOrderedListEnd() {}

	virtual void OnListItemStart() {}
	virtual void OnListItemEnd() {}

	virtual void OnBlockQuoteStart() {}
	virtual void OnBlockQuoteEnd() {}

	virtual void OnCodeBlockStart() {}
	virtual void OnCodeBlockEnd() {}

	virtual void OnEmphasisStart() {}
	virtual void OnEmphasisEnd() {}

	virtual void OnStrongStart() {}
	virtual void OnStrongEnd() {}

	virtual void OnInlineCodeStart() {}
	virtual void OnInlineCodeEnd() {}

	virtual void OnLinkStart(const CString& /*href*/) {}
	virtual void OnLinkEnd() {}

	virtual void OnText(const CString& /*text*/) {}
	virtual void OnInlineCodeText(const CString& /*text*/) {}
	virtual void OnHtmlText(const CString& /*text*/) {}
	virtual void OnEntityText(const CString& /*text*/) {}

	//
	// TABLE RENDERING (Step 64)
	//
	virtual void OnTableStart() {}
	virtual void OnTableEnd() {}

	virtual void OnTableHeaderStart() {}
	virtual void OnTableHeaderEnd() {}

	virtual void OnTableRowStart() {}
	virtual void OnTableRowEnd() {}

	virtual void OnTableCell(const CString& /*text*/, int /*columnIndex*/) {}

	//
	// TABLE ALIGNMENT (Step 65)
	//
	virtual void OnTableColumnAlignment
	(
		int /*columnIndex*/, int /*alignment*/
	) {}

	virtual void OnTableColumnVerticalAlignment
	(
		int /*columnIndex*/, int /*alignment*/
	) {}

	virtual void OnTableColumnBackColor
	(
		int /*columnIndex*/, Gdiplus::Color /*color*/
	) {}

	virtual void OnTableCellBackColor
	(
		int /*row*/, int /*col*/, Gdiplus::Color /*color*/
	) {}

	virtual void OnTableCellTextColor
	(
		int /*nRowIndex*/, int /*nColumnIndex*/, Gdiplus::Color /*color*/
	) {}

	virtual void OnTableCellFontStyle
	(
		int /*row*/, int /*col*/, int /*style*/
	) {}

	virtual void OnTableCellPadding
	(
		int /*row*/,
		int /*col*/,
		double /*left*/,
		double /*right*/,
		double /*top*/,
		double /*bottom*/
	) {}

	virtual void OnTableCellBorder
	(
		int /*row*/,
		int /*col*/,
		bool /*drawTop*/,
		bool /*drawBottom*/,
		bool /*drawLeft*/,
		bool /*drawRight*/,
		Gdiplus::Color /*color*/,
		float /*thickness*/
	) {}

	virtual void OnTableCellCornerRadius
	(
		int /*row*/,
		int /*col*/,
		float /*tl*/,
		float /*tr*/,
		float /*br*/,
		float /*bl*/
	) {}

	virtual void OnTableCellShadow
	(
		int /*row*/,
		int /*col*/,
		bool /*enabled*/,
		float /*offsetX*/,
		float /*offsetY*/,
		float /*blur*/,
		Gdiplus::Color /*color*/
	) {}

	virtual void OnTableCellGradient
	(
		int /*row*/,
		int /*col*/,
		CellGradientType /*type*/,
		Gdiplus::Color /*startColor*/,
		Gdiplus::Color /*endColor*/,
		float /*angle*/,
		float /*radius*/
	) {}

	virtual void OnTableCellImageBackground
	(
		int /*row*/,
		int /*col*/,
		CellImageMode /*mode*/,
		Gdiplus::Image* /*pImage*/,
		float /*opacity*/
	) {}

	virtual void OnTableCellOverlay
	(
		int /*row*/,
		int /*col*/,
		CellOverlayMode /*mode*/,
		Gdiplus::Color /*color*/,
		float /*opacity*/,
		Gdiplus::Image* /*pImage*/,
		float /*imageOpacity*/,
		float /*glassOpacity*/
	) {}

	virtual void OnTableCellBlend
	(
		int /*row*/,
		int /*col*/,
		CellBlendMode /*mode*/,
		Gdiplus::Color /*color*/,
		float /*opacity*/
	) {}



// protected overrides
protected:

// public overrides
public:

// public constructor/destructor
public:
	CMarkdownRenderer()
	{
	}

	virtual ~CMarkdownRenderer()
	{
	}
}; // class CMarkdownRenderer

/////////////////////////////////////////////////////////////////////////////
