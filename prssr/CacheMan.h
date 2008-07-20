#if !defined(AFX_CACHEMANPG_H__F42FE2DF_B0CF_4ED5_8A0D_783CC8157E1A__INCLUDED_)
#define AFX_CACHEMANPG_H__F42FE2DF_B0CF_4ED5_8A0D_783CC8157E1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CacheManPg.h : header file
//

#include "feed.h"
#include "ctrls\CePropertySheet.h"
#include "ctrls\CePropertyPage.h"
#include "ctrls\CeTreeCtrl.h"
#include "../share/helpers.h"


class CDeleteCacheProgressDlg;

/////////////////////////////////////////////////////////////////////////////
// CCacheTreeCtrl class

class CCacheTreeCtrl : public CCeTreeCtrl {
public:
	CCacheTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCacheTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CCacheManPg;

/////////////////////////////////////////////////////////////////////////////

struct CDeleteItem {
	CString FileName;
	HTREEITEM HItem;
};

/////////////////////////////////////////////////////////////////////////////
// CCacheManPg dialog

class CCacheManPg : public CCePropertyPage
{
	DECLARE_DYNCREATE(CCacheManPg)

// Construction
public:
	CCacheManPg();
	CCacheManPg(UINT nIDCaption);
	~CCacheManPg();

	CString m_strLocation;
// Dialog Data
	enum { IDD_WIDE = IDD_CACHE_MANAGER_WIDE };
	//{{AFX_DATA(CCacheManPg)
	enum { IDD = IDD_CACHE_MANAGER };
	CCacheTreeCtrl	m_ctlCacheItems;
	CStatic	m_ctlCacheSize;
	//}}AFX_DATA
	CImageList m_ilIcons;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCacheManPg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDeleteCacheProgressDlg *m_pProgress;
	CList<CDeleteItem *, CDeleteItem *> DeleteFileList;
	EFileType FileType;
	CArray<CFeedItem *, CFeedItem *> CacheList;

	void InsertItems();
	void UpdateControls();
	
	virtual void ResizeControls();
	virtual int GetItemImage(CFeedItem *fi);
	virtual void DoInsertFeedItem(HTREEITEM hParent, CFeedItem *fi);
	virtual void DoInsertDelete(HTREEITEM hItem);
	virtual void DoInsertCache(HTREEITEM hItem);
	virtual void DoCacheItems();
	virtual DWORD DeleteThread();

	void LoadSite(HTREEITEM hItem);

	// Generated message map functions
	//{{AFX_MSG(CCacheManPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	LRESULT OnGroupChecked(WPARAM wParam, LPARAM lParam);
	void OnItemExpanding(NMHDR *pnmh, LRESULT *pResult);
	void OnRemove();
	void OnCache();

	DECLARE_MESSAGE_MAP()

	friend DWORD WINAPI DeleteCacheThreadProc(LPVOID lpParameter);
};


////////////////////////////////////////////////////////

class CCacheManHtmlPg : public CCacheManPg {
public:
//	DECLARE_DYNCREATE(CCacheManHtmlPg)

	CCacheManHtmlPg();

public:
	virtual BOOL OnInitDialog();

protected:
	virtual int GetItemImage(CFeedItem *fi);
	virtual void DoInsertFeedItem(HTREEITEM hParent, CFeedItem *fi);
	virtual void DoInsertDelete(HTREEITEM hItem);
	virtual void DoCacheItems();
};

////////////////////////////////////////////////////////

class CCacheManEnclosuresPg : public CCacheManPg {
public:
//	DECLARE_DYNCREATE(CCacheManEnclosuresPg)

	CCacheManEnclosuresPg();

public:
	virtual BOOL OnInitDialog();

protected:
	virtual int GetItemImage(CFeedItem *fi);
	virtual void DoInsertFeedItem(HTREEITEM hParent, CFeedItem *fi);
	virtual void DoInsertDelete(HTREEITEM hItem);
	virtual void DoCacheItems();
};

////////////////////////////////////////////////////////

class CCacheManager : public CCePropertySheet {
public:
	CCacheManager(CWnd *pParentWnd = NULL);
	~CCacheManager();

	CCacheManHtmlPg m_pgCache;
	CCacheManEnclosuresPg m_pgEnclosures;

protected:
	// Generated message map functions
	//{{AFX_MSG(CCacheManager)
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACHEMANPG_H__F42FE2DF_B0CF_4ED5_8A0D_783CC8157E1A__INCLUDED_)