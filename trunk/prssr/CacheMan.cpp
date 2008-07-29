/**
 *  CacheMan.cpp
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
#include "CacheMan.h"
#include "MainFrm.h"

#include "Config.h"
#include "Site.h"
//#include "ClearCacheDlg.h"
#include "../share/helpers.h"
#include "ProgressDlg.h"

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

//
// Cache Manager icons
//

// site
#define ICON_SITE						0
// html
#define ICON_HTML						1
#define ICON_HTML_CACHED				2
// enclosures
// sound
#define ICON_AUDIO						3
#define ICON_AUDIO_CACHED				4
// video
#define ICON_VIDEO						5
#define ICON_VIDEO_CACHED				6
// image
#define ICON_IMAGE						7
#define ICON_IMAGE_CACHED				8
// other
#define ICON_OTHER						9
#define ICON_OTHER_CACHED				10


///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI DeleteCacheThreadProc(LPVOID lpParameter) {
	CCacheManPg *dlg = (CCacheManPg *) lpParameter;
	return dlg->DeleteThread();
}

///////////////////////////////////////////////////////////////////////////////

class CDeleteCacheProgressDlg : public CProgressDlg {
public:
	CDeleteCacheProgressDlg(CCacheManPg *parent);
	virtual ~CDeleteCacheProgressDlg();

protected:
	CCacheManPg *Parent;

    //{{AFX_MSG(CDeleteCacheProgressDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	friend class CCacheManPg;
};

CDeleteCacheProgressDlg::CDeleteCacheProgressDlg(CCacheManPg *parent) {
	Parent = parent;
}

CDeleteCacheProgressDlg::~CDeleteCacheProgressDlg() {
}

BEGIN_MESSAGE_MAP(CDeleteCacheProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CDeleteCacheProgressDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDeleteCacheProgressDlg::OnInitDialog() {
	LOG0(3, "CDeleteCacheProgressDlg::OnInitDialog()");

	CProgressDlg::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteCacheProgressDlg::OnTimer(UINT nIDEvent) {

	CWnd::OnTimer(nIDEvent);
}

void CDeleteCacheProgressDlg::OnCancel() {
	LOG0(3, "CDeleteCacheProgressDlg::OnCancel()");

	CProgressDlg::OnCancel();
}


///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI PurgeCacheThreadProc(LPVOID lpParameter) {
	CCacheManPg *dlg = (CCacheManPg *) lpParameter;
	return dlg->PurgeThread();
}

///////////////////////////////////////////////////////////////////////////////

class CPurgeCacheProgressDlg : public CProgressDlg {
public:
	CPurgeCacheProgressDlg(CCacheManPg *parent);
	virtual ~CPurgeCacheProgressDlg();

protected:
	CCacheManPg *Parent;

    //{{AFX_MSG(CPurgeCacheProgressDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	friend class CCacheManPg;
};

CPurgeCacheProgressDlg::CPurgeCacheProgressDlg(CCacheManPg *parent) {
	Parent = parent;
}

CPurgeCacheProgressDlg::~CPurgeCacheProgressDlg() {
}

BEGIN_MESSAGE_MAP(CPurgeCacheProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CPurgeCacheProgressDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPurgeCacheProgressDlg::OnInitDialog() {
	LOG0(3, "CPurgeCacheProgressDlg::OnInitDialog()");

	CProgressDlg::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPurgeCacheProgressDlg::OnTimer(UINT nIDEvent) {

	CWnd::OnTimer(nIDEvent);
}

void CPurgeCacheProgressDlg::OnCancel() {
	LOG0(3, "CPurgeCacheProgressDlg::OnCancel()");

	CProgressDlg::OnCancel();
}

///////////////////////////////////////////////////////////////////////////////

CCacheTreeCtrl::CCacheTreeCtrl() {
}

BEGIN_MESSAGE_MAP(CCacheTreeCtrl, CCeTreeCtrl)
	//{{AFX_MSG_MAP(CCacheTreeCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCacheTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	LOG0(5, "CCacheTreeCtrl::OnLButtonDown()");

	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if (hItem != NULL && (uFlags & TVHT_ONITEMSTATEICON)) {
		int nImg, nSelImg;
		GetItemImage(hItem, nImg, nSelImg);

		// checked -> unchecked, unchecked -> checked
		BOOL check;
		if (GetCheck(hItem)) {
			// set the check for all childs
			check = FALSE;
		}
		else {
			// uncheck all childs
			check = TRUE;
		}

		if (nImg == ICON_SITE) {
			SetCheck(hItem, check);
			GetParent()->SendMessage(UWM_GROUP_CHECKED, (WPARAM) 0, (LPARAM) hItem);

			HTREEITEM hChild = GetChildItem(hItem);
			while (hChild != NULL) {
				SetCheck(hChild, check);
				hChild = GetNextSiblingItem(hChild);
			}
		}
		else {
			SetCheck(hItem, check);
		}

		return;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

CCacheManager::CCacheManager(CWnd *pParentWnd/* = NULL*/) :
	CCePropertySheet(IDS_CACHE_MANAGER, pParentWnd)
{
	LOG0(5, "CCacheManager::CCacheManager()");

	AddPage(&m_pgCache);
	AddPage(&m_pgEnclosures);
	SetMenu(IDR_CACHE_MANAGER);
}

