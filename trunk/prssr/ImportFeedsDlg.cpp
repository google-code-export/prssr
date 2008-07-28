/**
 *  ImportFeedsDlg.cpp
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
#include "ImportFeedsDlg.h"

#include "Config.h"
#include "../share/helpers.h"
#include "Appearance.h"

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
// CImportDlg control

CImportDlg::CImportDlg(CWnd *pParent/* = NULL*/) : CBrowseDlg(TRUE, pParent) {
	m_strCaption.LoadString(IDS_IMPORT_OPML_FILE);
	m_strPath = _T("\\");
	m_strExts = _T("opml");

	m_nMenuID = IDR_IMPORT_OPML;
}

BEGIN_MESSAGE_MAP(CImportDlg, CBrowseDlg)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_COMMAND(ID_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CImportDlg::OnImport() {
	if (UpdateData())
		EndDialog(IDOK);
}

//////////////////////////////////////////////////////////////////////

//
// Import Feeds icons
//

// site
#define ICON_SITE						0

/////////////////////////////////////////////////////////////////////////////
// CImportFeedsDlg property page

CImportFeedsDlg::CImportFeedsDlg(CWnd *pParentWnd/* = NULL*/) :
	CCeDialog(InWideMode() ? CImportFeedsDlg::IDD_WIDE : CImportFeedsDlg::IDD)
{
	//{{AFX_DATA_INIT(CImportFeedsDlg)
	m_bAppendFeeds = TRUE;
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_ADD;

	SiteList = NULL;
	m_bDefaultItemCheck = TRUE;
}

CImportFeedsDlg::~CImportFeedsDlg() {
}

void CImportFeedsDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CImportFeedsDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportFeedsDlg)
	DDX_Control(pDX, IDC_FEEDS, m_ctlFeeds);
	DDX_Control(pDX, IDC_HRULE1, m_ctlHrule);
	DDX_Control(pDX, IDC_APPEND_FEEDS, m_ctlAppendFeeds);
	DDX_Check(pDX, IDC_APPEND_FEEDS, m_bAppendFeeds);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportFeedsDlg, CCeDialog)
	//{{AFX_MSG_MAP(CImportFeedsDlg)
	ON_COMMAND(ID_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportFeedsDlg message handlers

BOOL CImportFeedsDlg::OnInitDialog() {
	LOG0(3, "CImportFeedsDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	m_ctlHrule.SetFrame(HR_TOP);
	
	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_SITE_ITEMS, SCALEX(16), 0, RGB(192, 192, 192));
	AfxSetResourceHandle(AfxGetInstanceHandle());
	// add folder icon
	SHFILEINFO sfi = { 0 };
	ZeroMemory(&sfi, sizeof(sfi));
	SHGetFileInfo(_T("\\"), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);
	m_nGroupIcon = m_ilIcons.Add(sfi.hIcon);
	m_ctlFeeds.m_nGroupIconIdx = m_nGroupIcon;

	m_ctlFeeds.SetImageList(&m_ilIcons, TVSIL_NORMAL);

	CSiteItem *root = SiteList->GetRoot();
	POSITION pos = root->SubItems.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *item = root->SubItems.GetNext(pos);
		InsertItems(TVI_ROOT, item);
	}
	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportFeedsDlg::UpdateControls() {
	
}

void CImportFeedsDlg::InsertItems(HTREEITEM hParent, CSiteItem *item) {
	LOG0(5, "CImportFeedsDlg::InsertItems()");

	if (item == NULL)
		return;

	if (item->Type == CSiteItem::Site) {
		HTREEITEM hItem = m_ctlFeeds.InsertItem(item->Name, ICON_SITE, ICON_SITE, hParent);
		m_ctlFeeds.SetCheck(hItem, m_bDefaultItemCheck);
		m_ctlFeeds.SetItemData(hItem, (DWORD) item);
	}
	else {
		HTREEITEM hItem = m_ctlFeeds.InsertItem(item->Name, m_nGroupIcon, m_nGroupIcon, hParent);
		m_ctlFeeds.SetCheck(hItem, m_bDefaultItemCheck);
		m_ctlFeeds.SetItemData(hItem, (DWORD) item);

		POSITION pos = item->SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = item->SubItems.GetNext(pos);
			InsertItems(hItem, si);
		}
	}
}

void CImportFeedsDlg::OnAdd() {
	LOG0(1, "CImportFeedsDlg::OnAdd()");

	if (UpdateData()) {
		OnOK();
	}
}

void CImportFeedsDlg::BuildSiteList(HTREEITEM hParent, CSiteItem *parent) {
	LOG0(5, "CImportFeedsDlg::BuildSiteList()");

	BOOL checked = m_ctlFeeds.GetCheck(hParent);
	CSiteItem *si = (CSiteItem *) m_ctlFeeds.GetItemData(hParent);

	if (si->Type == CSiteItem::Group) {
		CSiteItem *newParent;
		if (checked) {
			CSiteItem *newItem = new CSiteItem(parent, si);
			parent->AddItem(newItem);

			// current group will become a new parent site item
			newParent = newItem;
		}
		else {
			// keep current parent site item
			newParent = parent;
		}

		// process child items
		HTREEITEM hItem = m_ctlFeeds.GetChildItem(hParent);
		while (hItem != NULL) {
			BuildSiteList(hItem, newParent);
			hItem = m_ctlFeeds.GetNextSiblingItem(hItem);
		}
	}
	else if (si->Type == CSiteItem::Site) {
		if (checked) {
			CSiteItem *newItem = new CSiteItem(parent, si);
			parent->AddItem(newItem);
		}
	}
}

void CImportFeedsDlg::OnOK() {
	LOG0(1, "CImportFeedsDlg::OnOK()");

	CWaitCursor wait;

	// build up the site list which will be imported
	CSiteItem *newRoot = new CSiteItem(NULL, CSiteItem::Group);
	HTREEITEM hItem = m_ctlFeeds.GetRootItem();
	while (hItem != NULL) {
		BuildSiteList(hItem, newRoot);
		hItem = m_ctlFeeds.GetNextSiblingItem(hItem);
	}

	//
	SiteList->Destroy();
	SiteList->CreateFrom(newRoot);
	SiteList->SetRoot(newRoot);

	CCeDialog::OnOK();
}

void CImportFeedsDlg::ResizeControls() {
	LOG0(3, "CImportFeedsDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (!InWideMode()) {
		CRect rc;
		GetClientRect(rc);

		m_ctlFeeds.SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height() - SCALEY(54), SWP_NOZORDER | SWP_NOMOVE);
		m_ctlHrule.SetWindowPos(NULL, rc.left, rc.Height() - SCALEY(30), rc.Width(), SCALEY(1), SWP_NOZORDER);
		m_ctlAppendFeeds.SetWindowPos(NULL, rc.left + SCALEX(6), rc.Height() - SCALEY(22), SCALEX(157), SCALEY(16), SWP_NOZORDER);
	}
}
