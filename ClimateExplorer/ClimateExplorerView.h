/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseView.h"
#include "PageContent.h"
#include "ImagePlus.h"

/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerDoc;

/////////////////////////////////////////////////////////////////////////////
// CClimateExplorerView
//
// Specialized view class for ClimateExplorer, derived from CBaseView.
//
// Purpose:
//   • Render complete multi‑page photo books using device‑independent
//     logical coordinates (inches).
//   • Provide unified rendering for:
//         - Screen display
//         - Print preview
//         - Printer output
//         - High‑DPI image export
//         - PDF export
//   • Draw page components in layers:
//         1. Margins
//         2. Header
//         3. Footer
//         4. Title page
//         5. Table of contents
//         6. Image rectangles
//
//   • Support classic scrollbars, context menus, and keyboard navigation.
//   • Provide high‑quality image rendering using CImagePlus to honor
//     GDI mapping mode across all devices.
//
// Why this class exists:
//   ClimateExplorer builds entire books — not single pages. Rendering must
//   be consistent across screen, printer, and export. CBaseView provides
//   the logical coordinate system; CClimateExplorerView provides the book‑
//   specific drawing logic.
//
// Responsibilities:
//   • Override render() to draw multi‑page content.
//   • Draw margins, headers, footers, title page, TOC, and images.
//   • Export pages as JPEG or PDF using high‑DPI rendering.
//   • Manage scrollbars and context menus.
//   • Update the Properties pane with page metadata.
//
// All document‑specific drawing is performed here; CBaseView handles
// device context setup, scrolling, zooming, and mapping modes.
/////////////////////////////////////////////////////////////////////////////
class CClimateExplorerView : public CBaseView
{
// public types
public:
	enum IMAGE_ROTATION
	{
		RotateNone,
		RotateCW,
		RotateCCW
	};

// protected data
protected:
	// selected station
	CString m_csStation;
	// selected latitude
	float m_fLatitude;
	// selected longitude
	float m_fLongitude;

// public properties
public:
	// selected station
	CString GetStation()
	{
		return m_csStation;
	}
	// selected station
	void SetStation(CString value)
	{
		m_csStation = value;
	}
	// selected station
	__declspec(property(get = GetStation, put = SetStation))
		CString Station;

	// station latitude
	float GetLatitude()
	{
		return m_fLatitude;
	}
	// station latitude
	void SetLatitude(float value)
	{
		m_fLatitude = value;
	}
	// station latitude
	__declspec(property(get = GetLatitude, put = SetLatitude))
		float Latitude;

	// station longitude
	float GetLongitude()
	{
		return m_fLongitude;
	}
	// station longitude
	void SetLongitude(float value)
	{
		m_fLongitude = value;
	}
	// station longitude
	__declspec(property(get = GetLongitude, put = SetLongitude))
		float Longitude;

	// title height in logical pixels
	int GetTitleHeight();
	// title height in logical pixels
	__declspec(property(get = GetTitleHeight))
		int TitleHeight;

// protected methods
protected:
	// make room for the title when drawing images
	void AdjustRectForTitle
	(
		CRect& rect, int titleHeight, IMAGE_ROTATION ir
	);

	// draw the title on images and account for rotation and available space
	void DrawTitle
	(
		CDC* pDC, const CString& title,
		const CRect& rectImage, int titleHeight,
		IMAGE_ROTATION ir
	);

// public methods
public:
	CClimateExplorerDoc* GetDocument() const;

	/////////////////////////////////////////////////////////////////////////////
	// RenderMargins
	//
	// Draws page margins and page boundary lines.
	//
	// Behavior:
	//   • Draws outer margin rectangle on page 1.
	//   • Draws bottom‑of‑page line when not printing/exporting.
	//   • Uses logical inches for consistent layout across devices.
	/////////////////////////////////////////////////////////////////////////////
	void RenderMargins
	(
		CDC* pDC, double dLeftOfView, double dTopOfView,
		double dRightOfView, double dBottomOfView
	);

	/////////////////////////////////////////////////////////////////////////////
	// RenderHeader
	//
	// Draws page header containing:
	//   • Book title
	//   • Album/folder name
	//
	// Behavior:
	//   • Alternates left/right alignment for even/odd pages.
	//   • Skips overhead pages (title page + TOC pages).
	/////////////////////////////////////////////////////////////////////////////
	void RenderHeader
	(
		CDC* pDC, double dLeftOfView, double dTopOfView,
		double dRightOfView, double dBottomOfView
	);

	/////////////////////////////////////////////////////////////////////////////
	// RenderFooter
	//
	// Draws page footer containing:
	//   • "Page X of Y"
	//
	// Behavior:
	//   • Skips overhead pages.
	//   • Uses logical inches for consistent placement.
	/////////////////////////////////////////////////////////////////////////////
	void RenderFooter
	(
		CDC* pDC, double dLeftOfView, double dTopOfView,
		double dRightOfView, double dBottomOfView
	);

	/////////////////////////////////////////////////////////////////////////////
	// RenderTitlePage
	//
	// Draws the book’s title page (page 1):
	//   • Title
	//   • Subtitle
	//   • Publisher
	//   • ISBN
	//   • Copyright
	//   • Description
	//
	// Uses large fonts and centered layout.
	/////////////////////////////////////////////////////////////////////////////
	void RenderTitlePage
	(
		CDC* pDC, double dLeftOfView, double dTopOfView,
		double dRightOfView, double dBottomOfView
	);

