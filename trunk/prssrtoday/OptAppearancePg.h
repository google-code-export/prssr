#if !defined(AFX_OPTAPPEARANCEPG_H__8436A89D_9B3C_45AB_AA84_B0076604A8FA__INCLUDED_)
#define AFX_OPTAPPEARANCEPG_H__8436A89D_9B3C_45AB_AA84_B0076604A8FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptAppearancePg.h : header file
//

#include "../prssr/ctrls/Caption.h"

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg dialog

class COptAppearancePg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptAppearancePg)

// Construction
public:
	COptAppearancePg();
	~COptAppearancePg();

// Dialog Data
	//{{AFX_DATA(COptAppearancePg)
	enum { IDD = IDD_CFG_APPEARANCE };
	CEdit	m_ctlHeight;
	CStatic	m_ctlPluginHeightLbl;
	CButton	m_ctlUseFontSize;
	CComboBox	m_ctlFontSize;
	BOOL	m_bUseFontSize;
	CButton	m_ctlDisplayIcon;
	BOOL	m_bDisplayIcon;
	CButton	m_ctlHidePlugin;
	BOOL	m_bHidePlugin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptAppearancePg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(COptAppearancePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsefontsize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OptAppearancePg_H__8436A89D_9B3C_45AB_AA84_B0076604A8FA__INCLUDED_)
