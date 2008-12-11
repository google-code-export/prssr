/**
 *  FeedView.cpp
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

#include "StdAfx.h"
#include "prssr.h"
#include "FeedView.h"

#include "Appearance.h"
#include "Feed.h"
#include "Site.h"
#include "Config.h"
#include "misc.h"
#include "MainFrm.h"
#include "../share/date.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CLASS_NAME						_T("FeedViewClass")

#define SCROLL_SLEEP					10
#define SCROLL_SPEED					1
#define SCROLL_MAXSPEED					60

const int CFeedView::LEFT_SKIP = 20;
const int CFeedView::PADDING_TOP = 2;
const int CFeedView::PADDING_RIGHT = 4;
const int CFeedView::PADDING_BOTTOM = 1;
const int CFeedView::ITEM_MARGIN = 22;
const int CFeedView::DATE_HEIGHT = 18;

BOOL CFeedView::Register() {
	// Register your unique class name that you wish to use
	WNDCLASS wndcls;

	memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL
											// defaults

	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;

	//you can specify your own window procedure
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;

	// Specify your own class name for using FindWindow later
	wndcls.lpszClassName = CLASS_NAME;

	// Register the new class and exit if it fails
	if (!AfxRegisterClass(&wndcls)) {
		TRACE(_T("Class Registration Failed\n"));
		return FALSE;
	}

	return TRUE;
}

void CFeedView::Unregister() {
}

/////////////////////////////////////////////////////////////////////////////
// CFeedView

CFeedView::CFeedView() {
	m_nTotalHeight = 0;
	m_nViewTop = 0;
	m_nClientHeight = 0;
	m_nClientWidth = 0;

	m_nSelectStart = m_nSelectEnd = -1;		// none
	m_nSelectFirst = -1;
	m_bSelecting = FALSE;

	m_bScrolling = FALSE;

	CtxMenuTimer = 1;
	m_bOpenCtxMenu = FALSE;

	SiteItem = NULL;
}

CFeedView::~CFeedView() {
}


BEGIN_MESSAGE_MAP(CFeedView, CWnd)
	//{{AFX_MSG_MAP(CFeedView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_ITEM_OPEN, OnItemOpen)
	ON_COMMAND(ID_ITEM_MARKASREAD, OnItemMarkRead)
	ON_COMMAND(ID_ITEM_MARKASUNREAD, OnItemMarkUnread)
	ON_COMMAND(ID_ITEM_MARKASNEW, OnItemMarkNew)
	ON_COMMAND(ID_ITEM_DELETE, OnItemDelete)

	ON_UPDATE_COMMAND_UI(ID_ITEM_DELETE, OnUpdateItemSelected)
	ON_UPDATE_COMMAND_UI(ID_ITEM_MARKASREAD, OnUpdateItemSelected)
	ON_UPDATE_COMMAND_UI(ID_ITEM_MARKASUNREAD, OnUpdateItemSelected)
	ON_UPDATE_COMMAND_UI(ID_ITEM_MARKASNEW, OnUpdateItemSelected)

	ON_COMMAND(ID_SELECT_ALL, OnSelectAll)

	ON_COMMAND(ID_ITEM_FLAG, OnItemFlag)
	ON_UPDATE_COMMAND_UI(ID_ITEM_FLAG, OnUpdateItemFlag)

	ON_COMMAND(ID_COPY_URL, OnCopyUrl)
	ON_COMMAND(ID_SEND_BY_EMAIL, OnSendByEmail)

	ON_COMMAND(ID_VIEW_HIDEREADITEMS, OnViewHideReadItems)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDEREADITEMS, OnUpdateViewHideReadItems)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFeedView message handlers

void CFeedView::CreateFonts() {
	m_fntSmall.DeleteObject();
	m_fntBase.DeleteObject();
	m_fntBold.DeleteObject();

	HGDIOBJ hSysFont = ::GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	::GetObject(hSysFont, sizeof(LOGFONT), (LPVOID) &lf);
	lf.lfHeight = SCALEY(11) + 1;
	m_fntSmall.CreateFontIndirect(&lf);

	wcscpy(lf.lfFaceName, Appearance.FeedViewFontCfg.FontFace);
	CDC *pDC = GetDC();
	lf.lfHeight = ((Appearance.FeedViewFontCfg.Size * pDC->GetDeviceCaps(LOGPIXELSY)) / 72) + 1;
	m_fntBase.CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lf);

	ReleaseDC(pDC);
}

int CFeedView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	LOG1(3, "CFeedView::OnCreate(%p)", lpCreateStruct);

	BOOL ret = CWnd::OnCreate(lpCreateStruct);
	if (ret == -1)
		return -1;

	CreateFonts();

	// icons
	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_oIcons.Create(IDB_NEWS_ITEMS, SCALEX(16), 0, RGB(255, 0, 255));
	AfxSetResourceHandle(AfxGetInstanceHandle());

	// scroll bar
	CRect rcVert;
	m_oVScrollBar.Create(SBS_VERT | WS_CHILD, rcVert, this, 1);

	UpdateScrollBars();

	return ret;
}

void CFeedView::SelectAll() {
	LOG0(3, "CFeedView::SelectAll()");

	m_nSelectFirst = m_nSelectStart = 0;
	m_nSelectEnd = m_oItems.GetSize();
	Invalidate(FALSE);
}

void CFeedView::SelectItem(int item) {
	if (item >= 0 && item < m_oItems.GetSize()) {
		m_nSelectFirst = m_nSelectStart = m_nSelectEnd = item;
		Invalidate(FALSE);
	}
}

void CFeedView::DeleteItem(int idx) {
	LOG1(1, "CFeedView::DeleteItem(%d)", idx);

	if (idx >= 0  && idx < m_oItems.GetSize()) {
		m_oItems.RemoveAt(idx);

		if (idx <= m_nSelectStart) m_nSelectStart--;
		if (idx <= m_nSelectEnd) m_nSelectEnd--;
		if (idx <= m_nSelectFirst) m_nSelectFirst--;

		AdjustViewTop();
		UpdateItemHeights();
		UpdateScrollBars();
		Invalidate();
	}
}

void CFeedView::DeleteAllItems() {
	LOG0(1, "CFeedView::DeleteAllItems()");

	m_oItems.RemoveAll();
	m_oItemHeight.RemoveAll();
	Invalidate();

	m_nTotalHeight = 0;
	m_nViewTop = 0;
	m_nSelectStart = m_nSelectEnd = m_nSelectFirst = -1;
}

BOOL CFeedView::InsertItem(int i, CFeedItem *item) {
	LOG2(1, "CFeedView::InsertItem(%d, %p)", i, item);

	if (item->IsDeleted())
		return FALSE;
	else {
		m_oItems.SetAtGrow(i, item);
		int ht = CalcItemHeight(i);
		m_nTotalHeight += ht;
		m_oItemHeight.SetAtGrow(i, m_nTotalHeight);
		return TRUE;
	}
}

void CFeedView::EnsureVisible(int item) {
	LOG1(1, "CFeedView::EnsureVisible(%d)", item);

	int y;
	if (item > 0) y = m_oItemHeight[item - 1];
	else y = 0;

	if (m_nViewTop <= y && m_oItemHeight[item] <= m_nViewTop + m_nClientHeight) {
	}
	else {
		m_nViewTop = y;
		AdjustViewTop();
		UpdateScrollBars();
	}
}

void CFeedView::DeselectAllItems() {
	LOG0(1, "CFeedView::DeselectAllItems()");

	m_nSelectStart = m_nSelectEnd = m_nSelectFirst = -1;
	Invalidate();
}

void CFeedView::MarkItem(int item, DWORD mask) {
	CFeedItem *fi = m_oItems.GetAt(item);
	fi->SetFlags(mask, MESSAGE_READ_STATE);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->SyncItem(fi);
}

void CFeedView::MarkAllRead() {
	LOG0(1, "CFeedView::MarkAllRead()");

	for (int i = 0; i < m_oItems.GetSize(); i++)
		MarkItem(i, MESSAGE_READ);

	if (SiteItem != NULL) {
		if (SiteItem->Type == CSiteItem::VFolder && SiteItem->FlagMask == MESSAGE_READ_STATE)
			DeleteAllItems();
		else if (SiteItem->Type != CSiteItem::VFolder && Config.HideReadItems)
			DeleteAllItems();
	}

	AdjustViewTop();

	DeselectAllItems();
	SortItems();
	UpdateScrollBars();
	Invalidate(TRUE);
}

void CFeedView::MarkAllUnread() {
	LOG0(1, "CFeedView::MarkAllUnread()");

	for (int i = 0; i < m_oItems.GetSize(); i++)
		MarkItem(i, MESSAGE_UNREAD);

	AdjustViewTop();

	DeselectAllItems();
	SortItems();
	UpdateScrollBars();
	Invalidate(TRUE);
}

//
//
//

void CFeedView::AdjustViewTop() {
	if (m_nViewTop < 0 || m_nTotalHeight < m_nClientHeight)
		m_nViewTop = 0;
	else if (m_nTotalHeight > m_nClientHeight) {
		int max = m_nTotalHeight - m_nClientHeight;
		if (m_nViewTop > max)
			m_nViewTop = max;
	}
}

void CFeedView::DrawIcon(CDC &dc, int icon, BOOL selected, int xofs/* = 0*/, int yofs/* = 0*/) {
	IMAGELISTDRAWPARAMS drawing;
	drawing.cbSize = sizeof(IMAGELISTDRAWPARAMS);
	drawing.himl = m_oIcons.GetSafeHandle();
	drawing.i = icon;
	drawing.hdcDst = dc.GetSafeHdc();
	drawing.x = SCALEX(1) + SCALEX(xofs);
	drawing.y = SCALEY(2) + SCALEX(yofs);
	drawing.cx = SCALEX(16);
	drawing.cy = SCALEY(16);
	drawing.xBitmap = 0;
	drawing.yBitmap = 0;
	drawing.rgbBk = CLR_NONE;
	drawing.rgbFg = CLR_DEFAULT;
	if (selected)
		drawing.fStyle = ILD_BLEND50;
	else
		drawing.fStyle = ILD_NORMAL;
	drawing.dwRop = SRCCOPY;
	ImageList_DrawIndirect(&drawing);
}

