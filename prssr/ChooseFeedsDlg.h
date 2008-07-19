#if !defined(_CHOOSEFEEDSDLG_H_)
#define _CHOOSEFEEDSDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseFeedsDlg.h : header file
//

#include "Site.h"
#include "ctrls/CeDialog.h"

struct CHtmlFeed;

/////////////////////////////////////////////////////////////////////////////
// CChooseFeedsDlg dialog

class CChooseFeedsDlg : public CCeDialog {
// Construction
public:
	CChooseFeedsDlg(CWnd *pParentWnd = NULL);
	~CChooseFeedsDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_CHOOSE_FEEDS_WIDE };
	//{{AFX_DATA(CChooseFeedsDlg)
	enum { IDD = IDD_CHOOSE_FEEDS };
	CListCtrl m_ctlFeeds;
	//}}AFX_DATA
	CImageList m_ilIcons;

	CList<CHtmlFeed *, CHtmlFeed *> Feeds;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CChooseFeedsDlg)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nGroupIcon;					// index of the group icon (in image list)

	void UpdateControls();
	virtual void ResizeControls();

	void InsertItem(int n, CHtmlFeed *item);

	// Generated message map functions
	//{{AFX_MSG(CChooseFeedsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_IMPORTFEEDSPG_H_)
