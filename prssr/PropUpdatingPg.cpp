/**
 *  PropUpdatingPg.cpp
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

#include "stdafx.h"
#include "prssr.h"
#include "PropUpdatingPg.h"

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
// CPropUpdatingPg property page

IMPLEMENT_DYNCREATE(CPropUpdatingPg, CPropertyPage)

CPropUpdatingPg::CPropUpdatingPg() : CPropertyPage(CPropUpdatingPg::IDD) {
	//{{AFX_DATA_INIT(CPropUpdatingPg)
	m_nUpdate = 0;
	m_nUpdateMinutes = 0;
	//}}AFX_DATA_INIT
	m_nSuggestInterval = 0;
}

CPropUpdatingPg::~CPropUpdatingPg() {
}

void CPropUpdatingPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropUpdatingPg)
	DDX_Control(pDX, IDC_UPDATE_INTERVAL_SPIN, m_ctlUpdateMinutesSpin);
	DDX_Control(pDX, IDC_SUGGEST_INTERVAL, m_ctlSuggestInterval);
	DDX_Radio(pDX, IDC_USE_GLOBAL, m_nUpdate);
	DDX_Text(pDX, IDC_UPDATE_INTERVAL, m_nUpdateMinutes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropUpdatingPg, CPropertyPage)
	//{{AFX_MSG_MAP(CPropUpdatingPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropUpdatingPg message handlers

BOOL CPropUpdatingPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	m_ctlUpdateMinutesSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlUpdateMinutesSpin.SetRange(1, 30000);

	CString sSuggestInterval;
	if (m_nSuggestInterval > 0)
		sSuggestInterval.Format(IDS_SUGGEST_INTERVAL, m_nSuggestInterval);
	else
		sSuggestInterval.LoadString(IDS_NO_SUGGEST_INTERVAL);
	m_ctlSuggestInterval.SetWindowText(sSuggestInterval);
	
	UpdateControls();

	return TRUE;
}

void CPropUpdatingPg::UpdateControls() {
}
