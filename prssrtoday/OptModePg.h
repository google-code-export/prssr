#if !defined(AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_)
#define AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptModePg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptModePg dialog

class COptModePg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptModePg)

// Construction
public:
	COptModePg();
	~COptModePg();

// Dialog Data
	//{{AFX_DATA(COptModePg)
	enum { IDD = IDD_CFG_MODE };
	CComboBox	m_ctlMode;
	CButton		m_ctlSiteNames;
	CButton		m_ctlFeedItems;
	CButton		m_ctlShowSiteName;
	CButton		m_ctlShowDateTime;
	CStatic		m_ctlDisplaySpeedLbl;
	CComboBox	m_ctlDisplaySpeed;
	CButton	m_ctlShowOnlyNew;
	CSpinButtonCtrl	m_ctlHeightSpin;
	int		m_nMode;
	int		m_nCyclingSubMode;
	BOOL	m_bShowSiteName;
	BOOL	m_bShowDateTime;
	BOOL	m_bShowOnlyNew;
	int		m_nDisplaySpeedIdx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptModePg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	void UpdateControlsCycling(BOOL enabled = TRUE);
	void UpdateControlsTree(BOOL enabled = TRUE);
	// Generated message map functions
	//{{AFX_MSG(COptModePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCycleTitles();
	afx_msg void OnSelendokMode();
	afx_msg void OnSiteNames();
	afx_msg void OnFeedItems();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_)
