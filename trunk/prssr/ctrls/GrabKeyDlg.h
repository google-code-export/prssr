#if !defined(AFX_GRABKEYDLG_H__2A54DF08_1AC4_4B0F_BC84_8D2A9197AC83__INCLUDED_)
#define AFX_GRABKEYDLG_H__2A54DF08_1AC4_4B0F_BC84_8D2A9197AC83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrabKeyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrabKeyDlg dialog

class CGrabKeyDlg : public CDialog
{
// Construction
public:
	CGrabKeyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrabKeyDlg)
	enum { IDD = IDD_GRAB_KEY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int VKey;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrabKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrabKeyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRABKEYDLG_H__2A54DF08_1AC4_4B0F_BC84_8D2A9197AC83__INCLUDED_)
