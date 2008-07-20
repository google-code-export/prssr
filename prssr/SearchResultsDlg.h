/**
 *  SearchResultsDlg.h
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

#if !defined(_SEARCHRESULTDLG_H_)
#define _SEARCHRESULTDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeDialog.h"
#include "ctrls/CeListCtrl.h"
#include "ctrls/hrule.h"
#include "net/Searcher.h"

/////////////////////////////////////////////////////////////////////////////
// CSearchResultsDlg dialog

class CSearchResultsDlg : public CCeDialog {
// Construction
public:
	CSearchResultsDlg(CWnd *pParentWnd = NULL);
	~CSearchResultsDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_SEARCH_RESULTS_WIDE };
	//{{AFX_DATA(CSearchResultsDlg)
	enum { IDD = IDD_SEARCH_RESULTS };
	CCeListCtrl	m_ctlFeeds;
	CHrule m_ctlHrule;
	CStatic m_ctlDescription;
	//}}AFX_DATA
	CImageList m_ilIcons;

	CList<CSearchResultItem *, CSearchResultItem *> *SearchResults;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchResultsDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	virtual void ResizeControls();

	void InsertItem(CSearchResultItem *item);

	CBrush m_brEditBack;

	// Generated message map functions
	//{{AFX_MSG(CSearchResultsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnSelchangedFeeds(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_IMPORTFEEDSPG_H_)
