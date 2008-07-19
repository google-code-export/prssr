#if !defined(AFX_ERRORSDLG_H__DB67B61D_84BD_44A0_B810_A67CEB004735__INCLUDED_)
#define AFX_ERRORSDLG_H__DB67B61D_84BD_44A0_B810_A67CEB004735__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DownloadErrorsDlg.h : header file
//

#include "ctrls/CeDialog.h"
#include "ctrls/CeListCtrl.h"
#include "Errors.h"

/////////////////////////////////////////////////////////////////////////////
// CErrorsDlg dialog

class CErrorsDlg : public CCeDialog
{
// Construction
public:
	CErrorsDlg(CWnd* pParent = NULL);   // standard constructor
	~CErrorsDlg();

// Dialog Data
	//{{AFX_DATA(CErrorsDlg)
	enum { IDD = IDD_ERRORS };
	enum { IDD_WIDE = IDD_ERRORS_WIDE };
	CCeListCtrl	m_ctlErrors;
	//}}AFX_DATA
	BOOL Retry;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brEditBack;

	void AddErrorItem(CErrorItem *item);
	void InsertErrors();
	virtual void ResizeControls();

	// Generated message map functions
	//{{AFX_MSG(CErrorsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedErrors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCleanup();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRefresh();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnRetry();
	afx_msg void OnUpdateRetry(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADERRORSDLG_H__DB67B61D_84BD_44A0_B810_A67CEB004735__INCLUDED_)
