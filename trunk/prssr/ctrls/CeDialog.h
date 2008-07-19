#if !defined(AFX_CEDIALOG_H__96E58831_D1CA_4017_AA0F_27043AB58132__INCLUDED_)
#define AFX_CEDIALOG_H__96E58831_D1CA_4017_AA0F_27043AB58132__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCeDialog window

class CCeDialog : public CDialog
{
// Construction
public:
	CCeDialog();
	CCeDialog(UINT nTemplateID, CWnd *pParent = NULL);

// Attributes
public:

// Operations
public:
	void SetMenu(UINT menuId) { m_nMenuID = menuId; }
	void SetCaption(const CString &caption) { m_strCaption = caption; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeDialog)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeDialog();

//	void SessionRestore();

	// Generated message map functions
protected:
	UINT m_nMenuID;
	CString m_strCaption;
	HWND m_hwndCmdBar;

	int Mode;

	void DrawCaption(CDC &dc);

	//{{AFX_MSG(CCeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void ResizeControls();

	static const int CAPTION_PANEL_HEIGHT;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDIALOG_H__96E58831_D1CA_4017_AA0F_27043AB58132__INCLUDED_)
