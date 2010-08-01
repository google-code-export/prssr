/**
 *  OptAppearancePg.h
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


#if !defined(AFX_OPTAPPEARANCEPG_H__8436A89D_9B3C_45AB_AA84_B0076604A8FA__INCLUDED_)
#define AFX_OPTAPPEARANCEPG_H__8436A89D_9B3C_45AB_AA84_B0076604A8FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../prssr/ctrls/Caption.h"

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg dialog

class COptAppearancePg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptAppearancePg)

// Construction
public:
	COptAppearancePg();
	~COptAppearancePg();

// Dialog Data
	//{{AFX_DATA(COptAppearancePg)
	enum { IDD = IDD_CFG_APPEARANCE };
	CEdit	m_ctlHeight;
	CStatic	m_ctlPluginHeightLbl;
	CButton	m_ctlUseFontSize;
	CComboBox	m_ctlFontSize;
	BOOL	m_bUseFontSize;
	CButton	m_ctlDisplayIcon;
	BOOL	m_bDisplayIcon;
	CButton	m_ctlHidePlugin;
	BOOL	m_bHidePlugin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptAppearancePg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(COptAppearancePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsefontsize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OptAppearancePg_H__8436A89D_9B3C_45AB_AA84_B0076604A8FA__INCLUDED_)
