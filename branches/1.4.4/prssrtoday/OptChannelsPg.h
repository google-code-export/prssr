/**
 *  OptChannelsPg.h
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

#if !defined(_OPTCHANNELSPG_H_INCLUDED_)
#define _OPTCHANNELSPG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../prssr/ctrls/CeListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COptChannelsPg dialog

class COptChannelsPg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptChannelsPg)

// Construction
public:
	COptChannelsPg();
	~COptChannelsPg();

// Dialog Data
	//{{AFX_DATA(COptChannelsPg)
	enum { IDD = IDD_CFG_CHANNELS };
	CCeListCtrl m_ctlChannels;
	CButton m_ctlShowNewChannels;
	BOOL m_bShowNewChannels;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptChannelsPg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(COptChannelsPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_)
