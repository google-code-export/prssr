/**
 *  SiteManagerDlg.cpp
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
#include "SiteManagerDlg.h"

#include "Config.h"
#include "Appearance.h"
#include "Errors.h"

#include "AddFeedDlg.h"
#include "SearchDlg.h"
#include "SearchResultsDlg.h"
#include "ImportFeedsDlg.h"

#include "Properties.h"

#include "ctrls/CePropertySheet.h"
#include "ctrls/BrowseDlg.h"

#include "xml/FeedFile.h"
#include "xml/OpmlFile.h"

#include "misc.h"
#include "../share/fs.h"

#include "ProgressDlg.h"
#include "net/Download.h"
#include "net/Connection.h"
#include "sync/NetworkSync.h"
#include "sync/GReaderSync.h"

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

// OPML file name
#define OPML_FILENAME						_T("myChannels.opml")


/////////////////////////////////////////////////////////////////////////////
// CSiteTreeCtrl control

CSiteTreeCtrl::CSiteTreeCtrl(UINT nID) :
	CCeTreeCtrl(nID)
{
}

BEGIN_MESSAGE_MAP(CSiteTreeCtrl, CCeTreeCtrl)
	//{{AFX_MSG_MAP(CSiteTreeCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSiteTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	// Select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
		SelectItem(hItem);

	// The code below fixes wrong MFC behaviour on Pocket PC 2002 devices
	// (running circles appears twice while you tap and hold
	Default();
}

/////////////////////////////////////////////////////////////////////////////
// CExportDlg control

class CExportDlg : public CBrowseDlg {
public:
	CExportDlg(CWnd *pParent = NULL);

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	afx_msg void OnExport();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

CExportDlg::CExportDlg(CWnd *pParent/* = NULL*/) : CBrowseDlg(FALSE, pParent) {
	m_strCaption.LoadString(IDS_EXPORT_OPML_FILE);
	m_strPath = _T("\\");

	m_nMenuID = IDR_EXPORT_OPML;
}

