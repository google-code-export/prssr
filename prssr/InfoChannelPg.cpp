// InfoChannelPg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "InfoChannelPg.h"

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
// CInfoChannelPg property page

IMPLEMENT_DYNCREATE(CInfoChannelPg, CPropertyPage)

CInfoChannelPg::CInfoChannelPg() : CPropertyPage(CInfoChannelPg::IDD) {
	//{{AFX_DATA_INIT(CInfoChannelPg)
	m_strFormat = _T("");
	m_strWWW = _T("");
	m_strRSS = _T("");
	//}}AFX_DATA_INIT
}

CInfoChannelPg::~CInfoChannelPg() {
}

void CInfoChannelPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoChannelPg)
	DDX_Control(pDX, IDC_WWW, m_ctlWWW);
	DDX_Control(pDX, IDC_RSS, m_ctlRSS);
	DDX_Control(pDX, IDC_HRULE, m_ctlHrule);
	DDX_Text(pDX, IDC_FORMAT, m_strFormat);
	DDX_Text(pDX, IDC_WWW, m_strWWW);
	DDX_Text(pDX, IDC_RSS, m_strRSS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoChannelPg, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoChannelPg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoChannelPg message handlers

BOOL CInfoChannelPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	m_brEditBack.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CInfoChannelPg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// make the WWW and RSS control background same as the window background
	if (pWnd == &m_ctlRSS || pWnd == &m_ctlWWW) {
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		return (HBRUSH) m_brEditBack.GetSafeHandle();
	}
	
	return hbr;
}
