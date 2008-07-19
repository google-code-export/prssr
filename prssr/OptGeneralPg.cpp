// OptGeneralPg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "OptGeneralPg.h"
#include "Config.h"

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
// COptGeneralPg property page

IMPLEMENT_DYNCREATE(COptGeneralPg, CPropertyPage)

COptGeneralPg::COptGeneralPg() : CPropertyPage(COptGeneralPg::IDD) {
	//{{AFX_DATA_INIT(COptGeneralPg)
	m_bNotifyNew = Config.NotifyNew;
	m_bMoveToUnread = Config.MoveToUnread;
	m_bClearErrorLog = Config.ClearErrorLog;
	//}}AFX_DATA_INIT
}

COptGeneralPg::~COptGeneralPg() {
}

void COptGeneralPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptGeneralPg)
	DDX_Check(pDX, IDC_NOTIFY_NEW, m_bNotifyNew);
	DDX_Check(pDX, IDC_MOVETOUNREAD, m_bMoveToUnread);
	DDX_Check(pDX, IDC_CLEAR_ERRORLOG, m_bClearErrorLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptGeneralPg, CPropertyPage)
	//{{AFX_MSG_MAP(COptGeneralPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptGeneralPg message handlers

BOOL COptGeneralPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
		
	return TRUE;
}

BOOL COptGeneralPg::OnApply() {
	UpdateData();

	Config.NotifyNew = m_bNotifyNew;
	Config.MoveToUnread = m_bMoveToUnread;
	Config.ClearErrorLog = m_bClearErrorLog;

	return CPropertyPage::OnApply();
}