void CFeedView::DrawItem(CDC &dc, CRect &rc, int idx) {
	CFeedItem *item = m_oItems[idx];

	BOOL selected = (m_nSelectStart <= idx) && (idx <= m_nSelectEnd);

	COLORREF clrBg, clrFg, clrOld;
	if (selected) {
		clrBg = ::GetSysColor(COLOR_HIGHLIGHT);
		clrFg = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else {
		clrBg = Appearance.ClrFeedViewBg;

		if (item->IsNew() || item->IsUnread())
			clrFg = Appearance.ClrFeedViewFg;
		else
			clrFg = ::GetSysColor(COLOR_3DSHADOW);
	}

	// background
	dc.FillSolidRect(rc, clrBg);

	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	// icon
	if (item->IsNew())
		DrawIcon(dc, NEW_FEED_ICON, selected);
	else if (item->IsUnread())
		DrawIcon(dc, UNREAD_FEED_ICON, selected);
	else
		DrawIcon(dc, READ_FEED_ICON, selected);

	// has html cached
	if (IsHTMLCached(item->Link, TRUE)) {
		if (item->IsNew() || item->IsUnread())
			DrawIcon(dc, CACHED_ITEM_ICON, selected);
		else
			DrawIcon(dc, NOT_CACHED_ITEM_ICON, selected);
	}

	if (item->HasEnclosure()) {
		CEnclosureItem *ei = item->Enclosures.GetHead();
		if (IsEnclosureCached(ei->URL))
			DrawIcon(dc, CACHED_ITEM_ICON, selected);
		else
			DrawIcon(dc, NOT_CACHED_ITEM_ICON, selected);
	}

	// keyword
	if (item->HasKeywordMatch())
		DrawIcon(dc, KEYWORD_ICON, selected);

	// flagged?
	if (item->IsFlagged())
		DrawIcon(dc, FLAG_ICON, selected);

	// title
	CFont *pOldFont;
	pOldFont = dc.SelectObject(&m_fntBold);
	clrOld = dc.SetTextColor(clrFg);
	CRect rcTitle = rc;
	rcTitle.DeflateRect(SCALEX(LEFT_SKIP), SCALEY(PADDING_TOP), SCALEX(PADDING_RIGHT), SCALEY(PADDING_BOTTOM));

	CString strTitle = item->Title;
	ReplaceHTMLEntities(strTitle);
	if (m_bWrapTitles) {
		dc.DrawText(strTitle, &rcTitle, DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);

	}
	else
		DrawTextEndEllipsis(dc, strTitle, rcTitle, DT_LEFT | DT_TOP | DT_NOPREFIX);
	dc.SetTextColor(clrOld);
	dc.SelectObject(pOldFont);

	// date
	if (item->PubDate.wYear != 0) {
		pOldFont = dc.SelectObject(&m_fntSmall);

		CString sTime;
		SYSTEMTIME st = TimeToTimeZone(&item->PubDate);		// convert to local time zone
		FormatDateTime(sTime, st, Config.ShowRelativeDates);

		CRect rcTime = rc;
		rcTime.left = rcTitle.left;
		rcTime.bottom -= SCALEY(3);
		rcTime.right -= SCALEX(3);
		if (selected)
			clrOld = dc.SetTextColor(clrFg);
		else
			clrOld = dc.SetTextColor(Appearance.ClrDate);
		dc.DrawText(sTime, &rcTime, DT_LEFT | DT_BOTTOM | DT_NOPREFIX);
		dc.SetTextColor(clrOld);

		dc.SelectObject(pOldFont);
	}

	// draw separator
	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
	CPen *oldPen = dc.SelectObject(&pen);
	dc.MoveTo(rc.left, rc.bottom - 1);
	dc.LineTo(rc.right, rc.bottom - 1);
	dc.SelectObject(oldPen);

	dc.SetBkMode(oldBkMode);
}

void CFeedView::InvalidateItem(int idx, BOOL erase/* = TRUE*/) {
	if (idx >= 0 && idx < m_oItems.GetSize()) {
		CRect rcClient;
		GetClientRect(rcClient);

		int top;
		if (idx > 0) top = m_oItemHeight[idx - 1];
		else top = 0;
		CRect rc(rcClient.left, top - m_nViewTop, rcClient.right, m_oItemHeight[idx] - m_nViewTop);
		InvalidateRect(rc, erase);
	}
}

void CFeedView::OnDraw(CDC *pDC) {
}

void CFeedView::OnPaint() {
	LOG0(5, "CFeedView::OnPaint()");

	CPaintDC dc(this); // device context for painting

	int nSavedDC = dc.SaveDC();

	CRect rc;
	GetClientRect(rc);

	if (m_oItems.GetSize() > 0) {
		CRect rcClip;
		dc.GetClipBox(rcClip);

		if (m_oVScrollBar.IsWindowVisible()) {
			CRect rcVScroll;
			m_oVScrollBar.GetWindowRect(rcVScroll);
			rc.right -= rcVScroll.Width() - SCALEX(1);
		}

		int y = rc.top;

		int first = 0;
		for (; first < m_oItems.GetSize(); first++)
			if (m_oItemHeight[first] > m_nViewTop)
				break;
		int ofsY;
		if (first > 0) ofsY = m_nViewTop - m_oItemHeight[first - 1];
		else ofsY = m_nViewTop;

		for (int idx = first; idx < m_oItems.GetSize() && y < rc.bottom; idx++) {
			int wd = rc.Width();
			int ht;
			if (idx > 0) ht = m_oItemHeight[idx] - m_oItemHeight[idx - 1];
			else ht = m_oItemHeight[idx];

			CBitmap bmp;
			bmp.CreateCompatibleBitmap(&dc, wd, ht);

			CDC memDC;
			memDC.CreateCompatibleDC(&dc);
			CBitmap *saveBmp = memDC.SelectObject(&bmp);

			CRect rcPhys(rc.left, y, rc.right, y + ht);
			CRect rcDraw;
			if (rcDraw.IntersectRect(rcClip, rcPhys)) {
				CRect rcMsg(rc.left, 0, rc.right, ht);
				DrawItem(memDC, rcMsg, idx);

				// copy to screen
				dc.BitBlt(0, y, wd, ht - ofsY, &memDC, 0, ofsY, SRCCOPY);
			}

			y += ht - ofsY;
			ofsY = 0;				// for the rest of the items, the ofsY is zero

			//
			memDC.SelectObject(saveBmp);
		}

		// delete the rest of the window
		CRect rcRest(rc.left, y, rc.right, rc.bottom);
		dc.FillSolidRect(rcRest, Appearance.ClrFeedViewBg);
	}
	else {
		CString sText;
		sText.LoadString(IDS_NO_ITEMS_TO_DISPLAY);

		dc.FillSolidRect(rc, Appearance.ClrFeedViewBg);

		rc.DeflateRect(SCALEX(8), SCALEY(8));

		CFont *pFont = dc.SelectObject(&Appearance.BaseFont);
		dc.SetTextColor(Appearance.ClrFeedViewFg);
		dc.DrawText(sText, &rc, DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);
		dc.SelectObject(pFont);
	}

	dc.RestoreDC(nSavedDC);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CFeedView::OnEraseBkgnd(CDC *pDC) {
	return TRUE;
}

static DWORD WINAPI ScrollThread(LPVOID lpParam) {
	CFeedView *view = (CFeedView *) lpParam;

    int top = view->m_nViewTop;
	while (view->m_bScrolling) {
		int oldtop = view->m_nViewTop;
		top += view->m_nScrollDelta;
		view->m_nViewTop = top;
		view->AdjustViewTop();
		if (oldtop != view->m_nViewTop) {
			view->ScrollWindowEx(0, oldtop - view->m_nViewTop, &view->m_rcScroll, &view->m_rcScroll,
				NULL, NULL, SW_INVALIDATE);
			view->m_oVScrollBar.SetScrollPos(view->m_nViewTop, TRUE);
			view->UpdateWindow();
			view->OnMouseMove(view->m_nOldKeys, view->m_ptOldCursorPos);
		}

		Sleep(SCROLL_SLEEP);
	}

	return 0;
}

void CFeedView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
	int i = m_nViewTop;
	switch (nSBCode) {
		// FIXME
		case SB_LINEDOWN:   m_nViewTop += SCALEY(20); break;
		case SB_LINEUP:     m_nViewTop -= SCALEY(20); break;
		case SB_PAGEDOWN:   m_nViewTop += m_nClientHeight; break;
		case SB_PAGEUP:     m_nViewTop -= m_nClientHeight; break;
		case SB_THUMBTRACK: {
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_TRACKPOS;
			m_oVScrollBar.GetScrollInfo(&si);
			m_nViewTop = si.nTrackPos;
		}
	}

	AdjustViewTop();

	ScrollWindowEx(0, i - m_nViewTop, &m_rcScroll, &m_rcScroll, NULL, NULL, SW_INVALIDATE);
	m_oVScrollBar.SetScrollPos(m_nViewTop, TRUE);
	UpdateWindow();
}

void CFeedView::UpdateScrollBars() {
	LOG0(3, "CFeedView::UpdateScrollBars()");

	BOOL visible = m_oVScrollBar.IsWindowVisible();

	if (m_nClientHeight >= m_nTotalHeight) {
		m_oVScrollBar.ShowScrollBar(FALSE);
	}
	else {
		m_oVScrollBar.ShowScrollBar(TRUE);

		SCROLLINFO sbi = { 0 };
		sbi.cbSize = sizeof(sbi);
		sbi.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
		sbi.nMin = 0;
		sbi.nMax = m_nTotalHeight - 1;
		sbi.nPage = m_nClientHeight;
		sbi.nPos = m_nViewTop;
		m_oVScrollBar.SetScrollInfo(&sbi);
	}
}

void CFeedView::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);

	m_nClientHeight = cy;
	m_nClientWidth = cx;

	CRect rcClient;
	GetClientRect(rcClient);

	m_rcScroll = rcClient;

	// place the scroll bar
	CRect rcVert;
	rcVert.top    = -SCALEY(1);
	rcVert.bottom = rcClient.bottom + SCALEY(1);
	rcVert.right  = rcClient.right + SCALEX(1);
	rcVert.left   = rcVert.right - GetSystemMetrics(SM_CXVSCROLL);

	m_oVScrollBar.SetWindowPos(NULL, rcVert.left, rcVert.top, rcVert.Width(), rcVert.Height(), SWP_NOZORDER);

	AdjustViewTop();
	UpdateScrollBars();

	Invalidate();
}

