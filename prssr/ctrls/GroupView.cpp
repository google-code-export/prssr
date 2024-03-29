/**
 *  GroupView.cpp
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
#include "GroupView.h"

#include "../Appearance.h"
#include "../Config.h"
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

#define CLASS_NAME						_T("PrssrGroupViewClass")

const int CGroupView::ITEM_MARGIN = 9;

BOOL CGroupView::Register() {
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

void CGroupView::Unregister() {
}

/////////////////////////////////////////////////////////////////////////////
// CGroupView

CGroupView::CGroupView() {
	RootItem.Parent = NULL;
	RootItem.State = GVIS_EXPANDED;
	RootItem.yTop = 0;

	m_nTotalHeight = 0;
	m_nViewTop = 0;
	m_hSelectedItem = NULL;

	CtxMenuTimer = 1;
	m_bOpenCtxMenu = FALSE;

	m_hClickedItem = NULL;

	ItemHeight = ITEM_MARGIN + Appearance.SummaryViewFontCfg.Size;

	NoItemsId = IDS_NO_ITEMS_TO_DISPLAY;
}

CGroupView::~CGroupView() {
	while (!Items.IsEmpty())
		delete Items.RemoveHead();
}


BEGIN_MESSAGE_MAP(CGroupView, CView)
	//{{AFX_MSG_MAP(CGroupView)
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
END_MESSAGE_MAP()

void CGroupView::CreateFonts() {
	m_fntBase.DeleteObject();
	m_fntBold.DeleteObject();

	LOGFONT lf = { 0 };
	wcscpy(lf.lfFaceName, Appearance.SummaryViewFontCfg.FontFace);
	CDC *pDC = GetDC();
	lf.lfHeight = ((Appearance.SummaryViewFontCfg.Size * pDC->GetDeviceCaps(LOGPIXELSY)) / 72) + 1;
	m_fntBase.CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lf);

	ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CGroupView message handlers

int CGroupView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	LOG1(5, "CGroupView::OnCreate(%p)", lpCreateStruct);

	BOOL ret = CView::OnCreate(lpCreateStruct);
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

void CGroupView::OnDraw(CDC *pDC) {
}


void CGroupView::UpdateItemsY(HGROUPITEM hParent, int &top) {
	if (hParent == NULL)
		return;

	GVITEM *giParent = Items.GetAt(hParent);
	giParent->yTop = top;

	if (top != -1)
		top += SCALEY(ItemHeight);

	if (giParent->State == GVIS_COLLAPSED) {
		int t = -1;
		HGROUPITEM hChild = GetChildItem(hParent);
		while (hChild != NULL) {
			GVITEM *giChild = Items.GetAt(hChild);
			giChild->State = GVIS_COLLAPSED;

			UpdateItemsY(hChild, t);
			hChild = GetNextSiblingItem(hChild);
		}
	}
	else {
		HGROUPITEM hChild = GetChildItem(hParent);
		while (hChild != NULL) {
			UpdateItemsY(hChild, top);
			hChild = GetNextSiblingItem(hChild);
		}
	}
}

void CGroupView::DrawIcon(CDC &dc, int icon, BOOL selected) {
	IMAGELISTDRAWPARAMS drawing;
	drawing.cbSize = sizeof(IMAGELISTDRAWPARAMS);
	drawing.himl = m_pImageList->GetSafeHandle();
	drawing.i = icon;
	drawing.hdcDst = dc.GetSafeHdc();
	drawing.x = SCALEX(2);
	drawing.y = SCALEY(1);
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

void CGroupView::OnDrawItem(CDC &dc, CRect &rc, HGROUPITEM hItem, BOOL selected) {
	LOG0(5, "CGroupView::OnDrawItem");

	GVITEM *gi = Items.GetAt(hItem);

	COLORREF clrBg, clrFg, clrOld;
	if (selected) {
		clrBg = ::GetSysColor(COLOR_HIGHLIGHT);
		clrFg = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else {
		clrBg = ::GetSysColor(COLOR_WINDOW);
		clrFg = ::GetSysColor(COLOR_WINDOWTEXT);
	}

	// background
	dc.FillSolidRect(rc, clrBg);

	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	CRect rcItem = rc;

	// icon
	if (gi->Image != -1) {
		DrawIcon(dc, gi->Image, selected);
	}
	else {
		CRect rcIcon(rc.left, rc.top, rc.left + SCALEX(20), rc.bottom);
	}

	rcItem.left += SCALEX(20);

	// title
	CFont *pOldFont;
	pOldFont = dc.SelectObject(&m_fntBase);

	clrOld = dc.SetTextColor(clrFg);
	rcItem.DeflateRect(SCALEX(2), SCALEY(1), SCALEX(2), SCALEY(3));

	DrawTextEndEllipsis(dc, gi->Text, rcItem, DT_LEFT | DT_BOTTOM | DT_NOPREFIX);
	dc.SetTextColor(clrOld);
	dc.SelectObject(pOldFont);

	dc.SetBkMode(oldBkMode);
}

void CGroupView::DrawItems(CDC &dc, CRect &rc, HGROUPITEM hParent) {
	if (hParent == NULL)
		return;

	BOOL selected = (GetSelectedItem() == hParent);
	GVITEM *giParent = Items.GetAt(hParent);
	if (giParent->yTop != -1) {
		CRect rcPhys(rc.left, giParent->yTop - m_nViewTop, rc.right, giParent->yTop + SCALEY(ItemHeight) - m_nViewTop);

		CRect rcClip;
		dc.GetClipBox(rcClip);

		CRect rcDraw;
		if (rcDraw.IntersectRect(rcClip, rcPhys)) {
			// offscreen buffer
			CBitmap bmp;
			bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
			CDC memDC;
			memDC.CreateCompatibleDC(&dc);
			CBitmap *saveBmp = memDC.SelectObject(&bmp);

			CRect rcItem = rc;
			OnDrawItem(memDC, rcItem, hParent, selected);
			// copy to screen
			dc.BitBlt(0, giParent->yTop - m_nViewTop, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

			//
			memDC.SelectObject(saveBmp);
		}
	}

	if (giParent->State == GVIS_COLLAPSED) {
	}
	else {
		HGROUPITEM hChild = GetChildItem(hParent);
		while (hChild != NULL) {
			DrawItems(dc, rc, hChild);
			hChild = GetNextSiblingItem(hChild);
		}
	}
}

void CGroupView::OnPaint() {
	LOG0(5, "CGroupView::OnPaint()");

	CPaintDC dc(this); // device context for painting

	int nSavedDC = dc.SaveDC();

	CRect rc;
	GetClientRect(rc);

	if (Items.GetCount() > 0) {
		if (m_oVScrollBar.IsWindowVisible()) {
			CRect rcVScroll;
			m_oVScrollBar.GetWindowRect(rcVScroll);
			rc.right -= rcVScroll.Width() - SCALEX(1);
		}

		//
		int top = rc.top;
		int ofsY = 0;

		CRect rcItem(rc.left, 0, rc.right, SCALEY(ItemHeight));
		HGROUPITEM hRoot = GetRootItem();
		while (hRoot != NULL) {
			DrawItems(dc, rcItem, hRoot);
			hRoot = GetNextSiblingItem(hRoot);
		}

		if (m_nTotalHeight < m_nClientHeight) {
			dc.FillSolidRect(rc.left, m_nTotalHeight, rc.right, rc.bottom, Appearance.ClrSummaryBg);
		}
	}
	else {
		CString sText;
		sText.LoadString(NoItemsId);

		dc.FillSolidRect(rc, Appearance.ClrSummaryBg);
		rc.DeflateRect(SCALEX(8), SCALEY(8));

		CFont *pFont = dc.SelectObject(&Appearance.BaseFont);
		dc.SetTextColor(Appearance.ClrSummaryFg);
		dc.DrawText(sText, &rc, DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);
		dc.SelectObject(pFont);
	}

	dc.RestoreDC(nSavedDC);
}

// implementation

UINT CGroupView::GetCount() const {
	return Items.GetCount();
}

BOOL CGroupView::ItemHasChildren(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	return gi->Childs.GetCount() > 0;
}

HGROUPITEM CGroupView::GetChildItem(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	if (gi->Childs.GetCount() > 0)
		return gi->Childs.GetHead();
	else
		return NULL;
}

HGROUPITEM CGroupView::GetNextSiblingItem(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	HGROUPITEM hParent = gi->Parent;
	const GVITEM *giParent;
	if (hParent == NULL)
		giParent = &RootItem;
	else
		giParent = Items.GetAt(hParent);

	POSITION pos = giParent->Childs.Find(hItem);
	giParent->Childs.GetNext(pos);
	if (pos != NULL)
		return giParent->Childs.GetAt(pos);
	else
		return NULL;
}

HGROUPITEM CGroupView::GetPrevSiblingItem(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	HGROUPITEM hParent = gi->Parent;
	const GVITEM *giParent;
	if (hParent == NULL)
		giParent = &RootItem;
	else
		giParent = Items.GetAt(hParent);

	POSITION pos = giParent->Childs.Find(hItem);
	giParent->Childs.GetPrev(pos);
	if (pos != NULL)
		return giParent->Childs.GetAt(pos);
	else
		return NULL;
}

HGROUPITEM CGroupView::GetParentItem(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	return gi->Parent;
}

HGROUPITEM CGroupView::GetRootItem() const {
	if (RootItem.Childs.GetCount() > 0)
		return RootItem.Childs.GetHead();
	else
		return NULL;
}

HGROUPITEM CGroupView::GetPrevVisibleItem(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);

	HGROUPITEM hPrev = GetPrevSiblingItem(hItem);
	if (hPrev == NULL)
		return GetParentItem(hItem);
	else {
		GVITEM *giPrev = Items.GetAt(hPrev);
		while (giPrev->State == GVIS_EXPANDED) {
			hPrev = giPrev->Childs.GetTail();
			giPrev = Items.GetAt(hPrev);
		}
		return hPrev;
	}
}

HGROUPITEM CGroupView::GetNextVisibleItem(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	if (gi->State == GVIS_COLLAPSED) {
		HGROUPITEM hNext = GetNextSiblingItem(hItem);
		if (hNext == NULL) {
			HGROUPITEM hParent = GetParentItem(hItem);
			if (hParent == NULL)
				return NULL;
			else
				return GetNextSiblingItem(hParent);
		}
		else
			return hNext;
	}
	else if (gi->State == GVIS_EXPANDED) {
		HGROUPITEM hNext = GetChildItem(hItem);
		return hNext;
	}
	else
		return NULL;
}

UINT CGroupView::GetItemState(HGROUPITEM hItem, UINT nStateMask) const {
	GVITEM *gi = Items.GetAt(hItem);
	return gi->State;
}

BOOL CGroupView::SetItemState(HGROUPITEM hItem, UINT nState, UINT nStateMask) {
	GVITEM *gi = Items.GetAt(hItem);
	gi->State = nState;
	return TRUE;
}

BOOL CGroupView::GetItemImage(HGROUPITEM hItem, int &nImage) const {
	GVITEM *gi = Items.GetAt(hItem);
	nImage = gi->Image;
	return TRUE;
}

BOOL CGroupView::SetItemImage(HGROUPITEM hItem, int nImage) {
	GVITEM *gi = Items.GetAt(hItem);
	gi->Image = nImage;
	return TRUE;
}

CString CGroupView::GetItemText(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	return gi->Text;
}

BOOL CGroupView::SetItemText(HGROUPITEM hItem, LPCTSTR lpszItem) {
	GVITEM *gi = Items.GetAt(hItem);
	gi->Text = CString(lpszItem);
	return TRUE;
}

DWORD CGroupView::GetItemData(HGROUPITEM hItem) const {
	GVITEM *gi = Items.GetAt(hItem);
	return gi->Data;
}

BOOL CGroupView::SetItemData(HGROUPITEM hItem, DWORD dwData) {
	GVITEM *gi = Items.GetAt(hItem);
	gi->Data = dwData;
	return TRUE;
}

HGROUPITEM CGroupView::InsertItem(LPCTSTR lpszItem, HGROUPITEM hParent/* = GVI_ROOT*/, HGROUPITEM hInsertAfter/* = GVI_LAST*/) {
	LOG1(5, "CGroupView::InsertItem('%S', , )", lpszItem);

	GVITEM *giParent;

	GVITEM *giNew = new GVITEM;
	giNew->Text = CString(lpszItem);
	giNew->Image = -1;
	giNew->State = GVIS_COLLAPSED;
	if (hParent == GVI_ROOT) {
		giNew->Parent = NULL;
		giParent = &RootItem;
	}
	else {
		giNew->Parent = hParent;
		giParent = Items.GetAt(hParent);
	}

	if (giParent->State == GVIS_EXPANDED)
		m_nTotalHeight += SCALEY(ItemHeight);

	HGROUPITEM hNewItem = Items.AddTail(giNew);
	if (hInsertAfter == GVI_LAST)
		giParent->Childs.AddTail(hNewItem);
	else if (hInsertAfter == GVI_FIRST)
		giParent->Childs.AddHead(hNewItem);
	else {
		POSITION pos = giParent->Childs.Find(hInsertAfter);
		giParent->Childs.InsertAfter(pos, hNewItem);
	}

	//
	int top = 0;
	HGROUPITEM hItem = NULL;

	if (hParent == GVI_ROOT) {
		top = 0;
		hItem = GetRootItem();
	}
	else {
		top = giParent->yTop;
		hItem = hParent;
	}

	while (hItem != NULL) {
		UpdateItemsY(hItem, top);
		hItem = GetNextSiblingItem(hItem);
	}

	return hNewItem;
}

