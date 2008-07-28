/**
 *  Progress.cpp
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
#include "Progress.h"

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

#define BAR_HT				9

/////////////////////////////////////////////////////////////////////////////
// CProgress

CProgress::CProgress()
{
	m_bRunning = FALSE;
	m_nOffset = 0;

	m_bmp = NULL;
	m_dcMem = NULL;
	m_nTimer = 1;
}

CProgress::~CProgress()
{
	if (m_dcMem != NULL && m_bmp != NULL)
		m_dcMem->SelectObject(m_bmp);	// select original bitmap
	delete m_dcMem;
}


BEGIN_MESSAGE_MAP(CProgress, CStatic)
	//{{AFX_MSG_MAP(CProgress)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgress message handlers

void CProgress::DrawProg() {
	CRect rc = CRect(0, 0, m_rcBar.Width() + (2 * m_nPcWd), m_rcBar.Height());
	m_dcMem->FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT));

	CPen pen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
	CPen *oldPen = m_dcMem->SelectObject(&pen);
	for (int i = rc.left + m_rcBar.Height(); i < rc.right; i += m_nPcWd) {
		m_dcMem->MoveTo(i, rc.top);
		m_dcMem->LineTo(i - m_rcBar.Height(), rc.bottom);
		CDC *pDC = GetDC();
		if (pDC->GetDeviceCaps(LOGPIXELSX) > 130) {
			m_dcMem->MoveTo(i + 1, rc.top);
			m_dcMem->LineTo(i - m_rcBar.Height() + 1, rc.bottom);
			m_dcMem->MoveTo(i + 2, rc.top);
			m_dcMem->LineTo(i - m_rcBar.Height() + 2, rc.bottom);
		}
		ReleaseDC(pDC);
	}
	m_dcMem->SelectObject(oldPen);
}

void CProgress::OnPaint() {
	CPaintDC dc(this); // device context for painting
	
	int saveDC = dc.SaveDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcBar = rcClient;
	rcBar.bottom = rcBar.top + SCALEY(BAR_HT);
	rcBar.right = rcBar.right - SCALEX(1);

	if (m_bRunning) {
		CPen pen(PS_SOLID, SCALEX(1), GetSysColor(COLOR_WINDOWFRAME));
		dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		dc.SelectObject(&brush);
//		HGDIOBJ hbrush = GetStockObject(HOLLOW_BRUSH);
//		SelectObject(dc.GetSafeHdc(), hbrush);

		// hi res drawing correction
		if (dc.GetDeviceCaps(LOGPIXELSX) > 130) {
			rcBar.left++;
			rcBar.right++;
			rcBar.top++;
			rcBar.bottom--;
		}
		dc.Rectangle(rcBar);

		if (dc.GetDeviceCaps(LOGPIXELSX) < 130)
			rcBar.DeflateRect(2, 2);
		else
			rcBar.DeflateRect(3, 3);

		m_rcBar = rcBar;
		m_nPcWd = 5 * m_rcBar.Height() / 2;

		if (m_dcMem == NULL) {
			m_dcMem = new CDC;

			CBitmap bmp;
			bmp.CreateCompatibleBitmap(&dc, m_rcBar.Width() + (2 * m_nPcWd), m_rcBar.Height());

			m_dcMem->CreateCompatibleDC(&dc);
			m_bmp = m_dcMem->SelectObject(&bmp);
			
			DrawProg();
		}
		// copy gradient to screen
		dc.BitBlt(rcBar.left, rcBar.top, m_rcBar.Width(), m_rcBar.Height(), m_dcMem, (2 * m_nPcWd) - SCALEX(m_nOffset), 0, SRCCOPY);
	}
	else {
		dc.FillSolidRect(rcClient, GetSysColor(COLOR_WINDOW));
	}

	//
	CRect rcText = rcClient;
	rcText.top += SCALEX(BAR_HT);
	dc.FillSolidRect(rcText, GetSysColor(COLOR_WINDOW));

	dc.SetBkMode(TRANSPARENT);
	CString strText;
	GetWindowText(strText);
	rcText.DeflateRect(SCALEX(2), SCALEY(0), SCALEX(2), SCALEY(1));

	UINT uFormat = DT_TOP | DT_WORDBREAK;
	DWORD dwStyle = GetStyle();
	if (dwStyle & SS_LEFT) uFormat |= DT_LEFT;
	else if (dwStyle & SS_CENTER) uFormat |= DT_CENTER;
	else if (dwStyle & SS_RIGHT) uFormat |= DT_RIGHT;
	dc.DrawText(strText, rcText, uFormat);

	dc.RestoreDC(saveDC);

	// Do not call CStatic::OnPaint() for painting messages
}

void CProgress::Run() {
	SetTimer(m_nTimer, 100, NULL);
	m_bRunning = TRUE;

	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcBar = rcClient;
	rcBar.bottom = rcBar.top + SCALEY(BAR_HT);
	InvalidateRect(rcBar, TRUE);
}

void CProgress::Stop() {
	KillTimer(m_nTimer);
	m_bRunning = FALSE;

	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcBar = rcClient;
	rcBar.bottom = rcBar.top + SCALEY(BAR_HT);
	InvalidateRect(rcBar, TRUE);
}


void CProgress::OnTimer(UINT nIDEvent) {
	if (nIDEvent == m_nTimer) {
		m_nOffset = (m_nOffset + 1) % m_nPcWd;
		InvalidateRect(m_rcBar, FALSE);
		// copy gradient to screen
		CPaintDC dc(this); // device context for painting
		dc.BitBlt(m_rcBar.left, m_rcBar.top, m_rcBar.Width(), m_rcBar.Height(), m_dcMem, (2 * m_nPcWd) - SCALEX(m_nOffset), 0, SRCCOPY);
		ValidateRect(m_rcBar);
	}

	CStatic::OnTimer(nIDEvent);
}

BOOL CProgress::OnEraseBkgnd(CDC* pDC) {
	CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}
