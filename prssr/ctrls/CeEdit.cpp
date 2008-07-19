// CeEdit.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\prssr.h"
#include "CeEdit.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCeEdit

CCeEdit::CCeEdit() {
}

CCeEdit::~CCeEdit() {
	// to avoid assert in wincore.cpp, line 1143
	HWND hWnd = Detach();
	::DestroyWindow(hWnd);
}


BEGIN_MESSAGE_MAP(CCeEdit, CEdit)
	//{{AFX_MSG_MAP(CCeEdit)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_WM_INITMENUPOPUP()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
//	ON_NOTIFY_REFLECT(GN_CONTEXTMENU, OnContextMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCeEdit message handlers


void CCeEdit::OnLButtonDown(UINT nFlags, CPoint point) {
    SHRGINFO shrgi = {0};

    shrgi.cbSize        = sizeof(SHRGINFO);
    shrgi.hwndClient    = m_hWnd;
    shrgi.ptDown.x      = point.x;
    shrgi.ptDown.y      = point.y;
    shrgi.dwFlags       = SHRG_RETURNCMD;

    if(GN_CONTEXTMENU == ::SHRecognizeGesture(&shrgi))
        ContextMenu(point);
    else
        Default();
}

void CCeEdit::ContextMenu(CPoint point) {
	CMenu mnuCtxt;

	if (mnuCtxt.LoadMenu(IDR_EDIT)) {
		CMenu *pMenu = mnuCtxt.GetSubMenu(0);
		if (pMenu) {
			ClientToScreen(&point);
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	}
}

void CCeEdit::OnEditCopy() {
	LOG0(5, "CCeEdit::OnEditCopy()");

	Copy();
}

void CCeEdit::OnEditCut() {
	LOG0(5, "CCeEdit::OnEditCut()");

	Cut();
}

void CCeEdit::OnEditPaste() {
	LOG0(5, "CCeEdit::OnEditPaste()");

	Paste();
}

void CCeEdit::OnUpdateEditCopy(CCmdUI* pCmdUI) {
	int start, end;
	GetSel(start, end);
	pCmdUI->Enable(start < end);
}

void CCeEdit::OnUpdateEditCut(CCmdUI* pCmdUI) {
	int start, end;
	GetSel(start, end);
	pCmdUI->Enable(start < end && !(GetStyle() & ES_READONLY));
}

void CCeEdit::OnUpdateEditPaste(CCmdUI* pCmdUI) {
	if (OpenClipboard()) {
		pCmdUI->Enable(IsClipboardFormatAvailable(CF_UNICODETEXT) && !(GetStyle() & ES_READONLY));
		CloseClipboard();
	}
	else
		pCmdUI->Enable(FALSE);
}

void CCeEdit::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) {
	if (bSysMenu)
		return; // don't support system menu
	
	ASSERT(pMenu != NULL); // check the enabled state of various menu items
	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL); 
	ASSERT(state.m_pParentMenu == NULL); 	

	// determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup)
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

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1) {
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

void CCeEdit::OnSetfocus() {
	LOG0(5, "CCeEdit::OnSetfocus()");

	if (!(GetStyle() & ES_READONLY))
		SHSipPreference(GetSafeHwnd(), SIP_UP);
}

void CCeEdit::OnKillfocus() {
	LOG0(5, "CCeEdit::OnKillfocus()");

	if (!(GetStyle() & ES_READONLY))
		SHSipPreference(GetSafeHwnd(), SIP_DOWN);
}
