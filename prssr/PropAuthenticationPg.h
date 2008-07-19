#if !defined(AFX_PROPAUTHENTICATIONPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
#define AFX_PROPAUTHENTICATIONPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropAuthenticationPg.h : header file
//

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPropAuthenticationPg dialog

class CPropAuthenticationPg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropAuthenticationPg)

// Construction
public:
	CPropAuthenticationPg();
	~CPropAuthenticationPg();

// Dialog Data
	//{{AFX_DATA(CPropAuthenticationPg)
	enum { IDD = IDD_PROP_AUTHENTICATION };
	CStatic	m_ctlUserNameLbl;
	CStatic	m_ctlPasswordLbl;
	CCeEdit	m_ctlUserName;
	CCeEdit	m_ctlPassword;
	CButton	m_ctlAuthRequired;
	CString	m_strPassword;
	CString	m_strUserName;
	BOOL	m_bAuthRequired;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropAuthenticationPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropAuthenticationPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAuthRequired();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PropAuthenticationPg_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
