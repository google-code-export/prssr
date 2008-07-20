/**
 *  PropArchivePg.cpp
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

