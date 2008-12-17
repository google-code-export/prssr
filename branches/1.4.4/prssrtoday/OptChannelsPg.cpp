/**
 *  OptChannelsPg.cpp : implementation file
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

#include "OptChannelsPg.h"
#include "Config.h"
#include "../prssr/Site.h"
#include "../share/defs.h"

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

/////////////////////////////////////////////////////////////////////////////
// COptChannelsPg property page

IMPLEMENT_DYNCREATE(COptChannelsPg, CPropertyPage)

COptChannelsPg::COptChannelsPg() : CPropertyPage(COptChannelsPg::IDD)
{
	LOG0(5, "COptChannelsPg::COptChannelsPg()");

	//{{AFX_DATA_INIT(COptChannelsPg)
	m_bShowNewChannels = Config.ShowNewChannels;
	//}}AFX_DATA_INIT
}

COptChannelsPg::~COptChannelsPg()
{
	LOG0(5, "COptChannelsPg::~COptChannelsPg()");
}

void COptChannelsPg::DoDataExchange(CDataExchange* pDX)
{
	LOG0(5, "COptChannelsPg::DoDataExchange()");

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptChannelsPg)
	DDX_Control(pDX, IDC_CHANNELS, m_ctlChannels);
	DDX_Control(pDX, IDC_SHOW_NEW_CHANNELS, m_ctlShowNewChannels);
	DDX_Check(pDX, IDC_SHOW_NEW_CHANNELS, m_bShowNewChannels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptChannelsPg, CPropertyPage)
	//{{AFX_MSG_MAP(COptChannelsPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptChannelsPg message handlers

BOOL COptChannelsPg::OnInitDialog() {
	LOG0(5, "COptChannelsPg::OnInitDialog()");

	CPropertyPage::OnInitDialog();

	m_ctlChannels.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	CRect rc;
	m_ctlChannels.GetClientRect(&rc);
	m_ctlChannels.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width());
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *si = SiteList.GetAt(i);
		int item = m_ctlChannels.InsertItem(i, si->Name);
		if (si->Info != NULL)
			m_ctlChannels.SetCheck(item, si->Info->TodayShow);
		m_ctlChannels.SetItemData(item, (DWORD) si);
	}

	SetForegroundWindow();
	UpdateControls();

	return TRUE;
}

void COptChannelsPg::UpdateControls() {
	LOG0(3, "COptChannelsPg::UpdateControls()");
}

void COptChannelsPg::OnOK() {
	LOG0(3, "COptChannelsPg::OnOK()");

	for (int i = 0; i < m_ctlChannels.GetItemCount(); i++) {
		CSiteItem *si = (CSiteItem *) m_ctlChannels.GetItemData(i);
		si->Info->TodayShow = m_ctlChannels.GetCheck(i);

		// save to registry
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, SiteList.GetIndexOf(si) + 1);
		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		reg.Write(szTodayShow, si->Info->TodayShow);
	}

	Config.ShowNewChannels = m_bShowNewChannels;

	CPropertyPage::OnOK();
}
