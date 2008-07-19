#if !defined(_SITESEARCHPG_H_)
#define _SITESEARCHPG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddOfflinePg.h : header file
//

#include "ctrls/CeEdit.h"
#include "ctrls/CeDialog.h"

#include "net/Searcher.h"
//#include "Session.h"

class CSearchProgressDlg;
class CSyndic8ComSearcher;

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

class CSearchDlg : public CCeDialog//, public CRestorable
{
// Construction
public:
	CSearchDlg(CWnd *pParent = NULL);   // standard constructor
	~CSearchDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_SEARCH_WIDE };
	//{{AFX_DATA(CSearchDlg)
	enum { IDD = IDD_SEARCH };
	CCeEdit	m_ctlWhat;
	CString m_strWhat;
	//}}AFX_DATA
	CList<CSearchResultItem *, CSearchResultItem *> *SearchResults;

protected:
	CSearchProgressDlg *m_pProgress;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	virtual void ResizeControls();

	DWORD WINAPI SearchThread();

	CSyndic8ComSearcher *Searcher;

	// Generated message map functions
	//{{AFX_MSG(CSearchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSearch();
	afx_msg void OnChangeWhat();
	//}}AFX_MSG

	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	friend DWORD SearchThreadProc(LPVOID lpParameter);
	friend CSearchProgressDlg;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_SITESEARCHPG_H_)
