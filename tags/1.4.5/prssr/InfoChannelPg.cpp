/**
 *  InfoChannelPg.cpp
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
#include "InfoChannelPg.h"

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
// CInfoChannelPg property page

IMPLEMENT_DYNCREATE(CInfoChannelPg, CPropertyPage)

CInfoChannelPg::CInfoChannelPg() : CPropertyPage(CInfoChannelPg::IDD) {
	//{{AFX_DATA_INIT(CInfoChannelPg)
	m_strFormat = _T("");
	m_strWWW = _T("");
	m_strRSS = _T("");
	//}}AFX_DATA_INIT
}

CInfoChannelPg::~CInfoChannelPg() {
}

void CInfoChannelPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoChannelPg)
	DDX_Control(pDX, IDC_WWW, m_ctlWWW);
	DDX_Control(pDX, IDC_RSS, m_ctlRSS);
	DDX_Control(pDX, IDC_HRULE, m_ctlHrule);
	DDX_Text(pDX, IDC_FORMAT, m_strFormat);
	DDX_Text(pDX, IDC_WWW, m_strWWW);
	DDX_Text(pDX, IDC_RSS, m_strRSS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoChannelPg, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoChannelPg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoChannelPg message handlers

BOOL CInfoChannelPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	m_brEditBack.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CInfoChannelPg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// make the WWW and RSS control background same as the window background
	if (pWnd == &m_ctlRSS || pWnd == &m_ctlWWW) {
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		return (HBRUSH) m_brEditBack.GetSafeHandle();
	}
	
	return hbr;
}
