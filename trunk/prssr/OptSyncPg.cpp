/**
 *  OptSyncPg.cpp
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
#include "OptSyncPg.h"
#include "Config.h"

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
// COptSyncPg property page

IMPLEMENT_DYNCREATE(COptSyncPg, CPropertyPage)

COptSyncPg::COptSyncPg() : CPropertyPage(COptSyncPg::IDD) {
	//{{AFX_DATA_INIT(COptSyncPg)
	m_strUserName = Config.SyncUserName;
	m_strPassword = Config.SyncPassword;
	m_bTranslate = Config.TranslateLanguage;
	m_ctlTranslateLanguage.SetCurSel(Config.SelectedLanguage);
	//}}AFX_DATA_INIT
}

COptSyncPg::~COptSyncPg() {
}

void COptSyncPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptSyncPg)
	DDX_Control(pDX, IDC_SYNC_SITE, m_ctlSyncSite);
	DDX_Control(pDX, IDC_C_USERNAME, m_lblUserName);
	DDX_Control(pDX, IDC_SYNC_USERNAME, m_ctlUserName);
	DDX_Control(pDX, IDC_C_PASSWORD, m_lblPassword);
	DDX_Control(pDX, IDC_SYNC_PASSWORD, m_ctlPassword);
	DDX_Text(pDX, IDC_SYNC_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_SYNC_PASSWORD, m_strPassword);
	DDX_Check(pDX, IDC_TRANSLATE, m_bTranslate);
	DDX_Control(pDX, IDC_TRANSLATE, m_ctlTranslate);
	DDX_Control(pDX, IDC_TRANSLATE_LANGUAGE, m_ctlTranslateLanguage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptSyncPg, CPropertyPage)
	//{{AFX_MSG_MAP(COptSyncPg)
	ON_CBN_SELENDOK(IDC_SYNC_SITE, OnSelendokSyncSite)
	ON_BN_CLICKED(IDC_TRANSLATE, OnTranslate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptSyncPg message handlers

void COptSyncPg::UpdateControls() {
	int site = m_ctlSyncSite.GetCurSel();
	GetDlgItem(IDC_STATIC10)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC11)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);

	m_ctlTranslate.ShowWindow(FALSE);
	m_ctlTranslateLanguage.ShowWindow(FALSE);

	if (site == 0) { // No sync site
		m_lblUserName.EnableWindow(FALSE);
		m_ctlUserName.EnableWindow(FALSE);
		m_lblPassword.EnableWindow(FALSE);
		m_ctlPassword.EnableWindow(FALSE);

		GetDlgItem(IDC_STATIC10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC11)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);
	}
	else {
		m_lblUserName.EnableWindow();
		m_ctlUserName.EnableWindow();
		m_lblPassword.EnableWindow();
		m_ctlPassword.EnableWindow();
		if (site == 1) {
			if (GetSystemMetrics(SM_CXSCREEN) == 240) {
				GetDlgItem(IDC_STATIC12)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);
			} else if (GetSystemMetrics(SM_CXSCREEN) == 480) {
				GetDlgItem(IDC_STATIC10)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC11)->ShowWindow(SW_HIDE);
			}
		} else {
			m_ctlTranslate.ShowWindow(TRUE);
			if (m_ctlTranslate.GetCheck() == BST_CHECKED)
				m_ctlTranslateLanguage.ShowWindow(TRUE);
			else
				m_ctlTranslateLanguage.ShowWindow(FALSE);
			if (GetSystemMetrics(SM_CXSCREEN) == 240) {
				GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC13)->ShowWindow(SW_SHOW);
			} else if (GetSystemMetrics(SM_CXSCREEN) == 480) {
				GetDlgItem(IDC_STATIC10)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC11)->ShowWindow(SW_SHOW);
			}
		}
	}
}

	BOOL COptSyncPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	m_ctlSyncSite.AddString(_T("None"));
	m_ctlSyncSite.AddString(_T("Google Reader R.I.P."));
	m_ctlSyncSite.AddString(_T("Google Reader CL"));

	m_ctlTranslateLanguage.AddString(_T("Russian"));
	m_ctlTranslateLanguage.AddString(_T("Ukrainian"));;
	m_ctlTranslateLanguage.AddString(_T("Czech"));
	m_ctlTranslateLanguage.AddString(_T("Estonian"));
	m_ctlTranslateLanguage.AddString(_T("Latvian"));
	m_ctlTranslateLanguage.AddString(_T("Polish"));
	m_ctlTranslateLanguage.AddString(_T("Dutch"));
	m_ctlTranslateLanguage.AddString(_T("French"));
	m_ctlTranslateLanguage.AddString(_T("Italian"));
	m_ctlTranslateLanguage.AddString(_T("Spanish"));
	m_ctlTranslateLanguage.AddString(_T("English USA"));

	m_ctlSyncSite.SetCurSel(Config.SyncSite);
	m_ctlTranslateLanguage.SetCurSel(Config.SelectedLanguage);

	UpdateControls();

	return TRUE;
}

BOOL COptSyncPg::OnApply() {
	UpdateData();

	Config.SyncSite = (ESyncSite) m_ctlSyncSite.GetCurSel();
	Config.SelectedLanguage = (ESelectedLanguage) m_ctlTranslateLanguage.GetCurSel();

	Config.SyncUserName = m_strUserName;
	Config.SyncPassword = m_strPassword;
	Config.TranslateLanguage = m_bTranslate;

	Config.Save();

	return CPropertyPage::OnApply();
}

void COptSyncPg::OnSelendokSyncSite() {
	UpdateControls();
}

void COptSyncPg::OnTranslate() {
	UpdateControls();
}