BEGIN_MESSAGE_MAP(CExportDlg, CBrowseDlg)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_COMMAND(ID_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExportDlg::OnExport() {
	if (UpdateData())
		EndDialog(IDOK);
}


/////////////////////////////////////////////////////////////////////////////
// CSyncProgressDlg dialog

class CSyncProgressDlg : public CProgressDlg {
public:
	CSyncProgressDlg(CDownloader *downloader, CDialog *parent);
	virtual ~CSyncProgressDlg();

protected:
	UINT Timer;
	UINT ProgressRefreshTimer;
	EDownloadState State;
	CDownloader *Downloader;
	CDialog *Parent;

	int RangeHi, PosOffset;

    //{{AFX_MSG(CSyncProgressDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

CSyncProgressDlg::CSyncProgressDlg(CDownloader *downloader, CDialog *parent) {
	Downloader = downloader;
	State = DOWNLOAD_STATE_NONE;
	Timer = 1;
	ProgressRefreshTimer = 2;

	Parent = parent;
	RangeHi = 160000;
	PosOffset = 0;
}

CSyncProgressDlg::~CSyncProgressDlg() {
}

BEGIN_MESSAGE_MAP(CSyncProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CSyncProgressDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSyncProgressDlg::OnInitDialog() {
	LOG0(3, "CSyncProgressDlg::OnInitDialog()");

	SetTimer(Timer, 500, NULL);
	SetTimer(ProgressRefreshTimer, 700, NULL);
	CProgressDlg::OnInitDialog();

	SetRange(0, RangeHi);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSyncProgressDlg::OnDestroy() {
	KillTimer(Timer);
	KillTimer(ProgressRefreshTimer);

	CProgressDlg::OnDestroy();
}

void CSyncProgressDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == Timer) {
	}
	else if (nIDEvent == ProgressRefreshTimer) {
		if (Downloader != NULL) {
			int newPos = PosOffset + (int) Downloader->GetDownloadedFileSize();
			if (newPos > RangeHi) {
				RangeHi *= 10;
				SetRange(0, RangeHi);
			}
			SetPos(newPos);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CSyncProgressDlg::OnCancel() {
	LOG0(3, "CSyncProgressDlg::OnCancel()");

	KillTimer(ProgressRefreshTimer);
	KillTimer(Timer);

	if (Downloader != NULL)
		Downloader->Terminate();

	CProgressDlg::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CSiteManagerDlg dialog

#define ID_MOVE_TO_GROUP_BASE				0xA000

#define SITE_ICON							0
//#define GROUP_ICON							6


CSiteManagerDlg::CSiteManagerDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(InWideMode() ? CSiteManagerDlg::IDD_WIDE : CSiteManagerDlg::IDD, pParent),
	m_ctlSites(IDS_NO_FEEDS_DEFINED)
{
	//{{AFX_DATA_INIT(CSiteManagerDlg)
	//}}AFX_DATA_INIT
	m_bAddingGroup = FALSE;
	Root = NULL;
	m_nMenuID = IDR_SITE_MANAGER;
//	m_strCaption.LoadString(IDS_SITE_MANAGER);
	ShowNewChannelsOnToday = CONFIG_DEFAULT_SHOWNEWCHANNELS;

	Syncer = NULL;
}

CSiteManagerDlg::~CSiteManagerDlg() {
}

void CSiteManagerDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CSiteManagerDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteManagerDlg)
	DDX_Control(pDX, IDC_SITES, m_ctlSites);
	DDX_Control(pDX, IDC_HRULE1, m_ctlHrule);
	DDX_Control(pDX, IDC_MOVE_UP, m_ctlMoveUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_ctlMoveDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSiteManagerDlg, CCeDialog)
	//{{AFX_MSG_MAP(CSiteManagerDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SITES, OnSelchangedSites)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_SITES, OnEndlabeleditSites)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ADD_FEED, OnAddFeed)
	ON_COMMAND(ID_ADD_GROUP, OnAddGroup)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_SITE_EDIT, OnEdit)
	ON_COMMAND(ID_SEARCH_FOR_FEEDS, OnSearch)
	ON_COMMAND(ID_IMPORT_OPML_FILE, OnImportOpml)
	ON_COMMAND(ID_EXPORT_OPML_FILE, OnExportOpml)
	ON_COMMAND(ID_SYNC_SUBSCRIPTIONS, OnSyncSubscriptions)
	ON_UPDATE_COMMAND_UI(ID_SYNC_SUBSCRIPTIONS, OnUpdateSyncSubscriptions)

	ON_COMMAND(IDCANCEL, OnCancel)
	ON_NOTIFY(GN_CONTEXTMENU, IDC_SITES, OnContextMenu)
	ON_COMMAND_RANGE(ID_MOVE_TO_GROUP_BASE, ID_MOVE_TO_GROUP_BASE + 1000, OnMoveToGroup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiteManagerDlg message handlers

BOOL CSiteManagerDlg::OnInitDialog() {
	LOG0(3, "CSiteManagerDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	if (InWideMode())
		m_ctlHrule.SetFrame(HR_LEFT);
	else
		m_ctlHrule.SetFrame(HR_TOP);

	// icons
	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_oImageList.Create(IDB_SITE_ITEMS, SCALEX(16), 0, RGB(192, 192, 192));
	AfxSetResourceHandle(AfxGetInstanceHandle());

	// add folder icon
	SHFILEINFO sfi = { 0 };
	ZeroMemory(&sfi, sizeof(sfi));
	SHGetFileInfo(_T("\\"), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);
	m_nGroupIcon = m_oImageList.Add(sfi.hIcon);

	m_ctlSites.SetImageList(&m_oImageList, TVSIL_NORMAL);

	AddItem(TVI_ROOT, Root);

	m_ctlSites.Expand(m_ctlSites.GetRootItem(), TVE_EXPAND);	// root must be always expanded

	SetForegroundWindow();
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSiteManagerDlg::UpdateControls() {
	HTREEITEM selItem = m_ctlSites.GetSelectedItem();

	m_ctlMoveDown.EnableWindow(selItem != NULL && selItem != m_ctlSites.GetRootItem());
	m_ctlMoveUp.EnableWindow(selItem != NULL && selItem != m_ctlSites.GetRootItem());
}

HTREEITEM CSiteManagerDlg::GetGroupItem() {
	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	HTREEITEM hParent;
	if (hSelItem != NULL) {
		CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);
		if (si->Type == CSiteItem::Group)
			hParent = hSelItem;
		else
			hParent = m_ctlSites.GetParentItem(hSelItem);
	}
	else {
		hParent = m_ctlSites.GetRootItem();
	}

	return hParent;
}

HTREEITEM CSiteManagerDlg::InsertSite(HTREEITEM hParent, CSiteItem *item, HTREEITEM hInsertAfter/* = TVI_LAST*/) {
	LOG0(5, "CSiteManagerDlg::InsertSite()");

	HTREEITEM hItem = m_ctlSites.InsertItem(item->Name, SITE_ICON, SITE_ICON, hParent, hInsertAfter);
	m_ctlSites.SetItemData(hItem, (DWORD) item);

	MapUrlToTreeItem.SetAt(item->Info->XmlUrl, hItem);

	return hItem;
}

HTREEITEM CSiteManagerDlg::InsertGroup(HTREEITEM hParent, CSiteItem *group, HTREEITEM hInsertAfter/* = TVI_LAST*/) {
	LOG0(5, "CSiteManagerDlg::InsertGroup()");

	CString strName = group->Name;
	if (hParent == TVI_ROOT)
		strName.LoadString(IDS_ROOT_GROUP);

	HTREEITEM hItem = m_ctlSites.InsertItem(strName, m_nGroupIcon, m_nGroupIcon, hParent, hInsertAfter);
	m_ctlSites.SetItemData(hItem, (DWORD) group);

	MapGroupToTreeItem.SetAt(strName, hItem);

	return hItem;
}

void CSiteManagerDlg::OnSelchangedSites(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	UpdateControls();

	*pResult = 0;
}

HTREEITEM CSiteManagerDlg::AddItem(HTREEITEM hParent, CSiteItem *item, HTREEITEM hInsertAfter/* = TVI_LAST*/) {
	LOG0(5, "CSiteManagerDlg::AddItem()");

	HTREEITEM hItem;
	if (item->Type == CSiteItem::Site) {
		hItem = InsertSite(hParent, item, hInsertAfter);
	}
	else if (item->Type == CSiteItem::Group) {
		hItem = InsertGroup(hParent, item, hInsertAfter);

		POSITION pos = item->SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = item->SubItems.GetNext(pos);
			AddItem(hItem, si);
		}
	}

	return hItem;
}

HTREEITEM CSiteManagerDlg::MergeItem(HTREEITEM hParent, CSiteItem *item, HTREEITEM hInsertAfter/* = TVI_LAST*/) {
	LOG0(5, "CSiteManagerDlg::MergeItem()");

	CSiteItem *parentGroup = (CSiteItem *) m_ctlSites.GetItemData(hParent);

	HTREEITEM hItem;
	if (item->Type == CSiteItem::Site) {
		void *ptr;
		if (!MapUrlToTreeItem.Lookup(item->Info->XmlUrl, ptr)) {
			hItem = InsertSite(hParent, item, hInsertAfter);
		}
		else {
			item->Destroy();
			delete item;
		}
	}
	else if (item->Type == CSiteItem::Group) {
		void *ptr = NULL;
		if (!MapGroupToTreeItem.Lookup(item->Name, ptr))
			hItem = InsertGroup(hParent, item, hInsertAfter);
		else
			hItem = (HTREEITEM) ptr;

		POSITION pos = item->SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = item->SubItems.GetNext(pos);
			MergeItem(hItem, si);
		}

		if (hItem == (HTREEITEM) ptr)
			delete item;				// group already in the list, delete the duplicate
	}

	return hItem;
}

void CSiteManagerDlg::OnEndlabeleditSites(NMHDR* pNMHDR, LRESULT* pResult) {
	LOG0(5, "CSiteManagerDlg::OnEndlabeleditSites()");

	TV_DISPINFO *pTVDispInfo = (TV_DISPINFO*) pNMHDR;

	if (m_bAddingGroup) {
		HTREEITEM hItem = pTVDispInfo->item.hItem;
		if (pTVDispInfo->item.pszText != NULL &&
			lstrlen(pTVDispInfo->item.pszText) > 0)
		{
			m_ctlSites.SetItemText(hItem, pTVDispInfo->item.pszText);
			CSiteItem *group = (CSiteItem *) pTVDispInfo->item.lParam;
			// set group name
			MapGroupToTreeItem.RemoveKey(group->Name);
			group->Name = pTVDispInfo->item.pszText;
			MapGroupToTreeItem.SetAt(group->Name, hItem);

			*pResult = 0;
		}
		else {
			// delete temporary group (no childs, ok to delete)
			CSiteItem *group = (CSiteItem *) pTVDispInfo->item.lParam;
			DeleteItem(hItem, TRUE);
			*pResult = FALSE;
		}
	}
	else {
		if (pTVDispInfo->item.pszText != NULL &&
			lstrlen(pTVDispInfo->item.pszText) > 0)
		{
			// editing group name ends
			m_ctlSites.SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);
			CSiteItem *item = (CSiteItem *) pTVDispInfo->item.lParam;
			// set group name
			item->Name = pTVDispInfo->item.pszText;
			*pResult = 0;
		}
		else {
			*pResult = FALSE;
		}
	}

	m_ctlSites.ModifyStyle(TVS_EDITLABELS, 0);

//	*pResult = 0;
}

void CSiteManagerDlg::AppendMoveToMenu(CMenu &mnu, HTREEITEM hParent) {
	HTREEITEM hChild = m_ctlSites.GetChildItem(hParent);
	while (hChild != NULL) {
		CSiteItem *item = (CSiteItem *) m_ctlSites.GetItemData(hChild);
		if (item->Type == CSiteItem::Group) {
			CString sMenuItem = item->Name;

			CMenu mnuChild;
			mnuChild.CreatePopupMenu();
			AppendMoveToMenu(mnuChild, hChild);
			int idx = CtxMenuMoveToIDs.GetSize();
			if (mnuChild.GetMenuItemCount() > 0) {
				CString sHere;
				sHere.LoadString(IDS_HERE);
				mnuChild.InsertMenu(0, MF_STRING | MF_BYPOSITION, ID_MOVE_TO_GROUP_BASE + idx, sHere);
				mnuChild.InsertMenu(1, MF_SEPARATOR | MF_BYPOSITION);

				mnu.AppendMenu(MF_ENABLED | MF_POPUP, (UINT) mnuChild.GetSafeHmenu(), sMenuItem);
				mnuChild.Detach();
			}
			else {
				mnu.AppendMenu(MF_STRING, ID_MOVE_TO_GROUP_BASE + idx, sMenuItem);
			}

			CtxMenuMoveToIDs.Add(hChild);
		}

		hChild = m_ctlSites.GetNextSiblingItem(hChild);
	}
}

void CSiteManagerDlg::CreateMoveToMenu(CMenu &mnuMove) {
	CtxMenuMoveToIDs.RemoveAll();

	// root
	CString sRootGroup;
	sRootGroup.LoadString(IDS_ROOT_GROUP);

	mnuMove.AppendMenu(MF_STRING, ID_MOVE_TO_GROUP_BASE, sRootGroup);
	CtxMenuMoveToIDs.Add(m_ctlSites.GetRootItem());
	mnuMove.AppendMenu(MF_SEPARATOR);
	// add groups
	AppendMoveToMenu(mnuMove, m_ctlSites.GetRootItem());
}

void CSiteManagerDlg::OnMoveToGroup(UINT nID) {
	int idx = nID - ID_MOVE_TO_GROUP_BASE;

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	CSiteItem *srcItem = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);

	HTREEITEM hDestItem = CtxMenuMoveToIDs[idx];
	if (srcItem->Type == CSiteItem::Group) {
		// do not move the group the the same group
		HTREEITEM hParent = hDestItem;
		while (hParent != NULL) {
			if (hParent == hSelItem)
				return;
			hParent = m_ctlSites.GetParentItem(hParent);
		}
	}
	else if (srcItem->Type == CSiteItem::Site) {
		// do not move the site to the parent group
		HTREEITEM hParent = m_ctlSites.GetParentItem(hSelItem);
		if (hParent == hDestItem)
			return;
	}

	CreateSiteList(hSelItem, NULL);
	DeleteItem(hSelItem, FALSE);
	AddItem(hDestItem, srcItem);

	UpdateControls();
}

void CSiteManagerDlg::OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(1, "CSiteManagerDlg::OnContextMenu()");

	// GN_CONTEXTMENU sends NMRGINFO structure through notify struct parameter
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	if (hSelItem == m_ctlSites.GetRootItem()) {
		CMenu mnu;
		mnu.CreatePopupMenu();
		AppendMenuFromResource(&mnu, IDR_SITE_MAN_CTX_MNU_ADD);
		mnu.TrackPopupMenu(TPM_LEFTALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);
	}
	else {
		CSiteItem *item = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);

		if (item->Type == CSiteItem::Site) {
			CMenu mnu;
			mnu.CreatePopupMenu();
			// edit part
			AppendMenuFromResource(&mnu, IDR_SITE_MAN_CTX_MNU_EDIT);
			mnu.AppendMenu(MF_SEPARATOR);
			// 'move to' part
			CMenu mnuMove;
			mnuMove.CreatePopupMenu();
			CString sMoveTo;
			sMoveTo.LoadString(IDS_MOVE_TO);
			CreateMoveToMenu(mnuMove);
			mnu.AppendMenu(MF_POPUP | MF_ENABLED | MF_STRING, (UINT) mnuMove.GetSafeHmenu(), sMoveTo);
			mnuMove.Detach();

			mnu.TrackPopupMenu(TPM_LEFTALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);
		}
		else if (item->Type == CSiteItem::Group) {
			CMenu mnu;
			mnu.CreatePopupMenu();
			// add part
			AppendMenuFromResource(&mnu, IDR_SITE_MAN_CTX_MNU_ADD);
//			mnu.AppendMenu(MF_SEPARATOR);
			// edit part
			AppendMenuFromResource(&mnu, IDR_SITE_MAN_CTX_MNU_EDIT);
			mnu.DeleteMenu(ID_SITE_EDIT, MF_BYCOMMAND);
			mnu.AppendMenu(MF_SEPARATOR);
			// 'move to' part
			CMenu mnuMove;
			mnuMove.CreatePopupMenu();
			CString sMoveTo;
			sMoveTo.LoadString(IDS_MOVE_TO);
			CreateMoveToMenu(mnuMove);
			mnu.AppendMenu(MF_POPUP | MF_ENABLED | MF_STRING, (UINT) mnuMove.GetSafeHmenu(), sMoveTo);
			mnuMove.Detach();

			mnu.TrackPopupMenu(TPM_LEFTALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);
		}
	}

	*pResult = TRUE; // This is important!
}