CCacheManager::~CCacheManager() {
}

BEGIN_MESSAGE_MAP(CCacheManager, CCePropertySheet)
	//{{AFX_MSG_MAP(CCacheManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCacheManager::OnInitDialog() {
	LOG0(3, "CCacheManager::OnInitDialog()");

	BOOL bResult = CCePropertySheet::OnInitDialog();

	return bResult;
}

BOOL CCacheManager::OnCommand(WPARAM wParam, LPARAM lParam) {
	LOG0(3, "CCacheManager::OnCommand()");

	return CCePropertySheet::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////

CCacheManHtmlPg::CCacheManHtmlPg() : CCacheManPg(IDS_CACHE) {
	m_strLocation = GetCachePath(FILE_TYPE_HTML, Config.CacheLocation);
}

BOOL CCacheManHtmlPg::OnInitDialog() {
	CCacheManPg::OnInitDialog();

	return TRUE;
}

int CCacheManHtmlPg::GetItemImage(CFeedItem *fi) {
	LOG0(5, "CCacheManHtmlPg::GetItemImage()");

	int nImage;
	if (IsHTMLCached(fi->Link, TRUE)) {
		nImage = ICON_HTML_CACHED;
	}
	else {
		nImage = ICON_HTML;
	}

	return nImage;
}

void CCacheManHtmlPg::DoInsertFeedItem(HTREEITEM hParent, CFeedItem *fi) {
	LOG0(5, "CCacheManHtmlPg::DoInsertFeedItem()");

	int nImage = GetItemImage(fi);
	HTREEITEM hItem = m_ctlCacheItems.InsertItem(fi->Title, nImage, nImage, hParent);
	m_ctlCacheItems.SetItemData(hItem, (DWORD) fi);
}

void CCacheManHtmlPg::DoInsertDelete(HTREEITEM hItem) {
	LOG0(3, "CCacheManHtmlPg::DoInsertDelete()");

	CFeedItem *fi = (CFeedItem *) m_ctlCacheItems.GetItemData(hItem);
	if (fi != NULL && IsHTMLCached(fi->Link, TRUE)) {
		CDeleteItem *di = new CDeleteItem();
		di->FileName = GetCacheFile(FILE_TYPE_HTML, Config.CacheLocation, fi->Link);
		di->HItem = hItem;
		DeleteFileList.AddTail(di);

		FileType = FILE_TYPE_HTML;
	}
}

void CCacheManHtmlPg::DoCacheItems() {
	CMainFrame *pMainFrm = (CMainFrame *) AfxGetMainWnd();
	pMainFrm->m_wndUpdateBar.EnqueueImages(CacheList);
	pMainFrm->m_wndUpdateBar.EnqueueHtmls(CacheList);
	pMainFrm->m_wndUpdateBar.Start();
}

void CCacheManHtmlPg::DoPurgeCache() {
	PurgePath = GetCachePath(FILE_TYPE_HTML, Config.CacheLocation);
}

/////////////////////////////////////////////////////////////////////////////

CCacheManEnclosuresPg::CCacheManEnclosuresPg() : CCacheManPg(IDS_ENCLOSURES) {
	m_strLocation = GetCachePath(FILE_TYPE_ENCLOSURE, Config.CacheLocation);
}

BOOL CCacheManEnclosuresPg::OnInitDialog() {
	CCacheManPg::OnInitDialog();

	return TRUE;
}

int CCacheManEnclosuresPg::GetItemImage(CFeedItem *fi) {
	LOG0(5, "CCacheManEnclosuresPg::GetItemImage()");

	CEnclosureItem *ei = fi->Enclosures.GetHead();

	int nImage;
	switch (ei->Type) {
		case ENCLOSURE_TYPE_AUDIO: nImage = ICON_AUDIO; break;
		case ENCLOSURE_TYPE_VIDEO: nImage = ICON_VIDEO; break;
		case ENCLOSURE_TYPE_IMAGE: nImage = ICON_IMAGE; break;
		default: nImage = ICON_OTHER; break;
	}

	if (IsEnclosureCached(ei->URL)) {
		nImage++;
	}

	return nImage;
}

void CCacheManEnclosuresPg::DoInsertFeedItem(HTREEITEM hParent, CFeedItem *fi) {
	LOG0(5, "CCacheManEnclosuresPg::DoInsertFeedItem()");

	if (fi->HasEnclosure()) {
		int nImage = GetItemImage(fi);
		HTREEITEM hItem = m_ctlCacheItems.InsertItem(fi->Title, nImage, nImage, hParent);
		m_ctlCacheItems.SetItemData(hItem, (DWORD) fi);
	}
}

void CCacheManEnclosuresPg::DoInsertDelete(HTREEITEM hItem) {
	LOG0(3, "CCacheManEnclosuresPg::DoInsertDelete()");

	CFeedItem *fi = (CFeedItem *) m_ctlCacheItems.GetItemData(hItem);
	if (fi != NULL && fi->HasEnclosure()) {
		CEnclosureItem *ei = fi->Enclosures.GetHead();

		if (IsEnclosureCached(ei->URL)) {
			CDeleteItem *di = new CDeleteItem();
			di->FileName = GetCacheFile(FILE_TYPE_ENCLOSURE, Config.CacheLocation, ei->URL);
			di->HItem = hItem;
			DeleteFileList.AddTail(di);
		}
		FileType = FILE_TYPE_ENCLOSURE;
	}
}

void CCacheManEnclosuresPg::DoCacheItems() {
	CMainFrame *pMainFrm = (CMainFrame *) AfxGetMainWnd();
	pMainFrm->m_wndUpdateBar.EnqueueEnclosures(CacheList);
	pMainFrm->m_wndUpdateBar.Start();
}

void CCacheManEnclosuresPg::DoPurgeCache() {
	PurgePath = GetCachePath(FILE_TYPE_ENCLOSURE, Config.CacheLocation);
}

/////////////////////////////////////////////////////////////////////////////
// CCacheManPg property page

IMPLEMENT_DYNCREATE(CCacheManPg, CCePropertyPage)

CCacheManPg::CCacheManPg()
	: CCePropertyPage(InWideMode() ? CCacheManPg::IDD_WIDE : CCacheManPg::IDD) {
}

CCacheManPg::CCacheManPg(UINT nIDCaption)
	: CCePropertyPage(InWideMode() ? CCacheManPg::IDD_WIDE : CCacheManPg::IDD, nIDCaption)
{
	//{{AFX_DATA_INIT(CCacheManPg)
	//}}AFX_DATA_INIT
	m_pProgress = NULL;
}


CCacheManPg::~CCacheManPg()
{
}


void CCacheManPg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CCacheManPg::DoDataExchange()");

	CCePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCacheManPg)
	DDX_Control(pDX, IDC_CACHEITEMS, m_ctlCacheItems);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCacheManPg, CCePropertyPage)
	//{{AFX_MSG_MAP(CCacheManPg)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_CACHEITEMS, OnItemExpanding)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_COMMAND(ID_CACHE, OnCache)
	ON_COMMAND(ID_PURGE_CACHE, OnPurgeCache)
	ON_COMMAND(ID_SELECT_ALL, OnSelectAll)
	ON_MESSAGE(UWM_GROUP_CHECKED, OnGroupChecked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCacheManPg message handlers

BOOL CCacheManPg::OnInitDialog() {
	LOG0(3, "CCacheManPg::OnInitDialog()");

	CCePropertyPage::OnInitDialog();

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_CACHE_ITEMS, SCALEX(16), 0, RGB(255, 0, 255));
	AfxSetResourceHandle(AfxGetInstanceHandle());
	m_ctlCacheItems.SetImageList(&m_ilIcons, TVSIL_NORMAL);

	InsertItems();

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCacheManPg::UpdateControls() {

}

void CCacheManPg::DoInsertFeedItem(HTREEITEM hParent, CFeedItem *fi) {
}

void CCacheManPg::DoInsertDelete(HTREEITEM hItem) {
}

void CCacheManPg::DoInsertCache(HTREEITEM hItem) {
	CFeedItem *fi = (CFeedItem *) m_ctlCacheItems.GetItemData(hItem);
	if (fi != NULL)
		CacheList.Add(fi);
}

void CCacheManPg::DoCacheItems() {
}

int CCacheManPg::GetItemImage(CFeedItem *fi) {
	return 0;
}

void CCacheManPg::InsertItems() {
	LOG0(5, "CCacheManPg::InsertItems()");

	// iterate through sites
	for (int siteIdx = 0; siteIdx < SiteList.GetCount(); siteIdx++) {
		CSiteItem *si = SiteList.GetAt(siteIdx);

		HTREEITEM hItem = m_ctlCacheItems.InsertItem(si->Name, ICON_SITE, ICON_SITE);
		m_ctlCacheItems.SetItemData(hItem, (DWORD) si);

		// insert temporary item
		HTREEITEM hChild = m_ctlCacheItems.InsertItem(_T("No items"), hItem);
		m_ctlCacheItems.SetItemData(hChild, (DWORD) NULL);
	}

}

void CCacheManPg::OnOK() {
	LOG0(3, "CCacheManPg::OnOK()");
}

LRESULT CCacheManPg::OnGroupChecked(WPARAM wParam, LPARAM lParam) {
	LOG0(5, "CCacheManPg::OnGroupChecked()");

	LoadSite((HTREEITEM) lParam);

	return 0;
}

void CCacheManPg::ResizeControls() {
	LOG0(5, "CCacheManPg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	CRect rc;
	GetClientRect(&rc);

	m_ctlCacheItems.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
}

void CCacheManPg::LoadSite(HTREEITEM hItem) {
	LOG0(5, "CCacheManPg::LoadSite()");

	HTREEITEM hChild = m_ctlCacheItems.GetChildItem(hItem);
	if (hChild != NULL && m_ctlCacheItems.GetItemData(hChild) == NULL) {
		CSiteItem *si = (CSiteItem *) m_ctlCacheItems.GetItemData(hItem);
		si->EnsureSiteLoaded();

		// delete temporary child item
		m_ctlCacheItems.DeleteItem(m_ctlCacheItems.GetChildItem(hItem));

		if (si->Feed != NULL) {
			// insert feed items
			CFeed *feed = si->Feed;
			for (int fiIdx = 0; fiIdx < feed->GetItemCount(); fiIdx++) {
				CFeedItem *fi = feed->GetItem(fiIdx);
				DoInsertFeedItem(hItem, fi);
			}
		}
	}
}

void CCacheManPg::OnItemExpanding(NMHDR *pnmh, LRESULT *pResult) {
    NM_TREEVIEW *pnmtv = (NM_TREEVIEW *) pnmh;

	if (pnmtv->itemNew.hItem != NULL) {
		LoadSite(pnmtv->itemNew.hItem);
	}

    *pResult = 0;        // Allow the expansion to proceed.
}

// Delete ////

void CCacheManPg::OnRemove() {
	LOG0(3, "CCachemanPg::OnRemove()");

	if (m_pProgress == NULL) {
		// gather files to delete
		HTREEITEM hItem = m_ctlCacheItems.GetRootItem();
		while (hItem != NULL) {
			// child items
			HTREEITEM hChild = m_ctlCacheItems.GetChildItem(hItem);
			while (hChild != NULL) {
				if (m_ctlCacheItems.GetCheck(hChild))
					DoInsertDelete(hChild);
				hChild = m_ctlCacheItems.GetNextSiblingItem(hChild);
			}

			hItem = m_ctlCacheItems.GetNextSiblingItem(hItem);
		}


		//
		if (DeleteFileList.GetCount() > 0 && AfxMessageBox(IDS_DELETE_ITEMS, MB_YESNO | MB_ICONQUESTION) == IDYES) {
			m_pProgress = new CDeleteCacheProgressDlg(this);
			m_pProgress->OpenDialog(IDS_DELETING, this);

			CloseHandle(CreateThread(NULL, 0, DeleteCacheThreadProc, this, 0, NULL));
		}

		// remove checks from site items
		hItem = m_ctlCacheItems.GetRootItem();
		while (hItem != NULL) {
			m_ctlCacheItems.SetCheck(hItem, FALSE);

			// child items
			HTREEITEM hChild = m_ctlCacheItems.GetChildItem(hItem);
			while (hChild != NULL) {
				m_ctlCacheItems.SetCheck(hChild, FALSE);
				hChild = m_ctlCacheItems.GetNextSiblingItem(hChild);
			}


			hItem = m_ctlCacheItems.GetNextSiblingItem(hItem);
		}
	}
	else {
		m_pProgress->SetActiveWindow();
	}
}

DWORD CCacheManPg::DeleteThread() {
	LOG0(3, "CCacheManPg::DeleteThread() - start");

	m_pProgress->SetRange(0, DeleteFileList.GetCount());

	int pos = 0;
	while (!DeleteFileList.IsEmpty() && !m_pProgress->GetTerminated()) {
		CDeleteItem *di = DeleteFileList.RemoveHead();

		ClearCacheFile(FileType, Config.CacheLocation, di->FileName);

		HTREEITEM hItem = di->HItem;
		CFeedItem *fi = (CFeedItem *) m_ctlCacheItems.GetItemData(hItem);
		int nImage = GetItemImage(fi);
		m_ctlCacheItems.SetItemImage(hItem, nImage, nImage);

		pos++;
		m_pProgress->SetPos(pos);

		delete di;
	}

	m_pProgress->CloseDialog();
	m_pProgress = NULL;

	LOG0(3, "CCacheManPg::DeleteThread() - end");

	return 0;
}

// Cache ////

void CCacheManPg::OnCache() {
	// gather files to cache
	HTREEITEM hItem = m_ctlCacheItems.GetRootItem();
	while (hItem != NULL) {
		// child items
		HTREEITEM hChild = m_ctlCacheItems.GetChildItem(hItem);
		while (hChild != NULL) {
			if (m_ctlCacheItems.GetCheck(hChild))
				DoInsertCache(hChild);
			hChild = m_ctlCacheItems.GetNextSiblingItem(hChild);
		}

		hItem = m_ctlCacheItems.GetNextSiblingItem(hItem);
	}

	DoCacheItems();

	// remove checks from site items
	hItem = m_ctlCacheItems.GetRootItem();
	while (hItem != NULL) {
		m_ctlCacheItems.SetCheck(hItem, FALSE);

		// child items
		HTREEITEM hChild = m_ctlCacheItems.GetChildItem(hItem);
		while (hChild != NULL) {
			m_ctlCacheItems.SetCheck(hChild, FALSE);
			hChild = m_ctlCacheItems.GetNextSiblingItem(hChild);
		}


		hItem = m_ctlCacheItems.GetNextSiblingItem(hItem);
	}
}

// Purge ////

void CCacheManPg::DoPurgeCache() {
}

void CCacheManPg::OnPurgeCache() {
	if (AfxMessageBox(IDS_PURGE_CACHE_WARNING, MB_YESNO | MB_ICONQUESTION) == IDYES) {
		if (m_pPurgeProgress == NULL) {
			DoPurgeCache();

			m_pPurgeProgress = new CPurgeCacheProgressDlg(this);
			m_pPurgeProgress->OpenDialog(IDS_PURGING_CACHE, this);

			CloseHandle(CreateThread(NULL, 0, PurgeCacheThreadProc, this, 0, NULL));
		}
		else {
			m_pPurgeProgress->SetActiveWindow();
		}

	}
}

// TODO: progress
static void DeleteFiles(const CString &dir) {
	CString path;
	path.Format(_T("%s\\*.*"), dir);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			CString fileNamePath;
			fileNamePath.Format(_T("%s\\%s"), dir, CString(fd.cFileName));

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				DeleteFiles(fileNamePath);
				::RemoveDirectory(fileNamePath);
			}
			else {
				::DeleteFile(fileNamePath);
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}


DWORD CCacheManPg::PurgeThread() {
	LOG0(3, "CCacheManPg::PurgeThread() - start");

	DeleteFiles(PurgePath);

	m_pPurgeProgress->CloseDialog();
	m_pPurgeProgress = NULL;

	LOG0(3, "CCacheManPg::DeleteThread() - end");

	return 0;
}

// Select All ////

void CCacheManPg::OnSelectAll() {
	// remove checks from site items
	HTREEITEM hItem = m_ctlCacheItems.GetRootItem();
	while (hItem != NULL) {
		m_ctlCacheItems.SetCheck(hItem, TRUE);

		// child items
		HTREEITEM hChild = m_ctlCacheItems.GetChildItem(hItem);
		while (hChild != NULL) {
			m_ctlCacheItems.SetCheck(hChild, TRUE);
			hChild = m_ctlCacheItems.GetNextSiblingItem(hChild);
		}


		hItem = m_ctlCacheItems.GetNextSiblingItem(hItem);
	}
}
