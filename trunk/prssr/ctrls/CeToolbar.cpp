/**
 *  CeToolbar.cpp
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "..\stdafx.h"
#include "..\resource.h"
#include "CeToolBar.h"
#include "..\Appearance.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCeToolBar

CCeToolBar::CCeToolBar() {
	m_nImageIdx = -1;
	m_bSortButton = FALSE;
	m_nImageSortIdx = -1;
}

CCeToolBar::~CCeToolBar()
{
}


BEGIN_MESSAGE_MAP(CCeToolBar, CToolBar)
	//{{AFX_MSG_MAP(CCeToolBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCeToolBar message handlers

void CCeToolBar::OnPaint() {
	CWindowDC dc(this);

	CRect	rc;
	GetClientRect(&rc);

	dc.FillSolidRect(rc, ::GetSysColor(COLOR_3DFACE));

	CToolBarCtrl &ctlToolBar = GetToolBarCtrl();
	CImageList *pImageList = ctlToolBar.GetImageList();

	// icon
	if (m_nImageIdx != -1) {
		CPoint pt(SCALEX(3), SCALEY(1) + 1);
		pImageList->SetBkColor(RGB(0, 0, 0));
		pImageList->Draw(&dc, m_nImageIdx, pt, ILD_TRANSPARENT);
	}

	// title
	int oldBkMode = dc.SetBkMode(TRANSPARENT);
	CRect rcText = rc;
	rcText.DeflateRect(SCALEX(22), SCALEY(3), SCALEX(22 + 10), SCALEY(1));

	CFont *oldFont = dc.SelectObject(&Appearance.BaseFont);
	dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
//	DrawTextEndEllipsis(memDC, m_strTitle, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);
	
	UINT fmt = DT_LEFT | DT_TOP | DT_SINGLELINE;
	dc.DrawText(m_strText, rcText, fmt | DT_CALCRECT);
	dc.DrawText(m_strText, rcText, fmt);

	// down arrow
	CPen pen(PS_NULL, 0, 0);
	CPen *pPen = dc.SelectObject(&pen);

	CBrush br(RGB(0, 0, 0));
	CBrush *pBrush = dc.SelectObject(&br);

	CPoint pts[3] = {
		CPoint(rcText.right + SCALEX( 2) + 1, rc.top + SCALEY(9)),
		CPoint(rcText.right + SCALEX(10), rc.top + SCALEY(9)),
		CPoint(rcText.right + SCALEX( 6), rc.top + SCALEY(13)),
	};
	dc.Polygon(pts, 3);

	dc.SelectObject(pBrush);
	dc.SelectObject(pPen);

	m_rcText = rcText;
	m_rcText.left = SCALEX(2);
	m_rcText.right = rcText.right + SCALEX(12);

	dc.SelectObject(oldFont);

	// sort button
	if (m_bSortButton) {
		CPoint pt(rc.right - SCALEX(18), SCALEY(1) + 1);
		pImageList->Draw(&dc, m_nImageSortIdx, pt, ILD_TRANSPARENT);
	}

	ValidateRect(NULL);	
}


void CCeToolBar::OnLButtonDown(UINT nFlags, CPoint point) {
	LOG0(1, "CCeToolBar::OnLButtonDown()");

	CRect rc;
	GetClientRect(&rc);

	CRect rcSort(rc.right - SCALEX(18), rc.top + SCALEY(1) + 1, rc.right, rc.bottom);

	NMTOOLBAR nmtb = { 0 };
	if (m_rcText.PtInRect(point)) {
		// text
		UINT nID = (UINT) GetWindowLong(GetSafeHwnd(), GWL_ID);
		nmtb.hdr.hwndFrom = GetSafeHwnd();
		nmtb.hdr.idFrom = nID;
		nmtb.hdr.code = TBN_DROPDOWN;
		nmtb.iItem = IDC_ACT_SITE;
		GetParent()->SendMessage(WM_NOTIFY, nID, (LPARAM) &nmtb);
	}
	else if (m_bSortButton && rcSort.PtInRect(point)) {
		// sort button
		UINT nID = (UINT) GetWindowLong(GetSafeHwnd(), GWL_ID);
		nmtb.hdr.hwndFrom = GetSafeHwnd();
		nmtb.hdr.idFrom = nID;
		nmtb.hdr.code = TBN_DROPDOWN;
		nmtb.iItem = IDC_SORT_BUTTON;
//		GetParent()->SendMessage(WM_NOTIFY, TBN_DROPDOWN, (LPARAM) &nmtb);
		GetParent()->SendMessage(WM_NOTIFY, nID, (LPARAM) &nmtb);
	}

//	Default();
}
