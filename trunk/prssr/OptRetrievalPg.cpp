/**
 *  OptRetrievalPg.cpp
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
#include "prssr.h"
#include "OptRetrievalPg.h"
#include "Config.h"
#include <notify.h>

#include <connmgr.h>

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
// COptRetrievalPg property page

IMPLEMENT_DYNCREATE(COptRetrievalPg, CPropertyPage)

COptRetrievalPg::COptRetrievalPg() : CPropertyPage(COptRetrievalPg::IDD) {
	//{{AFX_DATA_INIT(COptRetrievalPg)
	m_bCheckNewIfCradled = Config.CheckOnCradling;
	m_nUserAgent = -1;
	m_bUseOptimizer = Config.UseHtmlOptimizer;
	m_strURL = Config.HtmlOptimizerURL;
	m_bBackgroundUpdate = Config.BackgroundUpdate;
	//}}AFX_DATA_INIT

	if (Config.UpdateInterval > 0) {
		m_bPeriodicUpdate = TRUE;
		m_nUpdateInterval = Config.UpdateInterval;
	}
	else {
		m_bPeriodicUpdate = FALSE;
		m_nUpdateInterval = 0;
	}

	m_bTimeUpdate = Config.TimeUpdate;
}

COptRetrievalPg::~COptRetrievalPg() {
}

void COptRetrievalPg::DoDataExchange(CDataExchange *pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptRetrievalPg)
	DDX_Control(pDX, IDC_UPDATE_LBL, m_lblAutoUpdate);
	DDX_Control(pDX, IDC_PERIODIC_UPDATE, m_ctlPeriodicUpdate);
	DDX_Control(pDX, IDC_UPDATE_INTERVAL_SPIN, m_ctlUpdateIntervalSpin);
	DDX_Control(pDX, IDC_UPDATE_INTERVAL, m_ctlUpdateInterval);
	DDX_Control(pDX, IDC_MINUTES, m_lblMinutes);
	DDX_Control(pDX, IDC_TIME_UPDATE, m_ctlTimeUpdate);
	DDX_Control(pDX, IDC_TIME, m_ctlTime);
	DDX_Control(pDX, IDC_OPTIMIZER_LBL, m_lblOptimizer);
	DDX_Control(pDX, IDC_USE_OPTIMIZER, m_ctlUseOptimizer);
	DDX_Control(pDX, IDC_URL, m_ctlURL);
	DDX_Control(pDX, IDC_MISC_LBL, m_lblMisc);
	DDX_Control(pDX, IDC_USERAGENT, m_ctlUserAgent);

	DDX_Check(pDX, IDC_PERIODIC_UPDATE, m_bPeriodicUpdate);
	DDX_Text(pDX, IDC_UPDATE_INTERVAL, m_nUpdateInterval);
	DDX_Check(pDX, IDC_CRADLED, m_bCheckNewIfCradled);
	DDX_Check(pDX, IDC_TIME_UPDATE, m_bTimeUpdate);
	DDX_Check(pDX, IDC_BKGND_UPDATE, m_bBackgroundUpdate);
	DDX_Check(pDX, IDC_USE_OPTIMIZER, m_bUseOptimizer);
	DDX_Text(pDX, IDC_URL, m_strURL);
	DDX_CBIndex(pDX, IDC_USERAGENT, m_nUserAgent);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		if (m_bUseOptimizer && m_strURL.IsEmpty()) {
			AfxMessageBox(IDS_URL_CAN_NOT_BE_EMPTY);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(COptRetrievalPg, CPropertyPage)
	//{{AFX_MSG_MAP(COptRetrievalPg)
	ON_BN_CLICKED(IDC_USE_OPTIMIZER, OnUseOptimizer)
	ON_BN_CLICKED(IDC_PERIODIC_UPDATE, OnPeriodicalUpdate)
	ON_BN_CLICKED(IDC_TIME_UPDATE, OnTimeUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptRetrievalPg message handlers

void COptRetrievalPg::UpdateControls() {
	if (m_ctlPeriodicUpdate.GetCheck() == BST_CHECKED) {
		m_ctlUpdateInterval.EnableWindow();
		m_ctlUpdateInterval.SetReadOnly(FALSE);
		m_ctlUpdateIntervalSpin.EnableWindow();
		m_lblMinutes.EnableWindow();
	}
	else {
		m_ctlUpdateInterval.EnableWindow(FALSE);
		m_ctlUpdateInterval.SetReadOnly();
		m_ctlUpdateIntervalSpin.EnableWindow(FALSE);
		m_lblMinutes.EnableWindow(FALSE);
	}

	//
	if (m_ctlTimeUpdate.GetCheck() == BST_CHECKED) {
		m_ctlTime.EnableWindow();
	}
	else {
		m_ctlTime.EnableWindow(FALSE);
	}

	//
	if (m_ctlUseOptimizer.GetCheck() == BST_CHECKED) {
		m_ctlURL.EnableWindow();
		m_ctlURL.SetReadOnly(FALSE);
	}
	else {
		m_ctlURL.EnableWindow(FALSE);
		m_ctlURL.SetReadOnly();
	}
}

BOOL COptRetrievalPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	SYSTEMTIME time;
	GetLocalTime(&time);
	time.wHour = Config.UpdateAtTime.wHour;
	time.wMinute = Config.UpdateAtTime.wMinute;
	time.wSecond = Config.UpdateAtTime.wSecond;
	m_ctlTime.SendMessage(DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM) &time);

	m_ctlUpdateIntervalSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlUpdateIntervalSpin.SetRange(0, 30000);

	m_ctlUserAgent.AddString(_T("pRSSreader"));
	m_ctlUserAgent.AddString(_T("Pocket IE"));
	if (Config.UserAgent.Compare(UA_PRSSREADER) == 0)
		m_ctlUserAgent.SetCurSel(0);
	else if (Config.UserAgent.Compare(UA_PIE) == 0)
		m_ctlUserAgent.SetCurSel(1);
	else {
		m_ctlUserAgent.AddString(_T("Custom"));
		m_ctlUserAgent.SetCurSel(2);
	}

	UpdateControls();

	return TRUE;
}

BOOL COptRetrievalPg::OnApply() {
	UpdateData();

	// auto update
	if (m_bPeriodicUpdate)
		Config.UpdateInterval = m_nUpdateInterval;
	else
		Config.UpdateInterval = 0;

	Config.CheckOnCradling = m_bCheckNewIfCradled;

	Config.TimeUpdate = m_bTimeUpdate;
	SYSTEMTIME time = { 0 };
	m_ctlTime.SendMessage(DTM_GETSYSTEMTIME, 0, (LPARAM) &time);
	ZeroMemory(&Config.UpdateAtTime, sizeof(Config.UpdateAtTime));
	Config.UpdateAtTime.wHour = time.wHour;
	Config.UpdateAtTime.wMinute = time.wMinute;
	Config.UpdateAtTime.wSecond = time.wSecond;

	Config.BackgroundUpdate = m_bBackgroundUpdate;

	// html optimizer
	Config.UseHtmlOptimizer = m_bUseOptimizer;
	Config.HtmlOptimizerURL = m_strURL;

	// it is not custom UA string
	switch (m_nUserAgent) {
		case 0: Config.UserAgent = UA_PRSSREADER; break;
		case 1: Config.UserAgent = UA_PIE; break;
		case 2: /* do not destroy user-defined UA string */ break;
	}

	return CPropertyPage::OnApply();
}

void COptRetrievalPg::OnUseOptimizer() {
	LOG0(1, "COptRetrievalPg::OnUseOptimizer");

	UpdateControls();
}

void COptRetrievalPg::OnPeriodicalUpdate() {
	LOG0(1, "COptRetrievalPg::OnPeriodicalUpdate");

	UpdateControls();
}

void COptRetrievalPg::OnTimeUpdate() {
	LOG0(1, "COptRetrievalPg::OnTimeUpdate");

	UpdateControls();
}
