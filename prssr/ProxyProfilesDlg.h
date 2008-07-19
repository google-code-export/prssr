#if !defined(AFX_PROXYPROFILESDLG_H__D19A0F3F_1450_4746_BDC5_81FC7A790AD2__INCLUDED_)
#define AFX_PROXYPROFILESDLG_H__D19A0F3F_1450_4746_BDC5_81FC7A790AD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProxyProfilesDlg.h : header file
//

#include "ctrls/CeDialog.h"
#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"
#include "net/proxy.h"

/////////////////////////////////////////////////////////////////////////////
// CProxyProfileDlg dialog

class CProxyProfileDlg : public CCeDialog
{
// Construction
public:
	CProxyProfileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProxyProfileDlg)
	enum { IDD = IDD_PROXY };
	CCeEdit	m_ctlName;
	CComboBox	m_ctlType;
	CCeEdit	m_ctlHost;
	CCeEdit	m_ctlPort;
	CButton m_ctlUseAuthentication;
	CCeEdit m_ctlUserName;
	CStatic m_lblUserName;
	CCeEdit m_ctlPassword;
	CStatic m_lblPassword;
	CString	m_strHost;
	CString	m_strName;
	CString	m_strPassword;
	int		m_nPort;
	EProxyType	m_nType;
	BOOL	m_bUseAuthentication;
	CString	m_strUserName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxyProfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CProxyProfileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokType();
	afx_msg void OnUseAuthentication();
	//}}AFX_MSG

	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYPROFILESDLG_H__D19A0F3F_1450_4746_BDC5_81FC7A790AD2__INCLUDED_)
