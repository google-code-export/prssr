/**
 *  ImportFeedsDlg.h
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined(_IMPORTFEEDSDLG_H_)
#define _IMPORTFEEDSDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Site.h"
#include "ctrls/CeDialog.h"
#include "ctrls/BrowseDlg.h"
#include "ctrls/Hrule.h"
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
