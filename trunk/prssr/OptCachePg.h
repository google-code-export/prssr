#if !defined(AFX_OPTCACHEPG_H__DE327E2C_DE54_483C_87C1_8580559173AE__INCLUDED_)
#define AFX_OPTCACHEPG_H__DE327E2C_DE54_483C_87C1_8580559173AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptCachePg.h : header file
//

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// COptCachePg dialog

class COptCachePg : public CPropertyPage {
	DECLARE_DYNCREATE(COptCachePg)

// Construction
public:
	COptCachePg();
	~COptCachePg();

// Dialog Data
	//{{AFX_DATA(COptCachePg)
	enum { IDD = IDD_OPT_CACHE };
	CCeEdit	m_ctlLocation;
	CButton	m_ctlBrowse;
	CString	m_strLocation;
	int m_nCacheLimit;
	CSpinButtonCtrl m_ctlCacheLimitSpin;
	BOOL m_bCacheImages;
	BOOL m_bCacheHtml;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptCachePg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptCachePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTCACHEPG_H__DE327E2C_DE54_483C_87C1_8580559173AE__INCLUDED_)
