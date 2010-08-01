/**
 *  InfoBar.cpp
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
#include "../MainFrm.h"

#include "InfoBar.h"
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

//
// CInfoBar
//

CInfoBar::CInfoBar() {
	LOG0(5, "CInfoBar::CInfoBar()");

	HideTimer = 1;
}

CInfoBar::~CInfoBar() {
	LOG0(5, "CInfoBar::~CInfoBar()");
}

#define INFOBAR_CLASSNAME				_T("pRSSreader_InfoBar")

BOOL CInfoBar::Register() {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = INFOBAR_CLASSNAME;
	if (!AfxRegisterClass(&wc))
		return FALSE;

	return TRUE;
}

BOOL CInfoBar::Create(CWnd *pParentWnd) {
	BOOL ret;

	m_dwStyle = CBRS_BOTTOM | CBRS_BORDER_TOP;

	CRect rect;
	ret = CWnd::Create(INFOBAR_CLASSNAME, NULL, WS_CHILD, rect, pParentWnd, AFX_IDW_TOOLBAR + 3);

	m_ctlStopBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(0, 0, 0, 0), this, IDC_CLOSE);

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ctlStopBtn.LoadBitmaps(IDB_CLOSE);
	AfxSetResourceHandle(AfxGetInstanceHandle());
	m_ctlStopBtn.SizeToContent();

	return ret;
}

void CInfoBar::SetText(UINT nID) {
	m_strText.LoadString(nID);
}

BEGIN_MESSAGE_MAP(CInfoBar, CControlBar)
	//{{AFX_MSG_MAP(CInfoBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_TEXT, OnTextClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInfoBar::OnClose() {
	LOG0(1, "CInfoBar::OnClose()");

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->ShowControlBar(this, FALSE, FALSE);
	KillTimer(HideTimer);
}

void CInfoBar::OnTextClicked() {
	LOG0(1, "CInfoBar::OnTextClicked()");

	OnClose();
}

void CInfoBar::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect	rc;
	GetClientRect(&rc);

	// fill
	dc.FillSolidRect(rc, GetSysColor(COLOR_BTNFACE));

	CFont *pFont = dc.SelectObject(&Appearance.BaseFont);
	CRect rcText(SCALEX(5), SCALEY(3), rc.right - SCALEX(5) - SCALEX(17) - 1, SCALEY(17) - 1);
	DrawTextEndEllipsis(dc, m_strText, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);
	dc.SelectObject(pFont);

	m_ctlStopBtn.Invalidate();

	ValidateRect(NULL);
}

void CInfoBar::OnSize(UINT nType, int cx, int cy) {

	if (IsWindow(m_ctlStopBtn.GetSafeHwnd()))
		m_ctlStopBtn.SetWindowPos(NULL, cx - SCALEX(2) - SCALEX(17) + 1, SCALEY(3),
			SCALEX(16) - 1, SCALEY(16) - 1, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CInfoBar::StartTimer() {
	SetTimer(HideTimer, 2000, NULL);
}

void CInfoBar::OnTimer(UINT nIDEvent) {
	if (nIDEvent == HideTimer) {
		OnClose();
	}

	CControlBar::OnTimer(nIDEvent);
}

void CInfoBar::OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler) {
}

CSize CInfoBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz) {
	CDC *pDC = GetDC();
	int cx = pDC->GetDeviceCaps(HORZRES);
	ReleaseDC(pDC);

	return CSize(cx, SCALEY(21) - 1);
}
