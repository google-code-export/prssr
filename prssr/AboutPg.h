/**
 *  AboutPg.h : header file
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

#if !defined(AFX_BATTIABOUTPG_H__09295F7D_AC63_4C8C_B9EC_82EE35D48DB1__INCLUDED_)
#define AFX_BATTIABOUTPG_H__09295F7D_AC63_4C8C_B9EC_82EE35D48DB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CePropertyPage.h"
#include "ctrls/linkctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutPg dialog

class CAboutPg : public CCePropertyPage {
	DECLARE_DYNCREATE(CAboutPg)

// Construction
public:
	CAboutPg();
	~CAboutPg();

// Dialog Data
	//{{AFX_DATA(CAboutPg)
	enum { IDD = IDD_ABOUTBOX, IDD_WIDE = IDD_ABOUTBOX_WIDE };
	CLinkCtrl	m_ctlHomepageLink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAboutPg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFont m_fntUnderlined;

	// Generated message map functions
	//{{AFX_MSG(CAboutPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHomepageLink();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	void ResizeControls();

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATTIABOUTPG_H__09295F7D_AC63_4C8C_B9EC_82EE35D48DB1__INCLUDED_)