int CFeedView::ItemFromPoint(CPoint pt) {
	if (m_oItemHeight.GetSize() > 0) {
		int y = m_nViewTop + pt.y;
		for (int i = 0; i < m_oItemHeight.GetSize(); i++)
			if (m_oItemHeight[i] >= y)
				return i;
		return m_oItemHeight.GetSize();
	}
	else
		return -1;
}

// Normal

void CFeedView::OnLButtonDownNormal(UINT nFlags, CPoint point) {
	SetFocus();

	int item = ItemFromPoint(point);
	if (item >= 0 && item < m_oItems.GetSize()) {
		CRect rcClient;
		GetClientRect(rcClient);

		if (point.x >= 0 && point.x <= SCALEY(20)) {
			m_nFlagItem = item;
		}

		if (m_nSelectStart <= item && item <= m_nSelectEnd) {
			m_nSelectFirst = item;
		}
		else {
			// deselect
			m_nSelectFirst = m_nSelectStart = m_nSelectEnd = item;
			Invalidate(FALSE);
		}

		m_bSelecting = TRUE;
		m_bClick = TRUE;

		LastCursorPos = point;

		// tap and hold
		SHRGINFO  shrg;
		shrg.cbSize = sizeof(shrg);
		shrg.hwndClient = GetSafeHwnd();
		shrg.ptDown.x = LastCursorPos.x;
		shrg.ptDown.y = LastCursorPos.y;
		shrg.dwFlags = SHRG_NOTIFYPARENT | SHRG_RETURNCMD;

		if (::SHRecognizeGesture(&shrg) == GN_CONTEXTMENU) {
			Invalidate(FALSE);

			CPoint pt = point;
			ClientToScreen(&pt);
			ContextMenu(&pt);

			m_nFlagItem = -1;
			m_bSelecting = FALSE;
		}
		else {
			SetCapture();
			Default();
		}
	}
	else {
		// tap out of items (deselect all)
		m_nSelectStart = m_nSelectEnd = m_nSelectFirst = -1;
		m_bSelecting = FALSE;
		Invalidate(FALSE);
	}
}