void CSiteManagerDlg::DeleteItem(HTREEITEM hItem, BOOL free/* = TRUE*/) {
	LOG0(1, "CSiteManagerDlg::DeleteItem()");

	HTREEITEM hChild = m_ctlSites.GetChildItem(hItem);
	while (hChild != NULL) {
		DeleteItem(hChild, free);
		hChild = m_ctlSites.GetChildItem(hItem);
	}

	CSiteItem *item = (CSiteItem *) m_ctlSites.GetItemData(hItem);
	HTREEITEM hSelItem = m_ctlSites.GetPrevVisibleItem(hItem);
	m_ctlSites.DeleteItem(hItem);

	if (item->Type == CSiteItem::Group)
		MapGroupToTreeItem.RemoveKey(item->Name);
	else if (item->Type == CSiteItem::Site && item->Info != NULL)
		MapUrlToTreeItem.RemoveKey(item->Info->XmlUrl);

	if (free) {
		if (item->Info != NULL) {
			DeleteFile(GetCachePath(FILE_TYPE_FEED, Config.CacheLocation) + _T("\\") + item->Info->FileName);
			DeleteFile(GetCacheFile(FILE_TYPE_FAVICON, Config.CacheLocation, item->Info->FileName));
		}

		item->Destroy();
		delete item;
	}

	m_ctlSites.SelectItem(hSelItem);
	m_ctlSites.EnsureVisible(hSelItem);
}

