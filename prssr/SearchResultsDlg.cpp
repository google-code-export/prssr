// SearchResultsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "SearchResultsDlg.h"

#include "Config.h"
#include "../share/helpers.h"
#include "Appearance.h"

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

//
// Import Feeds icons
//

// site
#define ICON_SITE						0

/////////////////////////////////////////////////////////////////////////////
// CSearchResultsDlg property page

CSearchResultsDlg::CSearchResultsDlg(CWnd *pParentWnd/* = NULL*/) :
	CCeDialog(InWideMode() ? CSearchResultsDlg::IDD_WIDE : CSearchResultsDlg::IDD),
	m_ctlFeeds(IDS_NO_SEARCH_RESULTS)
{
	//{{AFX_DATA_INIT(CSearchResultsDlg)
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_ADD;
	SearchResults = NULL;
}

CSearchResultsDlg::~CSearchResultsDlg() {
}

void CSearchResultsDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CSearchResultsDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchResultsDlg)
	DDX_Control(pDX, IDC_FEEDS, m_ctlFeeds);
	DDX_Control(pDX, IDC_HRULE1, m_ctlHrule);
	DDX_Control(pDX, IDC_DESCRIPTION, m_ctlDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchResultsDlg, CCeDialog)
	//{{AFX_MSG_MAP(CSearchResultsDlg)
	ON_COMMAND(ID_ADD, OnAdd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FEEDS, OnSelchangedFeeds)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchResultsDlg message handlers

BOOL CSearchResultsDlg::OnInitDialog() {
	LOG0(3, "CSearchResultsDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	m_ctlHrule.SetFrame(HR_TOP);

	m_brEditBack.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_SITE_ITEMS, SCALEX(16), 0, RGB(192, 192, 192));
	AfxSetResourceHandle(AfxGetInstanceHandle());
	m_ctlFeeds.SetImageList(&m_ilIcons, LVSIL_SMALL);
	m_ctlFeeds.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	m_ctlFeeds.InsertColumn(0, _T(""), LVCFMT_LEFT);

	// insert items
	POSITION pos = SearchResults->GetHeadPosition();
	while (pos != NULL) {
		CSearchResultItem *sri = SearchResults->GetNext(pos);
		InsertItem(sri);
	}

	ResizeControls();
	SetForegroundWindow();
	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CSearchResultsDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) {
	HBRUSH hbr = CCeDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// make the Description control background same as the window background
	if (pWnd == &m_ctlDescription) {
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		return (HBRUSH) m_brEditBack.GetSafeHandle();
	}
	
	return hbr;
}

void CSearchResultsDlg::UpdateControls() {
}

void CSearchResultsDlg::InsertItem(CSearchResultItem *item) {
	LOG0(5, "CSearchResultsDlg::InsertItem()");

	if (item == NULL)
		return;

	int hItem = m_ctlFeeds.InsertItem(m_ctlFeeds.GetItemCount(), item->SiteName, ICON_SITE);
	m_ctlFeeds.SetItemData(hItem, (DWORD) item);
	m_ctlFeeds.SetCheck(hItem, FALSE);
}

void CSearchResultsDlg::OnAdd() {
	LOG0(1, "CSearchResultsDlg::OnAdd()");

	if (UpdateData()) {
		OnOK();
	}
}

void CSearchResultsDlg::OnOK() {
	LOG0(1, "CSearchResultsDlg::OnOK()");

	CWaitCursor wait;

	// build up the site list which will be imported
	SearchResults->RemoveAll();
	for (int i = 0;i < m_ctlFeeds.GetItemCount(); i++) {
		BOOL checked = m_ctlFeeds.GetCheck(i);
		CSearchResultItem *sri = (CSearchResultItem *) m_ctlFeeds.GetItemData(i);

		if (checked)
			SearchResults->AddTail(sri);
		else
			delete sri;
	}

	CCeDialog::OnOK();
}

void CSearchResultsDlg::ResizeControls() {
	LOG0(3, "CSearchResultsDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (InWideMode()) {
	}
	else {
		CRect rc;
		GetClientRect(rc);

		m_ctlFeeds.SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height() - SCALEY(59), SWP_NOZORDER | SWP_NOMOVE);
		m_ctlHrule.SetWindowPos(NULL, 0, rc.Height() - SCALEY(35), rc.Width(), 1, SWP_NOZORDER);
		m_ctlDescription.SetWindowPos(NULL, rc.left + SCALEX(1), rc.Height() - SCALEY(34), rc.Width(), SCALEY(34), SWP_NOZORDER);

		m_ctlFeeds.SetColumnWidth(0, rc.Width() - GetSystemMetrics(SM_CXVSCROLL));
	}
}

void CSearchResultsDlg::OnSelchangedFeeds(NMHDR *pNMHDR, LRESULT *pResult) {
	NMLISTVIEW *pnmlv = (NMLISTVIEW *) pNMHDR;

	if (pnmlv->uNewState & LVIS_SELECTED) {
		CSearchResultItem *sri = (CSearchResultItem *) pnmlv->lParam;
		m_ctlDescription.SetWindowText(sri->Description);
	}

	*pResult = 0;
}
