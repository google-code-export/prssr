// ProxyProfilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "ProxyProfilesDlg.h"
#include "net/proxy.h"
#include "Config.h"

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
// CProxyProfileDlg dialog


CProxyProfileDlg::CProxyProfileDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(CProxyProfileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProxyProfileDlg)
	m_strHost = _T("");
	m_strName = _T("");
	m_strPassword = _T("");
	m_nPort = 0;
	m_nType = PROXY_TYPE_SOCKS4;
	m_bUseAuthentication = FALSE;
	m_strUserName = _T("");
	//}}AFX_DATA_INIT
	SetMenu(IDR_DONE);
//	CurrProfile = NULL;
}


void CProxyProfileDlg::DoDataExchange(CDataExchange* pDX) {
	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxyProfileDlg)
//	DDX_Control(pDX, IDC_REMOVE, m_ctlRemove);
//	DDX_Control(pDX, IDC_PROFILES, m_ctlProfiles);
//	DDX_Control(pDX, IDC_SETTINGS, m_lblSettings);

	DDX_Control(pDX, IDC_NAME, m_ctlName);
	DDX_Control(pDX, IDC_TYPE, m_ctlType);
	DDX_Control(pDX, IDC_HOST, m_ctlHost);
	DDX_Control(pDX, IDC_PORT, m_ctlPort);
	DDX_Control(pDX, IDC_USE_AUTHENTICATION, m_ctlUseAuthentication);
	DDX_Control(pDX, IDC_USERNAME, m_ctlUserName);
	DDX_Control(pDX, IDC_PASSWORD, m_ctlPassword);
	DDX_Control(pDX, IDC_C_USERNAME, m_lblUserName);
	DDX_Control(pDX, IDC_C_PASSWORD, m_lblPassword);
	
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_NotEmpty(pDX, m_strName, IDS_EMPTY_NAME);
//	DDX_CBIndex(pDX, IDC_TYPE, m_nType);
	DDX_Text(pDX, IDC_HOST, m_strHost);
	DDV_NotEmpty(pDX, m_strHost, IDS_EMPTY_HOST_NAME);
	DDX_Text(pDX, IDC_PORT, m_nPort);
	DDX_Check(pDX, IDC_USE_AUTHENTICATION, m_bUseAuthentication);
	DDX_Text(pDX, IDC_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProxyProfileDlg, CCeDialog)
	//{{AFX_MSG_MAP(CProxyProfileDlg)
	ON_CBN_SELENDOK(IDC_TYPE, OnSelendokType)
	ON_BN_CLICKED(IDC_USE_AUTHENTICATION, OnUseAuthentication)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProxyProfileDlg message handlers

void CProxyProfileDlg::UpdateControls() {
	m_ctlName.EnableWindow();
	m_ctlType.EnableWindow();
	m_ctlHost.EnableWindow();
	m_ctlPort.EnableWindow();
	m_ctlUseAuthentication.EnableWindow();

	if (m_ctlUseAuthentication.GetCheck() == BST_CHECKED) {
		m_lblUserName.EnableWindow();
		m_ctlUserName.EnableWindow();
		m_lblPassword.EnableWindow();
		m_ctlPassword.EnableWindow();
	}
	else {
		m_lblUserName.EnableWindow(FALSE);
		m_ctlUserName.EnableWindow(FALSE);
		m_lblPassword.EnableWindow(FALSE);
		m_ctlPassword.EnableWindow(FALSE);
	}
}

struct CBData {
	CString Text;
	DWORD Data;

	CBData(const CString &s, DWORD d) {
		Text = s;
		Data = d;
	}
};

BOOL CProxyProfileDlg::OnInitDialog() {
	CCeDialog::OnInitDialog();

	// proxy types
	CBData proxyTypeStr[] = {
		CBData(_T("SOCKS4"), PROXY_TYPE_SOCKS4),
		CBData(_T("SOCKS5"), PROXY_TYPE_SOCKS5),
		CBData(_T("HTTP 1.1"), PROXY_TYPE_HTTP),
	};

	for (int i = 0; i < countof(proxyTypeStr); i++) {
		int item = m_ctlType.AddString(proxyTypeStr[i].Text);
		m_ctlType.SetItemData(item, proxyTypeStr[i].Data);
		if (m_nType == (EProxyType) proxyTypeStr[i].Data)
			m_ctlType.SetCurSel(i);
	}
	if (m_ctlType.GetCurSel() == CB_ERR)
		m_ctlType.SetCurSel(0);
	OnSelendokType();

	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProxyProfileDlg::OnSelendokType() {
	EProxyType type = (EProxyType) m_ctlType.GetItemData(m_ctlType.GetCurSel());

	switch (type) {
		case PROXY_TYPE_SOCKS4: m_ctlPort.SetWindowText(_T("1080")); break;
		case PROXY_TYPE_SOCKS5: m_ctlPort.SetWindowText(_T("1080")); break;
		default:
		case PROXY_TYPE_HTTP: m_ctlPort.SetWindowText(_T("8080")); break;
	}
}

void CProxyProfileDlg::OnUseAuthentication() {
	UpdateControls();
}

void CProxyProfileDlg::OnOK() {	
	CCeDialog::OnOK();

	m_nType = (EProxyType) m_ctlType.GetItemData(m_ctlType.GetCurSel());
}
