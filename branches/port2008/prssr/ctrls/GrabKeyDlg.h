/**
 *  GrabKeyDlg.h
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

#if !defined(AFX_GRABKEYDLG_H__2A54DF08_1AC4_4B0F_BC84_8D2A9197AC83__INCLUDED_)
#define AFX_GRABKEYDLG_H__2A54DF08_1AC4_4B0F_BC84_8D2A9197AC83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGrabKeyDlg dialog

class CGrabKeyDlg : public CDialog
{
// Construction
public:
	CGrabKeyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrabKeyDlg)
	enum { IDD = IDD_GRAB_KEY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int VKey;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrabKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrabKeyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRABKEYDLG_H__2A54DF08_1AC4_4B0F_BC84_8D2A9197AC83__INCLUDED_)
