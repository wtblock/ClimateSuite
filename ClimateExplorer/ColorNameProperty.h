/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <afxpropertygridctrl.h>
#include "ColorPlus.h"

class CColorNameProperty : public CMFCPropertyGridProperty
{
public:
	CColorNameProperty(const CString& name, const CString& value)
		: CMFCPropertyGridProperty(name, COleVariant(value), NULL, 0)
	{
		// Populate dropdown options from CColorPlus
		for (auto& csName : theApp.ColorPlus->AllColorNames)
		{
			AddOption(csName);
		}

		AllowEdit(FALSE); // force selection from list
	}

	// Draw the selected value in the grid cell
	virtual void OnDrawValue(CDC* pDC, CRect rect) override
	{
		CString csName = (LPCTSTR)(_bstr_t)GetValue();
		COLORREF rgb = theApp.ColorPlus->RGBByName[csName];

		// Draw color swatch
		CRect swatch = rect;
		swatch.right = swatch.left + 20;

		pDC->FillSolidRect(swatch, rgb);
		pDC->Draw3dRect(swatch, RGB(0, 0, 0), RGB(0, 0, 0));

		// Draw text
		rect.left = swatch.right + 4;
		pDC->DrawText(csName, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	void OnClickButton(CPoint /*point*/)
	{
		CString csName = (LPCTSTR)(_bstr_t)GetValue();
		COLORREF initial = theApp.ColorPlus->RGBByName[csName];

		CColorDialog dlg(initial, CC_FULLOPEN);

		if (dlg.DoModal() == IDOK)
		{
			COLORREF rgb = dlg.GetColor();

			// Convert to nearest named color
			CString nearest = theApp.ColorPlus->NearestColorName[rgb];

			// Set the new value
			SetValue(nearest);

			// Force redraw so the swatch appears immediately
			if (m_pWndList != nullptr)
			{
				m_pWndList->EndEditItem();
			}
		}
	}

};
