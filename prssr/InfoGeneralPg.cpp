/**
 *  InfoGeneralPg.cpp
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
#include "InfoGeneralPg.h"
#include "Config.h"
#include "../share/helpers.h"

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
// CInfoGeneralPg property page

IMPLEMENT_DYNCREATE(CInfoGeneralPg, CPropertyPage)

CInfoGeneralPg::CInfoGeneralPg() : CPropertyPage(CInfoGeneralPg::IDD) {
	//{{AFX_DATA_INIT(CInfoGeneralPg)
	//}}AFX_DATA_INIT
}

CInfoGeneralPg::~CInfoGeneralPg() {
}

void CInfoGeneralPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoGeneralPg)
	DDX_Control(pDX, IDC_NEWITEMS, m_ctlNewItems);
	DDX_Control(pDX, IDC_PUBLISHED, m_ctlPublished);
	DDX_Control(pDX, IDC_SITENAME, m_ctlSiteName);
	DDX_Control(pDX, IDC_HRULE1, m_ctlHrule1);
	DDX_Control(pDX, IDC_HRULE2, m_ctlHrule2);
	DDX_Control(pDX, IDC_UNREADITEMS, m_ctlUnreadItems);
	DDX_Control(pDX, IDC_TOTALITEMS, m_ctlTotalItems);
	DDX_Control(pDX, IDC_LASTUPDATED, m_ctlLastUpdate);
	DDX_Control(pDX, IDC_LASTUPDATED_LBL, m_lblLastUpdate);
	DDX_Control(pDX, IDC_LASTSTATUS, m_ctlLastStatus);
	DDX_Control(pDX, IDC_FILESIZE, m_ctlFileSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoGeneralPg, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoGeneralPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoGeneralPg message handlers

BOOL CInfoGeneralPg::OnInitDialog() {
	LOG0(5, "CInfoGeneralPg::OnInitDialog()");

	CPropertyPage::OnInitDialog();

	m_ctlSiteName.SetWindowText(m_strSiteName);
	
	CString buffer;
	buffer.Format(IDS_TOTAL_ITEMS, m_nTotalItems);
	m_ctlTotalItems.SetWindowText(buffer);

	buffer.Format(IDS_NEW_ITEMS, m_nNewItems);
	m_ctlNewItems.SetWindowText(buffer);

	buffer.Format(IDS_UNREAD_ITEMS, m_nUnreadItems);
	m_ctlUnreadItems.SetWindowText(buffer);

	buffer = FormatFileSize(m_nFileSize);
	m_ctlFileSize.SetWindowText(buffer);

	if (!FormatDateTime(buffer, m_stPublished, Config.ShowRelativeDates))
		buffer.LoadString(IDS_UNKNOWN);
	m_ctlPublished.SetWindowText(buffer);

	if (m_stLastUpdated.wYear != 0) {
		if (!FormatDateTime(buffer, m_stLastUpdated, Config.ShowRelativeDates))
			buffer.LoadString(IDS_UNKNOWN);
		m_ctlLastUpdate.SetWindowText(buffer);
	}
	else {
		m_ctlHrule2.ShowWindow(SW_HIDE);
		m_lblLastUpdate.SetWindowText(_T(""));
		m_ctlLastUpdate.SetWindowText(_T(""));
	}

	m_ctlLastStatus.SetWindowText(L"");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