HGROUPITEM CGroupView::InsertItem(LPCTSTR lpszItem, int nImage, HGROUPITEM hParent/* = GVI_ROOT*/, HGROUPITEM hInsertAfter/* = GVI_LAST*/) {
	LOG2(5, "CGroupView::InsertItem('%S', %d, , )", lpszItem, nImage);

	GVITEM *giParent;

	GVITEM *giNew = new GVITEM;
	giNew->Text = CString(lpszItem);
	giNew->Image = nImage;
	giNew->State = GVIS_COLLAPSED;
	if (hParent == GVI_ROOT) {
		giNew->Parent = NULL;
		giParent = &RootItem;
	}
	else {
		giNew->Parent = hParent;
		giParent = Items.GetAt(hParent);
	}

	if (giParent->State == GVIS_EXPANDED)
		m_nTotalHeight += SCALEY(ItemHeight);

	HGROUPITEM hNewItem = Items.AddTail(giNew);
	if (hInsertAfter == GVI_LAST)
		giParent->Childs.AddTail(hNewItem);
	else if (hInsertAfter == GVI_FIRST)
		giParent->Childs.AddHead(hNewItem);
	else {
		POSITION pos = giParent->Childs.Find(hInsertAfter);
		giParent->Childs.InsertAfter(pos, hNewItem);
	}

	//
	int top = 0;
	HGROUPITEM hItem = NULL;

	if (hParent == GVI_ROOT) {
		top = 0;
		hItem = GetRootItem();
	}
	else {
		top = giParent->yTop;
		hItem = hParent;
	}

	while (hItem != NULL) {
		UpdateItemsY(hItem, top);
		hItem = GetNextSiblingItem(hItem);
	}

	return hNewItem;
}

