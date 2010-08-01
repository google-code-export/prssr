/**
 *  OptRetrievalPg.h
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

#if !defined(AFX_OPTRETRIEVALPG_H__24873488_CDA8_4BC6_BA05_D472769B8E67__INCLUDED_)
#define AFX_OPTRETRIEVALPG_H__24873488_CDA8_4BC6_BA05_D472769B8E67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// COptRetrievalPg dialog

class COptRetrievalPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptRetrievalPg)

// Construction
public:
	COptRetrievalPg();
	~COptRetrievalPg();

// Dialog Data
	//{{AFX_DATA(COptRetrievalPg)
	enum { IDD = IDD_OPT_RETRIEVAL };
	CSpinButtonCtrl m_ctlUpdateIntervalSpin;
	BOOL	m_bCheckNewIfCradled;
	int		m_nUpdateInterval;
	CCaption	m_lblOptimizer;
	CButton m_ctlUseOptimizer;
	CCeEdit m_ctlURL;
	CCaption m_lblAutoUpdate;
	CDateTimeCtrl m_ctlTime;
	BOOL m_bUseOptimizer;
	CString m_strURL;
	CCeEdit	m_ctlUpdateInterval;
	CStatic	m_lblMinutes;
	CButton	m_ctlPeriodicUpdate;
	BOOL	m_bPeriodicUpdate;
	CButton	m_ctlTimeUpdate;
	BOOL	m_bTimeUpdate;
	BOOL	m_bBackgroundUpdate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptRetrievalPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(COptRetrievalPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnUseOptimizer();
	afx_msg void OnPeriodicalUpdate();
	afx_msg void OnTimeUpdate();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTRETRIEVALPG_H__24873488_CDA8_4BC6_BA05_D472769B8E67__INCLUDED_)
