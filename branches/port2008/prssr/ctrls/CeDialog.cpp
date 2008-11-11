/**
 *  CeDialog.cpp
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

#include "../StdAfx.h"
#include "CeDialog.h"
#include "../../share/helpers.h"
#include "../Appearance.h"

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

const int CCeDialog::CAPTION_PANEL_HEIGHT = 24;

/////////////////////////////////////////////////////////////////////////////
// CCeDialog

CCeDialog::CCeDialog() {
	m_nMenuID = 0;
}

CCeDialog::CCeDialog(UINT nTemplateID, CWnd *pParent/* = NULL*/) :
	CDialog(nTemplateID, pParent)
{
	m_nMenuID = 0;
	Mode = -1;
}

CCeDialog::~CCeDialog()
{
}


BEGIN_MESSAGE_MAP(CCeDialog, CDialog)
	//{{AFX_MSG_MAP(CCeDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCeDialog message handlers

BOOL CCeDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	
/*	SetFont(&Appearance.BaseFont);
	CWnd *child = GetWindow(GW_CHILD);
	while (child != NULL) {
		child->SetFont(&Appearance.BaseFont);
		child = child->GetWindow(GW_HWNDNEXT);
	}
*/

	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();
	if (m_nMenuID == 0) {
		mbi.dwFlags = SHCMBF_EMPTYBAR;
		mbi.nToolBarId = 0;
	}
	else {
		mbi.dwFlags = SHCMBF_HMENU;
		mbi.nToolBarId = m_nMenuID;
	}

	fSuccess = SHCreateMenuBar(&mbi);
	m_hwndCmdBar = mbi.hwndMB;

	if (m_strCaption.IsEmpty())
		GetWindowText(m_strCaption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCeDialog::DrawCaption(CDC &dc) {
	CRect rtPage;
	GetClientRect(&rtPage);

	if (!m_strCaption.IsEmpty()) {
		// Paint caption

		CFont* pCurrentFont = dc.GetCurrentFont();
		LOGFONT lf;
		pCurrentFont->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = SCALEX(12) + 1;
		CFont newFont;
		newFont.CreateFontIndirect(&lf);

		CFont* pOldFont = dc.SelectObject(&newFont);
		dc.SetTextColor(::GetSysColor(COLOR_ACTIVECAPTION));

		DrawTextEndEllipsis(dc, m_strCaption,
			CRect(SCALEX(8), 0, rtPage.right - SCALEX(8), SCALEY(CAPTION_PANEL_HEIGHT)),
			DT_VCENTER | DT_SINGLELINE);
		dc.SelectObject(pOldFont);

		// Paint lines

		CPen blackPen(PS_SOLID, SCALEX(1), RGB(0,0,0));
		CPen *pOldPen = dc.SelectObject(&blackPen);

		dc.MoveTo(0, SCALEY(CAPTION_PANEL_HEIGHT) - 1);
		dc.LineTo(rtPage.right, SCALEY(CAPTION_PANEL_HEIGHT) - 1);

		dc.SelectObject(pOldPen);
	}
}

void CCeDialog::OnPaint() {
	CPaintDC dc(this); // device context for painting

	DrawCaption(dc);
}

void CCeDialog::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);

	int width = GetSystemMetrics(SM_CXSCREEN);	
	if (width == cx && Mode != InWideMode()) {
		Mode = InWideMode();
		ResizeControls();
	}
}

void CCeDialog::ResizeControls() {
}

void CCeDialog::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) {
	LOG0(5, "CCeDialog::OnInitMenuPopup()");

	if (bSysMenu)
		return; // don't support system menu
	
	ASSERT(pMenu != NULL); // check the enabled state of various menu items
	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL); 
	ASSERT(state.m_pParentMenu == NULL); 	

	// determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup)
/*
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu; // parent == child for tracking popup
	else if ((hParentMenu = ::WCE_FCTN(GetMenu)(m_hWnd)) != NULL) {
		CWnd *pParent = GetTopLevelParent(); // child windows don't have menus -- need to go to the top!
	
		if (pParent != NULL &&
			(hParentMenu = ::WCE_FCTN(GetMenu)(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::WCE_FCTN(GetMenuItemCount)(hParentMenu); 
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu) {
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}
*/
	CMenu *pParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu; // parent == child for tracking popup
	else if ((pParentMenu = GetMenu()) != NULL) {
		CWnd *pParent = GetTopLevelParent(); // child windows don't have menus -- need to go to the top!

		if (pParent != NULL &&
			((pParentMenu = pParent->GetMenu()) != NULL))
		{
			int nIndexMax = pParentMenu->GetMenuItemCount();
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				if (pParentMenu->GetSubMenu(nIndex) == pMenu) {
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = pParentMenu;
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (pMenu->GetSubMenu(state.m_nIndex) != NULL) {
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue; // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE); // popups are never auto disabled
		}
		else {
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable' 
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, TRUE);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax) {
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	} 
}
