#if !defined(AFX_OPTGENERALPG_H__8613B051_6B46_487B_87E8_176998981663__INCLUDED_)
#define AFX_OPTGENERALPG_H__8613B051_6B46_487B_87E8_176998981663__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptGeneralPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptGeneralPg dialog

class COptGeneralPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptGeneralPg)

// Construction
public:
	COptGeneralPg();
	~COptGeneralPg();

// Dialog Data
	//{{AFX_DATA(COptGeneralPg)
	enum { IDD = IDD_OPT_GENERAL };
	BOOL	m_bNotifyNew;
	BOOL	m_bMoveToUnread;
	BOOL	m_bClearErrorLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptGeneralPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptGeneralPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTGENERALPG_H__8613B051_6B46_487B_87E8_176998981663__INCLUDED_)