void CFeedView::OnMouseMoveNormal(UINT nFlags, CPoint pt) {
	// Scrolling
	if (m_nTotalHeight > m_nClientHeight && GetCapture() == this) {
		int delta = 0;
		if (pt.y < 0) {
			delta = ((pt.y / SCALEY(SCROLL_SPEED)) - SCALEY(1));
			pt.y = 0;
		}
		else if (pt.y >= m_nClientHeight) {
			delta = (((pt.y - m_nClientHeight) / SCALEY(SCROLL_SPEED)) + SCALEY(1));
			pt.y = m_nClientHeight - SCALEY(1);
		}

		if (delta) {
			if (delta > SCALEY(SCROLL_MAXSPEED))
				delta = SCALEY(SCROLL_MAXSPEED);
			else if (delta < -SCALEY(SCROLL_MAXSPEED))
				delta = -SCALEY(SCROLL_MAXSPEED);

			m_nScrollDelta = delta;
			if (!m_bScrolling) {
				m_bScrolling = TRUE;
				CloseHandle(CreateThread(NULL, 0, ScrollThread, (LPVOID) this, 0, NULL));
			}
		}
		else
			m_bScrolling = FALSE;
	}

	if (m_bSelecting) {
		int item = ItemFromPoint(pt);
		if (item < 0) item = 0;
		if (item >= m_oItems.GetSize()) item = m_oItems.GetSize() - 1;

		if (item <= m_nSelectFirst) {
			m_nSelectStart = item;
			m_nSelectEnd = m_nSelectFirst;
		}
		if (item >= m_nSelectFirst) {
			m_nSelectStart = m_nSelectFirst;
			m_nSelectEnd = item;
		}

		Invalidate(FALSE);

		if (abs(pt.x - LastCursorPos.x) > SCALEX(3) || abs(pt.y - LastCursorPos.y) > SCALEX(3))
			m_bClick = FALSE;
	}

	m_nOldKeys = nFlags;
	m_ptOldCursorPos = pt;
}

void CFeedView::OnLButtonUpNormal(UINT nFlags, CPoint point) {
	ReleaseCapture();

	int item = ItemFromPoint(point);
	if (item >= 0 && item < m_oItems.GetSize()) {
		if (item == m_nFlagItem && point.x >= 0 && point.x <= SCALEY(20)) {
			if (GetItem(item)->IsFlagged())
				UnflagItem(item);
			else
				FlagItem(item);

			m_nSelectFirst = item;
			m_nSelectStart = m_nSelectEnd = item;

			Invalidate(FALSE);
		}
		else if (m_bSelecting) {
			if (item == m_nSelectFirst && m_bClick) {
				OpenItem(item);
				Invalidate(FALSE);
			}
		}
	}

	m_bScrolling = FALSE;
	m_bSelecting = FALSE;
}

// TOUCH

