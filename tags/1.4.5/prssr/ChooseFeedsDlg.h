/**
 *  ChooseFeedsDlg.h
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

#if !defined(_CHOOSEFEEDSDLG_H_)
#define _CHOOSEFEEDSDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
