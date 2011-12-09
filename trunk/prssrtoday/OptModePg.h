/**
 *  OptModePg.h
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

#if !defined(AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_)
#define AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COptModePg dialog

class COptModePg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptModePg)

// Construction
public:
	COptModePg();
	~COptModePg();

// Dialog Data
	//{{AFX_DATA(COptModePg)
	enum { IDD = IDD_CFG_MODE };
	CComboBox	m_ctlMode;
	CButton		m_ctlSiteNames;
	CButton		m_ctlFeedItems;
	CButton		m_ctlShowSiteName;
	CButton		m_ctlShowDateTime;
	CButton		m_ctlShowDateTimeTail;
	CStatic		m_ctlDisplaySpeedLbl;
	CComboBox	m_ctlDisplaySpeed;
	CStatic		m_ctlDisplaySpeedSecs;
	CButton	m_ctlShowOnlyNew;
	CSpinButtonCtrl	m_ctlHeightSpin;
	CButton		m_ctlShowTodayWrap;
	CStatic		m_ctlDisplayRowsLbl;
	CComboBox	m_ctlDisplayRows;

	int		m_nMode;
	int		m_nCyclingSubMode;
	BOOL	m_bShowSiteName;
	BOOL	m_bShowDateTime;
	BOOL	m_bShowDateTimeTail;
	BOOL	m_bShowOnlyNew;
	int		m_nDisplaySpeedIdx;
	BOOL	m_bShowTodayWrap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptModePg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	void UpdateControlsCycling(BOOL enabled = TRUE);
	void UpdateControlsTree(BOOL enabled = TRUE);
	// Generated message map functions
	//{{AFX_MSG(COptModePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCycleTitles();
	afx_msg void OnSelendokMode();
	afx_msg void OnSiteNames();
	afx_msg void OnFeedItems();
	afx_msg void OnShowTodayWrap();
	afx_msg void OnShowDateTime();
	afx_msg void OnShowDateTimeTail();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_)
