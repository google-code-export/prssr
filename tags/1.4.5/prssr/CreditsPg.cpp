/**
 *  CreditsPg.cpp
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
#include "CreditsPg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreditsPg property page

IMPLEMENT_DYNCREATE(CCreditsPg, CCePropertyPage)

CCreditsPg::CCreditsPg() : CCePropertyPage(CCreditsPg::IDD)
{
	//{{AFX_DATA_INIT(CCreditsPg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCreditsPg::~CCreditsPg()
{
}

void CCreditsPg::DoDataExchange(CDataExchange* pDX)
{
	CCePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreditsPg)
	DDX_Control(pDX, IDC_CREDITS, m_ctlCredits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreditsPg, CCePropertyPage)
	//{{AFX_MSG_MAP(CCreditsPg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreditsPg message handlers

void CCreditsPg::OnOK() {
	CCePropertyPage::OnOK();
}

BOOL CCreditsPg::OnInitDialog() {
	CCePropertyPage::OnInitDialog();

	// load the license
	CString credits;
	credits.Format(
		L" David Andrs\r\n"
		L" Jörg (eljoest)\r\n");

	m_ctlCredits.SetWindowText(credits);
	m_ctlCredits.SetReadOnly();

	return TRUE;
}

void CCreditsPg::OnSize(UINT nType, int cx, int cy) {
	m_ctlCredits.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}
