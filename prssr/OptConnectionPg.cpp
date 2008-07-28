/**
 *  OptConnectionPg.cpp
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
#include "OptConnectionPg.h"
#include "Config.h"
#include "net/proxy.h"
#include "ProxyProfilesDlg.h"

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
// COptConnectionPg property page

IMPLEMENT_DYNCREATE(COptConnectionPg, CPropertyPage)

COptConnectionPg::COptConnectionPg() : CPropertyPage(COptConnectionPg::IDD)
{
	//{{AFX_DATA_INIT(COptConnectionPg)
	m_bAutoConnect = Config.AutoConnect;
	//}}AFX_DATA_INIT
}

COptConnectionPg::~COptConnectionPg()
{
}

void COptConnectionPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptConnectionPg)
	DDX_Check(pDX, IDC_AUTO_CONNECT, m_bAutoConnect);
	DDX_Control(pDX, IDC_PROXIES, m_ctlProxies);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptConnectionPg, CPropertyPage)
	//{{AFX_MSG_MAP(COptConnectionPg)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptConnectionPg message handlers

void COptConnectionPg::AddProfile(CProxyProfile *prf) {
	int item = m_ctlProxies.AddString(prf->Name);
	m_ctlProxies.SetItemData(item, (DWORD) prf);
}

BOOL COptConnectionPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	// 
	CString sNone;
	sNone.LoadString(IDS_NONE);
	int item = m_ctlProxies.AddString(sNone);
	m_ctlProxies.SetItemData(item, (DWORD) NULL);
	m_ctlProxies.SetCurSel(item);

	for (int i = 0; i < Config.ProxyProfiles.GetSize(); i++) {
		CProxyProfile *prf = Config.ProxyProfiles[i];
		AddProfile(prf);
	}

	m_ctlProxies.SetCurSel(Config.ProxyProfileIdx + 1);
	
	UpdateControls();
	
	return TRUE;
}

void COptConnectionPg::UpdateControls() {
}

BOOL COptConnectionPg::OnApply() {
	if (!UpdateData())
		return FALSE;

	Config.AutoConnect = m_bAutoConnect;

	int idx = m_ctlProxies.GetCurSel();
	if (idx > 0)
		Config.ProxyProfileIdx = idx - 1;
	else
		Config.ProxyProfileIdx = -1;

	return CPropertyPage::OnApply();
}

void COptConnectionPg::OnNew() {
	CProxyProfileDlg dlg;
	if (dlg.DoModal() == IDOK) {
		CProxyProfile *prf = new CProxyProfile;

		prf->Name = dlg.m_strName;
		prf->ProxyConfig.Type = (EProxyType) dlg.m_nType;
		prf->ProxyConfig.HostName = dlg.m_strHost;
		prf->ProxyConfig.Port = dlg.m_nPort;

		prf->ProxyConfig.NeedAuth = dlg.m_bUseAuthentication;
		prf->ProxyConfig.UserName = dlg.m_strUserName;
		prf->ProxyConfig.Password = dlg.m_strPassword;

		Config.ProxyProfiles.Add(prf);

		AddProfile(prf);
	}
}

void COptConnectionPg::OnEdit() {
	int idx = m_ctlProxies.GetCurSel();
	if (idx > 0) {
		CProxyProfile *prf = Config.ProxyProfiles[idx - 1];

		CProxyProfileDlg dlg;
		dlg.m_strName = prf->Name;
		dlg.m_nType = prf->ProxyConfig.Type;
		dlg.m_strHost = prf->ProxyConfig.HostName;
		dlg.m_nPort = prf->ProxyConfig.Port;

		dlg.m_bUseAuthentication = prf->ProxyConfig.NeedAuth;
		dlg.m_strUserName = prf->ProxyConfig.UserName;
		dlg.m_strPassword = prf->ProxyConfig.Password;
		

		if (dlg.DoModal() == IDOK) {
			prf->Name = dlg.m_strName;
			prf->ProxyConfig.Type = dlg.m_nType;

			prf->ProxyConfig.HostName = dlg.m_strHost;
			prf->ProxyConfig.Port = dlg.m_nPort;

			prf->ProxyConfig.NeedAuth = dlg.m_bUseAuthentication;
			prf->ProxyConfig.UserName = dlg.m_strUserName;
			prf->ProxyConfig.Password = dlg.m_strPassword;
			
			m_ctlProxies.DeleteString(idx);
			int item = m_ctlProxies.InsertString(idx, prf->Name);
			m_ctlProxies.SetItemData(item, (DWORD) prf);

			if (m_ctlProxies.SetCurSel(idx) == CB_ERR)
				m_ctlProxies.SetCurSel(0);
		}
	}
}

void COptConnectionPg::OnRemove() {
	int idx = m_ctlProxies.GetCurSel();
	if (idx > 0 && AfxMessageBox(IDS_DELETE_PROXY_PROFILE, MB_YESNO | MB_ICONQUESTION) == IDYES) {
		CProxyProfile *prf = Config.ProxyProfiles[idx - 1];
		Config.ProxyProfiles.RemoveAt(idx - 1);
		m_ctlProxies.DeleteString(idx);

		delete prf;

		if (m_ctlProxies.SetCurSel(idx) == CB_ERR)
			m_ctlProxies.SetCurSel(0);
	}
}
