/**
 *  MainFrm.h
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

#if !defined(AFX_MAINFRM_H__AEB80540_589D_4649_A124_DBE9B3C23C48__INCLUDED_)
#define AFX_MAINFRM_H__AEB80540_589D_4649_A124_DBE9B3C23C48__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../share/SyncList.h"
#include "ctrls/CeToolBar.h"
#include "ctrls/Banner.h"
#include "ctrls/EnclosureBar.h"
#include "ctrls/InfoBar.h"
#include "Site.h"
//#include "Download.h"
#include "UpdateBar.h"

#include "FeedView.h"
#include "SummaryView.h"
#include "ArticleView.h"
#include "sync/FeedSync.h"

#define MF_NUM_TOOLTIPS				6

//

class CMainFrame : public CFrameWnd {
public:
	CMainFrame();
protected:
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
//	BOOL StartupUpdateAll;			// TRUE if the update all operation has to be performed right after startup

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

public:
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	HWND m_hwndCmdBar;
	CCeToolBar m_wndTopBar;

	enum EView {
		FeedView = 1,
		SummaryView = 2,
		ArticleView = 3
	} View;
	CFeedView m_wndFeedView;
	CSummaryView m_wndSummaryView;

	// message view
	CBanner m_wndBanner;
	CEnclosureBar m_wndEnclosureBar;
	CInfoBar m_wndInfoBar;
	CArticleView m_wndArticleView;

	// update bar
	CUpdateBar m_wndUpdateBar;

	// image list
	CImageList m_ilIcons;

	// interal variables
	BOOL SiteMenuOpened;
	BOOL Loading;				// loading site list
	BOOL WorkOffline;			// TRUE if in Offline mode

	// article view touch scrolling
	BOOL m_bClick;
	CPoint LastCursorPos;
	// ItemNext / ItemLast marker touch
	BOOL m_articleNotSwitched;

	UINT CtxMenuTimer;
	BOOL m_bOpenCtxMenu;
	BOOL m_bMenuOpened;

// Generated message map functions
protected:
	void SwitchView(EView view);
	void SwitchToView(CWnd *pOldActiveView, CWnd *pNewView);
	void SetupSummaryView();
	void SetupFeedView();
	void SetupArticleView();
	void SetupBanner(CFeedItem *fi, int item, int total);

	void SetupEnclosureBar(CFeedItem *fi);
	void NoNewMessage();
	void SetTopBarText(UINT nID, int nIconIdx);
	void SetTopBarText(const CString &strText, int nIconIdx);
	void UpdateTopBar();
	void FillMenuWithSites(HMENU hMenu, CSiteList *siteList, CSiteItem *parent);
	CString GetMyChannelsItem();
	CString GetUnreadItem();
	CString GetFlaggedItem();
	void SelectSite(int nSite);

	void LoadSites();
	void LoadFaviconForSite(int idx, CSiteItem *si);

	HANDLE HTerminate;

	// saving
	void AddSiteToSave(CSiteItem *si);
	void AddSiteToSave(int siteIdx);
	void SaveSite(CSiteItem *si);
	void SaveSitesThread();
	HANDLE HSaveSitesThread;
	CSyncList<CSiteItem *, CSiteItem *> SitesToSave;

	BOOL CheckCachePresence();
	BOOL CheckOnlineMode();

	void CollectSelectedFeedItems(CArray<CFeedItem *, CFeedItem *> &items);

	HANDLE HPreloadThread;
	HANDLE HPreloadSiteEvent;
	CSyncList<CSiteItem *, CSiteItem *> PreloadList;
	void PreloadThread();
	void PreloadSite(int idx);

	void UpdateMenu();

	CDownloader Downloader;
	CFeedSync *Syncer;
	HANDLE HSyncItemsThread;
	HANDLE HSyncItemEvent;
	CSyncList<CFeedItem *, CFeedItem *> ItemsToSync;
	void SyncItemsThread();
	void CreateSyncer();
	void SyncItem(CFeedItem *fi);

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	afx_msg void OnFileProperties();
	afx_msg void OnUpdateFileProperties(CCmdUI *pCmdUI);

	afx_msg void OnFileInformation();
	afx_msg void OnUpdateFileInformation(CCmdUI *pCmdUI);

	afx_msg void OnFileWorkOffline();
	void UpdateWorkOfflineMenu();
	afx_msg void OnUpdateFileWorkOffline(CCmdUI *pCmdUI);
	afx_msg void OnFileCacheFlagged();
	afx_msg void OnFileCacheUnread();

	afx_msg void OnToolsSummaryview();
	afx_msg void OnUpdateToolsSummaryview(CCmdUI *pCmdUI);

	afx_msg void OnToolsSiteManager();
	afx_msg void OnUpdateToolsSiteManager(CCmdUI *pCmdUI);

	afx_msg void OnToolsKeywordManager();
	afx_msg void OnUpdateToolsKeywordManager(CCmdUI *pCmdUI);

	afx_msg void OnToolsOptions();

	afx_msg void OnToolsMarkAllRead();
	afx_msg void OnUpdateToolsMarkAllRead(CCmdUI *pCmdUI);
	afx_msg void OnToolsMarkAllUnread();
	afx_msg void OnUpdateToolsMarkAllUnread(CCmdUI *pCmdUI);
	afx_msg void OnToolsMarkAllUnflagged();

	afx_msg void OnToolsUpdateChannels();
	afx_msg void OnUpdateToolsUpdateChannels(CCmdUI *pCmdUI);

	afx_msg void OnToolsUpdateAllChannels();
	afx_msg void OnUpdateToolsUpdateAllChannels(CCmdUI *pCmdUI);

	afx_msg void OnToolsErrors();
	afx_msg void OnToolsCacheManager();

	afx_msg void OnCacheGetOnlineContent();
	afx_msg void OnCacheGetImages();
	afx_msg void OnEnclosuresOpen();
	afx_msg void OnEnclosuresGet();
	afx_msg void OnEnclosuresDelete();

	afx_msg void OnTopBarClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpenSiteList();
	afx_msg void OnSiteSelected(UINT nID);

	afx_msg void OnOpenPRSSreader();
	afx_msg void OnHide();

	afx_msg void OnRewriteRules();

	afx_msg void OnFeedListView();

	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);

	// sort
	void UpdateSort();
	afx_msg void OnSortChange();
	afx_msg void OnViewSortbyDate();
	afx_msg void OnViewSortbyRead();
	afx_msg void OnViewSortbyKeyword();
	afx_msg void OnUpdateSortby(CCmdUI *pCmdUI);

	LRESULT OnHibernate(WPARAM wParam, LPARAM lParam);
	LRESULT OnShowUpdateBar(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateFeed(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateFavicon(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateAll(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateFinished(WPARAM wParam, LPARAM lParam);

	LRESULT OnOpenSite(WPARAM wParam, LPARAM lParam);
	LRESULT OnOpenFeedItem(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	friend class CPrssrApp;
	friend class CFeedView;
	friend class CSummaryView;
	friend class CArticleView;
	friend class CCacheManPg;
	friend class CCacheManHtmlPg;
	friend class CCacheManEnclosuresPg;
	friend class CUpdateBar;
	friend DWORD WINAPI LoadSitesStubProc(LPVOID lpParameter);
	friend DWORD WINAPI SaveSitesStubProc(LPVOID lpParameter);
	friend DWORD WINAPI PreloadSitesStubProc(LPVOID lpParameter);
	friend DWORD WINAPI SyncItemsStubProc(LPVOID lpParameter);
};

/////////////////////////////////////////////////////////////////////////////

extern HANDLE HSaveSiteListEvent;

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AEB80540_589D_4649_A124_DBE9B3C23C48__INCLUDED_)
