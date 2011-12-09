/**
 *  TodayPluginWnd.h
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

#if !defined(AFX_TODAYPLUGINWND_H__2905BEE8_5F40_469E_A55A_B16089E759AB__INCLUDED_)
#define AFX_TODAYPLUGINWND_H__2905BEE8_5F40_469E_A55A_B16089E759AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../share/helpers.h"
#include "../prssr/Site.h"

enum EFileType {
		FILE_TYPE_FEED,
		FILE_TYPE_ENCLOSURE,
		FILE_TYPE_FAVICON,
		FILE_TYPE_HTML,
		FILE_TYPE_IMAGE,
		FILE_TYPE_OPML
};

/////////////////////////////////////////////////////////////////////////////
// CTodayPluginWnd window

class CTodayPluginWnd : public CWnd
{
// Construction
public:
	CTodayPluginWnd();
	virtual ~CTodayPluginWnd();

// Attributes
public:

// Operations
public:
	BOOL Create(HWND hwndParent);
	void OpenConfig();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTodayPluginWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	static BOOL Register();
	static BOOL Unregister();

	// Generated message map functions
protected:
	HWND m_hWndParent;

//	CSiteList *SiteList;
	int UnreadItems, NewItems;
	CString FeedItemHash;

	CFont m_fntNormal;
	CFont m_fntBold;
	HICON HIcon;
	HICON HIcons[1024]; 

	BOOL Selected;
	int RowHeight;

//	FILETIME LastOPMLWrite;
	int SiteIdx;
	int ItemIdx;
	int CycleTimer;

	CPoint LastCursorPos;
	UINT TapAndHoldTimer;

	void LoadFonts();
	BOOL NewsAvailable();
	void OnAction();
	void UpdateAll();

//	UINT OpmlCheckTimer;
//	UINT FileCheckTimer;
	int FileCheckIdx;

	BOOL Loading;							// TRUE if loading sites (on startup)

//	BOOL LoadingOPML;						// TRUE if OPML file is loading
//	BOOL ReloadingFeeds;					// TRUE if reloading feeds

//	CStringList FeedUpdateQueue;			// list of feed file names that needs to be reloaded

	HANDLE HReloadSubscriptions;
	HANDLE HCheckFeeds;
	HANDLE HUpdateFeedFlags;
	HANDLE HTerminate;
	HANDLE HThread;
	DWORD Thread();

	CRITICAL_SECTION CSSiteList;

	// draw helpers
	int DrawSiteTitle(CDC &dc, CSiteItem *si, CRect *rc, UINT uFormat);
	void DrawMessage(CDC &dc, CRect &rect, UINT nID);
	void DrawBrief(CDC &dc, CRect &rcLabel);
	void DrawCycling(CDC &dc, CRect &rcLabel);

	void InvalidateSiteTitle();
	void InvalidateFeedItem();

	void ContextMenu(CPoint pt);

	CString GetPrssFilePath();
	CString LoadSiteFeed;					// active feed site name

	//{{AFX_MSG(CTodayPluginWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTodayBrief();
	afx_msg void OnTodayCycling();
	afx_msg void OnTodayUpdateall();
	afx_msg void OnTodaySettings();
	afx_msg void OnFaviconsReset(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG


	LRESULT OnUIMetricChange(WPARAM wParam, LPARAM lParam);
	BOOL OnQueryRefreshCache(TODAYLISTITEM *ptli);
	void OnClearCache(TODAYLISTITEM *ptli);
	void OnReadConfig();

	void OpenReader();
	void OpenReaderSite(int siteIdx);
	void OpenReaderFeedItem(int siteIdx, int feedIdx);

//	void LoadSiteList();
//	void ReloadOPML();
//	void ReloadFeed();
	void UpdateUnreadNewCounts();

	void Cycle();
	void CheckFeedFile(CSiteItem *si);
	void UpdateFeedFlags(CSiteItem *si);

	void ReloadFeed(CSiteItem *si);
	void UnloadFeed(CSiteItem *si);

	friend class CPrssrtodayApp;
	friend DWORD WINAPI ThreadStubProc(LPVOID lpPar);

	DECLARE_MESSAGE_MAP()

	// cache from misc
	CString GetCachePath(EFileType type, const CString &strPath);
	CString GetCacheFile(EFileType type, const CString &strPath, const CString &url);
};

extern CTodayPluginWnd		TodayPluginWnd;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODAYPLUGINWND_H__2905BEE8_5F40_469E_A55A_B16089E759AB__INCLUDED_)
