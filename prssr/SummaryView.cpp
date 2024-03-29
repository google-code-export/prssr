/**
 *  SummaryView.cpp
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
#include "SummaryView.h"
#include "../share/helpers.h"
#include "Config.h"
#include "Appearance.h"
#include "MainFrm.h"
#include "Properties.h"


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

#include "Site.h"


/////////////////////////////////////////////////////////////////////////////
// CSummaryView

CSummaryView::CSummaryView() {
	NoItemsId = IDS_NO_FEEDS_SUMM;

	SetItemHeight();
}

CSummaryView::~CSummaryView() {
}

BEGIN_MESSAGE_MAP(CSummaryView, CGroupView)
	//{{AFX_MSG_MAP(CSummaryView)
	ON_COMMAND(ID_UPDATE_CHANNEL, OnUpdateChannel)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_CHANNEL, OnUpdateUpdateChannel)
	ON_COMMAND(ID_TOOLS_MARKALLITEMSREAD, OnToolsMarkItemsRead)
	ON_COMMAND(ID_TOOLS_MARKALLITEMSUNREAD, OnToolsMarkItemsUnread)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperies)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileProperies)
	ON_COMMAND(ID_VIEW_HIDEGROUPS, OnViewHideGroups)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDEGROUPS, OnUpdateViewHideGroups)
	ON_BN_CLICKED(IDC_SITEMAN_LINK, OnSiteManLink)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSummaryView::SetItemHeight() {
	ItemHeight = ITEM_MARGIN + Appearance.SummaryViewFontCfg.Size + 11;
}

void CSummaryView::CreateFonts() {
	m_fntSmall.DeleteObject();

	HGDIOBJ hSysFont = ::GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	::GetObject(hSysFont, sizeof(LOGFONT), (LPVOID) &lf);
	lf.lfHeight = SCALEY(11) + 1;
	m_fntSmall.CreateFontIndirect(&lf);
}

void CSummaryView::OnInitialUpdate() {
	LOG0(3, "CSummaryView::OnInitialUpdate()");

	CGroupView::OnInitialUpdate();

	int height = GetSystemMetrics(SM_CYSCREEN);
	int width = GetSystemMetrics(SM_CXSCREEN);
	CRect rc(0, height - SCALEY(54) - SCALEY(20), width, height - SCALEY(54));

	CString strText;
	strText.LoadString(IDS_OPEN_SITE_MAN);
	DWORD dwStyle = WS_CHILD | SS_NOTIFY | SS_CENTER | SS_NOPREFIX;
	if (Items.GetCount() == 0)
		dwStyle |= WS_VISIBLE;
	m_ctlSiteManLink.Create(strText, dwStyle, rc, this, IDC_SITEMAN_LINK);

	CreateFonts();
}

void CSummaryView::InsertSites(CSiteList *siteList) {
	LOG0(1, "CSummaryView::InsertSites()");

	if (siteList->GetCount() > 0) {
		InsertItems(GVI_ROOT, siteList->GetRoot());
		InsertSiteItem(GVI_ROOT, &UnreadItems);
		InsertSiteItem(GVI_ROOT, &FlaggedItems);

		if (::IsWindow(m_ctlSiteManLink.GetSafeHwnd())) {
			m_ctlSiteManLink.ShowWindow(SW_HIDE);
			m_ctlSiteManLink.EnableWindow(FALSE);
		}
	}
	else {
		if (::IsWindow(m_ctlSiteManLink.GetSafeHwnd())) {
			m_ctlSiteManLink.ShowWindow(SW_SHOW);
			m_ctlSiteManLink.EnableWindow(TRUE);
		}
	}
}

void CSummaryView::InsertItems(HGROUPITEM hParent, CSiteItem *parent) {
	LOG2(5, "CSummaryView::InsertItems(%d, %p)", hParent, parent);

	if (parent == NULL)
		return;

	POSITION pos = parent->SubItems.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *si = parent->SubItems.GetNext(pos);
		InsertSiteItem(hParent, si);
	}
}

void CSummaryView::InsertSiteItem(HGROUPITEM hParent, CSiteItem *si) {
	LOG2(5, "CSummaryView::InsertSiteItem(%d, %p)", hParent, si);

	if (si == NULL)
		return;

	HGROUPITEM hItem;
	if (si->Type == CSiteItem::Site || si->Type == CSiteItem::VFolder) {
		hItem = InsertItem(si->Name, si->ImageIdx, hParent);
		SetItemData(hItem, (DWORD) si);
	}
	else {
		if (!Config.HideGroups) {
			hItem = InsertItem(si->Name, hParent);
			SetItemData(hItem, (DWORD) si);
			InsertItems(hItem, si);
		}
		else
			InsertItems(hParent, si);
	}
}

void CSummaryView::OnDrawItem(CDC &dc, CRect &rc, HGROUPITEM hItem, BOOL selected) {
	LOG0(5, "CSummaryView::OnDrawItem");

	CSiteItem *si = (CSiteItem *) GetItemData(hItem);

	GVITEM *gi = Items.GetAt(hItem);

	COLORREF clrBg, clrFg, clrOld;
	if (selected) {
		clrBg = ::GetSysColor(COLOR_HIGHLIGHT);
		clrFg = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else {
		if (si->Type == CSiteItem::Group) {
			clrBg = Appearance.ClrGroupBg;
			clrFg = Appearance.ClrGroupFg;
		}
		else if (si->Type == CSiteItem::VFolder) {
			clrBg = Appearance.ClrVFolderBg;
			clrFg = Appearance.ClrVFolderFg;
		}
		else {
			clrBg = Appearance.ClrSummaryBg;
			clrFg = Appearance.ClrSummaryFg;
		}
	}

	// background
	dc.FillSolidRect(rc, clrBg);

	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	CRect rcItem = rc;

	// icon
	int nImg = si->ImageIdx;
	if (nImg != -1) {
		DrawIcon(dc, nImg, selected);
	}
	else {
		CRect rcIcon(rc.left, rc.top, rc.left + SCALEX(20), rc.bottom);

		CPen pen(PS_NULL, 0, 0);
		CPen *pPen = dc.SelectObject(&pen);

		CBrush br(RGB(0, 0, 0));
		CBrush *pBrush = dc.SelectObject(&br);
		if (gi->State == GVIS_COLLAPSED && gi->Childs.GetCount() > 0) {
			CPoint pts[3] = {
				CPoint(rc.left + SCALEX( 6), rc.top + SCALEY(5)),
				CPoint(rc.left + SCALEX( 6), rc.top + SCALEY(13)),
				CPoint(rc.left + SCALEX(10), rc.top + SCALEY(9)),
			};
			dc.Polygon(pts, 3);
		}
		else if (gi->State == GVIS_EXPANDED) {
			CPoint pts[3] = {
				CPoint(rc.left + SCALEX( 4) + 1, rc.top + SCALEY(8)),
				CPoint(rc.left + SCALEX(12), rc.top + SCALEY(8)),
				CPoint(rc.left + SCALEX( 8), rc.top + SCALEY(12)),
			};
			dc.Polygon(pts, 3);
		}
		dc.SelectObject(pBrush);
		dc.SelectObject(pPen);
	}

	rcItem.left += SCALEX(20);
	rcItem.DeflateRect(SCALEX(2), SCALEY(1), SCALEX(2), SCALEY(3));

	// title
	int newCnt;
	if (si->Type == CSiteItem::Site || si->Type == CSiteItem::Group)
		newCnt = si->GetUnreadCount();
	else {
		// vfolder
		if (si->FlagMask == MESSAGE_FLAG) newCnt = SiteList.GetRoot()->GetFlaggedCount();
		else newCnt = SiteList.GetRoot()->GetUnreadCount();
	}

	CFont *pOldFont;
	pOldFont = dc.SelectObject(&m_fntBold);
	if (!selected) {
		if (newCnt > 0) clrOld = dc.SetTextColor(Appearance.ClrFeedViewFg);
		else clrOld = dc.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
	}
	else clrOld = dc.SetTextColor(clrFg);
	DrawTextEndEllipsis(dc, gi->Text, rcItem, DT_LEFT | DT_TOP | DT_NOPREFIX);
	dc.SetTextColor(clrOld);
	dc.SelectObject(pOldFont);

	// 2nd line
	if (si != NULL) {
		pOldFont = dc.SelectObject(&m_fntSmall);

		CString sInfo;
/*		CString sTime;
		if (si->Info != NULL) {
			FILETIME ftFile = { 0 };
			CString fileName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, si->Info->FileName);
			if (GetFileTime(fileName, &ftFile)) {
				FILETIME ftLocal;
				FileTimeToLocalFileTime(&ftFile, &ftLocal);

				SYSTEMTIME stFile;
				FileTimeToSystemTime(&ftLocal, &stFile);
				FormatDateTime(sTime, stFile, Config.ShowRelativeDates);
			}
		}
*/
		CRect rcInfo = rcItem;
		if (selected) clrOld = dc.SetTextColor(clrFg);
		else clrOld = dc.SetTextColor(Appearance.ClrDate);

		if (newCnt > 0) sInfo.Format(IDS_UNREAD_ITEMS, newCnt);
		else sInfo.Format(IDS_NO_UNREAD_ITEMS);