void CSiteManagerDlg::ResizeControls() {
	LOG0(3, "CSiteManagerDlg::ResizeControls()");

	if (InWideMode())
		m_ctlHrule.SetFrame(HR_LEFT);
	else
		m_ctlHrule.SetFrame(HR_TOP);

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (!InWideMode()) {
		CRect rc;
		GetClientRect(rc);

		m_ctlSites.SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height() - SCALEY(56), SWP_NOZORDER | SWP_NOMOVE);
		m_ctlHrule.SetWindowPos(NULL, 0, rc.Height() - SCALEY(32), rc.Width(), SCALEY(1), SWP_NOZORDER);
		m_ctlMoveDown.SetWindowPos(NULL, rc.right - SCALEX(75), rc.Height() - SCALEY(26), SCALEX(70), SCALEY(20), SWP_NOZORDER);
		m_ctlMoveUp.SetWindowPos(NULL, rc.right - SCALEX(75 + 80), rc.Height() - SCALEY(26), SCALEX(70), SCALEY(20), SWP_NOZORDER);
	}
}

void CSiteManagerDlg::CreateSiteList(HTREEITEM hItem, CSiteItem *parent/* = NULL*/) {
	CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(hItem);

	si->Parent = parent;
	si->SubItems.RemoveAll();

	HTREEITEM hChild = m_ctlSites.GetChildItem(hItem);
	while (hChild != NULL) {
		CSiteItem *child = (CSiteItem *) m_ctlSites.GetItemData(hChild);
		si->AddItem(child);
		CreateSiteList(hChild, si);

		hChild = m_ctlSites.GetNextSiblingItem(hChild);
	}
}

