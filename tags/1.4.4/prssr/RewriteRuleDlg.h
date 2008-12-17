/**
 *  RewriteRuleDlg.h
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

#if !defined(AFX_REWRITERULEDLG_H__INCLUDED_)
#define AFX_REWRITERULEDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeDialog.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CRewriteRuleDlg dialog

class CRewriteRuleDlg : public CCeDialog {
// Construction
public:
	CRewriteRuleDlg(CWnd *pParent = NULL);   // standard constructor
	~CRewriteRuleDlg();

// Dialog Data
	//{{AFX_DATA(CRewriteRuleDlg)
	enum { IDD = IDD_REWRITE_RULE };
	CCeEdit	m_ctlMatch;
	CCeEdit	m_ctlReplace;
	CString m_strMatch;
	CString m_strReplace;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRewriteRuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRewriteRuleDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REWRITERULEDLG_H__INCLUDED_)