void CFeedView::OnLButtonDownTouch(UINT nFlags, CPoint point) {
	int item = ItemFromPoint(point);
	if (item >= 0 && item < m_oItems.GetSize()) {
		CRect rcClient;
		GetClientRect(rcClient);

		if (point.x >= 0 && point.x <= SCALEY(20)) {
			m_nFlagItem = item;
		}

		m_bClick = TRUE;

		LastCursorPos = point;

		// tap and hold
		SHRGINFO  shrg;
		shrg.cbSize = sizeof(shrg);
		shrg.hwndClient = GetSafeHwnd();
		shrg.ptDown.x = LastCursorPos.x;
		shrg.ptDown.y = LastCursorPos.y;
		shrg.dwFlags = SHRG_NOTIFYPARENT | SHRG_RETURNCMD;

		if (::SHRecognizeGesture(&shrg) == GN_CONTEXTMENU) {
			InvalidateItem(m_nSelectFirst, FALSE);
			m_nSelectStart = m_nSelectEnd = m_nSelectFirst = item;
			InvalidateItem(item, FALSE);

			CPoint pt = point;
			ClientToScreen(&pt);
			ContextMenu(&pt);

			m_nFlagItem = -1;
			m_bSelecting = FALSE;
		}
		else {
			SetCapture();
			Default();
		}
	}
	else {
		// tap out of items (deselect all)
		m_nSelectStart = m_nSelectEnd = m_nSelectFirst = -1;
		m_bSelecting = FALSE;
		Invalidate(FALSE);
	}
}

void CFeedView::OnMouseMoveTouch(UINT nFlags, CPoint pt) {
	if (abs(pt.x - LastCursorPos.x) > SCALEX(3) || abs(pt.y - LastCursorPos.y) > SCALEX(3))
		m_bClick = FALSE;

	// Scrolling
	if (m_nTotalHeight > m_nClientHeight && GetCapture() == this) {
		m_bScrolling = TRUE;

		int delta = LastCursorPos.y - pt.y;

		int top = m_nViewTop;
		m_nViewTop += delta;

		AdjustViewTop();

		ScrollWindowEx(0, top - m_nViewTop, &m_rcScroll, &m_rcScroll, NULL, NULL, SW_INVALIDATE);
		m_oVScrollBar.SetScrollPos(m_nViewTop, TRUE);
		UpdateWindow();

		LastCursorPos = pt;
	}
}

void CFeedView::OnLButtonUpTouch(UINT nFlags, CPoint point) {
	ReleaseCapture();

	int item = ItemFromPoint(point);
	if (item >= 0 && item < m_oItems.GetSize()) {
		if (m_bClick) {
			if (m_nFlagItem == item && point.x >= 0 && point.x <= SCALEY(20)) {
				if (GetItem(item)->IsFlagged())
					UnflagItem(item);
				else
					FlagItem(item);

				InvalidateItem(m_nSelectFirst, FALSE);
				m_nSelectFirst = item;
				m_nSelectStart = m_nSelectEnd = item;
				InvalidateItem(item, FALSE);
			}
			else if (m_bClick) {
				InvalidateItem(m_nSelectFirst, FALSE);
				m_nSelectFirst = item;
				m_nSelectStart = m_nSelectEnd = item;
				InvalidateItem(item, FALSE);

				OpenItem(item);
			}
		}
		else {

		}
	}

	m_bClick = FALSE;
	m_bScrolling = FALSE;
	m_bSelecting = FALSE;
}


void CFeedView::OnLButtonDown(UINT nFlags, CPoint point) {
	switch (Config.NavigationType) {
		case NAVIGATION_NORMAL: OnLButtonDownNormal(nFlags, point); break;
		default:
		case NAVIGATION_TOUCH: OnLButtonDownTouch(nFlags, point); break;
	}
}

void CFeedView::OnMouseMove(UINT nFlags, CPoint pt) {
	switch (Config.NavigationType) {
		case NAVIGATION_NORMAL: OnMouseMoveNormal(nFlags, pt); break;
		default:
		case NAVIGATION_TOUCH: OnMouseMoveTouch(nFlags, pt); break;
	}
}

void CFeedView::OnLButtonUp(UINT nFlags, CPoint point) {
	switch (Config.NavigationType) {
		case NAVIGATION_NORMAL: OnLButtonUpNormal(nFlags, point); break;
		default:
		case NAVIGATION_TOUCH: OnLButtonUpTouch(nFlags, point); break;
	}
}


// keys

void CFeedView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	LOG3(5, "CFeedView::OnKeyDown(%d, %d, %d)", nChar, nRepCnt, nFlags);

	switch (nChar) {
		case VK_UP:
			MoveToPrevItem();
			break;

		case VK_DOWN:
			MoveToNextItem();
			break;

		case VK_LEFT:
			if (SiteItem->Type != CSiteItem::VFolder) {
				int oldSiteIdx = Config.ActSiteIdx;
				MoveToPrevChannel();

				CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
				frame->AddSiteToSave(oldSiteIdx);
				frame->PreloadSite(Config.ActSiteIdx);
			}
			break;

		case VK_RIGHT:
			if (SiteItem->Type != CSiteItem::VFolder) {
				int oldSiteIdx = Config.ActSiteIdx;
				MoveToNextChannel();

				CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
				frame->AddSiteToSave(oldSiteIdx);
				frame->PreloadSite(Config.ActSiteIdx);
			}
			break;

		case VK_RETURN:
			if (!(nFlags & 0x4000)) {
				m_bOpenCtxMenu = FALSE;
				SetTimer(CtxMenuTimer, TIMER_KEY_CTX_MENU, NULL);
			}
			break;

		default:
			break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFeedView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	LOG3(5, "CFeedView::OnKeyUp(%d, %d, %d)", nChar, nRepCnt, nFlags);

	switch (nChar) {
		case VK_RETURN:
			KillTimer(CtxMenuTimer);
			if (m_bOpenCtxMenu) {
				m_bOpenCtxMenu = FALSE;
				if (m_oItems.GetSize() > 0 && m_nSelectFirst >= 0) {
					int y;
					if (m_nSelectFirst > 0) y = (m_oItemHeight[m_nSelectFirst] + m_oItemHeight[m_nSelectFirst - 1]) / 2;
					else y = m_oItemHeight[m_nSelectFirst] / 2;
					y -= m_nViewTop;

					CPoint pt(m_nClientWidth / 2, y);
					ClientToScreen(&pt);
					ContextMenu(&pt);
				}
				else
					CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
			}
			else {
				if (m_oItems.GetSize() > 0 && m_nSelectFirst >= 0)
					OpenItem(m_nSelectFirst);
				CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
			}
			break;

		default:
			CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
			break;
	}
}

