// RewriteRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "RewriteRuleDlg.h"
#include "../share/helpers.h"

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
// CRewriteRuleDlg dialog


CRewriteRuleDlg::CRewriteRuleDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(CRewriteRuleDlg::IDD)
{
	LOG0(5, "CRewriteRuleDlg::CRewriteRuleDlg()");
	//{{AFX_DATA_INIT(CRewriteRuleDlg)
	//}}AFX_DATA_INIT
	SetMenu(IDR_DONE);
}

CRewriteRuleDlg::~CRewriteRuleDlg() {
	LOG0(5, "CRewriteRuleDlg::~CRewriteRuleDlg()");

}

void CRewriteRuleDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CRewriteRuleDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRewriteRuleDlg)
	DDX_Control(pDX, IDC_MATCH, m_ctlMatch);
	DDX_Control(pDX, IDC_REPLACE, m_ctlReplace);
	DDX_Text(pDX, IDC_MATCH, m_strMatch);
	DDX_Text(pDX, IDC_REPLACE, m_strReplace);
	//}}AFX_DATA_MAP

	DDV_NotEmpty(pDX, m_strMatch, IDS_EMPTY_RULE);
	DDV_NotEmpty(pDX, m_strReplace, IDS_EMPTY_RULE);
}


BEGIN_MESSAGE_MAP(CRewriteRuleDlg, CCeDialog)
	//{{AFX_MSG_MAP(CRewriteRuleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRewriteRuleDlg message handlers

BOOL CRewriteRuleDlg::OnInitDialog() {
	LOG0(3, "CRewriteRuleDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
