#if !defined(AFX_CEEDIT_H__B4848F28_54CD_4385_A299_DC1B9905C3D8__INCLUDED_)
#define AFX_CEEDIT_H__B4848F28_54CD_4385_A299_DC1B9905C3D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CeEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCeEdit window

class CCeEdit : public CEdit
{
// Construction
public:
	CCeEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeEdit();

	// Generated message map functions
protected:
	void ContextMenu(CPoint point);

	//{{AFX_MSG(CCeEdit)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();
	//}}AFX_MSG
	afx_msg BOOL OnContextMenu(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEEDIT_H__B4848F28_54CD_4385_A299_DC1B9905C3D8__INCLUDED_)