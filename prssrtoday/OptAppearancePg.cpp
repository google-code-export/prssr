/**
 *  OptAppearancePg.cpp : implementation file
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
#include "prssrtoday.h"
#include "OptAppearancePg.h"
#include "Config.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../prssr/debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int FontSize[] = {
	8, 9, 10, 11, 12, 13, 14, 16
};

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg property page

IMPLEMENT_DYNCREATE(COptAppearancePg, CPropertyPage)

COptAppearancePg::COptAppearancePg() : CPropertyPage(COptAppearancePg::IDD) {
	LOG0(5, "COptAppearancePg::COptAppearancePg()");

	//{{AFX_DATA_INIT(COptAppearancePg)
	m_bUseFontSize = Config.FontSize != -1;
	m_bDisplayIcon = Config.DisplayIcon;
	m_bHidePlugin = Config.HidePlugin;
	m_bDisplayFavicon = Config.DisplayFavicon;
	//}}AFX_DATA_INIT
}

COptAppearancePg::~COptAppearancePg() {
	LOG0(5, "COptAppearancePg::~COptAppearancePg()");
}

void COptAppearancePg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "COptAppearancePg::DoDataExchange()");

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptAppearancePg)
	DDX_Control(pDX, IDC_USEFONTSIZE, m_ctlUseFontSize);
	DDX_Control(pDX, IDC_FONTSIZE, m_ctlFontSize);
	DDX_Check(pDX, IDC_USEFONTSIZE, m_bUseFontSize);
	DDX_Control(pDX, IDC_PLUGIN_ICON, m_ctlDisplayIcon);
	DDX_Control(pDX, IDC_PLUGIN_FAVICON, m_ctlDisplayFavicon);
	DDX_Check(pDX, IDC_PLUGIN_ICON, m_bDisplayIcon);
	DDX_Check(pDX, IDC_PLUGIN_FAVICON, m_bDisplayFavicon);
	DDX_Control(pDX, IDC_HIDE_PLUGIN, m_ctlHidePlugin);
	DDX_Check(pDX, IDC_HIDE_PLUGIN, m_bHidePlugin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptAppearancePg, CPropertyPage)
	//{{AFX_MSG_MAP(COptAppearancePg)
	ON_BN_CLICKED(IDC_USEFONTSIZE, OnUsefontsize)
	ON_BN_CLICKED(IDC_PLUGIN_ICON, OnDisplayIcon)
	ON_BN_CLICKED(IDC_PLUGIN_FAVICON, OnDisplayFavicon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg message handlers

BOOL COptAppearancePg::OnInitDialog() {
	LOG0(3, "COptAppearancePg::OnInitDialog()");

	CPropertyPage::OnInitDialog();

	// insert font sizes
	for (int i = 0; i < sizeof(FontSize) / sizeof(FontSize[0]); i++) {
		CString strSize;
		strSize.Format(_T("%d"), FontSize[i]);
		int item = m_ctlFontSize.AddString(strSize);
		if (FontSize[i] == Config.FontSize)
			m_ctlFontSize.SetCurSel(item);
	}

	int nCurSel = m_ctlFontSize.GetCurSel();
	if (Config.FontSize == -1 || nCurSel == CB_ERR)
		m_ctlFontSize.SetCurSel(0);

	SetForegroundWindow();
	UpdateControls();

	return TRUE;
}

void COptAppearancePg::OnOK() {
	LOG0(3, "COptAppearancePg::OnOK()");

	if (m_ctlUseFontSize.GetCheck() == BST_CHECKED) {
		CString strSize;
		m_ctlFontSize.GetWindowText(strSize);
		int size;
		if (swscanf(strSize, _T("%d"), &size) == 1)
			Config.FontSize = size;
		else
			Config.FontSize = -1;
	}
	else
		Config.FontSize = -1;

	Config.DisplayIcon = m_bDisplayIcon;
	Config.HidePlugin = m_bHidePlugin;
	Config.DisplayFavicon = m_bDisplayFavicon;

	CPropertyPage::OnOK();
}

void COptAppearancePg::UpdateControls() {
	LOG0(5, "COptAppearancePg::UpdateControls()");

	if (m_ctlUseFontSize.GetCheck() == BST_CHECKED)
		m_ctlFontSize.EnableWindow();
	else
		m_ctlFontSize.EnableWindow(FALSE);

	if (m_ctlDisplayIcon.GetCheck() == BST_CHECKED)
		m_ctlDisplayFavicon.EnableWindow();
	else
		m_ctlDisplayFavicon.EnableWindow(FALSE);
}

void COptAppearancePg::OnUsefontsize() {
	LOG0(5, "COptAppearancePg::OnUsefontsize()");

	UpdateControls();
}

void COptAppearancePg::OnDisplayIcon() {
	LOG0(5, "COptAppearancePg::OnDisplayFavicon()");

	UpdateControls();
}

void COptAppearancePg::OnDisplayFavicon() {
	LOG0(5, "COptAppearancePg::OnDisplayFavicon()");

	UpdateControls();
}