void CSiteManagerDlg::OnOk() {
	LOG0(1, "CSiteManagerDlg::OnOk()");

	// if we are in editing a group name, end editing
	if (m_ctlSites.GetEditControl() != NULL) {
		m_ctlSites.SendMessage(TVM_ENDEDITLABELNOW, FALSE);
		return;
	}

	HTREEITEM hRoot = m_ctlSites.GetRootItem();
	CreateSiteList(hRoot);
	Root = (CSiteItem *) m_ctlSites.GetItemData(hRoot);

	EndDialog(IDOK);
}

void CSiteManagerDlg::OnCancel() {
	LOG0(1, "CSiteManagerDlg::OnCancel()");

	// if we are editing a group name, end the editing
	if (m_ctlSites.GetEditControl() != NULL) {
		m_ctlSites.SendMessage(TVM_ENDEDITLABELNOW, TRUE);
		return;
	}

	HTREEITEM hRoot = m_ctlSites.GetRootItem();
	CreateSiteList(hRoot);
	Root = (CSiteItem *) m_ctlSites.GetItemData(hRoot);

	EndDialog(IDCANCEL);
}

void CSiteManagerDlg::OnAddFeed() {
	LOG0(1, "CSiteManagerDlg::OnAddFeed()");

	CAddFeedDlg dlgAdd(this);
	if (dlgAdd.DoModal() == IDOK) {
		HTREEITEM hParent = GetGroupItem();

		if (dlgAdd.m_bCheckOnline) {
			POSITION pos = dlgAdd.SiteItemsToAdd.GetHeadPosition();
			while (pos != NULL) {
				CSiteItem *si = dlgAdd.SiteItemsToAdd.GetNext(pos);
				// add site item to the list
				AddItem(hParent, si);

				// update subscription in syncer
				if (Syncer != NULL) {
					CWaitCursor wait;
					Syncer->AddSubscription(si->Info->XmlUrl, si->Name);
				}

			}
			m_ctlSites.Expand(hParent, TVE_EXPAND);
		}
		else {
			// prepare data structures
			CSiteItem *item = new CSiteItem(NULL, CSiteItem::Site);

			// add site offline
			CFeedInfo *info = new CFeedInfo();
			info->FileName = CFeedInfo::GenerateFileName(dlgAdd.m_strURL);
			info->XmlUrl= dlgAdd.m_strURL;
			info->TodayShow = ShowNewChannelsOnToday;

			item->Status = CSiteItem::Error;
			item->Name = dlgAdd.m_strName;
			item->Info = info;
			item->CheckFavIcon = TRUE;

			// add it to the list
			AddItem(hParent, item);
			m_ctlSites.Expand(hParent, TVE_EXPAND);
		}

//		if (Config.SetPropertiesAfterSiteAdded)
//			SetSiteProperties(item);
	}
}

