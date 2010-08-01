/**
 *  InfoChannelPg.h
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

#if !defined(AFX_INFOCHANNELPG_H__333A07C0_078A_4710_80FF_D97437007257__INCLUDED_)
#define AFX_INFOCHANNELPG_H__333A07C0_078A_4710_80FF_D97437007257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Hrule.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoChannelPg dialog

class CInfoChannelPg : public CPropertyPage {
	DECLARE_DYNCREATE(CInfoChannelPg)

// Construction
public:
	CInfoChannelPg();
	~CInfoChannelPg();

// Dialog Data
	//{{AFX_DATA(CInfoChannelPg)
	enum { IDD = IDD_INFO_CHANNEL };
	CCeEdit	m_ctlWWW;
	CCeEdit	m_ctlRSS;
	CHrule	m_ctlHrule;
	CString	m_strFormat;
	CString	m_strWWW;
	CString	m_strRSS;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInfoChannelPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brEditBack;

	// Generated message map functions
	//{{AFX_MSG(CInfoChannelPg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_InfoChannelPg_H__333A07C0_078A_4710_80FF_D97437007257__INCLUDED_)
