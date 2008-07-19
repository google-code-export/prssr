#if !defined(AFX_AUTHENTICATIONDLG_H__E0D7AD51_DB89_4DFE_AB9C_5646EF132EDC__INCLUDED_)
#define AFX_AUTHENTICATIONDLG_H__E0D7AD51_DB89_4DFE_AB9C_5646EF132EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuthenticationDlg.h : header file
//

#include "ctrls/CeDialog.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CAuthenticationPg dialog

class CAuthenticationPg : public CPropertyPage
{
// Construction
public:
	CAuthenticationPg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAuthenticationPg)
	enum { IDD = IDD_AUTHENTICATION };
	CString	m_strSite;
	CString	m_strRealm;
	CCeEdit	m_ctlUserName;
	CCeEdit	m_ctlPassword;
	CString	m_strPassword;
	CString	m_strUserName;
	BOOL m_bSavePassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuthenticationPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CAuthenticationPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTHENTICATIONDLG_H__E0D7AD51_DB89_4DFE_AB9C_5646EF132EDC__INCLUDED_)
