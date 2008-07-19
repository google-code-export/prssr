#if !defined(_UPDATESITESDLG_H_)
#define _UPDATESITESDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportFeedsPg.h : header file
//

#include "Site.h"
#include "ctrls/CeDialog.h"
#include "ctrls/hrule.h"
#include "FeedsTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CUpdateSitesDlg dialog

class CUpdateSitesDlg : public CCeDialog {
// Construction
public:
	CUpdateSitesDlg(CWnd *pParentWnd = NULL);
	~CUpdateSitesDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_UPDATE_SITES_WIDE };
	//{{AFX_DATA(CUpdateSitesDlg)
	enum { IDD = IDD_UPDATE_SITES };
	CFeedsTreeCtrl	m_ctlSites;
	CHrule m_ctlHrule;
	CButton	m_ctlUpdateOnly;
	CButton	m_ctlSelectAll;
	BOOL m_bUpdateOnly;
	//}}AFX_DATA
	CImageList m_ilIcons;
	CList<CSiteItem *, CSiteItem *> m_oUpdateSites;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUpdateSitesDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nGroupIcon;					// index of the group icon (in image list)

	void UpdateControls();
	virtual void ResizeControls();

	void InsertItems(HTREEITEM parent, CSiteItem *item);
	void GetCheckedItems(HTREEITEM parent);

	// Generated message map functions
	//{{AFX_MSG(CUpdateSitesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	afx_msg void OnSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_UPDATESITESDLG_H_)
