/**
 *  PropGeneralPg.cpp
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
#include "PropGeneralPg.h"

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
// CPropGeneralPg property page

IMPLEMENT_DYNCREATE(CPropGeneralPg, CPropertyPage)

CPropGeneralPg::CPropGeneralPg() : CPropertyPage(CPropGeneralPg::IDD) {
	//{{AFX_DATA_INIT(CPropGeneralPg)
	m_bShowOnToday = FALSE;
	//}}AFX_DATA_INIT
}

CPropGeneralPg::~CPropGeneralPg() {
}

void CPropGeneralPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropGeneralPg)
	DDX_Control(pDX, IDC_NAME, m_ctlName);
	DDX_Control(pDX, IDC_URL, m_ctlURL);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_URL, m_strURL);
	DDX_Check(pDX, IDC_SHOW_ON_TODAY, m_bShowOnToday);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropGeneralPg, CPropertyPage)
	//{{AFX_MSG_MAP(CPropGeneralPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropGeneralPg message handlers

BOOL CPropGeneralPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	UpdateControls();

	return TRUE;
}

void CPropGeneralPg::UpdateControls() {
}
