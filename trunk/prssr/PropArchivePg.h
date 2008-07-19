#if !defined(AFX_PROPARCHIVEPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
#define AFX_PROPARCHIVEPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropArchivePg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropArchivePg dialog

class CPropArchivePg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropArchivePg)

// Construction
public:
	CPropArchivePg();
	~CPropArchivePg();

// Dialog Data
	//{{AFX_DATA(CPropArchivePg)
	enum { IDD = IDD_PROP_ARCHIVE };
	CSpinButtonCtrl m_ctlCacheItemsSpin;
	int	m_nCacheLimit;
	int	m_nCacheItems;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropArchivePg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropArchivePg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPARCHIVEPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
