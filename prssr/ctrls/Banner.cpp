/**
 *  Banner.cpp
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

#include "../StdAfx.h"
#include "../prssr.h"

#include "../../share/UIHelper.h"
#include "../../share/helpers.h"
#include "Banner.h"
#include "../Appearance.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBanner

CBanner::CBanner() {
	HGDIOBJ hObj = GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	::GetObject(hObj, sizeof(LOGFONT), (LPVOID) &lf);
	lf.lfHeight = SCALEY(12) + 1;
	lf.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lf);

/*	m_rcPadding = CRect(0, 0, 0, 0);
	m_bBold = TRUE;
*/

//	m_oImageList.Create(SCALEX(16), SCALEY(16), ILC_COLOR | ILC_MASK, 1, 0);
//	m_oImageList.SetBkColor(RGB(192, 192, 192));

	m_clrBackground = GetSysColor(COLOR_BTNFACE);
	m_clrForeground = GetSysColor(COLOR_BTNTEXT);

	m_nPos = m_nTotal = 0;
	m_nImage = -1;
}

CBanner::~CBanner() {
	m_fntBold.DeleteObject();	
}


BEGIN_MESSAGE_MAP(CBanner, CStatic)
	//{{AFX_MSG_MAP(CBanner)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBanner message handlers

void CBanner::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(&rcClient);

	// offscreen buffer
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap *saveBmp = memDC.SelectObject(&bmp);

	//
	CFont *oldFont;

	memDC.FillSolidRect(&rcClient, m_clrBackground);

	// icon
	if (m_pImageList != NULL && m_nImage != -1) {
		CPoint pt(SCALEX(3), SCALEY(1) + 1);
		m_pImageList->SetBkColor(RGB(0, 0, 0));
		m_pImageList->Draw(&memDC, m_nImage, pt, ILD_TRANSPARENT);
	}

	// flagged?
	if (m_pSmbImageList != NULL && m_nFlaggedImage != -1) {
		CPoint pt(SCALEX(1), SCALEY(1) + 1);
		m_pSmbImageList->SetBkColor(RGB(0, 0, 0));
		m_pSmbImageList->Draw(&memDC, m_nFlaggedImage, pt, ILD_TRANSPARENT);
	}

	// title
	int oldBkMode = memDC.SetBkMode(TRANSPARENT);
	CRect rcText = rcClient;
	rcText.DeflateRect(SCALEX(22), SCALEY(3), SCALEX(50), SCALEY(1));

//	oldFont = dc.SelectObject(&m_fntBold);
	oldFont = memDC.SelectObject(&Appearance.BaseFont);
	memDC.SetTextColor(m_clrForeground);
	DrawTextEndEllipsis(memDC, m_strTitle, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);
	memDC.SelectObject(oldFont);

	// numbers
	CRect rcNum = rcClient;
	rcNum.DeflateRect(0, SCALEY(3), SCALEX(3), SCALEY(1));
//	rcNum.left = rcNum.right - SCALEX(50);
	oldFont = memDC.SelectObject(&Appearance.BaseFont);
	CString strNum;
	strNum.Format(_T("%d/%d"), m_nPos, m_nTotal);
	memDC.DrawText(strNum, rcNum, DT_RIGHT | DT_TOP | DT_SINGLELINE);
	memDC.SelectObject(oldFont);

	memDC.SetBkMode(oldBkMode);

	// line
	CPen blackPen(PS_SOLID, SCALEY(1), GetSysColor(COLOR_WINDOWFRAME));

	CPen *oldPen  = memDC.SelectObject(&blackPen);
//	int nHalf = (rcClient.bottom + rcClient.top) / SCALEY(2);
	memDC.MoveTo(rcClient.left, rcClient.bottom - 1);
	memDC.LineTo(rcClient.right, rcClient.bottom - 1);
	memDC.SelectObject(oldPen);

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

	//
	memDC.SelectObject(saveBmp);

	ValidateRect(NULL);	
	
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CBanner::OnEraseBkgnd(CDC *pDC) {
	return TRUE;
}

void CBanner::OnLButtonDown(UINT nFlags, CPoint point) {
	m_bClick = FALSE;

	CRect rc;
	GetClientRect(&rc);
	rc.right = rc.left + SCALEX(20);

	if (rc.PtInRect(point)) {
		m_bClick = TRUE;
		SetCapture();
	}
}

void CBanner::OnLButtonUp(UINT nFlags, CPoint point) {
	ReleaseCapture();

	CRect rc;
	GetClientRect(&rc);
	rc.right = rc.left + SCALEX(20);

	if (m_bClick && rc.PtInRect(point)) {
		GetParent()->PostMessage(WM_COMMAND, ID_ITEM_FLAG, 0);
	}

	m_bClick = FALSE;
}
