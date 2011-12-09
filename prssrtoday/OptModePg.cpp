/**
 *  OptModePg.cpp : implementation file
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "StdAfx.h"
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

/* int SpeedSecs[] = {
	1,
	3,
	5,
	7,
	10
}; */

int SpeedSecs[] = {
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30
};


int DisplayRowsCnt[] = {
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30
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
	m_bShowDateTimeTail = Config.ShowDateTimeTail;
	m_bShowOnlyNew = Config.ShowOnlyNew;
	m_bShowTodayWrap = Config.ShowTodayWrap;
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
	DDX_Control(pDX, IDC_SHOW_DATETIME_TAIL, m_ctlShowDateTimeTail);
	DDX_Control(pDX, IDC_SHOW_ONLY_NEW, m_ctlShowOnlyNew);
	DDX_Control(pDX, IDC_DISPLAY_SPEED_LBL, m_ctlDisplaySpeedLbl);
	DDX_Control(pDX, IDC_DISPLAYSPEED, m_ctlDisplaySpeed);
	DDX_Control(pDX, IDC_DISPLAY_SPEED_SECS, m_ctlDisplaySpeedSecs);
	DDX_Control(pDX, IDC_SHOW_TODAY_WRAP, m_ctlShowTodayWrap);
	DDX_Control(pDX, IDC_DISPLAY_ROWS_LBL, m_ctlDisplayRowsLbl);
	DDX_Control(pDX, IDC_DISPLAYROWS, m_ctlDisplayRows);
	// general
	DDX_CBIndex(pDX, IDC_MODE, m_nMode);
	// cycling
	DDX_Radio(pDX, IDC_SITE_NAMES, m_nCyclingSubMode);
	DDX_Check(pDX, IDC_SHOW_SITE_NAME, m_bShowSiteName);
	DDX_Check(pDX, IDC_SHOW_DATE_TIME, m_bShowDateTime);
	DDX_Check(pDX, IDC_SHOW_DATETIME_TAIL, m_bShowDateTimeTail);
	DDX_Check(pDX, IDC_SHOW_ONLY_NEW, m_bShowOnlyNew);
	DDX_Check(pDX, IDC_SHOW_TODAY_WRAP, m_bShowTodayWrap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptModePg, CPropertyPage)
	//{{AFX_MSG_MAP(COptModePg)
	ON_CBN_SELENDOK(IDC_MODE, OnSelendokMode)
	ON_BN_CLICKED(IDC_SITE_NAMES, OnSiteNames)
	ON_BN_CLICKED(IDC_FEED_ITEMS, OnFeedItems)
	ON_BN_CLICKED(IDC_SHOW_TODAY_WRAP, OnShowTodayWrap)
	ON_BN_CLICKED(IDC_SHOW_DATE_TIME, OnShowDateTime)
	ON_BN_CLICKED(IDC_SHOW_DATETIME_TAIL, OnShowDateTimeTail)
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
	CString strSpeed;
	for (i = 0; i < sizeof(SpeedSecs) / sizeof(SpeedSecs[0]); i++) {
		strSpeed.Format(_T("%d"), SpeedSecs[i]);
		int item = m_ctlDisplaySpeed.AddString(strSpeed);

		if (SpeedSecs[i] == Config.CyclingSpeed)
			m_ctlDisplaySpeed.SetCurSel(item);
	}

	// display rows
	CString strRows;
	for (i = 0; i < sizeof(DisplayRowsCnt) / sizeof(DisplayRowsCnt[0]); i++) {
		strRows.Format(_T("%d"), DisplayRowsCnt[i]);
		int item = m_ctlDisplayRows.AddString(strRows);

		if (DisplayRowsCnt[i] == Config.DisplayRows)
			m_ctlDisplayRows.SetCurSel(item);
	}

	if (m_ctlDisplaySpeed.GetCurSel() == CB_ERR) {
		strSpeed.Format(_T("%d"), Config.CyclingSpeed);
		m_ctlDisplaySpeed.SetWindowText(strSpeed);
	}

	if (m_ctlDisplayRows.GetCurSel() == CB_ERR) {
		strRows.Format(_T("%d"), Config.DisplayRows);
		m_ctlDisplayRows.SetWindowText(strRows);
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
	Config.ShowDateTimeTail = m_bShowDateTimeTail;
	Config.ShowOnlyNew = m_bShowOnlyNew;
	Config.ShowTodayWrap = m_bShowTodayWrap;

	CString strSpeed;
	m_ctlDisplaySpeed.GetWindowText(strSpeed);
	int speed;
	if (swscanf(strSpeed, _T("%d"), &speed) == 1)
		Config.CyclingSpeed = speed;
	else
		Config.CyclingSpeed = CONFIG_DEFAULT_CYCLING_SPEED;

	CString strRows;
	m_ctlDisplayRows.GetWindowText(strRows);
	int rows;
	if (swscanf(strRows, _T("%d"), &rows) == 1)
		Config.DisplayRows = rows;
	else
		Config.DisplayRows = CONFIG_DEFAULT_DISPLAY_ROWS;

	CPropertyPage::OnOK();
}

void COptModePg::UpdateControlsCycling(BOOL enabled/* = TRUE*/) {
	LOG1(3, "COptModePg::UpdateControlsCycling(%d)", enabled);

	int nShowCmd = enabled ? SW_SHOW : SW_HIDE;

	m_ctlSiteNames.ShowWindow(nShowCmd);
	m_ctlFeedItems.ShowWindow(nShowCmd);
	m_ctlShowSiteName.ShowWindow(nShowCmd);
	m_ctlShowDateTime.ShowWindow(nShowCmd);
	m_ctlShowDateTimeTail.ShowWindow(nShowCmd);
	m_ctlShowOnlyNew.ShowWindow(nShowCmd);
	m_ctlDisplaySpeedLbl.ShowWindow(nShowCmd);
	m_ctlDisplaySpeed.ShowWindow(nShowCmd);
	m_ctlDisplaySpeedSecs.ShowWindow(nShowCmd);
	m_ctlShowTodayWrap.ShowWindow(nShowCmd);
	m_ctlDisplayRowsLbl.ShowWindow(nShowCmd);
	m_ctlDisplayRows.ShowWindow(nShowCmd);

	if (enabled) {
		if (m_ctlFeedItems.GetCheck() == BST_CHECKED) {
			m_ctlShowSiteName.EnableWindow();
			m_ctlShowTodayWrap.EnableWindow();
		}

		if (m_ctlShowTodayWrap.GetCheck() == BST_CHECKED) {
			m_ctlDisplayRowsLbl.EnableWindow();
			m_ctlDisplayRows.EnableWindow();
		} else {
			m_ctlDisplayRowsLbl.EnableWindow(FALSE);
			m_ctlDisplayRows.EnableWindow(FALSE);
		}

		if (m_ctlShowDateTime.GetCheck() == BST_CHECKED)
			m_ctlShowDateTimeTail.EnableWindow(FALSE);
		else 
			m_ctlShowDateTimeTail.EnableWindow();

		if (m_ctlShowDateTimeTail.GetCheck() == BST_CHECKED)
			m_ctlShowDateTime.EnableWindow(FALSE);
		else 
			m_ctlShowDateTime.EnableWindow();

		if (m_ctlSiteNames.GetCheck() == BST_CHECKED) {
			m_ctlShowSiteName.EnableWindow(FALSE);
			m_ctlShowDateTime.EnableWindow(FALSE);
			m_ctlShowDateTimeTail.EnableWindow(FALSE);
			m_ctlShowTodayWrap.EnableWindow(FALSE);
			m_ctlDisplayRowsLbl.EnableWindow(FALSE);
			m_ctlDisplayRows.EnableWindow(FALSE);
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

void COptModePg::OnShowTodayWrap() {
	LOG0(1, "COptModePg::OnShowTodayWrap()");

	UpdateControls();
}

void COptModePg::OnShowDateTime() {
	LOG0(1, "COptModePg::OnShowDateTime()");

	UpdateControls();
}

void COptModePg::OnShowDateTimeTail() {
	LOG0(1, "COptModePg::OnShowDateTime()");

	UpdateControls();
}