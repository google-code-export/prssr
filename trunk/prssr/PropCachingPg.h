#if !defined(AFX_PROPCACHINGPG_H__901DC31C_81C1_4BBF_9E8D_2F2BD52316DF__INCLUDED_)
#define AFX_PROPCACHINGPG_H__901DC31C_81C1_4BBF_9E8D_2F2BD52316DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropCachingPg.h : header file
//

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPropCachingPg dialog

class CPropCachingPg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropCachingPg)

// Construction
public:
	CPropCachingPg();
	~CPropCachingPg();

// Dialog Data
	//{{AFX_DATA(CPropCachingPg)
	enum { IDD = IDD_PROP_CACHING };
	CButton	m_ctlSpecificCaching;
	CSpinButtonCtrl	m_ctlEnclosureLimitSpin;
	CStatic	m_ctlMBLbl;
	CCeEdit m_ctlMB;
	CStatic	m_ctlLargerLbl;
	CButton	m_ctlStoreImages;
	CButton	m_ctlCacheOnlineContent;
	CButton	m_ctlCacheEnclosures;
	CButton	m_ctlEnclosureLimit;
	BOOL	m_bSpecificCaching;
	BOOL	m_bCacheEnclosures;
	BOOL	m_bCacheOnlineContent;
	BOOL	m_bStoreImages;
	int		m_nMB;
	BOOL	m_bEnclosureLimit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropCachingPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropCachingPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCacheEnclosures();
	afx_msg void OnSpecificCaching();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PropCachingPg_H__901DC31C_81C1_4BBF_9E8D_2F2BD52316DF__INCLUDED_)
