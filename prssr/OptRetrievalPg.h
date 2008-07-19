#if !defined(AFX_OPTRETRIEVALPG_H__24873488_CDA8_4BC6_BA05_D472769B8E67__INCLUDED_)
#define AFX_OPTRETRIEVALPG_H__24873488_CDA8_4BC6_BA05_D472769B8E67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptRetrievalPg.h : header file
//

#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// COptRetrievalPg dialog

class COptRetrievalPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptRetrievalPg)

// Construction
public:
	COptRetrievalPg();
	~COptRetrievalPg();

// Dialog Data
	//{{AFX_DATA(COptRetrievalPg)
	enum { IDD = IDD_OPT_RETRIEVAL };
	CComboBox	m_ctlUserAgent;
	CSpinButtonCtrl m_ctlUpdateIntervalSpin;
	BOOL	m_bCheckNewIfCradled;
	int		m_nUserAgent;
	int		m_nUpdateInterval;
	CCaption	m_lblOptimizer;
	CButton m_ctlUseOptimizer;
	CCeEdit m_ctlURL;
	CCaption m_lblAutoUpdate;
	CDateTimeCtrl m_ctlTime;
	CCaption	m_lblMisc;
	BOOL m_bUseOptimizer;
	CString m_strURL;
	CCeEdit	m_ctlUpdateInterval;
	CStatic	m_lblMinutes;
	CButton	m_ctlPeriodicUpdate;
	BOOL	m_bPeriodicUpdate;
	CButton	m_ctlTimeUpdate;
	BOOL	m_bTimeUpdate;
	BOOL	m_bBackgroundUpdate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptRetrievalPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(COptRetrievalPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnUseOptimizer();
	afx_msg void OnPeriodicalUpdate();
	afx_msg void OnTimeUpdate();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTRETRIEVALPG_H__24873488_CDA8_4BC6_BA05_D472769B8E67__INCLUDED_)
