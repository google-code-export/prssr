/**
 *  InfoDescriptionPg.cpp
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
#include "InfoDescriptionPg.h"

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
// CInfoDescriptionPg property page

IMPLEMENT_DYNCREATE(CInfoDescriptionPg, CPropertyPage)

CInfoDescriptionPg::CInfoDescriptionPg() : CPropertyPage(CInfoDescriptionPg::IDD) {
	//{{AFX_DATA_INIT(CInfoDescriptionPg)
	m_strCopyright = _T("");
	m_strDescription = _T("");
	m_strLanguage = _T("");
	//}}AFX_DATA_INIT
}

CInfoDescriptionPg::~CInfoDescriptionPg() {
}

void CInfoDescriptionPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDescriptionPg)
	DDX_Control(pDX, IDC_HRULE, m_ctlHrule);
	DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_LANGUAGE, m_strLanguage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDescriptionPg, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoDescriptionPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDescriptionPg message handlers

BOOL CInfoDescriptionPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