void CSiteManagerDlg::OnAddGroup() {
	LOG0(1, "CSiteManagerDlg::OnAddGroup()");

	HTREEITEM hItem = m_ctlSites.GetSelectedItem();
	if (hItem == NULL) return;

	CSiteItem *parent = (CSiteItem *) m_ctlSites.GetItemData(hItem);
	if (parent->Type == CSiteItem::Site) {
		hItem = m_ctlSites.GetParentItem(hItem);
		parent = (CSiteItem *) m_ctlSites.GetItemData(hItem);
	}

	m_bAddingGroup = TRUE;
	m_ctlSites.ModifyStyle(0, TVS_EDITLABELS);

	m_ctlSites.Expand(hItem, TVE_EXPAND);
	HTREEITEM hNewItem = InsertGroup(hItem, new CSiteItem(parent, CSiteItem::Group), TVI_FIRST);
	m_ctlSites.Expand(hItem, TVE_EXPAND);

	m_ctlSites.EnsureVisible(hNewItem);
	m_ctlSites.EditLabel(hNewItem);
}

void CSiteManagerDlg::OnEdit() {
	LOG0(1, "CSiteManagerDlg::OnEdit()");

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	if (hSelItem == NULL || hSelItem == m_ctlSites.GetRootItem())
		return;

	CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);
	if (si->Type == CSiteItem::Site)
		SiteProperties(si, this);
}

void CSiteManagerDlg::OnRemove() {
	LOG0(1, "CSiteManagerDlg::OnRemove()");

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	if (hSelItem == NULL || hSelItem == m_ctlSites.GetRootItem())
		return;

	if (PrssrMessageBox(IDS_CONFIRM_OPERATION, IDS_DELETE_ITEMS, MB_YESNO | MB_ICONSTOP, IDS_DELETE) == IDYES) {
		CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);

		// update subscription in syncer
		if (Syncer != NULL && Connection.IsAvailable() == S_OK) {
			CWaitCursor wait;
			Syncer->RemoveSubscription(si->Info->XmlUrl);
		}

		DeleteItem(hSelItem, TRUE);
		UpdateControls();
	}
}

void CSiteManagerDlg::OnRename() {
	LOG0(1, "CSiteManagerDlg::OnRename()");

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	if (hSelItem == NULL || hSelItem == m_ctlSites.GetRootItem())
		return;

	CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);
	// edit name
	m_bAddingGroup = FALSE;
	m_ctlSites.ModifyStyle(0, TVS_EDITLABELS);
	m_ctlSites.EnsureVisible(hSelItem);
	m_ctlSites.EditLabel(hSelItem);
}

