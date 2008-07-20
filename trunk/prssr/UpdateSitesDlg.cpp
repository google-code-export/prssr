/**
 *  UpdateSitesDlg.cpp
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
#include "UpdateSitesDlg.h"

#include "Config.h"
#include "../share\helpers.h"
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

#define ICON_SITE						0


/////////////////////////////////////////////////////////////////////////////
// CUpdateSitesDlg dialog

CUpdateSitesDlg::CUpdateSitesDlg(CWnd *pParentWnd/* = NULL*/) :
	CCeDialog(InWideMode() ? CUpdateSitesDlg::IDD_WIDE : CUpdateSitesDlg::IDD)
{
	//{{AFX_DATA_INIT(CUpdateSitesDlg)
	m_bUpdateOnly = Config.UpdateOnly;
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_UPDATE_SITES;
//	m_strCaption.LoadString(IDS_UPDATE_SITES);
}

CUpdateSitesDlg::~CUpdateSitesDlg() {
}

void CUpdateSitesDlg::DoDataExchange(CDataExchange *pDX) {
	LOG0(5, "CUpdateSitesDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateSitesDlg)
	DDX_Control(pDX, IDC_SITES, m_ctlSites);
	DDX_Control(pDX, IDC_HRULE1, m_ctlHrule);
	DDX_Control(pDX, IDC_UPDATE_ONLY, m_ctlUpdateOnly);
	DDX_Control(pDX, IDC_SELECT_ALL, m_ctlSelectAll);
	DDX_Check(pDX, IDC_UPDATE_ONLY, m_bUpdateOnly);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUpdateSitesDlg, CCeDialog)
	//{{AFX_MSG_MAP(CUpdateSitesDlg)
	ON_COMMAND(ID_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateSitesDlg message handlers

BOOL CUpdateSitesDlg::OnInitDialog() {
	LOG0(3, "CUpdateSitesDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_SITE_ITEMS, SCALEX(16), 0, RGB(192, 192, 192));
	AfxSetResourceHandle(AfxGetInstanceHandle());
	// add folder icon
	SHFILEINFO sfi = { 0 };
	ZeroMemory(&sfi, sizeof(sfi));
	SHGetFileInfo(_T("\\"), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);
	m_nGroupIcon = m_ilIcons.Add(sfi.hIcon);
	m_ctlSites.m_nGroupIconIdx = m_nGroupIcon;

	m_ctlSites.SetImageList(&m_ilIcons, TVSIL_NORMAL);

	//
	CSiteItem *root = SiteList.GetRoot();
	POSITION pos = root->SubItems.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *item = root->SubItems.GetNext(pos);
		InsertItems(0, item);
	}
	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateSitesDlg::UpdateControls() {
	
}


void CUpdateSitesDlg::InsertItems(HTREEITEM parent, CSiteItem *item) {
	LOG0(5, "CUpdateSitesDlg::InsertItems()");

	if (item == NULL)
		return;

	if (item->Type == CSiteItem::Site) {
/*		//
		CString sTime;
		CString fileName = FormatFilePath(FILE_TYPE_FEED, Config.CacheLocation, item->Info->FileName);
		HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != INVALID_HANDLE_VALUE) {
			// last update
			SYSTEMTIME stLastWrite;
			FILETIME lastWrite = { 0 };
			GetFileTime(hFile, NULL, NULL, &lastWrite);

			FILETIME lastWriteLocal = { 0 };
			FileTimeToLocalFileTime(&lastWrite, &lastWriteLocal);
			FileTimeToSystemTime(&lastWriteLocal, &stLastWrite);
			CloseHandle(hFile);

			FormatDateTime(sTime, stLastWrite, Config.ShowRelativeDates);
		}
		m_ctlSites.SetItemText(nItem, 1, sTime);
*/

		HTREEITEM hItem = m_ctlSites.InsertItem(item->Name, ICON_SITE, ICON_SITE, parent);
		m_ctlSites.SetCheck(hItem, FALSE);
		m_ctlSites.SetItemData(hItem, (DWORD) item);
	}
	else {
		HTREEITEM hItem = m_ctlSites.InsertItem(item->Name, m_nGroupIcon, m_nGroupIcon, parent);
		m_ctlSites.SetCheck(hItem, FALSE);
		m_ctlSites.SetItemData(hItem, (DWORD) item);

		POSITION pos = item->SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = item->SubItems.GetNext(pos);
			InsertItems(hItem, si);
		}
	}
}

void CUpdateSitesDlg::OnUpdate() {
	LOG0(1, "CUpdateSitesDlg::OnUpdate()");

	if (UpdateData()) {
		OnOK();
	}
}

void CUpdateSitesDlg::GetCheckedItems(HTREEITEM parent) {
	CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(parent);
	if (si->Type == CSiteItem::Site && m_ctlSites.GetCheck(parent))
		m_oUpdateSites.AddTail(si);

	HTREEITEM hChild = m_ctlSites.GetChildItem(parent);
	while (hChild != NULL) {
		GetCheckedItems(hChild);
		hChild = m_ctlSites.GetNextSiblingItem(hChild);
	}
}

void CUpdateSitesDlg::OnOK() {
	LOG0(1, "CUpdateSitesDlg::OnOK()");

	CWaitCursor wait;

	HTREEITEM hItem = m_ctlSites.GetRootItem();
	while (hItem != NULL) {
		GetCheckedItems(hItem);
		hItem = m_ctlSites.GetNextSiblingItem(hItem);
	}

	CCeDialog::OnOK();
}

void CUpdateSitesDlg::ResizeControls() {
	LOG0(3, "CUpdateSitesDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (!InWideMode()) {
		CRect rc;
		GetClientRect(rc);

		m_ctlSites.SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height() - SCALEY(54), SWP_NOZORDER | SWP_NOMOVE);
		m_ctlHrule.SetWindowPos(NULL, rc.left, rc.Height() - SCALEY(30), rc.Width(), SCALEY(1), SWP_NOZORDER);
		m_ctlUpdateOnly.SetWindowPos(NULL, rc.left + SCALEX(6), rc.Height() - SCALEY(22), SCALEX(157), SCALEY(16), SWP_NOZORDER);
		m_ctlSelectAll.SetWindowPos(NULL, rc.right - SCALEX(68 + 5), rc.Height() - SCALEY(25), SCALEX(68), SCALEY(20), SWP_NOZORDER);
	}
}

void CUpdateSitesDlg::OnSelectAll() {
	LOG0(3, "CUpdateSitesDlg::OnSelectAll()");

	HTREEITEM hItem = m_ctlSites.GetRootItem();
	while (hItem != NULL) {
		CSiteItem *si = (CSiteItem *) m_ctlSites.GetItemData(hItem);
		if (si->Type == CSiteItem::Group)
			m_ctlSites.SetGroupCheck(hItem, TRUE);
		else
			m_ctlSites.SetCheck(hItem, TRUE);

		hItem = m_ctlSites.GetNextSiblingItem(hItem);
	}
}
