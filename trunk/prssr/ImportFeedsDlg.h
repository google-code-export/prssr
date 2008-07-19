#if !defined(_IMPORTFEEDSDLG_H_)
#define _IMPORTFEEDSDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportFeedsPg.h : header file
//

#include "Site.h"
#include "ctrls/CeDialog.h"
#include "ctrls/BrowseDlg.h"
#include "ctrls/hrule.h"
#include "FeedsTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CImportDlg control

class CImportDlg : public CBrowseDlg {
public:
	CImportDlg(CWnd *pParent = NULL);

	// Generated message map functions
	//{{AFX_MSG(CImportDlg)
	afx_msg void OnImport();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CImportFeedsDlg dialog

class CImportFeedsDlg : public CCeDialog {
// Construction
public:
	CImportFeedsDlg(CWnd *pParentWnd = NULL);
	~CImportFeedsDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_IMPORT_FEEDS_WIDE };
	//{{AFX_DATA(CImportFeedsDlg)
	enum { IDD = IDD_IMPORT_FEEDS };
	CFeedsTreeCtrl	m_ctlFeeds;
	CHrule m_ctlHrule;
	CButton	m_ctlAppendFeeds;
	BOOL m_bAppendFeeds;
	//}}AFX_DATA
	CImageList m_ilIcons;
	CSiteList *SiteList;
	BOOL m_bDefaultItemCheck;					// if the item if checked/unchecked by default

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImportFeedsDlg)
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

	void InsertItems(HTREEITEM hParent, CSiteItem *item);
	void BuildSiteList(HTREEITEM hParent, CSiteItem *parent);

	// Generated message map functions
	//{{AFX_MSG(CImportFeedsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_IMPORTFEEDSPG_H_)
