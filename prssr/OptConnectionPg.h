#if !defined(AFX_OPTPROXYPG_H__0AB1AC84_FE0A_4BD6_8F93_17AA4E0BB408__INCLUDED_)
#define AFX_OPTPROXYPG_H__0AB1AC84_FE0A_4BD6_8F93_17AA4E0BB408__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptProxyPg.h : header file
//

#include "ctrls/CeEdit.h"

struct CProxyProfile;

/////////////////////////////////////////////////////////////////////////////
// COptConnectionPg dialog

class COptConnectionPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptConnectionPg)

// Construction
public:
	COptConnectionPg();
	~COptConnectionPg();

// Dialog Data
	//{{AFX_DATA(COptConnectionPg)
	enum { IDD = IDD_OPT_CONNECTION };
	CComboBox m_ctlProxies;
	BOOL	m_bAutoConnect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptConnectionPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	void AddProfile(CProxyProfile *prf);

	// Generated message map functions
	//{{AFX_MSG(COptConnectionPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnRemove();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTPROXYPG_H__0AB1AC84_FE0A_4BD6_8F93_17AA4E0BB408__INCLUDED_)
