/**
 *  CePropertyPage.h
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

#if !defined(AFX_CEPROPERTYPAGE_H__C2B9A61A_7ED0_40D8_807D_73441E2CA384__INCLUDED_)
#define AFX_CEPROPERTYPAGE_H__C2B9A61A_7ED0_40D8_807D_73441E2CA384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCePropertyPage dialog

class CCePropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCePropertyPage)

// Construction
public:
	CCePropertyPage();
	CCePropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
	~CCePropertyPage();

// Dialog Data
	//{{AFX_DATA(CCePropertyPage)
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCePropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL WideMode;	

	virtual void ResizeControls();

	// Generated message map functions
	//{{AFX_MSG(CCePropertyPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEPROPERTYPAGE_H__C2B9A61A_7ED0_40D8_807D_73441E2CA384__INCLUDED_)
