/**
 *  TextProgressCtrl.cpp
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
#include "../prssr.h"
#include "../../share/uihelper.h"
#include "TextProgressCtrl.h"

#include "../../share/helpers.h"

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


//
// CTextProgressCtrl
//

CTextProgressCtrl::CTextProgressCtrl() {
	Lo = 0;
	Hi = 100;
	Step = 1;
	Pos = 0;

	RedrawTimer = 1;
//	RedrawPlanned = FALSE;

	ProgressRefreshTimer = 2;
}

#define TEXT_PROGRESS_CLASSNAME			_T("pRSSreader_TextProgress")

BOOL CTextProgressCtrl::Register() {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT_PROGRESS_CLASSNAME;
	if (!AfxRegisterClass(&wc))
		return FALSE;

	return TRUE;
}

BOOL CTextProgressCtrl::Create(DWORD dwStyle, CRect &rc, CWnd *parent, UINT nID) {
	BOOL ret = CWnd::Create(TEXT_PROGRESS_CLASSNAME, _T(""), dwStyle, rc, parent, nID);

	HGDIOBJ hSysFont = ::GetStockObject(SYSTEM_FONT);
	LOGFONT lf = { 0 };
	GetObject(hSysFont, sizeof(LOGFONT), &lf);

	lf.lfWeight = FW_NORMAL;
	lf.lfHeight = SCALEY(13);
	m_fntText.CreateFontIndirect(&lf);

	return ret;
}

void CTextProgressCtrl::OnDestroy() {
	KillTimer(RedrawTimer);
}

void CTextProgressCtrl::SetRange(int lo, int hi) {
	Lo = lo;
	Hi = hi;
}

void CTextProgressCtrl::GetRange(int &lo, int &hi) {
	lo = Lo;
	hi = Hi;
}

void CTextProgressCtrl::SetPos(int pos) {
	Pos = pos;
}

int CTextProgressCtrl::GetPos() {
	return Pos;
}

void CTextProgressCtrl::SetStep(int step) {
	Step = step;
}

void CTextProgressCtrl::StepIt() {
	Pos += Step;
}

//{{AFX_MSG_MAP(CTextProgressCtrl)
BEGIN_MESSAGE_MAP(CTextProgressCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
//}}AFX_MSG_MAP

void CTextProgressCtrl::OnPaint() {
	CPaintDC dc(this); // device context for painting

	int saveDC = dc.SaveDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	CBrush brHollow;
	brHollow.Attach(::GetStockObject(NULL_BRUSH));
	dc.SelectObject(&brHollow);

	// frame
	CPen pen(PS_SOLID, SCALEX(1), ::GetSysColor(COLOR_WINDOWFRAME));
	dc.SelectObject(&pen);
	HIDPI_BorderRectangle(dc.GetSafeHdc(), rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

	// inside
	rcClient.DeflateRect(SCALEX(1), SCALEY(1));

	int nWd = (Hi - Lo);
	double pos = (double) Pos / nWd;

	// bar
	int nPhysPos = (int) (rcClient.Width() * pos);
	COLORREF clrBar = ::GetSysColor(COLOR_HIGHLIGHT);
	CRect rcBar(rcClient.left, rcClient.top, rcClient.left + nPhysPos, rcClient.bottom);
	dc.FillSolidRect(&rcBar, clrBar);

	COLORREF clrBkgnd = ::GetSysColor(COLOR_3DSHADOW);
	CRect rcBack(rcClient.left + nPhysPos, rcClient.top, rcClient.right, rcClient.bottom);
	dc.FillSolidRect(&rcBack, clrBkgnd);

	// draw text over the progress bar
	rcClient.DeflateRect(SCALEX(4), -1, SCALEX(4), 1);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	dc.SelectObject(&m_fntText);

	CRect rcSize(0, 0, 0, 0);
	if (!m_strSize.IsEmpty()) {
		rcSize = rcClient;
		dc.DrawText(m_strSize, rcSize, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);
		dc.DrawText(m_strSize, rcClient, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	}

	CRect rcProgress = rcClient;
	rcProgress.right -= rcSize.Width() - SCALEX(6);
	DrawTextEndEllipsis(dc, m_strText, rcProgress, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	// validate
	GetClientRect(&rcClient);
	ValidateRect(rcClient);

	dc.RestoreDC(saveDC);
}

BOOL CTextProgressCtrl::OnEraseBkgnd(CDC *pDC) {
	return TRUE;
}

void CTextProgressCtrl::Reset() {
	m_strSize = _T("");
}

void CTextProgressCtrl::OnTimer(UINT nIDEvent) {
	if (nIDEvent == RedrawTimer) {
		FinishRedraw();
	}
	else if (nIDEvent == ProgressRefreshTimer) {
		if (Downloader != NULL) {
			if (Downloader->GetTotalFileSize() > 0) {
				SetRange(0, Downloader->GetTotalFileSize());
				SetPos(Downloader->GetDownloadedFileSize());

				CString sText;
				sText.Format(_T("%s / %s"), 
					FormatFileSize(Downloader->GetDownloadedFileSize()),
					FormatFileSize(Downloader->GetTotalFileSize()));
				m_strSize = sText;
			}
			else {
				int newPos = (int) Downloader->GetDownloadedFileSize();
				if (newPos > Hi) {
					Hi *= 10;
					SetRange(0, Hi);
				}
				SetPos(newPos);
			}
			Redraw(FALSE);
		}
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CTextProgressCtrl::FinishRedraw() {
	KillTimer(RedrawTimer);
	Invalidate();
	UpdateWindow();
}

void CTextProgressCtrl::Redraw(BOOL delayedPaint/* = FALSE*/) {
	if (delayedPaint) {
		KillTimer(RedrawTimer);
		SetTimer(RedrawTimer, 333, NULL);
	}
	else {
		FinishRedraw();
	}
}

void CTextProgressCtrl::SetText(const CString &text) {
	m_strText = text;
}

void CTextProgressCtrl::SetDownloader(CDownloader *downloader) {
	Downloader = downloader;
	if (downloader == NULL) {
		KillTimer(ProgressRefreshTimer);
		m_strSize = _T("");
	}
	else {
		SetTimer(ProgressRefreshTimer, 750, NULL);
		SetRange(0, 0xFFFFFF);
	}
}
