// PropArchivePg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "PropArchivePg.h"

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
// CPropArchivePg property page

IMPLEMENT_DYNCREATE(CPropArchivePg, CPropertyPage)

CPropArchivePg::CPropArchivePg() : CPropertyPage(CPropArchivePg::IDD) {
	//{{AFX_DATA_INIT(CPropArchivePg)
	m_nCacheLimit = 0;
	m_nCacheItems = 0;
	//}}AFX_DATA_INIT
}

CPropArchivePg::~CPropArchivePg() {
}

void CPropArchivePg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropArchivePg)
	DDX_Control(pDX, IDC_CACHE_ITEMS_SPIN, m_ctlCacheItemsSpin);
	DDX_Radio(pDX, IDC_DEFAULT_SETTINGS, m_nCacheLimit);
	DDX_Text(pDX, IDC_CACHE_ITEMS, m_nCacheItems);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropArchivePg, CPropertyPage)
	//{{AFX_MSG_MAP(CPropArchivePg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropArchivePg message handlers

BOOL CPropArchivePg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	m_ctlCacheItemsSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlCacheItemsSpin.SetRange(1, 30000);

	UpdateControls();

	return TRUE;
}

void CPropArchivePg::UpdateControls() {
}