void CSiteManagerDlg::OnMoveDown() {
	LOG0(1, "CSiteManagerDlg::OnMoveDown()");

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	if (hSelItem == NULL) return;

	// find next item to insert
	HTREEITEM hNext = m_ctlSites.GetNextVisibleItem(hSelItem);
	HTREEITEM hParent = m_ctlSites.GetParentItem(hNext);
	while (hNext != NULL) {
		if (hParent == m_ctlSites.GetRootItem()) {
			break;
		}
		else if (hParent == hSelItem) {
			// try the next visible item
			hNext = m_ctlSites.GetNextVisibleItem(hNext);
			hParent = hNext;
		}
		hParent = m_ctlSites.GetParentItem(hParent);
	}

	if (hNext != NULL && hParent == m_ctlSites.GetRootItem()) {
		CSiteItem *item = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);

		// remove item
		CreateSiteList(hSelItem);
		DeleteItem(hSelItem, FALSE);

		// insert it back to the right place
		HTREEITEM hInsParent = m_ctlSites.GetParentItem(hNext);
		HTREEITEM hNewItem = AddItem(hInsParent, item, hNext);
		m_ctlSites.EnsureVisible(hNewItem);
		m_ctlSites.SelectItem(hNewItem);
	}
}

void CSiteManagerDlg::OnMoveUp() {
	LOG0(1, "CSiteManagerDlg::OnMoveUp()");

	HTREEITEM hSelItem = m_ctlSites.GetSelectedItem();
	if (hSelItem == NULL) return;

	HTREEITEM hParent = m_ctlSites.GetParentItem(hSelItem);
	HTREEITEM hPrev = m_ctlSites.GetPrevSiblingItem(hSelItem);

	if (hParent != NULL) {
		CSiteItem *parent = (CSiteItem *) m_ctlSites.GetItemData(hParent);
		CSiteItem *item = (CSiteItem *) m_ctlSites.GetItemData(hSelItem);

		if (hPrev == NULL) {
			HTREEITEM hPrevParent = m_ctlSites.GetParentItem(hParent);
			if (hPrevParent != NULL) {
				// remove item
				CreateSiteList(hSelItem);
				DeleteItem(hSelItem, FALSE);

				// insert
				HTREEITEM hAfterItem = m_ctlSites.GetPrevSiblingItem(hParent);
				HTREEITEM hNewItem;
				if (hAfterItem == NULL)
					hNewItem = AddItem(hPrevParent, item, TVI_FIRST);
				else
					hNewItem = AddItem(hPrevParent, item, hAfterItem);
				m_ctlSites.EnsureVisible(hNewItem);
				m_ctlSites.SelectItem(hNewItem);
			}
		}
		else {
			// remove item
			CreateSiteList(hSelItem);
			DeleteItem(hSelItem, FALSE);

			// insert
			HTREEITEM hAfterItem = m_ctlSites.GetPrevSiblingItem(hPrev);
			if (hAfterItem == NULL)
				hAfterItem = TVI_FIRST;

			HTREEITEM hNewItem = AddItem(hParent, item, hAfterItem);
			m_ctlSites.EnsureVisible(hNewItem);
			m_ctlSites.SelectItem(hNewItem);
		}
	}
}

void CSiteManagerDlg::OnSearch() {
	LOG0(1, "CSiteManagerDlg::OnSearch()");

	CList<CSearchResultItem *, CSearchResultItem *> SearchResults;
	CSearchDlg dlgSearch(this);
	dlgSearch.SearchResults = &SearchResults;
	if (dlgSearch.DoModal() == IDOK) {
		// display results in dialog
		CSearchResultsDlg dlgResults(this);
		dlgResults.SearchResults = &SearchResults;
		if (SearchResults.GetCount() > 0)
			dlgResults.SetMenu(IDR_ADD);
		else
			dlgResults.SetMenu(IDR_CANCEL);

		if (dlgResults.DoModal() == IDOK) {
			HTREEITEM hParent = m_ctlSites.GetRootItem();
			CSiteItem *parent = (CSiteItem *) m_ctlSites.GetItemData(hParent);

			// setup the feed info for sites added
			POSITION pos = SearchResults.GetHeadPosition();
			while (pos != NULL) {
				CSearchResultItem *sri = SearchResults.GetNext(pos);

				// prepare data structures
				CSiteItem *item = new CSiteItem(parent, CSiteItem::Site);
				item->Status = CSiteItem::Error;
				item->Name = sri->SiteName;
				item->Info = new CFeedInfo();
				item->Info->XmlUrl = sri->XMLURL;
				item->Info->FileName = CFeedInfo::GenerateFileName(sri->XMLURL);
				item->Info->TodayShow = ShowNewChannelsOnToday;

				// add it to the list
				AddItem(hParent, item);
			}
		}

		// free search results
		while (!SearchResults.IsEmpty())
			delete SearchResults.RemoveHead();
	}
}