void CFeedView::ContextMenu(CPoint *pt) {
	LOG0(5, "CFeedView::ContextMenu()");

	if (m_nSelectFirst != -1) {
		CMenu popup;
		popup.CreatePopupMenu();

		AppendMenuFromResource(&popup, IDR_OPEN);
		popup.AppendMenu(MF_SEPARATOR);
		AppendMenuFromResource(&popup, IDR_FEEDITEM);
		AppendMenuFromResource(&popup, IDR_CACHE);
		AppendMenuFromResource(&popup, IDR_ENCLOSURES);
		AppendMenuFromResource(&popup, IDR_COPY_URL);
		AppendMenuFromResource(&popup, IDR_SEND_BY_EMAIL);
		popup.AppendMenu(MF_SEPARATOR);
		AppendMenuFromResource(&popup, IDR_ITEM_OPER);

		// track popup menu
		popup.TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN, pt->x, pt->y, AfxGetMainWnd());
	}
}

void CFeedView::OnTimer(UINT nIDEvent) {
	LOG0(5, "CFeedView::OnTimer()");

	if (nIDEvent == CtxMenuTimer) {
		m_bOpenCtxMenu = TRUE;
		KillTimer(CtxMenuTimer);
	}

	CWnd::OnTimer(nIDEvent);
}

//
// handlers
//

void CFeedView::OnItemOpen() {
//	OpenOnlineMessage(m_oItems[m_nSelectFirst]->Link, m_oItems[m_nSelectFirst]->SiteItem);
}

void CFeedView::OnItemMarkRead() {
	LOG0(1, "CFeedView::OnItemMarkRead()");

	if (m_nSelectFirst != -1) {
		int i;
		for (i = m_nSelectStart; i <= m_nSelectEnd; i++) {
			MarkItem(i, MESSAGE_READ);
			InvalidateItem(i, FALSE);
		}

		if (SiteItem != NULL) {
			if (SiteItem->Type == CSiteItem::VFolder && SiteItem->FlagMask == MESSAGE_READ_STATE) {
				// delete items
				for (i = m_nSelectStart; i <= m_nSelectEnd; i++)
					m_oItems.RemoveAt(m_nSelectStart);
				AdjustViewTop();
				Invalidate(FALSE);
			}
			else if (SiteItem->Type != CSiteItem::VFolder && Config.HideReadItems) {
				// delete items
				for (i = m_nSelectStart; i <= m_nSelectEnd; i++)
					m_oItems.RemoveAt(m_nSelectStart);
				AdjustViewTop();
				Invalidate(FALSE);
			}
		}

		DeselectAllItems();
		SortItems();
		UpdateScrollBars();

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->UpdateTopBar();
	}
}

void CFeedView::OnItemMarkUnread() {
	LOG0(1, "CFeedView::OnItemMarkUnread()");

	if (m_nSelectFirst != -1) {
		for (int i = m_nSelectStart; i <= m_nSelectEnd; i++) {
			MarkItem(i, MESSAGE_UNREAD);
			InvalidateItem(i, FALSE);
		}
		DeselectAllItems();
		SortItems();

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->UpdateTopBar();
	}
}

void CFeedView::OnItemMarkNew() {
	LOG0(1, "CFeedView::OnItemMarkNew()");

	if (m_nSelectFirst != -1) {
		for (int i = m_nSelectStart; i <= m_nSelectEnd; i++) {
			MarkItem(i, MESSAGE_NEW);
			InvalidateItem(i, FALSE);
		}
		DeselectAllItems();
		SortItems();

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->UpdateTopBar();
	}
}

void CFeedView::OnItemFlag() {
	LOG0(1, "CFeedView::OnItemFlag()");

	if (m_nSelectFirst != -1) {
		if (SiteItem != NULL) {
			if (GetItem(m_nSelectFirst)->IsFlagged())
				for (int i = m_nSelectStart; i <= m_nSelectEnd; i++)
					UnflagItem(i);
			else
				for (int i = m_nSelectStart; i <= m_nSelectEnd; i++)
					FlagItem(i);
		}
		DeselectAllItems();

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->UpdateTopBar();
	}
}

void CFeedView::OnUpdateItemFlag(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(GetItem(m_nSelectFirst)->IsFlagged());
}

void CFeedView::OnSelectAll() {
	LOG0(1, "CFeedView::SelectAll()");

	SelectAll();
}

void CFeedView::OnItemDelete() {
	LOG0(1, "CFeedView::OnItemDelete()");

	if (m_nSelectFirst != -1) {
		for (int i = m_nSelectStart; i <= m_nSelectEnd; i++) {
			CFeedItem *fi = GetItem(i);
			fi->SetFlags(MESSAGE_DELETED, MESSAGE_DELETED);
		}

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();

		// delete from view
		int cnt = m_nSelectEnd - m_nSelectStart + 1;
		m_oItems.RemoveAt(m_nSelectStart, cnt);

		DeselectAllItems();
		SortItems();
		UpdateScrollBars();

		frame->UpdateTopBar();
	}
}

void CFeedView::OnUpdateItemSelected(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_nSelectFirst != -1);
}

//
//
//

int CFeedView::MoveToNextItem() {
	LOG0(3, "CFeedView::MoveToNextItem()");

	if (m_nSelectFirst < m_oItems.GetSize() - 1) {
		m_nSelectFirst++;
		m_nSelectEnd = m_nSelectStart = m_nSelectFirst;

		// FIXME
		int y;
		if (m_nSelectFirst > 0) y = m_oItemHeight[m_nSelectFirst - 1];
		else y = 0;

		if (y < m_nViewTop)
			m_nViewTop = y;
		else if (m_oItemHeight[m_nSelectFirst] > m_nViewTop + m_nClientHeight)
			m_nViewTop = m_oItemHeight[m_nSelectFirst] - m_nClientHeight;
		AdjustViewTop();

		UpdateScrollBars();
		Invalidate(FALSE);
	}

	return m_nSelectFirst;
}