	/////////////////////////////////////////////////////////////////////////////
	// RenderTableOfContentsPage
	//
	// Draws TOC pages (pages 2..N):
	//   • Section labels
	//   • Page numbers
	//   • Dot leaders
	//
	// Supports multi‑page TOC with 55 lines per page.
	/////////////////////////////////////////////////////////////////////////////
	void RenderTableOfContentsPage
	(
		CDC* pDC, double dLeftOfView, double dTopOfView,
		double dRightOfView, double dBottomOfView
	);

	/////////////////////////////////////////////////////////////////////////////
	// change the image rectangle to make room for the title if necessary
	CRect CClimateExplorerView::ComputeImageRect
	(
		shared_ptr<Image>& pImage,
		const CRect* pRect,
		IMAGE_ROTATION ir
	);

	/////////////////////////////////////////////////////////////////////////////
	// DrawImage
	//
	// Draws a single image inside its assigned rectangle.
	//
	// Behavior:
	//   • Preserves aspect ratio.
	//   • Centers image within rectangle.
	//   • Uses CImagePlus to honor GDI mapping mode across devices.
	/////////////////////////////////////////////////////////////////////////////
	CRect DrawImage
	(
		CDC* pDC, 
		shared_ptr<Image>& pImage, 
		const CRect* pRect, 
		bool bSelected,
		IMAGE_ROTATION ir = RotateNone
	);

	/////////////////////////////////////////////////////////////////////////////
	void CClimateExplorerView::DrawImageWithTitle
	(
		CDC* pDC,
		shared_ptr<CPageContent> pContent,
		shared_ptr<Image>& pImage,
		const CRect* pRect,
		bool bSelected,
		IMAGE_ROTATION ir = RotateNone
	);

	/////////////////////////////////////////////////////////////////////////////
	// RenderImagePage
	//
	// Draws all images on the current page.
	//
	// Behavior:
	//   • Skips overhead pages.
	//   • Only draws images whose rectangles intersect the visible region.
	//   • Uses FindImage() to retrieve cached GDI+ Image objects.
	/////////////////////////////////////////////////////////////////////////////
	void RenderImagePage
	(
		CDC* pDC, double dLeftOfView, double dTopOfView,
		double dRightOfView, double dBottomOfView
	);

	/////////////////////////////////////////////////////////////////////////////
	// ExportDocument
	//
	// Exports selected pages as:
	//   • High‑DPI JPEG images
	//   • Multi‑page PDF (with bookmarks + metadata)
	//
	// Behavior:
	//   • Creates high‑resolution GDI+ Bitmap per page.
	//   • Renders page using logical inches.
	//   • Encodes JPEG in memory for PDF embedding.
	//   • Shows progress dialog with cancel support.
	//   • Writes PDF metadata (title, author, subject, keywords).
	/////////////////////////////////////////////////////////////////////////////
	void ExportDocument();

	// protected overrides
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// public overrides
public:
	/////////////////////////////////////////////////////////////////////////////
	// render
	//
	// Master rendering function for ClimateExplorer.
	//
	// Behavior:
	//   • Loops through all pages in the document.
	//   • Draws only pages that intersect the visible region.
	//   • Calls all page‑component renderers in order:
	//         RenderMargins
	//         RenderHeader
	//         RenderFooter
	//         RenderTitlePage
	//         RenderTableOfContentsPage
	//         RenderImagePage
	//
	//   • Updates Properties pane with current page metadata.
	/////////////////////////////////////////////////////////////////////////////
	virtual void render
	(
		CDC* pDC,
		double dLeftOfView,
		double dTopOfView,
		double dRightOfView,
		double dBottomOfView
	);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	/////////////////////////////////////////////////////////////////////////////
	// PreCreateWindow
	//
	// Forces classic scrollbars and disables composited rendering.
	//
	// Reason:
	//   • WS_EX_COMPOSITED causes scrollbars to flicker and behave poorly
	//     with custom mapping modes.
	//   • WS_EX_CLIENTEDGE restores classic scrollbar appearance.
	/////////////////////////////////////////////////////////////////////////////
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// public constructor/destructor
public:
	CClimateExplorerView() noexcept;
	virtual ~CClimateExplorerView();
	DECLARE_DYNCREATE(CClimateExplorerView)

	// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnScrollTop();
	afx_msg void OnScrollBottom();
	afx_msg void OnScrollPageup();
	afx_msg void OnScrollPagedown();
	afx_msg void OnScrollLineup();
	afx_msg void OnScrollLinedown();
	afx_msg void OnFileExportPages();
	afx_msg void OnUpdateFileExportPages(CCmdUI* pCmdUI);
	afx_msg void OnFilePdf();
	afx_msg void OnUpdateFilePdf(CCmdUI* pCmdUI);
	afx_msg void OnFileExportimages();
	afx_msg void OnUpdateFileExportimages(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBingMap();
	afx_msg void OnUpdateBingMap(CCmdUI* pCmdUI);
	afx_msg void OnGoogleMap();
	afx_msg void OnUpdateGoogleMap(CCmdUI* pCmdUI);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG
// debug version in ClimateExplorerView.cpp
inline CClimateExplorerDoc* CClimateExplorerView::GetDocument() const
{
	return reinterpret_cast<CClimateExplorerDoc*>(m_pDocument);
}
#endif

/////////////////////////////////////////////////////////////////////////////
