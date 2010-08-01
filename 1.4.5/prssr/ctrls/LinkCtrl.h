/**
 *  LinkCtrl.h
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

#if !defined(AFX_LINKCTRL_H__47977831_311F_46D3_AA72_814FF93B1E9D__INCLUDED_)
#define AFX_LINKCTRL_H__47977831_311F_46D3_AA72_814FF93B1E9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLinkCtrl window

class CLinkCtrl : public CStatic
{
// Construction
public:
	CLinkCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLinkCtrl();

	// Generated message map functions
protected:
	CFont m_fntUnderlined;

	//{{AFX_MSG(CLinkCtrl)
//	afx_msg void OnClicked();
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKCTRL_H__47977831_311F_46D3_AA72_814FF93B1E9D__INCLUDED_)
