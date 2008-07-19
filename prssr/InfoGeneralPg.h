#if !defined(AFX_INFOGENERALPG_H__89FC53B1_A9D3_4565_BD69_057C1D7EE641__INCLUDED_)
#define AFX_INFOGENERALPG_H__89FC53B1_A9D3_4565_BD69_057C1D7EE641__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoGeneralPg.h : header file
//

#include "ctrls/Hrule.h"
#include "ctrls/Caption.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoGeneralPg dialog

class CInfoGeneralPg : public CPropertyPage {
	DECLARE_DYNCREATE(CInfoGeneralPg)

// Construction
public:
	CInfoGeneralPg();
	~CInfoGeneralPg();

// Dialog Data
	//{{AFX_DATA(CInfoGeneralPg)
	enum { IDD = IDD_INFO_GENERAL };
	CStatic	m_ctlNewItems;
	CStatic	m_ctlPublished;
	CCaption	m_ctlSiteName;
	CHrule	m_ctlHrule1;
	CHrule	m_ctlHrule2;
	CStatic	m_ctlUnreadItems;
	CStatic	m_ctlTotalItems;
	CStatic	m_ctlLastUpdate;
	CStatic	m_lblLastUpdate;
	CStatic	m_ctlLastStatus;
	CStatic	m_ctlFileSize;
	//}}AFX_DATA

	CString m_strSiteName;
	int m_nTotalItems;
	int m_nUnreadItems;
	int m_nNewItems;

	DWORD m_nFileSize;
	SYSTEMTIME m_stLastUpdated;
	SYSTEMTIME m_stPublished;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInfoGeneralPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInfoGeneralPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_InfoGeneralPg_H__89FC53B1_A9D3_4565_BD69_057C1D7EE641__INCLUDED_)
