/**
 *  OptGeneralPg.cpp
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
	m_nNavigation = Config.NavigationType;
	//}}AFX_DATA_INIT
}

COptGeneralPg::~COptGeneralPg() {
}

void COptGeneralPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptGeneralPg)
	DDX_Control(pDX, IDC_NAVIGATION, m_ctlNavigation);

	DDX_Check(pDX, IDC_NOTIFY_NEW, m_bNotifyNew);
	DDX_Check(pDX, IDC_MOVETOUNREAD, m_bMoveToUnread);
	DDX_Check(pDX, IDC_CLEAR_ERRORLOG, m_bClearErrorLog);
	DDX_CBIndex(pDX, IDC_NAVIGATION, m_nNavigation);
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

	CString sText;
	sText.LoadString(IDS_NAVIGATION_TOUCH);
	m_ctlNavigation.AddString(sText);
	sText.LoadString(IDS_NAVIGATION_NORMAL);
	m_ctlNavigation.AddString(sText);
	m_ctlNavigation.SetCurSel(m_nNavigation);

	return TRUE;
}

BOOL COptGeneralPg::OnApply() {
	UpdateData();

	Config.NotifyNew = m_bNotifyNew;
	Config.MoveToUnread = m_bMoveToUnread;
	Config.ClearErrorLog = m_bClearErrorLog;
	Config.NavigationType = m_nNavigation;

	return CPropertyPage::OnApply();
}
