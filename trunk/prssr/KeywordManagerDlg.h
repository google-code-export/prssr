#if !defined(AFX_KEYWORDMANAGERDLG_H__B98DBF4F_40A8_4DBC_978B_ED8AB1CA5F4F__INCLUDED_)
#define AFX_KEYWORDMANAGERDLG_H__B98DBF4F_40A8_4DBC_978B_ED8AB1CA5F4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeywordManagerDlg.h : header file
//

#include "ctrls/CeDialog.h"
#include "ctrls/CeListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CKeywordManagerDlg dialog

class CKeywordManagerDlg : public CCeDialog {
// Construction
public:
	CKeywordManagerDlg(CWnd* pParent = NULL);   // standard constructor
	~CKeywordManagerDlg();

// Dialog Data
	//{{AFX_DATA(CKeywordManagerDlg)
	enum { IDD = IDD_KEYWORD_MANAGER };
	enum { IDD_WIDE = IDD_KEYWORD_MANAGER_WIDE };
	CCeListCtrl	m_ctlKeywords;
	//}}AFX_DATA
	// list for keyword exchange
	CStringArray Keywords;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeywordManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	void EditKeyword(int item);
	virtual void ResizeControls();

	// Generated message map functions
	//{{AFX_MSG(CKeywordManagerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedKeywords(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditKeywords(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRemove();
	afx_msg void OnUpdateRemove(CCmdUI *pCmdUI);
	afx_msg void OnEdit();
	afx_msg void OnUpdateEdit(CCmdUI *pCmdUI);
	afx_msg void OnNew();
	afx_msg void OnKeydownKeywords(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickKeywords(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYWORDMANAGERDLG_H__B98DBF4F_40A8_4DBC_978B_ED8AB1CA5F4F__INCLUDED_)
