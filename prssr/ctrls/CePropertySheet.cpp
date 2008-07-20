/**
 *  CePropertySheet.cpp
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

#if defined PRSSR_APP
	#include "../stdafx.h"
//	#include "resource.h"
	#include "../prssr.h"
#elif defined PRSSR_TODAY
	#include "../../prssrtoday/stdafx.h"
	#include "../../prssrtoday/prssrtoday.h"
#endif

#include "CePropertySheet.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


PFNPROPSHEETCALLBACK	CCePropertySheet::m_pCallBack = NULL;

CString CCePropertySheet::m_strLink;
CString CCePropertySheet::m_strTitle;


//---------------------------------------------------------------------------
//
//	CCePropertySheet
//
//---------------------------------------------------------------------------


IMPLEMENT_DYNAMIC(CCePropertySheet, CPropertySheet)

CCePropertySheet::CCePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
:	CPropertySheet	(nIDCaption, pParentWnd, iSelectPage),
	m_hwndCmdBar    (NULL),
	m_nMenuID       (0)
{
	m_strTitle.LoadString(nIDCaption);

	HookCallback();
}


CCePropertySheet::CCePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
:	CPropertySheet	(pszCaption, pParentWnd, iSelectPage),
	m_hwndCmdBar    (NULL),
	m_nMenuID       (0)
{
	m_strTitle = pszCaption;

	HookCallback();
}


CCePropertySheet::~CCePropertySheet()
{
}


BEGIN_MESSAGE_MAP(CCePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCePropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CCePropertySheet::CePropertySheetCallBack
//
//		The property sheet callback
//
int CALLBACK CCePropertySheet::CePropertySheetCallBack(HWND hWnd, UINT message, LPARAM lParam)
{
	if(message == PSCB_GETLINKTEXT)
	{
		if(!m_strLink.IsEmpty())
		{
			LPTSTR	pBuf = (LPTSTR)lParam;

			wcscpy(pBuf, m_strLink);
		}
	}
	else if(message == PSCB_GETTITLE)
	{
		if(!m_strTitle.IsEmpty())
		{
			LPTSTR	pBuf = (LPTSTR)lParam;

			wcscpy(pBuf, m_strTitle);
		}
	}
	return (m_pCallBack)(hWnd, message, lParam);
}


// CCePropertySheet::HookCallback
//
//		Hooks the MFC property sheet callback function
//
void CCePropertySheet::HookCallback()
{
	m_pCallBack = m_psh.pfnCallback;

	m_psh.pfnCallback = CePropertySheetCallBack;
}


//---------------------------------------------------------------------------
//
//	CCePropertySheet virtual methods
//
//---------------------------------------------------------------------------


// CCePropertySheet::OnInitDialog
//
//		Initialize the dialog.
//		Create the empty command bar.
//
BOOL CCePropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	SHINITDLGINFO sid;
	sid.dwMask   = SHIDIM_FLAGS;
	sid.dwFlags  = SHIDIF_SIZEDLGFULLSCREEN;
	sid.hDlg     = GetSafeHwnd();
	SHInitDialog(&sid);

	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	if (m_nMenuID == 0) {
//		mbi.dwFlags = SHCMBF_HMENU;
		mbi.dwFlags = SHCMBF_EMPTYBAR;
		mbi.nToolBarId = 0;
	}
	else {
		mbi.dwFlags = SHCMBF_HMENU;
		mbi.nToolBarId = m_nMenuID;
	}
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();

	fSuccess = SHCreateMenuBar(&mbi);
	m_hwndCmdBar = mbi.hwndMB;

	return bResult;
}


//---------------------------------------------------------------------------
//
//	CCePropertySheet message handlers
//
//---------------------------------------------------------------------------



