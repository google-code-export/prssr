// UpdateBar.h : interface of the CUpdateBar class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPDATEBAR__INCLUDED_)
#define AFX_UPDATEBAR__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ctrls/CeToolbar.h"
#include "ctrls/TextProgressCtrl.h"

#include "net/Download.h"
#include "../share/helpers.h"

class CSiteItem;
class CFeedItem;
class CFeed;

#define DOWNLOAD_TRIES					3

class CUpdateItem {
public:
	CUpdateItem(CSiteItem *siteItem, BOOL updateOnly = FALSE) {
		SiteItem = siteItem;
		UpdateOnly = updateOnly;
	}

	BOOL UpdateOnly;
	CSiteItem *SiteItem;
};

/////////////////////////////////////////////////////////////////////////////
// CDownloadQueue class

class CDownloadItem {
public:
	EFileType Type;
	CString URL;
	CString FileName;
	int Try;
	int SiteIdx;
};

//
// CDownloadQueue class
//

class CDownloadQueue {
public:
	CDownloadQueue();
	virtual ~CDownloadQueue();

	BOOL IsEmpty();
	int GetCount();

	void Enqueue(CDownloadItem *item);
	CDownloadItem *Dequeue();

protected:
	CList<CDownloadItem*, CDownloadItem*> Items;
	CRITICAL_SECTION CS;
	int FinishedItems;

	friend class CUpdateBar;
};

//
// CUpdateBar class
//
class CUpdateBar : public CControlBar {
// Construction
public:
	CUpdateBar();
	virtual ~CUpdateBar();

	BOOL Create(CWnd *pParentWnd);

	virtual void OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	void Start();
	void EnqueueSite(CSiteItem *site, BOOL updateOnly = FALSE);
	void EnqueueSites(CList<CSiteItem *, CSiteItem *> &sites, BOOL updateOnly = FALSE);
	void EnqueueImages(CArray<CFeedItem *, CFeedItem *> &items);
	void EnqueueHtmls(CArray<CFeedItem *, CFeedItem *> &items);
	void EnqueueHtml(const CString &url, CSiteItem  *item);
	void EnqueueEnclosures(CArray<CFeedItem *, CFeedItem *> &items, DWORD limit = 0);
	void EnqueueItem(const CString &strUrl, EFileType type);

	BOOL IsUpdating() { return HUpdateThread != NULL; }

	void Redraw();

	void ShowError(UINT nID);
	void ShowError(const CString &str);

// Implementation
public:

protected:
	CTextProgressCtrl m_ctlProgress;
	CBitmapButton m_ctlStopBtn;
	CStatic m_ctlText;

	HANDLE HUpdateThread;
	CRITICAL_SECTION CSUpdateList;
	CList<CUpdateItem *, CUpdateItem *> UpdateList;
	CRITICAL_SECTION CSUpdater;
	CUpdateDownloader *Updater;
	CRITICAL_SECTION CSDownloader;
	CDownloader *Downloader;
	int ErrorCount;
	BOOL Terminate;
	int TotalItems;

	void UpdateThread();
	BOOL UpdateFeed(CSiteItem *ui, BOOL updateOnly);
	BOOL UpdateFeeds();

	void DownloadHtmlPage(CDownloadItem *di);
	void DownloadFile(CDownloadItem *di);
	void DownloadItems();
	void UpdateProgressText();

	CDownloadQueue DownloadQueue;

	void MergeFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly);

	enum {
		UPDATE_STATE_RSS,
		UPDATE_STATE_CACHING
	} State;
	CString SiteName;

    //{{AFX_MSG(CUpdateBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnStop();
	afx_msg void OnTextClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CMainFrame;
	friend DWORD WINAPI UpdateStubProc(LPVOID lpParameter);
};

/////////////////////////////////////////////////////////////////////////////

void FeedDiff(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);

void ClearImages(CArray<CFeedItem *, CFeedItem *> &items);
void ClearHtmlPages(CArray<CFeedItem *, CFeedItem *> &items);
void ClearEnclosures(CArray<CFeedItem *, CFeedItem *> &items);

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEBAR__INCLUDED_)