#if !defined(AFX_PROPREWRITINGPG_H_INCLUDED_)
#define AFX_PROPREWRITINGPG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropRewritePg.h : header file
//

#include "ctrls/CeListCtrl.h"
#include "ctrls/CePropertyPage.h"

struct CRewriteRule;

/////////////////////////////////////////////////////////////////////////////
// CPropRewritingPg dialog

class CPropRewritingPg : public CCePropertyPage {
	DECLARE_DYNCREATE(CPropRewritingPg)

// Construction
public:
	CPropRewritingPg();
	~CPropRewritingPg();

// Dialog Data
	//{{AFX_DATA(CPropRewritingPg)
	enum { IDD = IDD_PROP_REWRITING };
	CCeListCtrl m_ctlRules;
	//}}AFX_DATA
	CArray<CRewriteRule *, CRewriteRule *> Rules;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropRewritingPg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InsertRule(int idx, CRewriteRule *rule);
	void EditRule(int item);

	void UpdateControls();
	virtual void ResizeControls();
	// Generated message map functions
	//{{AFX_MSG(CPropRewritingPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeydownRules(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickRules(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	afx_msg void OnRemove();
	afx_msg void OnUpdateRemove(CCmdUI *pCmdUI);

	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPREWRITINGPG_H_INCLUDED_)