void CSiteManagerDlg::OnImportOpml() {
	LOG0(1, "CSiteManagerDlg::OnImport()");

	CImportDlg dlgImport(this);
	if (dlgImport.DoModal() == IDOK) {
		CWaitCursor *wait = new CWaitCursor();
		// load site list
		COpmlFile opml;
		BOOL gotOpml = FALSE;
		CSiteList siteListToImport;
		if (opml.LoadFromFile(dlgImport.m_strPath)) {
			CSiteItem *root = new CSiteItem(NULL, CSiteItem::Group);
			if (opml.Parse(root)) {
				siteListToImport.CreateFrom(root);
				siteListToImport.SetRoot(root);
				gotOpml = TRUE;
			}
			else
				delete root;
		}
		delete wait;

		if (gotOpml) {
			// select sites to import
			CImportFeedsDlg dlgFeeds;
			dlgFeeds.SiteList = &siteListToImport;

			if (dlgFeeds.DoModal() == IDOK) {
				CWaitCursor wait;

				// append/overwrite
				if (dlgFeeds.m_bAppendFeeds) {
					// append
					HTREEITEM hRoot = m_ctlSites.GetRootItem();

					// insert new items
					CSiteItem *root = siteListToImport.GetRoot();
					POSITION pos = root->SubItems.GetHeadPosition();
					while (pos != NULL) {
						CSiteItem *si = root->SubItems.GetNext(pos);
						MergeItem(hRoot, si);
					}

					delete root;		// free root node which was not added
				}
				else {
					// overwrite sites

					// free items
					CSiteItem *group = (CSiteItem *) m_ctlSites.GetItemData(m_ctlSites.GetRootItem());
					group->Destroy();
					delete group;

					// insert new items
					m_ctlSites.DeleteAllItems();
					MapGroupToTreeItem.RemoveAll();
					MapUrlToTreeItem.RemoveAll();

					AddItem(TVI_ROOT, siteListToImport.GetRoot());
					siteListToImport.Detach();
				}

				m_ctlSites.Expand(m_ctlSites.GetRootItem(), TVE_EXPAND);

				UpdateControls();
			}
		}
		else {
			Error(IDS_ERROR_LOADING_OPML);
		}
	}
}

void CSiteManagerDlg::OnExportOpml() {
	LOG0(1, "CSiteManagerDlg::OnExportOpml()");

	CExportDlg dlg(this);
	if (dlg.DoModal() == IDOK) {
		CString strDestinationFileName = GetCacheFile(FILE_TYPE_OPML, dlg.m_strPath, OPML_FILENAME);

		BOOL bExport = TRUE;
		if (FileExists(strDestinationFileName))
			bExport = PrssrMessageBox(IDS_CONFIRM_OPERATION, IDS_OVERWRITE_OPML, MB_YESNO | MB_ICONSTOP, IDS_OVERWRITE) == IDYES;

		if (bExport) {
			HTREEITEM hRoot = m_ctlSites.GetRootItem();
			CreateSiteList(hRoot);

			CSiteItem *root = (CSiteItem *) m_ctlSites.GetItemData(hRoot);
			CSiteList siteListToExport;
			siteListToExport.SetRoot(root);

			COpmlFile opml;
			if (opml.Export(strDestinationFileName, siteListToExport)) AfxMessageBox(IDS_EXPORT_OK);
			else Error(IDS_CANT_EXPORT_FILE);

			siteListToExport.Detach();
		}
	}
}

void CSiteManagerDlg::OnSyncSubscriptions() {
	LOG0(1, "CSiteManagerDlg::OnSyncSubscriptions()");

	if (Syncer != NULL) {
		CSyncProgressDlg progress(Syncer->Downloader, this);
		progress.OpenDialog(IDS_DOWNLOADING_FEED, this);

		BOOL bOK = FALSE;
		BOOL disconnect;
		if (CheckConnection(Config.AutoConnect, disconnect)) {
			CSiteList siteListToImport;
			if (Syncer->GetSubscriptions(siteListToImport)) {
				// append
				HTREEITEM hRoot = m_ctlSites.GetRootItem();

				// insert new items
				CSiteItem *root = siteListToImport.GetRoot();
				POSITION pos = root->SubItems.GetHeadPosition();
				while (pos != NULL) {
					CSiteItem *si = root->SubItems.GetNext(pos);
					MergeItem(hRoot, si);
				}

				delete root;		// free root node which was not added

				m_ctlSites.Expand(m_ctlSites.GetRootItem(), TVE_EXPAND);

				UpdateControls();
			}
			else {
				// TODO: report an error
			}

			if (disconnect)
				Connection.HangupConnection();
		}
		else {
			// error: can not connect to the Internet
		}

		progress.CloseDialog();
	}
}

void CSiteManagerDlg::OnUpdateSyncSubscriptions(CCmdUI *pCmdUI) {
	LOG0(3, "CSiteManagerDlg::OnUpdateSyncSubscriptions()");

	pCmdUI->Enable(Config.SyncSite != SYNC_SITE_NONE);
}
