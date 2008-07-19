#if !defined(AFX_INFOCHANNELPG_H__333A07C0_078A_4710_80FF_D97437007257__INCLUDED_)
#define AFX_INFOCHANNELPG_H__333A07C0_078A_4710_80FF_D97437007257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoChannelPg.h : header file
//

#include "ctrls/Hrule.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoChannelPg dialog

class CInfoChannelPg : public CPropertyPage {
	DECLARE_DYNCREATE(CInfoChannelPg)

// Construction
public:
	CInfoChannelPg();
	~CInfoChannelPg();

// Dialog Data
	//{{AFX_DATA(CInfoChannelPg)
	enum { IDD = IDD_INFO_CHANNEL };
	CCeEdit	m_ctlWWW;
	CCeEdit	m_ctlRSS;
	CHrule	m_ctlHrule;
	CString	m_strFormat;
	CString	m_strWWW;
	CString	m_strRSS;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInfoChannelPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brEditBack;

	// Generated message map functions
	//{{AFX_MSG(CInfoChannelPg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_InfoChannelPg_H__333A07C0_078A_4710_80FF_D97437007257__INCLUDED_)
