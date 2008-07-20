/**
 *  PropCachingPg.h
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

#if !defined(AFX_PROPCACHINGPG_H__901DC31C_81C1_4BBF_9E8D_2F2BD52316DF__INCLUDED_)
#define AFX_PROPCACHINGPG_H__901DC31C_81C1_4BBF_9E8D_2F2BD52316DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPropCachingPg dialog

class CPropCachingPg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropCachingPg)

// Construction
public:
	CPropCachingPg();
	~CPropCachingPg();

// Dialog Data
	//{{AFX_DATA(CPropCachingPg)
	enum { IDD = IDD_PROP_CACHING };
	CButton	m_ctlSpecificCaching;
	CSpinButtonCtrl	m_ctlEnclosureLimitSpin;
	CStatic	m_ctlMBLbl;
	CCeEdit m_ctlMB;
	CStatic	m_ctlLargerLbl;
	CButton	m_ctlStoreImages;
	CButton	m_ctlCacheOnlineContent;
	CButton	m_ctlCacheEnclosures;
	CButton	m_ctlEnclosureLimit;
	BOOL	m_bSpecificCaching;
	BOOL	m_bCacheEnclosures;
	BOOL	m_bCacheOnlineContent;
	BOOL	m_bStoreImages;
	int		m_nMB;
	BOOL	m_bEnclosureLimit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropCachingPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropCachingPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCacheEnclosures();
	afx_msg void OnSpecificCaching();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PropCachingPg_H__901DC31C_81C1_4BBF_9E8D_2F2BD52316DF__INCLUDED_)
