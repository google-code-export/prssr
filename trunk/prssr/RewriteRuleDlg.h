#if !defined(AFX_REWRITERULEDLG_H__INCLUDED_)
#define AFX_REWRITERULEDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RewriteRuleDlg.h : header file
//

#include "ctrls/CeDialog.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CRewriteRuleDlg dialog

class CRewriteRuleDlg : public CCeDialog {
// Construction
public:
	CRewriteRuleDlg(CWnd *pParent = NULL);   // standard constructor
	~CRewriteRuleDlg();

// Dialog Data
	//{{AFX_DATA(CRewriteRuleDlg)
	enum { IDD = IDD_REWRITE_RULE };
	CCeEdit	m_ctlMatch;
	CCeEdit	m_ctlReplace;
	CString m_strMatch;
	CString m_strReplace;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRewriteRuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRewriteRuleDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REWRITERULEDLG_H__INCLUDED_)
