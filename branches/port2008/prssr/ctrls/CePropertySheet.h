/**
 *  CePropertySheet.h
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

#if !defined(AFX_CEPROPERTYSHEET_H__EC61A6B0_CB70_4280_860A_C85ED7EB1472__INCLUDED_)
#define AFX_CEPROPERTYSHEET_H__EC61A6B0_CB70_4280_860A_C85ED7EB1472__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//---------------------------------------------------------------------------
//
//	CCePropertySheet
//
//---------------------------------------------------------------------------


class CCePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCePropertySheet)

// Construction
public:
	CCePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
	void SetMenu(UINT menuID) { m_nMenuID = menuID; }
	void SetLink(LPCTSTR pszLink) {m_strLink = pszLink;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCePropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCePropertySheet();

	// Generated message map functions
protected:
//	CCeCommandBar*	m_pWndEmptyCB;
	HWND m_hwndCmdBar;
	UINT m_nMenuID;

	void HookCallback();

	static CString				m_strLink;
	static CString				m_strTitle;
	static PFNPROPSHEETCALLBACK	m_pCallBack;

	static int CALLBACK CePropertySheetCallBack(HWND hWnd, UINT message, LPARAM lParam);

	//{{AFX_MSG(CCePropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//---------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEPROPERTYSHEET_H__EC61A6B0_CB70_4280_860A_C85ED7EB1472__INCLUDED_)
