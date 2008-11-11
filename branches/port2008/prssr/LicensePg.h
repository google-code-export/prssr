/**
 *  LicensePg.h
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

#if !defined(_LICENSEPG_H_)
#define _LICENSEPG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CePropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// CLicensePg dialog

class CLicensePg : public CCePropertyPage {
	DECLARE_DYNCREATE(CLicensePg)

// Construction
public:
	CLicensePg();
	~CLicensePg();

// Dialog Data
	//{{AFX_DATA(CLicensePg)
	enum { IDD = IDD_LICENSE };
	//}}AFX_DATA
	CEdit m_ctlLicense;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLicensePg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLicensePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TMAN_LICENSEPG_H_)
