// OptModePg.cpp : implementation file
//

#include "stdafx.h"
#include "prssrtoday.h"
#include "OptModePg.h"
#include "Config.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../prssr/debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ModeID[] = {
	IDS_MODE_BRIEF,
	IDS_MODE_CYCLING
//	IDS_MODE_TREE
};

UINT SpeedID[] = {
	IDS_FASTEST,
	IDS_FAST,
	IDS_MEDIUM,
	IDS_SLOW,
	IDS_SLOWEST
};

/////////////////////////////////////////////////////////////////////////////
// COptModePg property page

IMPLEMENT_DYNCREATE(COptModePg, CPropertyPage)

COptModePg::COptModePg() : CPropertyPage(COptModePg::IDD)
{
	LOG0(5, "COptModePg::COptModePg()");

	//{{AFX_DATA_INIT(COptModePg)
	m_nMode = Config.Mode;
	m_nCyclingSubMode = Config.CyclingSubMode;
	m_bShowSiteName = Config.ShowSiteName;
	m_bShowDateTime = Config.ShowDateTime;
	m_bShowOnlyNew = Config.ShowOnlyNew;
	m_nDisplaySpeedIdx = -1;
	//}}AFX_DATA_INIT
}

COptModePg::~COptModePg()
{
	LOG0(5, "COptModePg::~COptModePg()");
}

void COptModePg::DoDataExchange(CDataExchange* pDX)
{
	LOG0(5, "COptModePg::DoDataExchange()");

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptModePg)
	// general
	DDX_Control(pDX, IDC_MODE, m_ctlMode);
	// cycling
	DDX_Control(pDX, IDC_SITE_NAMES, m_ctlSiteNames);
	DDX_Control(pDX, IDC_FEED_ITEMS, m_ctlFeedItems);
	DDX_Control(pDX, IDC_SHOW_SITE_NAME, m_ctlShowSiteName);
	DDX_Control(pDX, IDC_SHOW_DATE_TIME, m_ctlShowDateTime);
	DDX_Control(pDX, IDC_SHOW_ONLY_NEW, m_ctlShowOnlyNew);
	DDX_Control(pDX, IDC_DISPLAY_SPEED_LBL, m_ctlDisplaySpeedLbl);
	DDX_Control(pDX, IDC_DISPLAYSPEED, m_ctlDisplaySpeed);
	// general
	DDX_CBIndex(pDX, IDC_MODE, m_nMode);
	// cycling
	DDX_Radio(pDX, IDC_SITE_NAMES, m_nCyclingSubMode);
	DDX_Check(pDX, IDC_SHOW_SITE_NAME, m_bShowSiteName);
	DDX_Check(pDX, IDC_SHOW_DATE_TIME, m_bShowDateTime);
	DDX_Check(pDX, IDC_SHOW_ONLY_NEW, m_bShowOnlyNew);
	DDX_CBIndex(pDX, IDC_DISPLAYSPEED, m_nDisplaySpeedIdx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptModePg, CPropertyPage)
	//{{AFX_MSG_MAP(COptModePg)
	ON_CBN_SELENDOK(IDC_MODE, OnSelendokMode)
	ON_BN_CLICKED(IDC_SITE_NAMES, OnSiteNames)
	ON_BN_CLICKED(IDC_FEED_ITEMS, OnFeedItems)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptModePg message handlers

BOOL COptModePg::OnInitDialog() {
	LOG0(3, "COptModePg::OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
	int i;

	m_ctlHeightSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlHeightSpin.SetRange(0, 30000);

	// mode
	for (i = 0; i < countof(ModeID); i++) {
		CString strMode;
		strMode.LoadString(ModeID[i]);
		int item = m_ctlMode.AddString(strMode);

		if (i == Config.Mode)
			m_ctlMode.SetCurSel(item);
	}

	// display speed
	for (i = 0; i < sizeof(SpeedID) / sizeof(SpeedID[0]); i++) {
		CString strSpeed;
		strSpeed.LoadString(SpeedID[i]);
		int item = m_ctlDisplaySpeed.AddString(strSpeed);

		if (i == Config.CyclingSpeedIdx)
			m_ctlDisplaySpeed.SetCurSel(item);
	}

	SetForegroundWindow();
	UpdateControls();

	return TRUE;
}

void COptModePg::OnOK() {
	LOG0(3, "COptModePg::OnOK()");

	Config.Mode = m_nMode;
	Config.CyclingSubMode = m_nCyclingSubMode;
	Config.ShowSiteName = m_bShowSiteName;
	Config.ShowDateTime = m_bShowDateTime;
	Config.ShowOnlyNew = m_bShowOnlyNew;

	Config.CyclingSpeedIdx = m_nDisplaySpeedIdx;
	if (Config.CyclingSpeedIdx == -1)
		Config.CyclingSpeedIdx = CONFIG_DEFAULT_CYCLING_SPEED;

	CPropertyPage::OnOK();
}

void COptModePg::UpdateControlsCycling(BOOL enabled/* = TRUE*/) {
	LOG1(3, "COptModePg::UpdateControlsCycling(%d)", enabled);

	int nShowCmd = enabled ? SW_SHOW : SW_HIDE;

	m_ctlSiteNames.ShowWindow(nShowCmd);
	m_ctlFeedItems.ShowWindow(nShowCmd);
	m_ctlShowSiteName.ShowWindow(nShowCmd);
	m_ctlShowDateTime.ShowWindow(nShowCmd);
	m_ctlShowOnlyNew.ShowWindow(nShowCmd);
	m_ctlDisplaySpeedLbl.ShowWindow(nShowCmd);
	m_ctlDisplaySpeed.ShowWindow(nShowCmd);

	if (enabled) {
		if (m_ctlSiteNames.GetCheck() == BST_CHECKED) {
			m_ctlShowSiteName.EnableWindow(FALSE);
			m_ctlShowDateTime.EnableWindow(FALSE);
		}

		if (m_ctlFeedItems.GetCheck() == BST_CHECKED) {
			m_ctlShowSiteName.EnableWindow();
			m_ctlShowDateTime.EnableWindow();
		}
	}
}

void COptModePg::UpdateControls() {
	LOG0(3, "COptModePg::UpdateControls()");

	switch (m_ctlMode.GetCurSel()) {
		case 0:
			UpdateControlsCycling(FALSE);
			break;

		case 1:
			UpdateControlsCycling(TRUE);
			break;
	}
}

void COptModePg::OnSelendokMode() {
	LOG0(1, "COptModePg::OnSelendokMode()");

	UpdateControls();
}

void COptModePg::OnSiteNames() {
	LOG0(1, "COptModePg::OnSiteNames()");

	UpdateControls();
}

void COptModePg::OnFeedItems() {
	LOG0(1, "COptModePg::OnFeedItems()");

	UpdateControls();
}
