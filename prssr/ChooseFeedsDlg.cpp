/**
 *  ChooseFeedsDlg.cpp
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

#include "stdafx.h"
#include "prssr.h"
#include "ChooseFeedsDlg.h"

#include "Config.h"
#include "../share\helpers.h"
#include "Appearance.h"

#include "www/AutoFeedHtmlFile.h"

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

//////////////////////////////////////////////////////////////////////

//
// Import Feeds icons
//

// site
#define ICON_SITE						0

/////////////////////////////////////////////////////////////////////////////
// CChooseFeedsDlg property page

CChooseFeedsDlg::CChooseFeedsDlg(CWnd *pParentWnd/* = NULL*/) :
	CCeDialog(InWideMode() ? CChooseFeedsDlg::IDD_WIDE : CChooseFeedsDlg::IDD)
{
	//{{AFX_DATA_INIT(CChooseFeedsDlg)
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_ADD;
//	m_strCaption.LoadString(IDS_CHOOSE_FEEDS);
}

CChooseFeedsDlg::~CChooseFeedsDlg() {
}

void CChooseFeedsDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CChooseFeedsDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseFeedsDlg)
	DDX_Control(pDX, IDC_FEEDS, m_ctlFeeds);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChooseFeedsDlg, CCeDialog)
	//{{AFX_MSG_MAP(CChooseFeedsDlg)
	ON_COMMAND(ID_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseFeedsDlg message handlers

BOOL CChooseFeedsDlg::OnInitDialog() {
	LOG0(3, "CChooseFeedsDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

//	m_ctlFeeds.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	m_ctlFeeds.SetExtendedStyle(LVS_EX_CHECKBOXES);	
	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_SITE_ITEMS, SCALEX(16), 0, RGB(192, 192, 192));
	AfxSetResourceHandle(AfxGetInstanceHandle());
/*	// add folder icon
	SHFILEINFO sfi = { 0 };
	ZeroMemory(&sfi, sizeof(sfi));
	SHGetFileInfo(_T("\\"), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);
	m_nGroupIcon = m_ilIcons.Add(sfi.hIcon);
	m_ctlFeeds.m_nGroupIconIdx = m_nGroupIcon;
*/
	m_ctlFeeds.SetImageList(&m_ilIcons, TVSIL_NORMAL);

	m_ctlFeeds.InsertColumn(0, _T(""));
	//
	POSITION pos = Feeds.GetHeadPosition();
	while (pos != NULL) {
		CHtmlFeed *hfeed = Feeds.GetNext(pos);
		InsertItem(m_ctlFeeds.GetItemCount(), hfeed);
	}

	m_ctlFeeds.SetColumnWidth(0, LVSCW_AUTOSIZE);

	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseFeedsDlg::UpdateControls() {
	
}

void CChooseFeedsDlg::InsertItem(int n, CHtmlFeed *hf) {
	LOG0(5, "CChooseFeedsDlg::InsertItem()");

	if (hf == NULL)
		return;

	int item = m_ctlFeeds.InsertItem(n, hf->Name);
	m_ctlFeeds.SetItemData(item, (DWORD) hf);
}

void CChooseFeedsDlg::OnAdd() {
	LOG0(1, "CChooseFeedsDlg::OnAdd()");

	if (UpdateData()) {
		OnOK();
	}
}

void CChooseFeedsDlg::OnOK() {
	LOG0(1, "CChooseFeedsDlg::OnOK()");

	Feeds.RemoveAll();
	for (int i = 0; i < m_ctlFeeds.GetItemCount(); i++) {
		CHtmlFeed *hf = (CHtmlFeed *) m_ctlFeeds.GetItemData(i);
		if (m_ctlFeeds.GetCheck(i))
			Feeds.AddTail(hf);
		else
			delete hf;
	}

	CCeDialog::OnOK();
}

void CChooseFeedsDlg::OnCancel() {
	Feeds.RemoveAll();
	for (int i = 0; i < m_ctlFeeds.GetItemCount(); i++) {
		CHtmlFeed *hf = (CHtmlFeed *) m_ctlFeeds.GetItemData(i);
		delete hf;
	}

	CCeDialog::OnCancel();
}

void CChooseFeedsDlg::ResizeControls() {
	LOG0(3, "CChooseFeedsDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	m_ctlFeeds.SetColumnWidth(0, LVSCW_AUTOSIZE);
}