int CFeedView::MoveToPrevItem() {
	LOG0(3, "CFeedView::MoveToPrevItem()");

	if (m_nSelectFirst > 0) {
		m_nSelectFirst--;
		m_nSelectEnd = m_nSelectStart = m_nSelectFirst;

		// FIXME
		int y;
		if (m_nSelectFirst > 0) y = m_oItemHeight[m_nSelectFirst - 1];
		else y = 0;

		if (y < m_nViewTop)
			m_nViewTop = y;
		else if (m_oItemHeight[m_nSelectFirst] > m_nViewTop + m_nClientHeight)
			m_nViewTop = m_oItemHeight[m_nSelectFirst] - m_nClientHeight;
		AdjustViewTop();

		UpdateScrollBars();
		Invalidate(FALSE);
	}

	return m_nSelectFirst;
}

int CFeedView::MoveToNextChannel() {
	LOG0(3, "CFeedView::MoveToNextChannel()");

	int oldSiteIdx = Config.ActSiteIdx;
	if (Config.ActSiteIdx < SiteList.GetCount() - 1)
		Config.ActSiteIdx++;
	else if (Config.WrapAround)
		Config.ActSiteIdx = 0;

	if (oldSiteIdx != Config.ActSiteIdx) {
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->SelectSite(Config.ActSiteIdx);
	}

	return Config.ActSiteIdx;
}

int CFeedView::MoveToPrevChannel() {
	LOG0(3, "CFeedView::MoveToPrevChannel()");

	int oldSiteIdx = Config.ActSiteIdx;
	if (Config.ActSiteIdx > 0)
		Config.ActSiteIdx--;
	else  if (Config.WrapAround)
		Config.ActSiteIdx = SiteList.GetCount() - 1;

	if (oldSiteIdx != Config.ActSiteIdx) {
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->SelectSite(Config.ActSiteIdx);
	}

	return Config.ActSiteIdx;
}

void CFeedView::OpenItem(CFeedItem *fi) {
	LOG1(3, "CFeedView::OpenItem(%p)", fi);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();

	CSiteItem *si = fi->SiteItem;

	// enclosure bar
	frame->SwitchView(CMainFrame::ArticleView);

	frame->m_wndArticleView.SetFeedItem(fi);
	frame->m_wndArticleView.CreateMenu(frame->m_hwndCmdBar);
	frame->SetupEnclosureBar(fi);

	// setup controls
	CDC *pDC = GetDC();
	CString strSiteTitle = GetNumberItemText(pDC, si->Name, si->GetUnreadCount(), GetSystemMetrics(SM_CXSCREEN) - SCALEX(22 + 50));
	ReleaseDC(pDC);

	frame->m_wndBanner.SetTitle(strSiteTitle);
	frame->m_wndBanner.SetItems(m_nSelectFirst + 1, m_oItems.GetSize());
	frame->m_wndBanner.SetIcon(si->ImageIdx);

	if (fi != NULL) frame->m_wndBanner.SetFlagged(fi->IsFlagged() ? FLAG_ICON : -1);

	if (::IsWindow(frame->m_wndBanner.GetSafeHwnd()))
		frame->m_wndBanner.Invalidate();

	frame->m_wndArticleView.ShowArticle();
	frame->UpdateTopBar();

	if (si->Feed != NULL) {
		for (int i = 0; i < si->Feed->GetItemCount(); i++) {
			if (si->Feed->GetItem(i) == fi) {
				Config.ActFeedItem = i;
				break;
			}
		}
	}
}

void CFeedView::OpenItem(int item) {
	LOG1(3, "CFeedView::OpenItem(%d)", item);

	if (item < 0 || item >= m_oItems.GetSize())
		return;

	m_nSelectFirst = m_nSelectStart = m_nSelectEnd = item;

	OpenItem(m_oItems.GetAt(item));
	MarkItem(item, MESSAGE_READ);
}

void CFeedView::FlagItem(int item) {
	LOG1(3, "CFeedView::FlagItem(%d)", item);

	CFeedItem *fi = m_oItems.GetAt(item);
	fi->SetFlags(MESSAGE_FLAG, MESSAGE_FLAG);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->SyncItem(fi);
}

void CFeedView::UnflagItem(int item) {
	LOG1(3, "CFeedView::UnflagItem(%d)", item);

	CFeedItem *fi = m_oItems.GetAt(item);
	fi->SetFlags(0, MESSAGE_FLAG);
	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->SyncItem(fi);
}

void CFeedView::OnCopyUrl() {
	LOG0(3, "CFeedView::OnCopyUrl()");

	if (m_nSelectFirst != -1) {
		CString links;
		for (int i = m_nSelectStart; i <= m_nSelectEnd; i++) {
			CFeedItem *fi = m_oItems.GetAt(i);
			if (fi != NULL) {
				if (i > m_nSelectStart)
					links += _T("\n");
				links += fi->Link;
			}
		}

		if (!links.IsEmpty())
			CopyTextToClipboard(GetSafeHwnd(), links);

		DeselectAllItems();
	}
}


void CFeedView::OnSendByEmail() {
	LOG0(3, "CFeedView::OnSendByEmail()");

	if (m_nSelectFirst != -1) {
		CString links;
		for (int i = m_nSelectStart; i <= m_nSelectEnd; i++) {
			CFeedItem *fi = m_oItems.GetAt(i);
			if (fi != NULL) {
				if (i > m_nSelectStart)
					links += _T("\n");
				links += fi->Link;
			}
		}

		if (!links.IsEmpty())
			SendByEmail(links);

		DeselectAllItems();
	}
}

// Sort ////

int CompareFeedItemDate(CFeedItem *fi1, CFeedItem *fi2) {
	FILETIME ft1, ft2;
	SystemTimeToLinearDate(&fi1->Date, &ft1);
	SystemTimeToLinearDate(&fi2->Date, &ft2);
	return CompareFileTime(&ft1, &ft2);
}

int CompareFeedItemFlag(CFeedItem *fi1, CFeedItem *fi2) {
	return ((int) fi1->GetReadState() - (int) fi2->GetReadState());
}

int CompareFlagsAsc(const void *arg1, const void *arg2) {
	CFeedItem *fi1 = (*(CFeedItem **) arg1);
	CFeedItem *fi2 = (*(CFeedItem **) arg2);
	return CompareFeedItemFlag(fi1, fi2);
}

int CompareFlagsDesc(const void *arg1, const void *arg2) {
	CFeedItem *fi1 = (*(CFeedItem **) arg1);
	CFeedItem *fi2 = (*(CFeedItem **) arg2);
	return CompareFeedItemFlag(fi2, fi1);
}

