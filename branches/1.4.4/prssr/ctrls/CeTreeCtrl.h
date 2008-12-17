/**
 *  CeTreeCtrl.h
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

#if !defined(AFX_CETREECTRL_H__DF29C8F9_0C86_4917_9CCD_4C41D0954C34__INCLUDED_)
#define AFX_CETREECTRL_H__DF29C8F9_0C86_4917_9CCD_4C41D0954C34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCeTreeCtrl window

class CCeTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CCeTreeCtrl(UINT nID = IDS_NO_ITEMS_TO_DISPLAY);

// Attributes
public:

protected:
	UINT m_nID;
//	CFont m_oSmallFont;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeTreeCtrl();

	// Generated message map functions
protected:
//	void CreateFont();
	//{{AFX_MSG(CCeTreeCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CETREECTRL_H__DF29C8F9_0C86_4917_9CCD_4C41D0954C34__INCLUDED_)
