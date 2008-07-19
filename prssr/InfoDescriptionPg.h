#if !defined(AFX_INFODESCRIPTIONPG_H__80352AD3_8952_40D5_9ABC_EC185756E414__INCLUDED_)
#define AFX_INFODESCRIPTIONPG_H__80352AD3_8952_40D5_9ABC_EC185756E414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoDescriptionPg.h : header file
//

#include "ctrls/Hrule.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoDescriptionPg dialog

class CInfoDescriptionPg : public CPropertyPage {
	DECLARE_DYNCREATE(CInfoDescriptionPg)

// Construction
public:
	CInfoDescriptionPg();
	~CInfoDescriptionPg();

// Dialog Data
	//{{AFX_DATA(CInfoDescriptionPg)
	enum { IDD = IDD_INFO_DESCRIPTION };
	CHrule	m_ctlHrule;
	CString	m_strCopyright;
	CString	m_strDescription;
	CString	m_strLanguage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInfoDescriptionPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInfoDescriptionPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_InfoDescriptionPg_H__80352AD3_8952_40D5_9ABC_EC185756E414__INCLUDED_)
