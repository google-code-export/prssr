// InfoDescriptionPg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "InfoDescriptionPg.h"

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
// CInfoDescriptionPg property page

IMPLEMENT_DYNCREATE(CInfoDescriptionPg, CPropertyPage)

CInfoDescriptionPg::CInfoDescriptionPg() : CPropertyPage(CInfoDescriptionPg::IDD) {
	//{{AFX_DATA_INIT(CInfoDescriptionPg)
	m_strCopyright = _T("");
	m_strDescription = _T("");
	m_strLanguage = _T("");
	//}}AFX_DATA_INIT
}

CInfoDescriptionPg::~CInfoDescriptionPg() {
}

void CInfoDescriptionPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDescriptionPg)
	DDX_Control(pDX, IDC_HRULE, m_ctlHrule);
	DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_LANGUAGE, m_strLanguage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDescriptionPg, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoDescriptionPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDescriptionPg message handlers

BOOL CInfoDescriptionPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
