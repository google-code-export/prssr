#if !defined(AFX_PROPGENERALPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
#define AFX_PROPGENERALPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropGeneralPg.h : header file
//

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPropGeneralPg dialog

class CPropGeneralPg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropGeneralPg)

// Construction
public:
	CPropGeneralPg();
	~CPropGeneralPg();

// Dialog Data
	//{{AFX_DATA(CPropGeneralPg)
	enum { IDD = IDD_PROP_GENERAL };
	CCeEdit m_ctlName;
	CCeEdit m_ctlURL;
	CString m_strName;
	CString m_strURL;
	BOOL m_bShowOnToday;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropGeneralPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropGeneralPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPGENERALPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
