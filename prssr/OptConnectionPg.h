/**
 *  OptConnectionPg.h
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

#if !defined(AFX_OPTPROXYPG_H__0AB1AC84_FE0A_4BD6_8F93_17AA4E0BB408__INCLUDED_)
#define AFX_OPTPROXYPG_H__0AB1AC84_FE0A_4BD6_8F93_17AA4E0BB408__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"

struct CProxyProfile;

/////////////////////////////////////////////////////////////////////////////
// COptConnectionPg dialog

class COptConnectionPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptConnectionPg)

// Construction
public:
	COptConnectionPg();
	~COptConnectionPg();

// Dialog Data
	//{{AFX_DATA(COptConnectionPg)
	enum { IDD = IDD_OPT_CONNECTION };
	CComboBox m_ctlProxies;
	BOOL	m_bAutoConnect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptConnectionPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	void AddProfile(CProxyProfile *prf);

	// Generated message map functions
	//{{AFX_MSG(COptConnectionPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnRemove();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTPROXYPG_H__0AB1AC84_FE0A_4BD6_8F93_17AA4E0BB408__INCLUDED_)
