/**
 *  OptGeneralPg.h
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

#if !defined(AFX_OPTGENERALPG_H__8613B051_6B46_487B_87E8_176998981663__INCLUDED_)
#define AFX_OPTGENERALPG_H__8613B051_6B46_487B_87E8_176998981663__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COptGeneralPg dialog

class COptGeneralPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptGeneralPg)

// Construction
public:
	COptGeneralPg();
	~COptGeneralPg();

// Dialog Data
	//{{AFX_DATA(COptGeneralPg)
	enum { IDD = IDD_OPT_GENERAL };
	BOOL	m_bNotifyNew;
	BOOL	m_bMoveToUnread;
	BOOL	m_bClearErrorLog;
	BOOL	m_bWrapTitles;
	int		m_nNavigation;
	//}}AFX_DATA
	CComboBox m_ctlNavigation;
	CComboBox m_ctlEmailAccount;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptGeneralPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HRESULT EnumEmailAccounts();

	// Generated message map functions
	//{{AFX_MSG(COptGeneralPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTGENERALPG_H__8613B051_6B46_487B_87E8_176998981663__INCLUDED_)
