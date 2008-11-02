/**
 *  MainFrm.cpp
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
#include "../share/UIHelper.h"
#include "../share/reg.h"

#include "MainFrm.h"
#include "FeedView.h"
#include "SummaryView.h"
#include "misc/icon.h"

#include "SiteManagerDlg.h"
#include "KeywordManagerDlg.h"
#include "ErrorsDlg.h"
#include "UpdateSitesDlg.h"

#include "ctrls/CePropertySheet.h"

#include "Config.h"
#include "Appearance.h"

#include "Site.h"
#include "misc/shnotif.h"

// info
#include "InfoGeneralPg.h"
#include "InfoChannelPg.h"
#include "InfoDescriptionPg.h"

#include "Properties.h"

// options
#include "OptGeneralPg.h"
#include "OptCachePg.h"
#include "OptRetrievalPg.h"
#include "OptSyncPg.h"
#include "OptConnectionPg.h"
#include "OptAppearancePg.h"
#include "../share/notif.h"

#include "RewritingDlg.h"
// cache manager
#include "CacheMan.h"

#include "sync/GReaderSync.h"
#include "sync/NetworkSync.h"
#include "net/Connection.h"

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

#define ID_MENU_MY_CHANNELS				0x1FFF
#define ID_MENU_SITE_BASE				0x1000


// width of icon in the topbar + width of drop-down button + margin + width of sort indicator
#define CX_ICON							SCALEX(22 + 17)

// top bar icon indexes
#define TOPBAR_IMAGE_COUNT				6

#define TOPBAR_IMAGE_OK					0
#define TOPBAR_IMAGE_FOLDER				1
#define TOPBAR_IMAGE_ERROR				2
#define TOPBAR_IMAGE_LOADING			3

#define TOPBAR_IMAGE_ASC				4
#define TOPBAR_IMAGE_DESC				5

// icons for enclosures
#define ICON_SITE						0
#define ICON_AUDIO						4
#define ICON_VIDEO						6
#define ICON_IMAGE						8
#define ICON_OTHER						10

//


DWORD WINAPI LoadSitesStubProc(LPVOID lpParameter) {
	CMainFrame *frame = (CMainFrame *) lpParameter;
	frame->LoadSites();
	return 0;
}

DWORD WINAPI SaveSitesStubProc(LPVOID lpParameter) {
	CMainFrame *frame = (CMainFrame *) lpParameter;
	frame->SaveSitesThread();
	return 0;
}

DWORD WINAPI PreloadSitesStubProc(LPVOID lpParameter) {
	CMainFrame *frame = (CMainFrame *) lpParameter;
	frame->PreloadThread();
	return 0;
}

DWORD WINAPI SyncItemsStubProc(LPVOID lpParameter) {
	CMainFrame *frame = (CMainFrame *) lpParameter;
	frame->SyncItemsThread();
	return 0;
}

//// forward declaration
//DWORD WINAPI PeriodicUpdateThreadStub(LPVOID lpParameter);

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
//	ON_WM_INITMENUPOPUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_FILE_INFORMATION, OnFileInformation)
	ON_UPDATE_COMMAND_UI(ID_FILE_INFORMATION, OnUpdateFileInformation)

	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileProperties)

	ON_COMMAND(ID_WORK_OFFLINE, OnFileWorkOffline)
	ON_COMMAND(ID_CACHE_FLAGGED, OnFileCacheFlagged)
	ON_COMMAND(ID_CACHE_UNREAD, OnFileCacheUnread)

	ON_COMMAND(ID_TOOLS_UPDATEALLCHANNELS, OnToolsUpdateAllChannels)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_UPDATEALLCHANNELS, OnUpdateToolsUpdateAllChannels)
	ON_COMMAND(ID_TOOLS_UPDATECHANNELS, OnToolsUpdateChannels)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_UPDATECHANNELS, OnUpdateToolsUpdateChannels)

	ON_COMMAND(ID_TOOLS_SUMMARYVIEW, OnToolsSummaryview)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SUMMARYVIEW, OnUpdateToolsSummaryview)

	ON_COMMAND(ID_TOOLS_SITEMANAGER, OnToolsSiteManager)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SITEMANAGER, OnUpdateToolsSiteManager)

	ON_COMMAND(ID_TOOLS_KEYWORDMANAGER, OnToolsKeywordManager)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_KEYWORDMANAGER, OnUpdateToolsKeywordManager)

	ON_COMMAND(ID_TOOLS_ERRORS, OnToolsErrors)
	ON_COMMAND(ID_TOOLS_CACHEMANAGER, OnToolsCacheManager)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)

	ON_COMMAND(ID_TOOLS_MARKALLITEMSREAD, OnToolsMarkAllRead)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MARKALLITEMSREAD, OnUpdateToolsMarkAllRead)
	ON_COMMAND(ID_TOOLS_MARKALLITEMSUNREAD, OnToolsMarkAllUnread)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MARKALLITEMSUNREAD, OnUpdateToolsMarkAllUnread)

	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR + 1, OnTopBarClick)
//	ON_COMMAND(IDC_ACT_SITE, OnOpenSiteList)
	ON_COMMAND_RANGE(ID_MENU_SITE_BASE - 2, ID_MENU_SITE_BASE + 1000, OnSiteSelected)
	ON_COMMAND(ID_OPEN_SITE_LIST, OnOpenSiteList)
	ON_COMMAND(ID_MENU_MY_CHANNELS, OnToolsSummaryview)

	ON_COMMAND(ID_CACHE_GET_ONLINECONTENT, OnCacheGetOnlineContent)
	ON_COMMAND(ID_CACHE_GET_IMAGES, OnCacheGetImages)
	ON_COMMAND(ID_ENCLOSURE_OPEN, OnEnclosuresOpen)
	ON_COMMAND(ID_ENCLOSURE_GET, OnEnclosuresGet)
	ON_COMMAND(ID_ENCLOSURE_DELETE, OnEnclosuresDelete)

	ON_COMMAND(ID_VIEW_SORTBY_DATE, OnViewSortbyDate)
	ON_COMMAND(ID_VIEW_SORTBY_READ, OnViewSortbyRead)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SORTBY_DATE, ID_VIEW_SORTBY_READ, OnUpdateSortby)

	ON_COMMAND(ID_OPEN, OnOpenPRSSreader)
	ON_COMMAND(ID_HIDE, OnHide)

	ON_COMMAND(ID_REWRITE_RULES, OnRewriteRules)

	ON_COMMAND(ID_TOOLS_FEEDVIEW, OnFeedListView)

	ON_MESSAGE(WM_HIBERNATE, OnHibernate)
	ON_MESSAGE(UWM_SHOW_UPDATEBAR, OnShowUpdateBar)
	ON_MESSAGE(UWM_UPDATE_FEED, OnUpdateFeed)
	ON_MESSAGE(UWM_UPDATE_FAVICON, OnUpdateFavicon)
	ON_MESSAGE(UWM_UPDATE_ALL, OnUpdateAll)
	ON_MESSAGE(UWM_UPDATE_FINISHED, OnUpdateFinished)

	ON_MESSAGE(UWM_OPEN_SITE, OnOpenSite)
	ON_MESSAGE(UWM_OPEN_FEEDITEM, OnOpenFeedItem)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() {
	View = SummaryView;

	SiteMenuOpened = FALSE;

	HTerminate = CreateEvent(NULL, TRUE, FALSE, NULL);

//	HSaveSiteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HPreloadSiteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HSaveSitesThread = NULL;
	HSyncItemsThread = NULL;
	Syncer = NULL;
	HSyncItemEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CMainFrame::~CMainFrame() {
	CloseHandle(HTerminate);
//	CloseHandle(HSaveSiteEvent);
	CloseHandle(HPreloadSiteEvent);
	CloseHandle(HSyncItemEvent);

	delete Syncer;
}

void CMainFrame::CreateSyncer() {
	switch (Config.SyncSite) {
		case SYNC_SITE_GOOGLE_READER: Syncer = new CGReaderSync(&Downloader, Config.SyncUserName, Config.SyncPassword);  break;
		default: Syncer = new CNetworkSync(&Downloader); break;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	LOG0(3, "CMainFrame::OnCreate()");

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

//	Config.MainView = MAIN_VIEW_SUMMARY_VIEW;

	// view
	switch (Config.MainView) {
		case MAIN_VIEW_FEED_LIST:
			m_wndFeedView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_wndSummaryView.Create(NULL, NULL, WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 1, NULL);
			m_wndArticleView.Create(NULL, NULL, WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 2, NULL);
			break;

		case MAIN_VIEW_ARTICLE:
			m_wndArticleView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW | WS_VISIBLE | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_wndSummaryView.Create(NULL, NULL, WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 1, NULL);
			m_wndFeedView.Create(NULL, NULL, WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 2, NULL);
			break;

		default:
		case MAIN_VIEW_SUMMARY_VIEW:
			m_wndSummaryView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_wndFeedView.Create(NULL, NULL, WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 1, NULL);
			m_wndArticleView.Create(NULL, NULL, WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 2, NULL);
			break;
	}
	m_wndArticleView.View = &m_wndFeedView;

	// top bar
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_BORDER_BOTTOM;
	CRect rcBorder(0, 0, 0, 0);
	if (!m_wndTopBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_DROPDOWN | TBSTYLE_LIST, dwStyle,
		rcBorder, AFX_IDW_TOOLBAR + 1)) {
	   	TRACE0("Failed to create Top Bar\n");
		return -1;      // fail to create
	}
	m_wndTopBar.SetHeight(SCALEY(21));

	// top bar
	CToolBarCtrl &ctlToolBar = m_wndTopBar.GetToolBarCtrl();
	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_SITE_ITEMS, SCALEX(16), 100, RGB(192, 192, 192));
	AfxSetResourceHandle(AfxGetInstanceHandle());
	ctlToolBar.SetImageList(&m_ilIcons);
	ctlToolBar.SetBitmapSize(CSize(SCALEX(16), SCALEY(16)));
	ctlToolBar.AddStrings(_T("\0"));

	ctlToolBar.SetFont(&Appearance.BaseFont);

	// separator
	TBBUTTON btn = { 0 };
	btn.iBitmap		= I_IMAGENONE;
	btn.idCommand	= ID_TOOLBAR_SEPARATOR;
	btn.fsState		= TBSTATE_ENABLED;
	btn.iString     = -1;
	ctlToolBar.InsertButton(0, &btn);


	EnableDocking(CBRS_ALIGN_ANY);

	// update bar
	if (!m_wndUpdateBar.Create(this)) {
	   	TRACE0("Failed to create Update Bar\n");
		return -1;      // fail to create
	}

	// banner
	if (!m_wndBanner.Create(this)) {
	   	TRACE0("Failed to create Banner\n");
		return -1;      // fail to create
	}

	// enclosure bar
	m_wndEnclosureBar.Create(this);

	// info bar
	m_wndInfoBar.Create(this);

//	m_wndTopBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTopBar.EnableDocking(CBRS_ALIGN_TOP);

	// banner
	m_wndBanner.SetImageList(&m_ilIcons);
	m_wndBanner.SetSmbImageList(&m_wndFeedView.m_oIcons);
//	m_wndBanner.EnableDocking(CBRS_ALIGN_ANY);
	m_wndBanner.EnableDocking(CBRS_ALIGN_TOP);

	m_wndEnclosureBar.EnableDocking(CBRS_ALIGN_BOTTOM);
	m_wndInfoBar.EnableDocking(CBRS_ALIGN_BOTTOM);

	DockControlBar(&m_wndTopBar);
//	DockControlBar(&m_wndUpdateBar);
	DockControlBar(&m_wndBanner);
	DockControlBar(&m_wndEnclosureBar);
	DockControlBar(&m_wndInfoBar);

	///
	switch (Config.MainView) {
		case MAIN_VIEW_FEED_LIST:
			SetupFeedView();
			m_wndFeedView.SetFocus();
			break;

		case MAIN_VIEW_ARTICLE:
			// to display the HTML ctrl in proper dimensions
			m_wndArticleView.SetDlgCtrlID(AFX_IDW_PANE_FIRST);
			m_wndArticleView.ShowWindow(SW_SHOW);

			SetupArticleView();
			m_wndArticleView.SetFocus();
			break;

		default:
		case MAIN_VIEW_SUMMARY_VIEW:
			SetupSummaryView();
			m_wndSummaryView.SetFocus();
			break;
	}


	//
	CString strLabel;
	UnreadItems.FlagMask = MESSAGE_UNREAD | MESSAGE_NEW;
	UnreadItems.Name.LoadString(IDS_UNREAD);
	UnreadItems.ImageIdx = 1;

	FlaggedItems.FlagMask = MESSAGE_FLAG;
	FlaggedItems.Name.LoadString(IDS_FLAGGED);
	FlaggedItems.ImageIdx = 1;

	LoadSites();


	int height = GetSystemMetrics(SM_CYSCREEN);
	CRect rc;
	GetClientRect(&rc);
	SetWindowPos(NULL, 0, SCALEY(26), rc.Width(), height - SCALEY(52), SWP_NOZORDER);

	//

//	HSaveSitesThread = CreateThread(NULL, 0, SaveSitesStubProc, this, 0, NULL);
//	SetThreadPriority(HSaveSitesThread, THREAD_PRIORITY_LOWEST);
	HSaveSitesThread = NULL;

	HPreloadThread = CreateThread(NULL, 0, PreloadSitesStubProc, this, 0, NULL);
//	SetThreadPriority(HPreloadThread, THREAD_PRIORITY_LOWEST);

	CreateSyncer();
	HSyncItemsThread = CreateThread(NULL, 0, SyncItemsStubProc, this, 0, NULL);

	UpdateMenu();
	UpdateWorkOfflineMenu();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs) {
	LOG0(5, "CMainFrame::PreCreateWindow()");

	cs.lpszClass = PRSSR_CLASS_NAME;
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd) {
	LOG0(5, "CMainFrame::OnSetFocus()");

	// forward focus to the view window
	if (View == FeedView)
		m_wndFeedView.SetFocus();
	else
		m_wndSummaryView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	LOG0(5, "CMainFrame::OnCmdMsg()");

	// let the view have first crack at the command
	switch (View) {
		case FeedView:
			if (m_wndFeedView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;

		case SummaryView:
			if (m_wndSummaryView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;

		case ArticleView:
			if (m_wndArticleView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			if (m_wndBanner.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			if (m_wndEnclosureBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			if (m_wndInfoBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;
	}

	if (m_wndUpdateBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::DestroyWindow() {
	LOG0(1, "CMainFrame::DestroyWindow()");

	Downloader.Terminate();
	if (m_wndUpdateBar.IsUpdating())
		m_wndUpdateBar.OnStop();

	// wait for saving thread to terminate
	if (SiteList.GetCount() > 0) {
		AddSiteToSave(Config.ActSiteIdx);
		if (HSaveSitesThread != NULL)
			WaitForSingleObject(HSaveSitesThread, INFINITE);
	}

	SetEvent(HTerminate);
	if (HPreloadThread != NULL) WaitForSingleObject(HPreloadThread, INFINITE);
	if (HSyncItemsThread != NULL) WaitForSingleObject(HSyncItemsThread, INFINITE);

	prssrNotificationRemove();

	//
	Config.SaveUI();

	return CFrameWnd::DestroyWindow();
}

void CMainFrame::SwitchToView(CWnd *pOldActiveView, CWnd *pNewView) {
	int nSwitchChildID = pNewView->GetDlgCtrlID();
	pNewView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
	pOldActiveView->SetDlgCtrlID(nSwitchChildID);

	// Show the newly active view and hide the inactive view.
	pNewView->ShowWindow(SW_SHOW);
	pOldActiveView->ShowWindow(SW_HIDE);

	// Connect the newly active view to the document,
	// and disconnect the inactive view
//	SetActiveView(pNewView);
	RecalcLayout();
}

void CMainFrame::SetupSummaryView() {
	View = SummaryView;
	Config.MainView = MAIN_VIEW_SUMMARY_VIEW;

	ShowControlBar(&m_wndTopBar, FALSE, FALSE);
	m_wndTopBar.EnableSortButton(FALSE);

	// hide article view controls
	ShowControlBar(&m_wndBanner, FALSE, FALSE);
	ShowControlBar(&m_wndEnclosureBar, FALSE, FALSE);
	ShowControlBar(&m_wndInfoBar, FALSE, FALSE);

	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	mbi.dwFlags = SHCMBF_HMENU;
	mbi.nToolBarId = IDR_SUMMARYVIEW;
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();

	fSuccess = SHCreateMenuBar(&mbi);
	::DestroyWindow(m_hwndCmdBar);
	m_hwndCmdBar = mbi.hwndMB;

//	m_wndArticleView.UnsubclassWindow();
}

void CMainFrame::SetupFeedView() {
	View = FeedView;
	Config.MainView = MAIN_VIEW_FEED_LIST;

	ShowControlBar(&m_wndTopBar, TRUE, FALSE);
	m_wndTopBar.EnableSortButton();

	// hide article view controls
	ShowControlBar(&m_wndBanner, FALSE, FALSE);
	ShowControlBar(&m_wndEnclosureBar, FALSE, FALSE);
	ShowControlBar(&m_wndInfoBar, FALSE, FALSE);

	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	mbi.dwFlags = SHCMBF_HMENU;
	mbi.nToolBarId = IDR_MAINFRAME;
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();

	fSuccess = SHCreateMenuBar(&mbi);
	::DestroyWindow(m_hwndCmdBar);
	m_hwndCmdBar = mbi.hwndMB;

//	m_wndArticleView.UnsubclassWindow();
}

void CMainFrame::SetupArticleView() {
	View = ArticleView;
	Config.MainView = MAIN_VIEW_ARTICLE;

	ShowControlBar(&m_wndTopBar, FALSE, FALSE);
	m_wndTopBar.EnableSortButton(FALSE);

	// show article view controls
	ShowControlBar(&m_wndBanner, TRUE, FALSE);
	ShowControlBar(&m_wndEnclosureBar, FALSE, FALSE);		// initially hidden
	ShowControlBar(&m_wndInfoBar, FALSE, FALSE);		// initially hidden

	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	mbi.dwFlags = SHCMBF_HMENU;
	mbi.nToolBarId = IDR_ARTICLE;
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();

	fSuccess = SHCreateMenuBar(&mbi);
	::DestroyWindow(m_hwndCmdBar);
	m_hwndCmdBar = mbi.hwndMB;

//	m_wndArticleView.SubclassWindow(GetSafeHwnd());
}

void CMainFrame::SwitchView(EView view) {
	if (View == view)
		return;

	switch (view) {
		case SummaryView:
			SwitchToView(&m_wndFeedView, &m_wndSummaryView);
			SetupSummaryView();
			m_wndSummaryView.SetFocus();
			break;

		case FeedView:
			if (View == ArticleView)
				SwitchToView(&m_wndArticleView, &m_wndFeedView);
			else
				SwitchToView(&m_wndSummaryView, &m_wndFeedView);
			SetupFeedView();
			m_wndFeedView.SetFocus();
			break;

		case ArticleView:
			SwitchToView(&m_wndFeedView, &m_wndArticleView);
			SetupArticleView();
			m_wndArticleView.SetFocus();
			break;
	}

	Config.SaveUI();
}

///

void CMainFrame::OnSize(UINT nType, int cx, int cy) {
	LOG3(1, "CMainFrame::OnSize(%d, %d, %d)", nType, cx, cy);

	CFrameWnd::OnSize(nType, cx, cy);
}

LRESULT CMainFrame::OnHibernate(WPARAM wParam, LPARAM lParam) {
	// TODO: save session to XML file

	return 0;
}

BOOL CMainFrame::CheckCachePresence() {
	LOG0(3, "CMainFrame::CheckCachePresence()");

	if (DirectoryExists(Config.CacheLocation))
		return TRUE;
	else
		return FALSE;
}

void CMainFrame::LoadSites() {
	LOG0(1, "CMainFrame::LoadSites()");

	m_wndTopBar.EnableWindow(FALSE);

	CWaitCursor wait;
	Loading = TRUE;

	SetTopBarText(IDS_LOADING, TOPBAR_IMAGE_LOADING);

	LoadSiteList(SiteList);

//	int ret = LoadSiteList();
//	LOG1(1, "LoadSiteList() = %d", ret);

	// load favicons
	m_ilIcons.SetImageCount(TOPBAR_IMAGE_COUNT + SiteList.GetCount());
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *si = SiteList.GetAt(i);
		LoadFaviconForSite(i, si);
	}

	// insert items to summary view
	m_wndSummaryView.SetImageList(&m_ilIcons);
	m_wndSummaryView.SetRedraw(FALSE);
	m_wndSummaryView.InsertSites(&SiteList);
	m_wndSummaryView.SetRedraw(TRUE);
	m_wndSummaryView.Invalidate();

	//
	if (SiteList.GetCount() > 0) {
		// we have sites, but ActSiteIdx is out out the range -> activate the first site
		if (Config.ActSiteIdx < -2 || Config.ActSiteIdx >= SiteList.GetCount())
			Config.ActSiteIdx = 0;

		CSiteItem *si = SiteList.GetAt(Config.ActSiteIdx);
		if (View == FeedView || View == ArticleView) {
			if (Config.ActSiteIdx >= -2 && Config.ActSiteIdx < SiteList.GetCount()) {
				SelectSite(Config.ActSiteIdx);
				PreloadSite(Config.ActSiteIdx);
			}
			else {
				// TODO: switch to summary view
			}
		}

		if (View == ArticleView) {
			if (si->Feed != NULL) {
				LOG2(1, "FeedItem = %d, %d", Config.ActFeedItem, si->Feed->GetItemCount());
				if (Config.ActFeedItem >= 0 && Config.ActFeedItem < si->Feed->GetItemCount()) {
					m_wndFeedView.OpenItem(Config.ActFeedItem);
				}
				else {
					// switch to feed view
					SwitchView(FeedView);
					SelectSite(Config.ActSiteIdx);
				}
			}
		}
	}

	Loading = FALSE;
	m_wndTopBar.EnableWindow();

	UpdateTopBar();

	// to update command bar
	PostMessage(WM_CANCELMODE);

	if (!CheckCachePresence())
		m_wndUpdateBar.ShowError(IDS_CACHE_NOT_AVAILABLE);
}

//

void CMainFrame::SetTopBarText(UINT nID, int nIconIdx) {
	LOG2(5, "CMainFrame::SetTopBarText(%d, %d)", nID, nIconIdx);

	CString strText;
	strText.LoadString(nID);
	SetTopBarText(strText, nIconIdx);
}

void CMainFrame::SetTopBarText(const CString &strText, int nIconIdx) {
	LOG2(5, "CMainFrame::SetTopBarText(%S, %d)", strText, nIconIdx);

	m_wndTopBar.SetText(strText);
	m_wndTopBar.SetImageIdx(nIconIdx);
	m_wndTopBar.Invalidate();
	m_wndTopBar.UpdateWindow();
}

void CMainFrame::UpdateTopBar() {
	LOG0(5, "CMainFrame::UpdateTopBar()");

	if (SiteList.GetRoot()->GetCount() == 0) {
	}
	else {
		if (View == FeedView) {
			// update site label
			if (Config.ActSiteIdx >= 0 && Config.ActSiteIdx < SiteList.GetCount()) {
				CSiteItem *si = SiteList.GetAt(Config.ActSiteIdx);
				int unreadCount = si->GetUnreadCount();
				CDC *pDC = GetDC();
				CString strTitle = GetNumberItemText(pDC, si->Name, unreadCount, GetSystemMetrics(SM_CXSCREEN) - CX_ICON);
				ReleaseDC(pDC);

				if (si->Status == CSiteItem::Ok)
					SetTopBarText(strTitle, si->ImageIdx);
				else
					SetTopBarText(strTitle, TOPBAR_IMAGE_ERROR);
			}
			else if (Config.ActSiteIdx == SITE_UNREAD) {
				SetTopBarText(GetUnreadItem(), TOPBAR_IMAGE_FOLDER);
			}
			else if (Config.ActSiteIdx == SITE_FLAGGED) {
				SetTopBarText(GetFlaggedItem(), TOPBAR_IMAGE_FOLDER);
			}
		}
	}
}

void CMainFrame::OnTopBarClick(NMHDR* pNMHDR, LRESULT* pResult) {
	LOG0(3, "CMainFrame::OnTopBarClick()");

	NMTOOLBAR *ntb = (NMTOOLBAR *) pNMHDR;

	if (ntb->iItem == IDC_ACT_SITE) {
		OnOpenSiteList();
	}
	else if (ntb->iItem == IDC_SORT_BUTTON) {
		OnSortChange();
	}

	*pResult = 0;
}

void CMainFrame::FillMenuWithSites(HMENU hMenu, CSiteList *siteList, CSiteItem *parent) {
	LOG1(5, "CMainFrame::FillMenuWithSites( , , %p)", parent);

	if (parent == NULL)
		return;

	if (parent->Type == CSiteItem::Site) {
		int siteIdx = siteList->GetIndexOf(parent);
		int unreadCount = parent->GetUnreadCount();
		CDC *pDC = GetDC();
		CString strSiteTitle = GetNumberItemText(pDC, parent->Name, unreadCount, GetSystemMetrics(SM_CXSCREEN) - CX_ICON);
		ReleaseDC(pDC);
		AppendMenu(hMenu, MF_ENABLED | MF_STRING, ID_MENU_SITE_BASE + siteIdx, strSiteTitle);

		if (View == FeedView && Config.ActSiteIdx == siteIdx) {
			CheckMenuRadioItem(hMenu, ID_MENU_SITE_BASE, ID_MENU_SITE_BASE + 1000, ID_MENU_SITE_BASE + siteIdx, MF_BYCOMMAND);
		}
	}
	else {
		HMENU hSubMenu = CreatePopupMenu();

		if (parent->SubItems.GetCount() > 0) {
			POSITION pos = parent->SubItems.GetHeadPosition();
			while (pos != NULL) {
				CSiteItem *si = parent->SubItems.GetNext(pos);

				FillMenuWithSites(hSubMenu, siteList, si);
			}
		}
		else {
			// empty group
			CString sEmpty;
			sEmpty.LoadString(IDS_EMPTY);
			AppendMenu(hSubMenu, MF_GRAYED | MF_STRING, -1, sEmpty);
		}

		CDC *pDC = GetDC();
		int unreadCount = parent->GetUnreadCount();
		CString strGroupTitle = GetNumberItemText(pDC, parent->Name, unreadCount, GetSystemMetrics(SM_CXSCREEN) - CX_ICON);
		ReleaseDC(pDC);
		AppendMenu(hMenu, MF_POPUP, (UINT) hSubMenu, strGroupTitle);
	}
}

CString CMainFrame::GetUnreadItem() {
	CSiteItem *root = SiteList.GetRoot();
	int unreadCount = root->GetUnreadCount();
	CString strUnread;
	strUnread.LoadString(IDS_UNREAD);
	CDC *pDC = GetDC();
	CString str = GetNumberItemText(pDC, strUnread, unreadCount, GetSystemMetrics(SM_CXSCREEN) - CX_ICON);
	ReleaseDC(pDC);
	return str;
}

CString CMainFrame::GetFlaggedItem() {
	CSiteItem *root = SiteList.GetRoot();
	int flaggedCount = root->GetFlaggedCount();
	CString strFlagged;
	strFlagged.LoadString(IDS_FLAGGED);
	CDC *pDC = GetDC();
	CString str = GetNumberItemText(pDC, strFlagged, flaggedCount, GetSystemMetrics(SM_CXSCREEN) - CX_ICON);
	ReleaseDC(pDC);
	return str;
}

void CMainFrame::OnOpenSiteList() {
	LOG0(3, "CMainFrame::OnOpenSiteList()");

	if (SiteList.GetCount() > 0) {
		// close other possibly opened menu
		ReleaseCapture();
		SendMessage(WM_CANCELMODE);

		SiteMenuOpened = TRUE;

		HMENU hMenu = CreatePopupMenu();
		CSiteItem *root = SiteList.GetRoot();
//		root->ReadItemCountsFromCache();
		FillMenuWithSites(hMenu, &SiteList, root);

		HMENU hRootMenu = GetSubMenu(hMenu, 0);

		int siteCount = SiteList.GetCount();
		::AppendMenu(hRootMenu, MF_SEPARATOR, -1, NULL);
		::AppendMenu(hRootMenu, MF_ENABLED | MF_STRING, ID_MENU_SITE_BASE + SITE_UNREAD, GetUnreadItem());
		::AppendMenu(hRootMenu, MF_ENABLED | MF_STRING, ID_MENU_SITE_BASE + SITE_FLAGGED, GetFlaggedItem());

		if (View == FeedView && (Config.ActSiteIdx == SITE_UNREAD || Config.ActSiteIdx == SITE_FLAGGED)) {
			CheckMenuRadioItem(hRootMenu, ID_MENU_SITE_BASE - 2, ID_MENU_SITE_BASE + 1000, ID_MENU_SITE_BASE + Config.ActSiteIdx, MF_BYCOMMAND);
		}

		CRect rcItem;
		m_wndTopBar.GetWindowRect(&rcItem);
		TrackPopupMenuEx(hRootMenu, TPM_RIGHTALIGN, rcItem.left, rcItem.bottom - 1, GetSafeHwnd(), NULL);
		SiteMenuOpened = FALSE;

		DestroyMenu(hMenu);
	}
	else {
		// no sites in list, start Site Manager
		OnToolsSiteManager();
	}
}

void CMainFrame::OnSiteSelected(UINT nID) {
	LOG0(3, "CMainFrame::OnSiteSelected()");

	int nSite = nID - ID_MENU_SITE_BASE;

	if (Config.ActSiteIdx != nSite)
		AddSiteToSave(Config.ActSiteIdx);

	if (View == SummaryView) {
		SwitchView(FeedView);
		SelectSite(nSite);
		PreloadSite(nSite);
	}
	else if (View == ArticleView) {
	}
	else {
		if (Config.ActSiteIdx != nSite) {
			SelectSite(nSite);
			PreloadSite(nSite);
		}
	}
}

//

void CMainFrame::OnToolsSummaryview() {
	LOG0(1, "CMainFrame::OnToolsSummaryview()");

	SwitchView(SummaryView);
	AddSiteToSave(Config.ActSiteIdx);

	UpdateTopBar();
}

void CMainFrame::OnUpdateToolsSummaryview(CCmdUI *pCmdUI) {
	LOG0(5, "CMainFrame::OnUpdateToolsSummaryview()");

	pCmdUI->Enable(!Loading);

	BOOL bCheck = View == SummaryView;
	if (::IsWindow(pCmdUI->m_pOther->GetSafeHwnd()))
		pCmdUI->m_pOther->SendMessage(TB_CHECKBUTTON, pCmdUI->m_nID, MAKELONG(bCheck , 0));
	else
		pCmdUI->SetCheck(bCheck);
}

void CMainFrame::SelectSite(int nSite) {
	LOG0(3, "CMainFrame::SelectSite()");

	if (nSite < -2 || nSite >= SiteList.GetCount())
		nSite = 0;

	Config.ActSiteIdx = nSite;

	if (nSite >= 0 && nSite < SiteList.GetCount()) {
		CSiteItem *si = SiteList.GetAt(nSite);
		if (si->Status == CSiteItem::Empty)
			SetTopBarText(IDS_LOADING, TOPBAR_IMAGE_LOADING);

		// load it in background
		si->EnsureSiteLoaded();
		SaveSiteItemUnreadCount(si, nSite);
		SaveSiteItemFlaggedCount(si, nSite);

//		if (View == FeedView) {
			m_wndFeedView.InsertItems(si);
//			UpdateSort();
//		}
	}
	else if (nSite == SITE_UNREAD) {
		SetTopBarText(IDS_LOADING, TOPBAR_IMAGE_LOADING);

		CWaitCursor wait;
		CFeed *unreadFeed = new CFeed();
		for (int i = 0; i < SiteList.GetCount(); i++) {
			CSiteItem *si = SiteList.GetAt(i);
			if (si->GetUnreadCount() > 0) {
				si->EnsureSiteLoaded();
				CFeed *feed = si->Feed;
				if (feed != NULL) {
					for (int j = 0; j < feed->GetItemCount(); j++) {
						CFeedItem *fi = feed->GetItem(j);
						if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread()))
							unreadFeed->Add(fi);
					}
				}
			}
		}

		if (UnreadItems.Feed != NULL) {
			UnreadItems.Feed->Detach();
			delete UnreadItems.Feed;
		}
		UnreadItems.Feed = unreadFeed;
		UnreadItems.Status = CSiteItem::Ok;

		m_wndFeedView.InsertItems(&UnreadItems);
		if (View == FeedView) {
			UpdateSort();
		}
	}
	else if (nSite == SITE_FLAGGED) {
		SetTopBarText(IDS_LOADING, TOPBAR_IMAGE_LOADING);

		CWaitCursor wait;
		CFeed *flaggedFeed = new CFeed();
		for (int i = 0; i < SiteList.GetCount(); i++) {
			CSiteItem *si = SiteList.GetAt(i);
			if (si->GetFlaggedCount() > 0) {
				si->EnsureSiteLoaded();
				CFeed *feed = si->Feed;
				if (feed != NULL) {
					for (int j = 0; j < feed->GetItemCount(); j++) {
						CFeedItem *fi = feed->GetItem(j);
						if (!fi->IsDeleted() && fi->IsFlagged())
							flaggedFeed->Add(fi);
					}
				}
			}
		}

		if (FlaggedItems.Feed != NULL) {
			FlaggedItems.Feed->Detach();
			delete FlaggedItems.Feed;
		}
		FlaggedItems.Feed = flaggedFeed;
		FlaggedItems.Status = CSiteItem::Ok;

		m_wndFeedView.InsertItems(&FlaggedItems);
		if (View == FeedView) {
			UpdateSort();
		}
	}

	UpdateTopBar();
}

void CMainFrame::OnToolsSiteManager() {
	LOG0(1, "CMainFrame::OnToolsSiteManager()");

	// FIXME: vfolder
	CString selRSS;
	int actSite = Config.ActSiteIdx;
	if (actSite >= 0 && actSite < SiteList.GetCount())
		selRSS = SiteList.GetAt(actSite)->Info->XmlUrl;

	CSiteManagerDlg dlg;
	dlg.Syncer = Syncer;
	dlg.Root = SiteList.GetRoot();
	// new items on today
	CRegistry regToday(HKEY_CURRENT_USER, REG_KEY_TODAY);
	dlg.ShowNewChannelsOnToday = regToday.Read(szShowNewChannels, CONFIG_DEFAULT_SHOWNEWCHANNELS);

	if (dlg.DoModal() == IDOK) {
		// merge
		CWaitCursor wait;

		// save the structure
		SiteList.Detach();
		SiteList.CreateFrom(dlg.Root);
		SiteList.SetRoot(dlg.Root);
		SaveSiteList();

		if (SiteList.GetCount() > 0) {
			// update icons for top bar
			m_ilIcons.SetImageCount(TOPBAR_IMAGE_COUNT + SiteList.GetCount());
			for (int i = 0; i < SiteList.GetCount(); i++) {
				CSiteItem *si = SiteList.GetAt(i);
				LoadFaviconForSite(i, si);
			}

			///

			// There was nothing in the sitelist
			if (actSite == -1)
				actSite = 0;	// select the first site
			else if (actSite >= SiteList.GetCount())
				actSite = SiteList.GetCount() - 1;

			if (View == FeedView) {
				if (SiteList.GetAt(actSite)->Info->XmlUrl.CompareNoCase(selRSS) != 0) {
					// site could be moved -> find it!
					for (int item = 0; item < SiteList.GetCount(); item++) {
						if (SiteList.GetAt(item)->Info->XmlUrl.CompareNoCase(selRSS) == 0) {
							// we are successful, the code below will select the site
							actSite = item;
							break;
						}
					}
				}
				SelectSite(actSite);
			}

			Config.ActSiteIdx = actSite;
			Config.SaveUI();

			m_wndSummaryView.SetRedraw(FALSE);
			m_wndSummaryView.DeleteAllItems();
			m_wndSummaryView.InsertSites(&SiteList);
			m_wndSummaryView.SetRedraw(TRUE);
			m_wndSummaryView.UpdateScrollBars();
			m_wndSummaryView.Invalidate();
		}
		else {
			Config.SaveUI();

			SwitchView(SummaryView);
			m_wndSummaryView.SetRedraw(FALSE);
			m_wndSummaryView.DeleteAllItems();
			m_wndSummaryView.InsertSites(&SiteList);
			m_wndSummaryView.SetRedraw(TRUE);
			m_wndSummaryView.UpdateScrollBars();
			m_wndSummaryView.Invalidate();
		}

		UpdateTopBar();

		NotifyTodayPlugin(ReloadSubscriptionsMessage, -1);
	}
	else {
		dlg.Root->Destroy();
		delete dlg.Root;
	}

	UpdateMenu();
}

void CMainFrame::OnUpdateToolsSiteManager(CCmdUI *pCmdUI) {
	LOG0(5, "CMainFrame::OnUpdateToolsSiteManager()");

	pCmdUI->Enable(!Loading && !m_wndUpdateBar.IsUpdating());
}

void CMainFrame::OnToolsKeywordManager() {
	LOG0(1, "CMainFrame::OnToolsKeywordManager()");

	CKeywordManagerDlg dlg;

	for (int i = 0; i < Config.Keywords.GetSize(); i++)
		dlg.Keywords.Add(Config.Keywords[i]);

	if (dlg.DoModal() == IDOK) {
		// save keywords
		Config.Keywords.RemoveAll();
		for (int i = 0; i < dlg.Keywords.GetSize(); i++)
			Config.Keywords.Add(dlg.Keywords.GetAt(i));

		Config.SaveKeywords();
	}
}

void CMainFrame::OnUpdateToolsKeywordManager(CCmdUI *pCmdUI) {
	LOG0(5, "CMainFrame::OnUpdateToolsKeywordManager()");

	pCmdUI->Enable(!Loading);
}

void CMainFrame::OnToolsOptions() {
	LOG0(1, "CMainFrame::OnToolsOptions()");

	int oldUpdateInterval = Config.UpdateInterval;
	BOOL oldTimeUpdate = Config.TimeUpdate;
	SYSTEMTIME oldUat = Config.UpdateAtTime;

	COptGeneralPg pgGeneral;
	COptCachePg pgCache;
	COptRetrievalPg pgRetreival;
	COptSyncPg pgSync;
	COptConnectionPg pgConnection;
	COptAppearancePg pgAppearance;

	CCePropertySheet sheet(IDS_OPTIONS);
	sheet.AddPage(&pgGeneral);
	sheet.AddPage(&pgCache);
	sheet.AddPage(&pgRetreival);
	sheet.AddPage(&pgSync);
	sheet.AddPage(&pgConnection);
	sheet.AddPage(&pgAppearance);
	sheet.SetMenu(IDR_OPTIONS);

	if (sheet.DoModal()) {
		Config.Save();
		Config.SaveProxyProfiles();
		Appearance.Save();

		SetupCradlingEvent(Config.CheckOnCradling);

		// setup periodical update
		if (oldUpdateInterval != Config.UpdateInterval) {
			SYSTEMTIME st;
			GetLocalTime(&st);

			if (Config.UpdateInterval > 0) {
				AddSeconds(st, 60 * Config.UpdateInterval);
				SetupUpdateEvent(_T("/periodic"), TRUE, &st);			// set the event up
			}
			else
				SetupUpdateEvent(_T("/periodic"), FALSE, NULL);			// delete the event
		}

		// setup daily event
		if (oldTimeUpdate != Config.TimeUpdate ||
			oldUat.wHour != Config.UpdateAtTime.wHour || oldUat.wMinute != Config.UpdateAtTime.wMinute || oldUat.wSecond != Config.UpdateAtTime.wSecond)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);

			SYSTEMTIME uat = st;
			uat.wHour = Config.UpdateAtTime.wHour;
			uat.wMinute = Config.UpdateAtTime.wMinute;
			uat.wSecond = Config.UpdateAtTime.wSecond;

			FILETIME uatFt, localFt;
			SystemTimeToFileTime(&st, &localFt);
			SystemTimeToFileTime(&uat, &uatFt);

			if (CompareFileTime(&uatFt, &localFt) < 0)
				AddSeconds(uat, 60 * 60 * 24);			// 1 day

			if (Config.TimeUpdate)
				SetupUpdateEvent(_T("/daily"), TRUE, &uat);				// set the event up
			else
				SetupUpdateEvent(_T("/daily"), FALSE, NULL);			// delete the event
		}

		delete Syncer;
		CreateSyncer();

		//
		NotifyTodayPlugin(ReadConfigMessage);

		m_wndFeedView.UpdateItemHeight();
		m_wndSummaryView.SetItemHeight();
		m_wndSummaryView.UpdateItemHeight();
	}
}

void CMainFrame::OnToolsErrors() {
	LOG0(1, "CMainFrame::OnToolsErrors()");

	CErrorsDlg dlgError(this);
	dlgError.DoModal();

	if (dlgError.Retry) {
		Errors.Lock();
		POSITION pos = Errors.GetFirstPos();
		while (pos != NULL) {
			CErrorItem *ei = Errors.GetNext(pos);

			switch (ei->Type) {
				case CErrorItem::Site:
					if (ei->SiteIdx != SITE_INVALID)
						m_wndUpdateBar.EnqueueSite(SiteList.GetAt(ei->SiteIdx), ei->UpdateOnly);
					break;

				case CErrorItem::File:
					if (ei->FileType == FILE_TYPE_HTML) {
						if (ei->SiteIdx != SITE_INVALID)
							m_wndUpdateBar.EnqueueHtml(ei->Url, SiteList.GetAt(ei->SiteIdx));			// need to use rewriting
					}
					else
						m_wndUpdateBar.EnqueueItem(ei->Url, ei->FileType);
					break;
			}
		}
		Errors.Cleanup();
		Errors.Unlock();

		m_wndUpdateBar.Start();
		m_wndUpdateBar.Redraw();
		UpdateMenu();
	}
	else {
	}

	if (!m_wndUpdateBar.IsUpdating() && m_wndUpdateBar.IsWindowVisible()) {
		// hide update bar
		ShowControlBar(&m_wndUpdateBar, FALSE, FALSE);
	}

	m_wndUpdateBar.ErrorCount = Errors.GetCount();
	m_wndUpdateBar.Invalidate();
}

void CMainFrame::OnToolsMarkAllRead() {
	if (View == FeedView) {
		m_wndFeedView.MarkAllRead();

		UpdateTopBar();
	}
}

void CMainFrame::OnUpdateToolsMarkAllRead(CCmdUI *pCmdUI) {
	pCmdUI->Enable(!Loading);
}

void CMainFrame::OnToolsMarkAllUnread() {
	if (View == FeedView) {
		m_wndFeedView.MarkAllUnread();

		UpdateTopBar();
	}
}

void CMainFrame::OnUpdateToolsMarkAllUnread(CCmdUI *pCmdUI) {
	pCmdUI->Enable(!Loading);
}

void CMainFrame::OnToolsUpdateAllChannels() {
	LOG0(1, "CMainFrame::OnToolsUpdateAllChannels()");

	if (!CheckCachePresence()) {
		m_wndUpdateBar.ShowError(IDS_CACHE_NOT_AVAILABLE);
		return;
	}

	if (!CheckOnlineMode())
		return;

	CList<CSiteItem *, CSiteItem *> sites;
	for (int i = 0; i < SiteList.GetCount(); i++)
		sites.AddTail(SiteList.GetAt(i));
	m_wndUpdateBar.EnqueueSites(sites, FALSE);
	m_wndUpdateBar.Redraw();
	m_wndUpdateBar.Start();
	UpdateMenu();
}

void CMainFrame::OnUpdateToolsUpdateAllChannels(CCmdUI *pCmdUI) {
	LOG0(5, "CMainFrame::OnUpdateToolsUpdateAllChannels()");

	pCmdUI->Enable(!Loading && !m_wndUpdateBar.IsUpdating() && SiteList.GetCount() > 0);
}

void CMainFrame::OnToolsUpdateChannels() {
	LOG0(1, "CMainFrame::OnToolsUpdateChannels()");

	if (!CheckCachePresence()) {
		m_wndUpdateBar.ShowError(IDS_CACHE_NOT_AVAILABLE);
		return;
	}

	CUpdateSitesDlg dlg(this);
	if (dlg.DoModal() == IDOK) {
		Config.UpdateOnly = dlg.m_bUpdateOnly;
		Config.SaveUI();

		if (!CheckOnlineMode())
			return;
		m_wndUpdateBar.EnqueueSites(dlg.m_oUpdateSites, Config.UpdateOnly);
		m_wndUpdateBar.Redraw();
		m_wndUpdateBar.Start();
		UpdateMenu();
	}
}

void CMainFrame::OnUpdateToolsUpdateChannels(CCmdUI *pCmdUI) {
	LOG0(5, "CMainFrame::OnUpdateToolsUpdateChannels()");

	pCmdUI->Enable(!Loading && !m_wndUpdateBar.IsUpdating() && SiteList.GetCount() > 0);
}

LRESULT CMainFrame::OnShowUpdateBar(WPARAM wParam, LPARAM lParam) {
	LOG0(3, "CMainFrame::OnShowUpdateBar()");

	if (wParam == TRUE)
		ShowControlBar(&m_wndUpdateBar, TRUE, FALSE);
	else
		ShowControlBar(&m_wndUpdateBar, FALSE, FALSE);

	OnSetFocus(NULL);

	return 0;
}

LRESULT CMainFrame::OnUpdateFeed(WPARAM wParam, LPARAM lParam) {
	LOG0(3, "CMainFrame::OnUpdateFeed()");

	CSiteItem *si = (CSiteItem *) lParam;
	if (View == FeedView) {
		// TODO: prevent loss of selection when feed is updated (important for seq. reading via ArticleDlg)

		// reflect changes
		if (si != NULL) {
			if (Config.ActSiteIdx == SITE_UNREAD) {
				// virtual folder (unread)
				CFeed *newFeed = si->Feed;
				CFeed *curFeed = UnreadItems.Feed;

				// we need to preserve new items for later caching
				CArray<CFeedItem *, CFeedItem *> newItems;
				FeedDiff(newFeed, curFeed, &newItems);

				m_wndFeedView.SetRedraw(FALSE);
				for (int j = 0; j < newItems.GetSize(); j++) {
					CFeedItem *fi = newItems.GetAt(j);
					if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread())) {
						curFeed->Add(fi);
						m_wndFeedView.InsertItem(m_wndFeedView.GetItemCount(), fi);
					}
				}

				m_wndFeedView.SortItems();
				m_wndFeedView.SetRedraw(TRUE);
				m_wndFeedView.UpdateScrollBars();
				m_wndFeedView.Invalidate();

				UpdateSort();
			}
			else if (si == SiteList.GetAt(Config.ActSiteIdx)) {
				// just a site
				SelectSite(Config.ActSiteIdx);
			}
		}
	}

	if (si != NULL) {
		// update item in summary view
		m_wndSummaryView.Invalidate(FALSE);
		UpdateTopBar();
	}

	return 0;
}

void CMainFrame::SaveSite(CSiteItem *si) {
	LOG0(5, "CMainFrame::SaveSite()");

	if (si != NULL && si->Info != NULL && si->Status == CSiteItem::Ok) {
		CString feedPathName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, si->Info->FileName);
		if (si->Feed != NULL) {
			si->Feed->Save(feedPathName);
			SaveSiteItemUnreadCount(si, SiteList.GetIndexOf(si));
			SaveSiteItemFlaggedCount(si, SiteList.GetIndexOf(si));
			si->SetModified(FALSE);
			NotifyTodayPlugin(UpdateFeedFlagsMessage, SiteList.GetIndexOf(si));
		}
	}
}

void CMainFrame::SaveSitesThread() {
	LOG0(1, "CMainFrame::SaveSitesThread() - start");

	// save everything in the list
	while (!SitesToSave.IsEmpty()) {
		CSiteItem *si = SitesToSave.GetHead();
		SaveSite(si);
		SitesToSave.RemoveHead();
	}

	CloseHandle(HSaveSitesThread);
	HSaveSitesThread = NULL;

	LOG0(1, "CMainFrame::SaveSitesThread() - end");
}

void CMainFrame::AddSiteToSave(int siteIdx) {
	if (siteIdx >= 0 && siteIdx < SiteList.GetCount())
		AddSiteToSave(SiteList.GetAt(siteIdx));
	else if (siteIdx == SITE_UNREAD)
		AddSiteToSave(&UnreadItems);
	else if (siteIdx == SITE_FLAGGED)
		AddSiteToSave(&FlaggedItems);
}

void CMainFrame::AddSiteToSave(CSiteItem *si) {
	LOG1(5, "CMainFrame::AddSiteToSave(%p)", si);

	if (si == NULL)
		return;

	if (si->Type == CSiteItem::VFolder) {
		CFeed *feed = si->Feed;
		if (feed != NULL) {
			for (int i = 0; i < feed->GetItemCount(); i++) {
				CFeedItem *fi = feed->GetItem(i);
				if (fi->SiteItem->IsModified() && SitesToSave.Find(fi->SiteItem) == NULL)
					SitesToSave.AddTail(fi->SiteItem);
			}
		}
	}
	else if (si->IsModified() && SitesToSave.Find(si) == NULL) {
		SitesToSave.AddTail(si);
	}

	// start the thread
	if (HSaveSitesThread == NULL && SitesToSave.GetCount() > 0) {
		HSaveSitesThread = CreateThread(NULL, 0, SaveSitesStubProc, this, 0, NULL);
	}
}

void CMainFrame::OnFileInformation() {
	LOG0(1, "CMainFrame::OnFileInformation()");


	HGROUPITEM hItem = m_wndSummaryView.GetSelectedItem();
	if (hItem == NULL)
		return;
	CSiteItem *si = (CSiteItem *) m_wndSummaryView.GetItemData(hItem);

	si->EnsureSiteLoaded();

	CInfoGeneralPg pgGeneral;
	CInfoChannelPg pgChannel;
	CInfoDescriptionPg pgDescription;

	CCePropertySheet sheet(_T(""));
	sheet.SetMenu(IDR_CANCEL);
	sheet.AddPage(&pgGeneral);

	// general
	if (si->Feed != NULL) {
		pgGeneral.m_nTotalItems = si->Feed->GetItemCount();
		pgGeneral.m_nNewItems = si->Feed->GetNewCount();
		pgGeneral.m_nUnreadItems = si->Feed->GetUnreadCount();
	}
	else {
		pgGeneral.m_nTotalItems = 0;
		pgGeneral.m_nNewItems = 0;
		pgGeneral.m_nUnreadItems = 0;
	}

	if (si->Type == CSiteItem::Site) {
		pgGeneral.m_strSiteName = si->Name;

		CString fileName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, si->Info->FileName);
		HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != INVALID_HANDLE_VALUE) {
			// file size
			DWORD size = GetFileSize(hFile, NULL);
			if (size == 0xffffffff) size = 0;
			pgGeneral.m_nFileSize = size;

			// last update
			SYSTEMTIME stLastWrite;
			FILETIME lastWrite = { 0 };
			GetFileTime(hFile, NULL, NULL, &lastWrite);

			FILETIME lastWriteLocal = { 0 };
//			FileTimeToLocalFileTime(&lastWrite, &lastWriteLocal);
			FileTimeToSystemTime(&lastWrite, &stLastWrite);
			CloseHandle(hFile);

			pgGeneral.m_stLastUpdated = stLastWrite;
		}

		if (si->Feed != NULL)
			pgGeneral.m_stPublished = si->Feed->Published;

		// channel
		if (si->Status == CSiteItem::Ok && si->Feed != NULL)
			pgChannel.m_strFormat = si->Feed->Format;
		else
			pgChannel.m_strFormat.LoadString(IDS_UNKNOWN);

		if (si->Feed != NULL)
			pgChannel.m_strWWW = si->Feed->HtmlUrl;
		pgChannel.m_strRSS = si->Info->XmlUrl;

		// description
		if (si->Feed != NULL) {
			pgDescription.m_strDescription = si->Feed->Description;
			pgDescription.m_strCopyright = si->Feed->Copyright;
			pgDescription.m_strLanguage = si->Feed->Language;
		}

		sheet.AddPage(&pgChannel);
		sheet.AddPage(&pgDescription);
	}
	else if (si->Type == CSiteItem::VFolder) {
		pgGeneral.m_strSiteName = si->Name;
		pgGeneral.m_nFileSize = 0;

		memset(&pgGeneral.m_stLastUpdated, 0, sizeof(pgGeneral.m_stLastUpdated));
	}

	sheet.DoModal();
}

void CMainFrame::OnUpdateFileInformation(CCmdUI *pCmdUI) {
	HGROUPITEM hItem = m_wndSummaryView.GetSelectedItem();
	if (hItem != NULL) {
		CSiteItem *si = (CSiteItem *) m_wndSummaryView.GetItemData(hItem);
		pCmdUI->Enable(!Loading && (si->Type == CSiteItem::Site || si->Type == CSiteItem::VFolder));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnFileProperties() {
	LOG0(1, "CMainFrame::OnFileProperties()");

	HGROUPITEM hItem = m_wndSummaryView.GetSelectedItem();
	if (hItem != NULL) {
		CSiteItem *si = (CSiteItem *) m_wndSummaryView.GetItemData(hItem);
		if (si->Type == CSiteItem::Site) {
			if (SiteProperties(si, this)) {
				int idx = SiteList.GetIndexOf(si);
				SaveSiteItem(si, idx);										// save changes
				NotifyTodayPlugin(ReloadSubscriptionsMessage, idx);			// notify today plugin
			}
		}
	}
}

void CMainFrame::OnUpdateFileProperties(CCmdUI *pCmdUI) {
	HGROUPITEM hItem = m_wndSummaryView.GetSelectedItem();
	if (hItem != NULL) {
		CSiteItem *si = (CSiteItem *) m_wndSummaryView.GetItemData(hItem);
		pCmdUI->Enable(!Loading && si->Type == CSiteItem::Site);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::UpdateWorkOfflineMenu() {
	// mnu
	TBBUTTON tb;
	::SendMessage(m_hwndCmdBar, TB_GETBUTTON, 1, (LPARAM) &tb);

	CMenu mnu;
	mnu.Attach((HMENU) tb.dwData);

	CMenu *pOfflineMnu = mnu.GetSubMenu(2);		// offline menu
	if (pOfflineMnu != NULL) {
		UINT state = Config.WorkOffline ? MF_CHECKED : MF_UNCHECKED;
		pOfflineMnu->CheckMenuItem(ID_WORK_OFFLINE, state | MF_BYCOMMAND);
	}

	mnu.Detach();
}

void CMainFrame::OnFileWorkOffline() {
	LOG0(1, "CMainFrame::OnFileWorkOffline()");

	Config.WorkOffline = !Config.WorkOffline;
	Config.SaveUI();
	UpdateWorkOfflineMenu();
}

BOOL CMainFrame::CheckOnlineMode() {
	LOG0(3, "CMainFrame::CheckOnlineMode()");

	WorkOffline = Config.WorkOffline;
	if (Config.WorkOffline) {
		CString txt;
		txt.LoadString(IDS_GO_ONLINE);
		int res = AfxMessageBox(txt, MB_YESNO);
		if (res == IDNO)
			return FALSE;

		Config.WorkOffline = FALSE;
		UpdateWorkOfflineMenu();
	}

	return TRUE;
}

LRESULT CMainFrame::OnUpdateFinished(WPARAM wParam, LPARAM lParam) {
	Config.WorkOffline = WorkOffline;
	PostMessage(WM_CANCELMODE);
	UpdateMenu();
	UpdateWorkOfflineMenu();

	return 0;
}

void CMainFrame::OnFileCacheFlagged() {
	LOG0(1, "CMainFrame::OnFileCacheFlagged()");

	if (!CheckOnlineMode())
		return;

	CWaitCursor wait;
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *si = SiteList.GetAt(i);

		si->EnsureSiteLoaded();
		if (si->Feed != NULL && si->Info != NULL) {
			BOOL cacheHtml, cacheImages, cacheEnclosures;
			if (si->Info->UseGlobalCacheOptions) {
				cacheHtml = Config.CacheHtml;
				cacheImages = Config.CacheImages;
			}
			else {
				cacheHtml = si->Info->CacheHtml;
				cacheImages = si->Info->CacheItemImages;
			}
			cacheEnclosures = si->Info->CacheEnclosures;

			CArray<CFeedItem *, CFeedItem *> items;
			// get flagged items in the feed
			for (int j = 0; j < si->Feed->GetItemCount(); j++) {
				CFeedItem *fi = si->Feed->GetItem(j);
				if (fi->IsFlagged())
					items.Add(fi);
			}

			if (cacheImages) m_wndUpdateBar.EnqueueImages(items);
			if (cacheHtml) m_wndUpdateBar.EnqueueHtmls(items);
			if (cacheEnclosures) m_wndUpdateBar.EnqueueEnclosures(items, si->Info->EnclosureLimit);
		}
	}

	m_wndUpdateBar.Redraw();
	m_wndUpdateBar.Start();
}

void CMainFrame::OnFileCacheUnread() {
	LOG0(1, "CMainFrame::OnFileCacheUnread()");

	if (!CheckOnlineMode())
		return;

	CWaitCursor wait;
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *si = SiteList.GetAt(i);

		si->EnsureSiteLoaded();
		if (si->Feed != NULL && si->Info != NULL) {
			//
			BOOL cacheHtml, cacheImages, cacheEnclosures;
			if (si->Info->UseGlobalCacheOptions) {
				cacheHtml = Config.CacheHtml;
				cacheImages = Config.CacheImages;
			}
			else {
				cacheHtml = si->Info->CacheHtml;
				cacheImages = si->Info->CacheItemImages;
			}
			cacheEnclosures = si->Info->CacheEnclosures;

			CArray<CFeedItem *, CFeedItem *> items;
			// get flagged items in the feed
			for (int j = 0; j < si->Feed->GetItemCount(); j++) {
				CFeedItem *fi = si->Feed->GetItem(j);
				if (fi->IsNew() || fi->IsUnread())
					items.Add(fi);
			}

			if (cacheImages) m_wndUpdateBar.EnqueueImages(items);
			if (cacheHtml) m_wndUpdateBar.EnqueueHtmls(items);
			if (cacheEnclosures) m_wndUpdateBar.EnqueueEnclosures(items, si->Info->EnclosureLimit);
		}
	}

	m_wndUpdateBar.Redraw();
	m_wndUpdateBar.Start();
}


void CMainFrame::CollectSelectedFeedItems(CArray<CFeedItem *, CFeedItem *> &items) {
	LOG0(1, "CMainFrame::CollectSelectedFeedItems()");

	int start, end;
	m_wndFeedView.GetSelectedRange(start, end);

	items.SetSize(end - start + 1);
	for (int i = start; i <= end; i++) {
		CFeedItem *fi = m_wndFeedView.GetItem(i);
		items.SetAt(i - start, fi);
	}
}

void CMainFrame::OnCacheGetOnlineContent() {
	LOG0(1, "CMainFrame::OnCacheGetOnlineContent()");

	CArray<CFeedItem *, CFeedItem *> items;

	if (View == FeedView) {
		CollectSelectedFeedItems(items);
		m_wndFeedView.DeselectAllItems();
	}

	if (items.GetSize() > 0) {
		m_wndUpdateBar.EnqueueHtmls(items);
		m_wndUpdateBar.Redraw();
		m_wndUpdateBar.Start();
	}
}

void CMainFrame::OnCacheGetImages() {
	LOG0(1, "CMainFrame::OnCacheGetImages()");

	CArray<CFeedItem *, CFeedItem *> items;

	if (View == FeedView) {
		CollectSelectedFeedItems(items);
		m_wndFeedView.DeselectAllItems();
	}

	if (items.GetSize() > 0) {
		m_wndUpdateBar.EnqueueImages(items);
		m_wndUpdateBar.Redraw();
		m_wndUpdateBar.Start();
	}
}

void CMainFrame::OnEnclosuresOpen() {
	LOG0(1, "CMainFrame::OnEnclosuresOpen()");

	if (View == FeedView) {
		int sel = m_wndFeedView.GetSelectedItem();
		CFeedItem *fi = m_wndFeedView.GetItem(sel);
		if (fi->HasEnclosure()) {
			CEnclosureItem *ei = fi->Enclosures.GetHead();
			OpenEnclosure(ei);
		}
	}
}

void CMainFrame::OnEnclosuresGet() {
	LOG0(1, "CMainFrame::OnEnclosuresGet()");

	CArray<CFeedItem *, CFeedItem *> items;

	if (View == FeedView) {
		CollectSelectedFeedItems(items);
		m_wndFeedView.DeselectAllItems();
	}

	if (items.GetSize() > 0) {
		m_wndUpdateBar.EnqueueEnclosures(items);
		m_wndUpdateBar.Redraw();
		m_wndUpdateBar.Start();
	}
}

void CMainFrame::OnEnclosuresDelete() {
	LOG0(1, "CMainFrame::OnEnclosuresDelete()");

	CArray<CFeedItem *, CFeedItem *> items;

	if (View == FeedView) {
		CollectSelectedFeedItems(items);
		m_wndFeedView.DeselectAllItems();
	}

	ClearEnclosures(items);
}

LRESULT CMainFrame::OnUpdateFavicon(WPARAM wParam, LPARAM lParam) {
	LOG0(3, "CMainFrame::OnShowUpdateBar()");

	CSiteItem *si = (CSiteItem *) lParam;
	LoadFaviconForSite(SiteList.GetIndexOf(si), si);

	return 0;
}

void CMainFrame::LoadFaviconForSite(int idx, CSiteItem *si) {
	if (si != NULL) {
		if (si->Info != NULL) {
			CString fileName = GetCacheFile(FILE_TYPE_FAVICON, Config.CacheLocation, si->Info->FileName);

			HICON hIcon = LoadIconFromFile(fileName, SCALEX(16), SCALEY(16));
			if (hIcon != NULL) {
				si->ImageIdx = m_ilIcons.Replace(TOPBAR_IMAGE_COUNT + idx, hIcon);
				si->CheckFavIcon = FALSE;			// for sure
			}
			else
				si->ImageIdx = TOPBAR_IMAGE_OK;
		}
		else
			si->ImageIdx = TOPBAR_IMAGE_OK;
	}
}

LRESULT CMainFrame::OnUpdateAll(WPARAM wParam, LPARAM lParam) {
	WorkOffline = Config.WorkOffline;
	if (CheckCachePresence()) {
		CList<CSiteItem *, CSiteItem *> sites;
		for (int i = 0; i < SiteList.GetCount(); i++)
			sites.AddTail(SiteList.GetAt(i));
		m_wndUpdateBar.EnqueueSites(sites, FALSE);
		m_wndUpdateBar.Redraw();
		m_wndUpdateBar.Start();
	}
	else {
		m_wndUpdateBar.ShowError(IDS_CACHE_NOT_AVAILABLE);
	}

	return 0;
}

// Sort ////

void CMainFrame::OnSortChange() {
	LOG0(3, "CMainFrame::OnSortChange()");

	ReleaseCapture();
	SendMessage(WM_CANCELMODE);

	CRect rcItem;
	m_wndTopBar.GetWindowRect(&rcItem);

	CMenu popup;
	popup.CreatePopupMenu();
	AppendMenuFromResource(&popup, IDR_SORT);
	// NOTE: has to make it via TPM_RETURNCMD, because std. WM_CMD machinery did not work for me (strange)
	UINT cmd = (UINT) popup.TrackPopupMenu(TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, rcItem.right, rcItem.bottom - 1, this);
//	TrackPopupMenuEx(popup.GetSafeHmenu(), TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, rcItem.right, rcItem.bottom - 1, GetSafeHwnd(), NULL);
//	TrackPopupMenuEx(popup.GetSafeHmenu(), TPM_RIGHTALIGN | TPM_TOPALIGN, rcItem.right, rcItem.bottom - 1, GetSafeHwnd(), NULL);

	switch (cmd) {
		case ID_VIEW_SORTBY_DATE: OnViewSortbyDate(); break;
		case ID_VIEW_SORTBY_READ: OnViewSortbyRead(); break;
	}
}

void CMainFrame::UpdateSort() {
	LOG0(3, "CMainFrame::UpdateSort()");

	if (m_wndFeedView.SiteItem == NULL)
		return;

	switch (m_wndFeedView.SiteItem->Sort.Type) {
		case CSortInfo::Ascending:  m_wndTopBar.SetSortButton(TOPBAR_IMAGE_ASC); break;
		case CSortInfo::Descending: m_wndTopBar.SetSortButton(TOPBAR_IMAGE_DESC); break;
	}

	m_wndTopBar.Invalidate();
}

void CMainFrame::OnViewSortbyDate() {
	LOG0(1, "CMainFrame::OnViewSortbyDate()");

	if (m_wndFeedView.SiteItem == NULL)
		return;

	if (m_wndFeedView.SiteItem->Sort.Item != CSortInfo::Date) {
		m_wndFeedView.SiteItem->Sort.Item = CSortInfo::Date;
		m_wndFeedView.DeselectAllItems();
		m_wndFeedView.SortItems();
	}
	else {
		switch (m_wndFeedView.SiteItem->Sort.Type) {
			case CSortInfo::Descending:	m_wndFeedView.OnSortAscending(); break;
			case CSortInfo::Ascending:	m_wndFeedView.OnSortDescending(); break;
		}
		UpdateSort();
	}

	SaveSiteItem(SiteList.GetAt(Config.ActSiteIdx), Config.ActSiteIdx);
}

void CMainFrame::OnViewSortbyRead() {
	LOG0(1, "CMainFrame::OnViewSortbyRead()");

	if (m_wndFeedView.SiteItem == NULL)
		return;

	if (m_wndFeedView.SiteItem->Sort.Item != CSortInfo::Read) {
		m_wndFeedView.SiteItem->Sort.Item = CSortInfo::Read;
		m_wndFeedView.DeselectAllItems();
		m_wndFeedView.SortItems();
	}
	else {
		switch (m_wndFeedView.SiteItem->Sort.Type) {
			case CSortInfo::Descending:	m_wndFeedView.OnSortAscending(); break;
			case CSortInfo::Ascending:	m_wndFeedView.OnSortDescending(); break;
		}
		UpdateSort();
	}

	SaveSiteItem(SiteList.GetAt(Config.ActSiteIdx), Config.ActSiteIdx);
}

void CMainFrame::OnUpdateSortby(CCmdUI *pCmdUI) {
	LOG0(3, "CMainFrame::OnUpdateSortby()");

	CSiteItem *si = SiteList.GetAt(Config.ActSiteIdx);
	if (si != NULL) {
		pCmdUI->m_pMenu->CheckMenuRadioItem(ID_VIEW_SORTBY_DATE, ID_VIEW_SORTBY_READ, ID_VIEW_SORTBY_DATE + si->Sort.Item - 1, MF_BYCOMMAND);
	}
}

//  /////////////////

void CMainFrame::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) {
	LOG0(1, "CMainFrame::OnInitMenuPopup()");

	if (bSysMenu)
		return; // don't support system menu

	ASSERT(pMenu != NULL); // check the enabled state of various menu items
	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu; // parent == child for tracking popup
	else if ((hParentMenu = ::WCE_FCTN(GetMenu)(m_hWnd)) != NULL) {
		CWnd *pParent = GetTopLevelParent(); // child windows don't have menus -- need to go to the top!

		if (pParent != NULL &&
			(hParentMenu = ::WCE_FCTN(GetMenu)(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::WCE_FCTN(GetMenuItemCount)(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu) {
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (pMenu->GetSubMenu(state.m_nIndex) != NULL) {
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue; // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE); // popups are never auto disabled
		}
		else {
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, TRUE);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax) {
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CMainFrame::PreloadThread() {
	LOG0(1, "CMainFrame::PreloadThread() - start");

	HANDLE handles[] = { HTerminate, HPreloadSiteEvent };
	BOOL terminated = FALSE;
	while (!terminated) {
		DWORD dwResult = WaitForMultipleObjects(countof(handles), handles, FALSE, INFINITE);
		if (dwResult == WAIT_OBJECT_0) {
			// terminate
			terminated = TRUE;
		}
		else if (dwResult == WAIT_OBJECT_0 + 1) {
			// preload everything in the list
			while (!PreloadList.IsEmpty()) {
				CSiteItem *si = PreloadList.GetHead();
				si->EnsureSiteLoaded();
				PreloadList.RemoveHead();

				if (WaitForSingleObject(HTerminate, 0) == WAIT_OBJECT_0) {
					terminated = TRUE;
					break;
				}
			}

			ResetEvent(HPreloadSiteEvent);
		}
	}

	CloseHandle(HPreloadThread);
	HPreloadThread = NULL;

	LOG0(1, "CMainFrame::PreloadThread() - end");
}

void CMainFrame::PreloadSite(int idx) {
	LOG1(5, "CMainFrame::PreloadSite(%d)", idx);

//	if (SiteList.GetCount() > 0 && (idx >= 0 && idx < SiteList.GetCount())) {
//		idx = idx % SiteList.GetCount();
//
//		int site = idx;
//		do {
//			site = (site + 1) % SiteList.GetCount();
//
//			CSiteItem *si = SiteList.GetAt(site);
//			if (si->Status == CSiteItem::Empty) {
//				PreloadList.AddTail(si);
//				SetEvent(HPreloadSiteEvent);
//				break;
//			}
//		} while (idx != site);
//	}
}

LRESULT CMainFrame::OnOpenSite(WPARAM wParam, LPARAM lParam) {
	OnSiteSelected(ID_MENU_SITE_BASE + wParam);
	return 0;
}

LRESULT CMainFrame::OnOpenFeedItem(WPARAM wParam, LPARAM lParam) {
	int siteIdx = wParam;
	int feedIdx = lParam;

	OnSiteSelected(ID_MENU_SITE_BASE + siteIdx);

	CFeed *feed = SiteList.GetAt(siteIdx)->Feed;
	if (feed != NULL && (feedIdx >= 0 && feedIdx < feed->GetItemCount())) {
		CFeedItem *fi = feed->GetItem(feedIdx);
		if (fi != NULL)
			m_wndFeedView.OpenItem(fi);
	}

	return 0;
}

void CMainFrame::OnToolsCacheManager() {
	if (CheckCachePresence()) {
		CCacheManager man;
		if (man.DoModal()) {
		}
	}
	else {
		m_wndUpdateBar.ShowError(IDS_CACHE_NOT_AVAILABLE);
	}
}


void CMainFrame::OnOpenPRSSreader() {
	LOG0(3, "CMainFrame::OnRunPRSSreader()");

	CString sPrssrBin;
	sPrssrBin.Format(_T("%s\\%s"), Config.InstallDir, PRSSREADER_BIN);
	CreateProcess(sPrssrBin, _T(""), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);

	prssrNotificationRemove();
}

void CMainFrame::OnHide() {
	// do nothing
	prssrNotificationRemove();
}

void CMainFrame::UpdateMenu() {
	BOOL fEnableItem = !Loading && !m_wndUpdateBar.IsUpdating() && SiteList.GetCount() > 0;
	::SendMessage(m_hwndCmdBar, TB_ENABLEBUTTON, (WPARAM) ID_TOOLS_UPDATEALLCHANNELS, (LPARAM) MAKELONG(fEnableItem, 0));
}

void CMainFrame::OnRewriteRules() {
	CRewritingDlg dlg;

	// rewrite rules
	for (int i = 0; i < Config.RewriteRules.GetSize(); i++) {
		CRewriteRule *rr = Config.RewriteRules[i];

		CRewriteRule *dupRR = new CRewriteRule();
		*dupRR = *rr;
		dlg.Rules.SetAtGrow(i, dupRR);
	}

	if (dlg.DoModal() == IDOK) {
		// rewrite rules
		int i;
		for (i = 0; i < Config.RewriteRules.GetSize(); i++)
			delete Config.RewriteRules[i];
		for (i = 0; i < dlg.Rules.GetSize(); i++)
			Config.RewriteRules.SetAtGrow(i, dlg.Rules[i]);

		Config.SaveRewriteRules();
	}
	else {
		// free previously allocated rewrite rules (duplicates)
		for (int i = 0; i < dlg.Rules.GetSize(); i++)
			delete dlg.Rules[i];
	}
}

void CMainFrame::SyncItemsThread() {
	LOG0(1, "CMainFrame::SyncItemsThread() - start");

	HANDLE handles[] = { HTerminate, HSyncItemEvent };
	BOOL terminated = FALSE;
	while (!terminated) {
		DWORD dwResult = WaitForMultipleObjects(countof(handles), handles, FALSE, INFINITE);
		if (dwResult == WAIT_OBJECT_0) {
			// terminate
			terminated = TRUE;
		}
		else if (dwResult == WAIT_OBJECT_0 + 1) {
			// got connection?
			if (Connection.IsAvailable() == S_OK && Config.SyncSite != SYNC_SITE_NONE) {
				// sync items in the list
				while (!ItemsToSync.IsEmpty()) {
					CFeedItem *fi = ItemsToSync.GetHead();
					Syncer->SyncItem(fi, MESSAGE_MASK_ALL);
					ItemsToSync.RemoveHead();

					if (WaitForSingleObject(HTerminate, 0) == WAIT_OBJECT_0) {
						terminated = TRUE;
						break;
					}
				}
			}
			else {
				// no connection -> empty the list
				while (!ItemsToSync.IsEmpty())
					ItemsToSync.RemoveHead();
			}

			ResetEvent(HSyncItemEvent);
		}
	}

	CloseHandle(HSyncItemsThread);
	HSyncItemsThread = NULL;

	LOG0(1, "CMainFrame::SyncItemsThread() - end");
}

void CMainFrame::SyncItem(CFeedItem *fi) {
	ItemsToSync.AddTail(fi);
	SetEvent(HSyncItemEvent);
}

void CMainFrame::OnFeedListView() {
	SwitchView(FeedView);
	UpdateTopBar();
}

void CMainFrame::SetupEnclosureBar(CFeedItem *fi) {
	if (fi->HasEnclosure()) {
		CEnclosureItem *ei = fi->Enclosures.GetHead();

		CString name = GetUrlFileName(ei->URL);
		if (name.IsEmpty())
			name.LoadString(IDS_ENCLOSURE);
		m_wndEnclosureBar.SetName(name);
		BOOL cached = IsEnclosureCached(ei->URL);
		m_wndEnclosureBar.SetCached(cached);
		m_wndEnclosureBar.SetSize(ei->Length);
		// set icon according to the file type
		int nImage;
		switch (ei->Type) {
			case ENCLOSURE_TYPE_AUDIO: nImage = ICON_AUDIO; break;
			case ENCLOSURE_TYPE_VIDEO: nImage = ICON_VIDEO; break;
			case ENCLOSURE_TYPE_IMAGE: nImage = ICON_IMAGE; break;
			default: nImage = ICON_OTHER; break;
		}

		if (!cached) nImage--;
		m_wndEnclosureBar.SetIcon(nImage);

//		m_wndEnclosureBar.ShowWindow(SW_SHOW);
		ShowControlBar(&m_wndEnclosureBar, TRUE, FALSE);
		m_wndEnclosureBar.Invalidate();
		m_wndEnclosureBar.UpdateWindow();
	}
	else {
//		m_wndEnclosureBar.ShowWindow(SW_HIDE);
		ShowControlBar(&m_wndEnclosureBar, FALSE, FALSE);
	}


}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	if (View == ArticleView) {
		return m_wndArticleView.OnNotify(wParam, lParam, pResult);
	}
	else {
		return CFrameWnd::OnNotify(wParam, lParam, pResult);
	}
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
	if (View == ArticleView) {
		static BOOL bDoIdle = TRUE;

		MSG msg;
		if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && bDoIdle) {
			bDoIdle = FALSE;
		}
		else {
			if (AfxGetApp()->IsIdleMessage(pMsg) && pMsg->message != 0x3FC) {
				bDoIdle = TRUE;
			}
		}

		if (pMsg->message == WM_KEYDOWN) {
			if (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN) {
				m_wndArticleView.HotSpot = TRUE;
			}

			if (pMsg->wParam == VK_LEFT ||
				pMsg->wParam == VK_RIGHT)
			{
				TranslateMessage(pMsg);
				SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
			else if (Config.MsgScrollThrought && (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN)) {
				TranslateMessage(pMsg);
				SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
			else if (pMsg->wParam == VK_RETURN) {
				TranslateMessage(pMsg);
				SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				if (m_wndArticleView.HotSpot) {
					return CFrameWnd::PreTranslateMessage(pMsg);
				}
				else {
					return TRUE;
				}
			}
			else {
				return CFrameWnd::PreTranslateMessage(pMsg);
			}
		}
		else {
			return CFrameWnd::PreTranslateMessage(pMsg);
		}
	}
	else
		return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	LOG3(1, "CMainFrame::OnKeyDown(%d, %d, %d)", nChar, nRepCnt, nFlags);

	if (View == ArticleView) {
		switch (nChar) {
			case VK_RETURN:
				m_wndArticleView.OnItemOpen();
				break;

			case VK_LEFT:
				// Move to previous item
				m_wndArticleView.OnItemPrev();
				break;

			case VK_RIGHT:
				// Move to next item
				m_wndArticleView.OnItemNext();
				break;

			case VK_UP:
				m_wndArticleView.OnScrollUp();
				m_wndArticleView.HotSpot = TRUE;
				break;

			case VK_DOWN:
				m_wndArticleView.OnScrollDown();
				m_wndArticleView.HotSpot = TRUE;
				break;

			default:
				CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
				break;
		}
	}
	else
		CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::NoNewMessage() {
	// info bar
	m_wndInfoBar.SetText(IDS_NO_MORE_MESSAGES);
	ShowControlBar(&m_wndInfoBar, TRUE, FALSE);
	m_wndInfoBar.StartTimer();
}