BOOL CGroupView::DeleteItem(HGROUPITEM hItem) {
	return TRUE;
}

BOOL CGroupView::DeleteAllItems() {
	RootItem.Childs.RemoveAll();
	while (!Items.IsEmpty())
		delete Items.RemoveHead();

	m_nTotalHeight = 0;
	m_nViewTop = 0;
	m_hSelectedItem = NULL;

	return TRUE;
}

void CGroupView::AdjustViewTop() {

	if (m_nViewTop < 0)
		m_nViewTop = 0;
	else if (m_nTotalHeight > m_nClientHeight) {
		int max = m_nTotalHeight - m_nClientHeight;
		if (m_nViewTop > max)
			m_nViewTop = max;
	}
	else if (m_nTotalHeight <= m_nClientHeight) {
		m_nViewTop = 0;
	}
}


BOOL CGroupView::EnsureVisible(HGROUPITEM hItem) {
	GVITEM *gi = Items.GetAt(hItem);

	int i = m_nViewTop;
	int y = Items.GetAt(hItem)->yTop;
	if (y < m_nViewTop)
		m_nViewTop = y;
	else if (y + SCALEY(ItemHeight) > m_nViewTop + m_nClientHeight)
		m_nViewTop = y + SCALEY(ItemHeight) - m_nClientHeight;

	if (i - m_nViewTop)
		ScrollWindowEx(0, i - m_nViewTop, &m_rcScroll, &m_rcScroll, NULL, NULL, SW_INVALIDATE);

	UpdateScrollBars();

	return TRUE;
}

