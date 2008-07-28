/**
 *  PropRewritingPg.cpp
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

#include "StdAfx.h"
#include "prssr.h"
#include "PropRewritingPg.h"
#include "Site.h"
#include "RewriteRuleDlg.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropRewritingPg property page

IMPLEMENT_DYNCREATE(CPropRewritingPg, CCePropertyPage)

CPropRewritingPg::CPropRewritingPg() : CCePropertyPage(CPropRewritingPg::IDD) {
	//{{AFX_DATA_INIT(CPropRewritingPg)
	//}}AFX_DATA_INIT
}

CPropRewritingPg::~CPropRewritingPg() {
}

void CPropRewritingPg::DoDataExchange(CDataExchange* pDX) {
	CCePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropRewritingPg)
	DDX_Control(pDX, IDC_RULES, m_ctlRules);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropRewritingPg, CCePropertyPage)
	//{{AFX_MSG_MAP(CPropRewritingPg)
	ON_NOTIFY(LVN_KEYDOWN, IDC_RULES, OnKeydownRules)
	ON_NOTIFY(NM_CLICK, IDC_RULES, OnClickRules)
	ON_NOTIFY(GN_CONTEXTMENU, IDC_RULES, OnContextMenu)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_UPDATE_COMMAND_UI(ID_REMOVE, OnUpdateRemove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropRewritingPg message handlers

BOOL CPropRewritingPg::OnInitDialog() {
	CCePropertyPage::OnInitDialog();

	m_ctlRules.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CRect rc;
	m_ctlRules.GetClientRect(&rc);

	m_ctlRules.InsertColumn(0, _T("Pattern"), LVCFMT_LEFT, (int) (0.6 * rc.Width()));
	m_ctlRules.InsertColumn(1, _T("Rewrite to"), LVCFMT_LEFT, (int) (0.4 * rc.Width()));
	
	CString sText;
	sText.LoadString(IDS_NEW_RULE);
	int item = m_ctlRules.InsertItem(0, sText);
	m_ctlRules.SetItemData(item, NULL);

	for (int i = 0; i < Rules.GetSize(); i++)
		InsertRule(i + 1, Rules[i]);

	UpdateControls();

	return TRUE;
}

void CPropRewritingPg::InsertRule(int idx, CRewriteRule *rule) {
	int newItem = m_ctlRules.InsertItem(idx, rule->Match);
	m_ctlRules.SetItemText(newItem, 1, rule->Replace);
	m_ctlRules.SetItemData(newItem, (DWORD) rule);
}

void CPropRewritingPg::UpdateControls() {
}

void CPropRewritingPg::OnKeydownRules(NMHDR *pNMHDR, LRESULT *pResult) {
	LV_KEYDOWN *plvkd = (LV_KEYDOWN *) pNMHDR;
   
	if (plvkd->wVKey == VK_RETURN) {
		int item = m_ctlRules.GetNextItem(-1, LVNI_SELECTED);
		if (item != -1)
			EditRule(item);
	}

	*pResult = 0;
}

void CPropRewritingPg::OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(1, "CRewriteRuleDlg::OnContextMenu()");

	// GN_CONTEXTMENU sends NMRGINFO structure through notify struct parameter
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	CMenu mnu;
	mnu.LoadMenu(IDR_REWRITE_CTX);
	CMenu *popup = mnu.GetSubMenu(0);

	int item = m_ctlRules.GetNextItem(-1, LVNI_SELECTED);
	if (m_ctlRules.GetItemData(item) == NULL) {
		popup->EnableMenuItem(ID_REMOVE, MF_BYCOMMAND | MF_GRAYED);
	}

	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);

	*pResult = TRUE; // This is important!
}

void CPropRewritingPg::OnRemove() {
	int item = m_ctlRules.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1) {
		CRewriteRule *rr = (CRewriteRule *) m_ctlRules.GetItemData(item);
		if (rr != NULL && AfxMessageBox(IDS_DELETE_RULE, MB_YESNO | MB_ICONQUESTION) == IDYES) {
			m_ctlRules.DeleteItem(item);
			delete rr;
		}
	}
}

void CPropRewritingPg::OnUpdateRemove(CCmdUI *pCmdUI) {
	int item = m_ctlRules.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1)
		pCmdUI->Enable(m_ctlRules.GetItemData(item) != 0);
	else
		pCmdUI->Enable(FALSE);
}

void CPropRewritingPg::OnDestroy() {
	LOG0(1, "CRewriteRuleDlg::OnDestroy()");

	Rules.RemoveAll();
	for (int i = 1; i < m_ctlRules.GetItemCount(); i++) {			// starting with 1, becuase we have to ignore ("New Rule...") item
		CRewriteRule *rr = (CRewriteRule *) m_ctlRules.GetItemData(i);
		Rules.SetAtGrow(i - 1, rr);
	}

	CCePropertyPage::OnDestroy();
}

void CPropRewritingPg::OnClickRules(NMHDR *pNMHDR, LRESULT *pResult) {
	NMLISTVIEW *pnmlv = (NMLISTVIEW *) (pNMHDR);

	int item = m_ctlRules.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1)
		EditRule(item);

	*pResult = 0;
}

void CPropRewritingPg::EditRule(int item) {
	BOOL bNew = FALSE;
	CRewriteRule *rr = (CRewriteRule *) m_ctlRules.GetItemData(item);

	if (rr == NULL) {
		// add new rule
		rr = new CRewriteRule();
		bNew = TRUE;
	}

	CRewriteRuleDlg dlg;
	dlg.m_strMatch = rr->Match;
	dlg.m_strReplace = rr->Replace;
	if (dlg.DoModal() == IDOK) {
		rr->Match = dlg.m_strMatch;
		rr->Replace = dlg.m_strReplace;

		if (bNew) {
			InsertRule(m_ctlRules.GetItemCount(), rr);
		}
		else {
			m_ctlRules.SetItemText(item, 0, rr->Match);
			m_ctlRules.SetItemText(item, 1, rr->Replace);
		}
	}
	else {
		if (bNew) delete rr;
	}
}

void CPropRewritingPg::ResizeControls() {
	CRect rc;
	GetClientRect(rc);

	m_ctlRules.SetWindowPos(NULL, 0, 0, rc.Width() - SCALEX(13), rc.bottom - SCALEY(30 + 5), SWP_NOZORDER | SWP_NOMOVE);
}
