// PropAuthenticationPg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "PropAuthenticationPg.h"

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
// CPropAuthenticationPg property page

IMPLEMENT_DYNCREATE(CPropAuthenticationPg, CPropertyPage)

CPropAuthenticationPg::CPropAuthenticationPg() : CPropertyPage(CPropAuthenticationPg::IDD)
{
	//{{AFX_DATA_INIT(CPropAuthenticationPg)
	m_strPassword = _T("");
	m_strUserName = _T("");
	m_bAuthRequired = FALSE;
	//}}AFX_DATA_INIT
}

CPropAuthenticationPg::~CPropAuthenticationPg()
{
}

void CPropAuthenticationPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropAuthenticationPg)
	DDX_Control(pDX, IDC_C_USERNAME, m_ctlUserNameLbl);
	DDX_Control(pDX, IDC_C_PASSWORD, m_ctlPasswordLbl);
	DDX_Control(pDX, IDC_USERNAME, m_ctlUserName);
	DDX_Control(pDX, IDC_PASSWORD, m_ctlPassword);
	DDX_Control(pDX, IDC_AUTH_REQUIRED, m_ctlAuthRequired);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_USERNAME, m_strUserName);
	DDX_Check(pDX, IDC_AUTH_REQUIRED, m_bAuthRequired);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropAuthenticationPg, CPropertyPage)
	//{{AFX_MSG_MAP(CPropAuthenticationPg)
	ON_BN_CLICKED(IDC_AUTH_REQUIRED, OnAuthRequired)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropAuthenticationPg message handlers

BOOL CPropAuthenticationPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	UpdateControls();

	return TRUE;
}

void CPropAuthenticationPg::UpdateControls() {
	if (m_ctlAuthRequired.GetCheck() == BST_CHECKED) {
		m_ctlUserNameLbl.EnableWindow();
		m_ctlUserName.EnableWindow();
		m_ctlPasswordLbl.EnableWindow();
		m_ctlPassword.EnableWindow();
	}
	else {
		m_ctlUserNameLbl.EnableWindow(FALSE);
		m_ctlUserName.EnableWindow(FALSE);
		m_ctlPasswordLbl.EnableWindow(FALSE);
		m_ctlPassword.EnableWindow(FALSE);
	}
}

void CPropAuthenticationPg::OnAuthRequired() {	
	UpdateControls();
}