//		if (!sTime.IsEmpty()) sInfo += _T(", ") + sTime;

		dc.DrawText(sInfo, &rcInfo, DT_LEFT | DT_BOTTOM | DT_NOPREFIX);
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

void CSummaryView::OnItemClicked() {
	LOG0(3, "CSummaryView::OnItemClicked()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CSiteItem *si = (CSiteItem *) GetItemData(hItem);
	if (si != NULL && (si->Type == CSiteItem::Site || si->Type == CSiteItem::VFolder)) {
		int nSite;
		if (si == &UnreadItems) nSite = SITE_UNREAD;
		else if (si == &FlaggedItems) nSite = SITE_FLAGGED;
		else nSite = SiteList.GetIndexOf(si);

		Config.ActSiteIdx = nSite;
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		frame->SwitchView(CMainFrame::FeedView);
		frame->SelectSite(nSite);
	}
}

void CSummaryView::ContextMenu(CPoint *pt) {
	LOG0(3, "CSummaryView::ContextMenu()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CSiteItem *si = (CSiteItem *) GetItemData(hItem);
	if (si != NULL) {
		CMenu mnu;
		mnu.LoadMenu(IDR_SUMMARY_VIEW_CONTEXT_MENU);
		CMenu *popup = mnu.GetSubMenu(0);

		popup->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN, pt->x, pt->y, AfxGetMainWnd());
	}
}

void CSummaryView::OnUpdateChannel() {
	LOG0(3, "CSummaryView::OnUpdateChannel()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;

	CSiteItem *si = (CSiteItem *) GetItemData(hItem);
	if (si == NULL) return;

	CList<CSiteItem *, CSiteItem *> sites;
	si->GetSites(sites);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->m_wndUpdateBar.EnqueueSites(sites);
	frame->m_wndUpdateBar.Redraw();
	frame->m_wndUpdateBar.Start();
}

void CSummaryView::OnUpdateUpdateChannel(CCmdUI *pCmdUI) {
	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CSiteItem *si = (CSiteItem *) GetItemData(hItem);
	pCmdUI->Enable(si != NULL && si->Type != CSiteItem::VFolder);
}

void CSummaryView::SetReadState(CSiteItem *si, DWORD state) {
	LOG0(5, "CSummaryView::SetReadState()");

	if (si == NULL)
		return;

	si->EnsureSiteLoaded();
	CFeed *feed = si->Feed;
	if (feed != NULL) {
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		for (int i = 0; i < feed->GetItemCount(); i++) {
			CFeedItem *fi = feed->GetItem(i);
			fi->SetFlags(state, MESSAGE_READ_STATE);
			frame->SyncItem(fi);
		}

		frame->AddSiteToSave(si);
	}
}

void CSummaryView::OnToolsMarkItemsRead() {
	LOG0(3, "CSummaryView::OnToolsMarkItemsRead()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CWaitCursor wait;
	CSiteItem *sitem = (CSiteItem *) GetItemData(hItem);
	if (sitem != NULL) {
		if (sitem->Type == CSiteItem::VFolder) {
			for (int i = 0; i < SiteList.GetCount(); i++) {
				CSiteItem *si = SiteList.GetAt(i);
				if (si->GetUnreadCount() > 0)
					SetReadState(si, MESSAGE_READ);
			}
		}
		else if (sitem->Type == CSiteItem::Group) {
			POSITION pos = sitem->SubItems.GetHeadPosition();
			while (pos != NULL) {
				CSiteItem *si = sitem->SubItems.GetNext(pos);
				SetReadState(si, MESSAGE_READ);
			}
		}
		else {
			SetReadState(sitem, MESSAGE_READ);
		}

		sitem->UpdateCachedCounts();
		Invalidate(FALSE);
	}
}

void CSummaryView::OnToolsMarkItemsUnread() {
	LOG0(3, "CSummaryView::OnToolsMarkItemsUnread()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CSiteItem *sitem = (CSiteItem *) GetItemData(hItem);
	if (sitem != NULL) {
		if (sitem->Type == CSiteItem::VFolder) {
			for (int i = 0; i < SiteList.GetCount(); i++) {
				CSiteItem *si = SiteList.GetAt(i);
				SetReadState(si, MESSAGE_UNREAD);
			}
		}
		else if (sitem->Type == CSiteItem::Group) {
			POSITION pos = sitem->SubItems.GetHeadPosition();
			while (pos != NULL) {
				CSiteItem *si = sitem->SubItems.GetNext(pos);
				SetReadState(si, MESSAGE_UNREAD);
			}
		}
		else {
			SetReadState(sitem, MESSAGE_UNREAD);
		}

		sitem->UpdateCachedCounts();
		Invalidate(FALSE);
	}
}

void CSummaryView::OnFileProperies() {
	LOG0(3, "CSummaryView::OnFileProperies()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CSiteItem *si = (CSiteItem *) GetItemData(hItem);
	if (si != NULL && si->Type == CSiteItem::Site) {
		if (SiteProperties(si, this)) {
			SetItemText(hItem, si->Name);
			InvalidateItem(hItem);
			// save changes
			SaveSiteItem(si, SiteList.GetIndexOf(si));
		}
	}
}

void CSummaryView::OnUpdateFileProperies(CCmdUI *pCmdUI) {
	LOG0(5, "CSummaryView::OnUpdateFileProperies()");

	HGROUPITEM hItem = GetSelectedItem();
	if (hItem == NULL) return;
	CSiteItem *si = (CSiteItem *) GetItemData(hItem);
	pCmdUI->Enable(si != NULL && si->Type == CSiteItem::Site);
}

void CSummaryView::OnViewHideGroups() {
	Config.HideGroups = !Config.HideGroups;
	Config.SaveUI();

	SetRedraw(FALSE);
	DeleteAllItems();
	InsertSites(&SiteList);
	SetRedraw(TRUE);

	AdjustViewTop();
	UpdateScrollBars();

	Invalidate();
}

void CSummaryView::OnUpdateViewHideGroups(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(Config.HideGroups);
}

void CSummaryView::OnSiteManLink() {
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_TOOLS_SITEMANAGER);
}

void CSummaryView::OnSize(UINT nType, int cx, int cy) {
	CGroupView::OnSize(nType, cx, cy);

	int height = GetSystemMetrics(SM_CYSCREEN);
	int width = GetSystemMetrics(SM_CXSCREEN);
	if (::IsWindow(m_ctlSiteManLink.GetSafeHwnd()))
		m_ctlSiteManLink.SetWindowPos(NULL, 0, height - SCALEY(54) - SCALEY(20), width, SCALEY(20), SWP_NOZORDER);
}