int CompareDateAsc(const void *arg1, const void *arg2) {
	CFeedItem *fi1 = (*(CFeedItem **) arg1);
	CFeedItem *fi2 = (*(CFeedItem **) arg2);
	return CompareFeedItemDate(fi1, fi2);
}

int CompareDateDesc(const void *arg1, const void *arg2) {
	CFeedItem *fi1 = (*(CFeedItem **) arg1);
	CFeedItem *fi2 = (*(CFeedItem **) arg2);
	return CompareFeedItemDate(fi2, fi1);
}

int CompareVFolder(const void *arg1, const void *arg2) {
	CFeedItem *fi1 = (*(CFeedItem **) arg1);
	CFeedItem *fi2 = (*(CFeedItem **) arg2);

	if (fi1->SiteItem == fi2->SiteItem) {
		switch (fi1->SiteItem->Sort.Item) {
			case CSortInfo::Date:
				if (fi1->SiteItem->Sort.Type == CSortInfo::Ascending) return CompareFeedItemDate(fi1, fi2);
				else return CompareFeedItemDate(fi2, fi1);

			case CSortInfo::Read:
				if (fi1->SiteItem->Sort.Type == CSortInfo::Ascending) return CompareFeedItemFlag(fi1, fi2);
				else return CompareFeedItemFlag(fi2, fi1);
		}
	}
	else {
		return fi1->SiteItem - fi2->SiteItem;
	}

	return CompareFeedItemDate(fi2, fi1);
}

void CFeedView::Sort(int (__cdecl *compare)(const void *elem1, const void *elem2)) {
	int count = GetItemCount();
	int i;

	CFeedItem **items = new CFeedItem *[count];
	for (i = 0; i < count; i++)
		items[i] = m_oItems[i];

	qsort((void *) items, count, sizeof(CFeedItem *), compare);

	for (i = 0; i < count; i++)
		m_oItems.SetAt(i, items[i]);
//	UpdateItemHeights();

	delete [] items;
}

void CFeedView::SortItems() {
	LOG0(3, "CFeedView::SortItems()");

	if (SiteItem == NULL)
		return;				// no sorting if we dont know how to sort

	if (SiteItem->Type == CSiteItem::VFolder && Config.GroupByFeed) {
		Sort(CompareVFolder);
	}
	else {
		switch (SiteItem->Sort.Item) {
			case CSortInfo::Date:
				if (SiteItem->Sort.Type == CSortInfo::Ascending) Sort(CompareDateAsc);
				else Sort(CompareDateDesc);
				break;

			case CSortInfo::Read:
				if (SiteItem->Sort.Type == CSortInfo::Ascending) Sort(CompareFlagsAsc);
				else Sort(CompareFlagsDesc);
				break;
		}
	}

	UpdateItemHeights();
}

void CFeedView::OnSortAscending() {
	LOG0(3, "CFeedView::SortAscending()");

	if (SiteItem == NULL)
		return;

	if (SiteItem->Sort.Type != CSortInfo::Ascending) {
		SiteItem->Sort.Type = CSortInfo::Ascending;
		DeselectAllItems();
		SortItems();
	}
}

void CFeedView::OnSortDescending() {
	LOG0(3, "CFeedView::SortDescending()");

	if (SiteItem == NULL)
		return;

	if (SiteItem->Sort.Type != CSortInfo::Descending) {
		SiteItem->Sort.Type = CSortInfo::Descending;
		DeselectAllItems();
		SortItems();
	}
}

void CFeedView::InsertItems(CSiteItem *si) {
	SetRedraw(FALSE);
	DeleteAllItems();
	SiteItem = si;

	CFeed *feed = si->Feed;
	if (feed != NULL) {
		feed->Lock();
		if (si->Type == CSiteItem::Site) {
			for (int i = 0, j = 0; i < feed->GetItemCount(); i++) {
				CFeedItem *item = feed->GetItem(i);
				if (!Config.HideReadItems || !item->IsRead())
					if (InsertItem(j, item))
						j++;
			}
		}
		else if (si->Type == CSiteItem::VFolder) {
			for (int i = 0, j = 0; i < feed->GetItemCount(); i++) {
				CFeedItem *item = feed->GetItem(i);
				if ((item->GetFlags() & si->FlagMask) != 0)
					if (InsertItem(j, item))
						j++;
			}
		}
		feed->Unlock();
	}
	SortItems();
	UpdateScrollBars();

	SetRedraw(TRUE);
	Invalidate();
}

void CFeedView::OnViewHideReadItems() {
	Config.HideReadItems = !Config.HideReadItems;
	Config.SaveUI();

	InsertItems(SiteItem);
	UpdateScrollBars();
	UpdateItemHeights();
}

void CFeedView::OnUpdateViewHideReadItems(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(Config.HideReadItems);
}

int CFeedView::CalcItemHeight(int idx) {
	if (m_bWrapTitles) {
		CDC *pDC = GetDC();

		CRect rc(0, 0, m_nClientWidth, m_nClientHeight);
		rc.DeflateRect(SCALEX(LEFT_SKIP), SCALEY(PADDING_TOP), SCALEX(PADDING_RIGHT), SCALEY(PADDING_BOTTOM));
		int wd = rc.Width();

		CString strTitle = m_oItems[idx]->Title;
		ReplaceHTMLEntities(strTitle);

		CFont *pOldFont = pDC->SelectObject(&m_fntBold);
		pDC->DrawText(strTitle, &rc, DT_LEFT | DT_TOP | DT_NOPREFIX | DT_CALCRECT | DT_WORDBREAK);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);

		return rc.Height() + SCALEY(DATE_HEIGHT);
	}
	else
		return SCALEY(Appearance.FeedViewFontCfg.Size + ITEM_MARGIN);
}

void CFeedView::UpdateItemHeights() {
	CRect rc;
	GetClientRect(rc);
	m_nClientWidth = rc.Width();

	m_nTotalHeight = 0;
	bool refresh = false;
	for (int i = 0; i < m_oItems.GetSize(); i++) {
		m_nTotalHeight += CalcItemHeight(i);
		if (m_nTotalHeight > m_nClientHeight) {
			m_nClientWidth -= ::GetSystemMetrics(SM_CXVSCROLL);
			refresh = true;
			break;
		}
		m_oItemHeight[i] = m_nTotalHeight;
	}

	if (refresh) {
		m_nTotalHeight = 0;
		for (int i = 0; i < m_oItems.GetSize(); i++) {
			m_nTotalHeight += CalcItemHeight(i);
			m_oItemHeight[i] = m_nTotalHeight;
		}
	}
}

