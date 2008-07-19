#if !defined(_SEARCHRESULTDLG_H_)
#define _SEARCHRESULTDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchResultsDlg.h : header file
//

#include "ctrls/CeDialog.h"
#include "ctrls/CeListCtrl.h"
#include "ctrls/hrule.h"
#include "net/Searcher.h"

/////////////////////////////////////////////////////////////////////////////
// CSearchResultsDlg dialog

class CSearchResultsDlg : public CCeDialog {
// Construction
public:
	CSearchResultsDlg(CWnd *pParentWnd = NULL);
	~CSearchResultsDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_SEARCH_RESULTS_WIDE };
	//{{AFX_DATA(CSearchResultsDlg)
	enum { IDD = IDD_SEARCH_RESULTS };
	CCeListCtrl	m_ctlFeeds;
	CHrule m_ctlHrule;
	CStatic m_ctlDescription;
	//}}AFX_DATA
	CImageList m_ilIcons;

	CList<CSearchResultItem *, CSearchResultItem *> *SearchResults;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchResultsDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	virtual void ResizeControls();

	void InsertItem(CSearchResultItem *item);

	CBrush m_brEditBack;

	// Generated message map functions
	//{{AFX_MSG(CSearchResultsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnSelchangedFeeds(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_IMPORTFEEDSPG_H_)