// message handlers

BOOL CGroupView::OnEraseBkgnd(CDC *pDC) {
	return TRUE;
}

void CGroupView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
	int i = m_nViewTop;
	switch (nSBCode) {
		case SB_LINEDOWN:   m_nViewTop += SCALEY(ItemHeight); break;
		case SB_LINEUP:     m_nViewTop -= SCALEY(ItemHeight); break;
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

void CGroupView::UpdateScrollBars() {
	LOG0(5, "CGroupView::UpdateScrollBars()");

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

void CGroupView::OnSize(UINT nType, int cx, int cy) {
	CView::OnSize(nType, cx, cy);

	m_nClientHeight = cy;
	m_nClientWidth = cx + SCALEX(1);

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

	UpdateScrollBars();

	Invalidate();
}

void CGroupView::InvalidateItem(HGROUPITEM hItem, BOOL erase/* = TRUE*/) {
	if (hItem == NULL)
		return;

	GVITEM *gi = Items.GetAt(hItem);
	if (gi == NULL)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rc(rcClient.left, gi->yTop - m_nViewTop, rcClient.right, gi->yTop + SCALEY(ItemHeight) - m_nViewTop);

	CRect rcInv;
	if (rcInv.IntersectRect(rcClient, rc)) {
		InvalidateRect(rcInv, erase);
	}
}

HGROUPITEM CGroupView::ItemFromPoint(CPoint pt) {
	POSITION pos = Items.GetHeadPosition();
	while (pos != NULL) {
		POSITION posPrev = pos;
		GVITEM *gi = Items.GetNext(pos);
		if (gi->yTop != -1) {
			if (gi->yTop - m_nViewTop <= pt.y && pt.y <= gi->yTop + SCALEY(ItemHeight) - m_nViewTop) {
				return (HGROUPITEM) posPrev;
			}
		}
	}

	return NULL;
}

// Normal

void CGroupView::OnLButtonDownNormal(UINT nFlags, CPoint point) {
	SetFocus();

	HGROUPITEM item = ItemFromPoint(point);
	if (item != NULL) {
		m_hClickedItem = item;
		LastCursorPos = point;

		HGROUPITEM hOldItem = m_hSelectedItem;
		SelectItem(m_hClickedItem);
		InvalidateItem(hOldItem, FALSE);
		InvalidateItem(m_hClickedItem, FALSE);

		SHRGINFO shrgi = {0};
		shrgi.cbSize        = sizeof(SHRGINFO);
		shrgi.hwndClient    = m_hWnd;
		shrgi.ptDown.x      = point.x;
		shrgi.ptDown.y      = point.y;
		shrgi.dwFlags       = SHRG_RETURNCMD;

		if (GN_CONTEXTMENU == ::SHRecognizeGesture(&shrgi)) {
			m_hClickedItem = NULL;

			CPoint pt = point;
			ClientToScreen(&pt);
			ContextMenu(&pt);
		}
		else
			Default();
	}
	else {
		// tap out of items (deselect all)
		HGROUPITEM hOldItem = m_hSelectedItem;

		m_hSelectedItem = NULL;
		InvalidateItem(hOldItem, FALSE);
	}
}

void CGroupView::OnLButtonUpNormal(UINT nFlags, CPoint point) {
	HGROUPITEM hItem = ItemFromPoint(point);
	if (m_hClickedItem != NULL && hItem != NULL && hItem == m_hClickedItem) {
		if (ItemHasChildren(m_hClickedItem))
			ToggleItem(m_hClickedItem);
		else
			OnItemClicked();
	}
	m_hClickedItem = NULL;
}

// TOUCH

void CGroupView::OnLButtonDownTouch(UINT nFlags, CPoint point) {
	HGROUPITEM item = ItemFromPoint(point);
	if (item != NULL) {
		m_hClickedItem = item;
		LastCursorPos = point;

		SHRGINFO shrgi = { 0 };
		shrgi.cbSize        = sizeof(SHRGINFO);
		shrgi.hwndClient    = m_hWnd;
		shrgi.ptDown.x      = point.x;
		shrgi.ptDown.y      = point.y;
		shrgi.dwFlags       = SHRG_RETURNCMD;

		if (GN_CONTEXTMENU == ::SHRecognizeGesture(&shrgi)) {
			HGROUPITEM hOldItem = m_hSelectedItem;
			SelectItem(m_hClickedItem);
			InvalidateItem(hOldItem, FALSE);
			InvalidateItem(m_hClickedItem, FALSE);

			CPoint pt = point;
			ClientToScreen(&pt);
			ContextMenu(&pt);

			m_hClickedItem = NULL;
		}
		else {
			SetCapture();
			Default();
		}
	}
	else {
		// tap out of items (deselect all)
		HGROUPITEM hOldItem = m_hSelectedItem;

		m_hSelectedItem = NULL;
		InvalidateItem(hOldItem, FALSE);
	}
}

void CGroupView::OnMouseMoveTouch(UINT nFlags, CPoint pt) {
	if (abs(pt.x - LastCursorPos.x) > SCALEX(3) || abs(pt.y - LastCursorPos.y) > SCALEX(3))
		m_hClickedItem = NULL;

	// Scrolling
	if (m_nTotalHeight > m_nClientHeight && GetCapture() == this) {
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

void CGroupView::OnLButtonUpTouch(UINT nFlags, CPoint point) {
	ReleaseCapture();

	HGROUPITEM hItem = ItemFromPoint(point);
	if (m_hClickedItem != NULL) {
		if (hItem != NULL && hItem == m_hClickedItem) {
			SelectItem(m_hClickedItem);

			if (ItemHasChildren(m_hClickedItem))
				ToggleItem(m_hClickedItem);
			else
				OnItemClicked();
		}
		else {

		}
	}
	m_hClickedItem = NULL;
}

void CGroupView::OnLButtonDown(UINT nFlags, CPoint point) {
	switch (Config.NavigationType) {
		case NAVIGATION_NORMAL: OnLButtonDownNormal(nFlags, point); break;
		default:
		case NAVIGATION_TOUCH: OnLButtonDownTouch(nFlags, point); break;
	}
}

void CGroupView::OnMouseMove(UINT nFlags, CPoint pt) {
	switch (Config.NavigationType) {
		case NAVIGATION_NORMAL: CView::OnMouseMove(nFlags, pt); break;
		default:
		case NAVIGATION_TOUCH: OnMouseMoveTouch(nFlags, pt); break;
	}
}

void CGroupView::OnLButtonUp(UINT nFlags, CPoint point) {
	switch (Config.NavigationType) {
		case NAVIGATION_NORMAL: OnLButtonUpNormal(nFlags, point); break;
		default:
		case NAVIGATION_TOUCH: OnLButtonUpTouch(nFlags, point); break;
	}
}

// keys

void CGroupView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	LOG3(5, "CGroupView::OnKeyDown(%d, %d, %d)", nChar, nRepCnt, nFlags);

	HGROUPITEM hItem;
	switch (nChar) {
		case VK_UP:
			// move to previous visible item
			if (m_hSelectedItem != NULL) {
				hItem = GetPrevVisibleItem(m_hSelectedItem);
				if (hItem != NULL) {
					HGROUPITEM hOldItem = m_hSelectedItem;
					SelectItem(hItem);
				}
			}
			break;

		case VK_DOWN:
			// move to next visible item
			if (m_hSelectedItem == NULL) {
				hItem = GetRootItem();
				SelectItem(hItem);
			}
			else {
				hItem = GetNextVisibleItem(m_hSelectedItem);
				if (hItem != NULL) {
					SelectItem(hItem);
				}
			}
			break;

		case VK_LEFT:
			CollapseItem(m_hSelectedItem);
			break;

		case VK_RIGHT:
			ExpandItem(m_hSelectedItem);
			break;

		case VK_RETURN:
			if (!(nFlags & 0x4000)) {
				m_bOpenCtxMenu = FALSE;
				SetTimer(CtxMenuTimer, TIMER_KEY_CTX_MENU, NULL);
			}
			break;

		default:
			CView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

void CGroupView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	LOG3(5, "CGroupView::OnKeyUp(%d, %d, %d)", nChar, nRepCnt, nFlags);

	switch (nChar) {
		case VK_RETURN:
			KillTimer(CtxMenuTimer);
			if (m_bOpenCtxMenu) {
				m_bOpenCtxMenu = FALSE;

				if (Items.GetCount() > 0 && m_hSelectedItem != NULL) {
					GVITEM *gi = Items.GetAt(m_hSelectedItem);
					CPoint pt(m_nClientWidth / 2, gi->yTop - m_nViewTop + SCALEY(ItemHeight / 2));
					ClientToScreen(&pt);
					ContextMenu(&pt);
				}
				else
					CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
			}
			else {
				if (Items.GetCount() > 0 && m_hSelectedItem != NULL) {
					// toggle item state
					if (ItemHasChildren(m_hSelectedItem))
						ToggleItem(m_hSelectedItem);
					else
						OnItemClicked();
				}
				CView::OnKeyUp(nChar, nRepCnt, nFlags);
			}
			break;

		case VK_APPS: {
			CPoint pt;
			GVITEM *gi = Items.GetAt(m_hSelectedItem);
			pt.x = m_nClientWidth / 2;
			pt.y = gi->yTop - m_nViewTop + SCALEY(ItemHeight / 2);

			ClientToScreen(&pt);

			ContextMenu(&pt);
			} break;

		default:
			CView::OnKeyUp(nChar, nRepCnt, nFlags);
			break;
	}
}

void CGroupView::ContextMenu(CPoint *pt) {
	LOG0(5, "CGroupView::ContextMenu()");

}

void CGroupView::OnTimer(UINT nIDEvent) {
	LOG0(5, "CGroupView::OnTimer()");

	if (nIDEvent == CtxMenuTimer) {
		m_bOpenCtxMenu = TRUE;
		KillTimer(CtxMenuTimer);
	}

	CView::OnTimer(nIDEvent);
}

void CGroupView::ExpandItem(HGROUPITEM hItem) {
	LOG0(1, "CGroupView::ExpandItem()");

	if (hItem == NULL)
		return;

	GVITEM *gi = Items.GetAt(hItem);
	if (gi->State == GVIS_EXPANDED || gi->Childs.GetCount() <= 0)
		return;
	gi->State = GVIS_EXPANDED;

	int top = gi->yTop;
	HGROUPITEM hNext = hItem;
	while (hNext != NULL) {
		UpdateItemsY(hNext, top);
		hNext = GetNextSiblingItem(hNext);
	}

//	LOG1(1, "- diff = %d", top - m_nTotalHeight);
	int diff = top - m_nTotalHeight + SCALEY(ItemHeight);
	if (gi->yTop + diff > m_nViewTop + m_nClientHeight) {
		m_nViewTop = gi->yTop + diff - m_nClientHeight;
		if (gi->yTop < m_nViewTop)
			m_nViewTop = gi->yTop;
	}
	m_nTotalHeight = top;

	Invalidate(FALSE);
	AdjustViewTop();
	UpdateScrollBars();
}

void CGroupView::CollapseItem(HGROUPITEM hItem) {
	LOG0(5, "CGroupView::CollapseItem()");

	if (hItem == NULL)
		return;

	GVITEM *gi = Items.GetAt(hItem);
	if (gi->State == GVIS_COLLAPSED || gi->Childs.GetCount() <= 0)
		return;
	gi->State = GVIS_COLLAPSED;

	int top = gi->yTop;
	int t = -1;
	HGROUPITEM hNext = hItem;
	UpdateItemsY(hNext, t);
	gi->yTop = top;
	top += SCALEY(ItemHeight);

	if (hNext != NULL) {
		hNext = GetNextSiblingItem(hNext);
		while (hNext != NULL) {
			UpdateItemsY(hNext, top);
			hNext = GetNextSiblingItem(hNext);
		}
	}

	m_nTotalHeight = top;

	Invalidate(FALSE);
	AdjustViewTop();
	UpdateScrollBars();
}

void CGroupView::ToggleItem(HGROUPITEM hItem) {
	LOG1(5, "CGroupView::ToggleItem(%d)", hItem);

	if (hItem == NULL)
		return;

	if (GetItemState(hItem, 0) == GVIS_EXPANDED)
		CollapseItem(hItem);
	else if (GetItemState(hItem, 0) == GVIS_COLLAPSED)
		ExpandItem(hItem);
}

void CGroupView::SelectItem(HGROUPITEM hItem) {
	LOG1(5, "CGroupView::SelectItem(%d)", hItem);

	HGROUPITEM hOldItem = m_hSelectedItem;
	EnsureVisible(hItem);
	m_hSelectedItem = hItem;
	InvalidateItem(hOldItem, FALSE);
	InvalidateItem(m_hSelectedItem, FALSE);
}

void CGroupView::OnItemClicked() {
}

void CGroupView::UpdateItemHeight() {
	int items = m_nTotalHeight / SCALEY(ItemHeight);
	m_nTotalHeight = items * SCALEY(ItemHeight);
	int top = 0;
	HGROUPITEM hItem = GetRootItem();
	while (hItem != NULL) {
		UpdateItemsY(hItem, top);
		hItem = GetNextSiblingItem(hItem);
	}

	CreateFonts();
	UpdateScrollBars();
	Invalidate();
}
