/**
 *  OptGeneralPg.cpp
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
#include "OptGeneralPg.h"
#include "Config.h"

#include <atlbase.h>
#include <cemapi.h>
#include <mapiutil.h>
#include <mapidefs.h>

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptGeneralPg property page

IMPLEMENT_DYNCREATE(COptGeneralPg, CPropertyPage)

COptGeneralPg::COptGeneralPg() : CPropertyPage(COptGeneralPg::IDD) {
	//{{AFX_DATA_INIT(COptGeneralPg)
	m_bNotifyNew = Config.NotifyNew;
	m_bMoveToUnread = Config.MoveToUnread;
	m_bClearErrorLog = Config.ClearErrorLog;
	m_bWrapTitles = Config.WrapTitles;
	m_nNavigation = Config.NavigationType;
	//}}AFX_DATA_INIT
}

COptGeneralPg::~COptGeneralPg() {
}

void COptGeneralPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptGeneralPg)
	DDX_Control(pDX, IDC_NAVIGATION, m_ctlNavigation);
	DDX_Control(pDX, IDC_EMAIL, m_ctlEmailAccount);

	DDX_Check(pDX, IDC_NOTIFY_NEW, m_bNotifyNew);
	DDX_Check(pDX, IDC_MOVETOUNREAD, m_bMoveToUnread);
	DDX_Check(pDX, IDC_CLEAR_ERRORLOG, m_bClearErrorLog);
	DDX_Check(pDX, IDC_WRAP_TITLES, m_bWrapTitles);
	DDX_CBIndex(pDX, IDC_NAVIGATION, m_nNavigation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptGeneralPg, CPropertyPage)
	//{{AFX_MSG_MAP(COptGeneralPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptGeneralPg message handlers

#define CHR(x) if (FAILED(x)) { hr = x; goto Error; }


HRESULT COptGeneralPg::EnumEmailAccounts() {
	HRESULT hr;

	CComPtr<IMAPITable> ptbl;
	CComPtr<IMAPISession> pSession;
	SRowSet *prowset = NULL;
	SPropValue  *pval = NULL;
	SizedSPropTagArray (1, spta) = { 1, PR_DISPLAY_NAME };

	// Log onto MAPI
	hr = MAPILogonEx(0, NULL, NULL, 0, static_cast<LPMAPISESSION *>(&pSession));
	CHR(hr); // CHR will goto Error if FAILED(hr)

	// Get the table of accounts
	hr = pSession->GetMsgStoresTable(0, &ptbl);
	CHR(hr);

	// set the columns of the table we will query
	hr = ptbl->SetColumns ((SPropTagArray *) &spta, 0);
	CHR(hr);

	while (TRUE) {
		// Free the previous row
		FreeProws (prowset);
		prowset = NULL;

		hr = ptbl->QueryRows (1, 0, &prowset);
		if ((hr != S_OK) || (prowset == NULL) || (prowset->cRows == 0)) {
			break;
		}

		ASSERT (prowset->aRow[0].cValues == spta.cValues);
		pval = prowset->aRow[0].lpProps;

		ASSERT (pval[0].ulPropTag == PR_DISPLAY_NAME);
		int nItem = m_ctlEmailAccount.AddString(pval[0].Value.lpszW);
		if (Config.EmailAccount.CompareNoCase(pval[0].Value.lpszW) == 0)
			m_ctlEmailAccount.SetCurSel(nItem);
	}

	pSession->Logoff(0, 0, 0);

Error:
    FreeProws (prowset);

    return hr;

}

BOOL COptGeneralPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	CString sText;
	sText.LoadString(IDS_NAVIGATION_TOUCH);
	m_ctlNavigation.AddString(sText);
	sText.LoadString(IDS_NAVIGATION_NORMAL);
	m_ctlNavigation.AddString(sText);
	m_ctlNavigation.SetCurSel(m_nNavigation);

	// email accounts
	EnumEmailAccounts();
	if (m_ctlEmailAccount.GetCurSel() == CB_ERR)
		m_ctlEmailAccount.SelectString(-1, _T("ActiveSync"));

	return TRUE;
}

BOOL COptGeneralPg::OnApply() {
	UpdateData();

	Config.NotifyNew = m_bNotifyNew;
	Config.MoveToUnread = m_bMoveToUnread;
	Config.ClearErrorLog = m_bClearErrorLog;
	Config.WrapTitles = m_bWrapTitles;
	Config.NavigationType = m_nNavigation;

	m_ctlEmailAccount.GetWindowText(Config.EmailAccount);

	return CPropertyPage::OnApply();
}
