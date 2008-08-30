/**
 *  PropRewritingPg.h
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

#if !defined(AFX_PROPREWRITINGPG_H_INCLUDED_)
#define AFX_PROPREWRITINGPG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeListCtrl.h"
#include "ctrls/CePropertyPage.h"

struct CRewriteRule;

/////////////////////////////////////////////////////////////////////////////
// CPropRewritingPg dialog

class CPropRewritingPg : public CCePropertyPage {
	DECLARE_DYNCREATE(CPropRewritingPg)

// Construction
public:
	CPropRewritingPg();
	~CPropRewritingPg();

// Dialog Data
	//{{AFX_DATA(CPropRewritingPg)
	enum { IDD = IDD_PROP_REWRITING };
	CCeListCtrl m_ctlRules;
	//}}AFX_DATA
	CArray<CRewriteRule *, CRewriteRule *> Rules;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropRewritingPg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InsertRule(int idx, CRewriteRule *rule);
	void EditRule(int item);

	void UpdateControls();
	virtual void ResizeControls();
	// Generated message map functions
	//{{AFX_MSG(CPropRewritingPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeydownRules(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickRules(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	afx_msg void OnRemove();
	afx_msg void OnUpdateRemove(CCmdUI *pCmdUI);

	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPREWRITINGPG_H_INCLUDED_)
