// AuthenticationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "AuthenticationDlg.h"

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
// CAuthenticationPg dialog


CAuthenticationPg::CAuthenticationPg()
	: CPropertyPage(CAuthenticationPg::IDD)
{
	//{{AFX_DATA_INIT(CAuthenticationPg)
	m_strSite = _T("");
	m_strRealm = _T("");
	m_strPassword = _T("");
	m_strUserName = _T("");
	m_bSavePassword = FALSE;
	//}}AFX_DATA_INIT
}


void CAuthenticationPg::DoDataExchange(CDataExchange* pDX)
{
	LOG0(5, "CAuthenticationPg::DoDataExchange()");

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAuthenticationPg)
	DDX_Text(pDX, IDC_SITE, m_strSite);
	DDX_Text(pDX, IDC_REALM, m_strRealm);
	DDX_Control(pDX, IDC_USERNAME, m_ctlUserName);
	DDX_Control(pDX, IDC_PASSWORD, m_ctlPassword);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_USERNAME, m_strUserName);
	DDX_Check(pDX, IDC_SAVE_PASSWORD, m_bSavePassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAuthenticationPg, CPropertyPage)
	//{{AFX_MSG_MAP(CAuthenticationPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuthenticationPg message handlers

BOOL CAuthenticationPg::OnInitDialog() {
	LOG0(3, "CAuthenticationPg::OnInitDialog()");

	CPropertyPage::OnInitDialog();

	return TRUE;
}

void CAuthenticationPg::UpdateControls() {

}
