/**
 *  PropUpdatingPg.h
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

#if !defined(AFX_PROPUPDATINGPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
#define AFX_PROPUPDATINGPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPropUpdatingPg dialog

class CPropUpdatingPg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropUpdatingPg)

// Construction
public:
	CPropUpdatingPg();
	~CPropUpdatingPg();

// Dialog Data
	//{{AFX_DATA(CPropUpdatingPg)
	enum { IDD = IDD_PROP_UPDATING };
	CStatic m_ctlSuggestInterval;
	CSpinButtonCtrl m_ctlUpdateMinutesSpin;
	int	m_nUpdate;
	int	m_nUpdateMinutes;
	//}}AFX_DATA
	int m_nSuggestInterval;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropUpdatingPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropUpdatingPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPUPDATINGPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
