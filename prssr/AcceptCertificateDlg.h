#if !defined(_ACCEPT_CERTIFICATE_H_)
#define _ACCEPT_CERTIFICATE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAcceptCertificateDlg.h : header file
//

#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"
#include "ctrls/CeDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CAcceptCertificateDlg dialog

class CAcceptCertificateDlg : public CCeDialog
{
// Construction
public:
	CAcceptCertificateDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAcceptCertificateDlg)
	enum { IDD = IDD_ACCEPT_CERTIFICATE };
	CStatic m_ctlAlertIcon;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcceptCertificateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	BOOL Trusted;
	BOOL ValidDate;
	BOOL ValidName;

// Implementation
protected:
	HWND m_hwndCmdBar;

	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CAcceptCertificateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_ACCEPT_CERTIFICATE_H_)
