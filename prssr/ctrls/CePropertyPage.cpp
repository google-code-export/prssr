/**
 *  CePropertyPage.cpp
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

#include "..\stdafx.h"
#include "..\prssr.h"
#include "CePropertyPage.h"
#include "../../share/helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCePropertyPage property page

IMPLEMENT_DYNCREATE(CCePropertyPage, CPropertyPage)

CCePropertyPage::CCePropertyPage()
{
}


CCePropertyPage::CCePropertyPage(UINT nIDTemplate, UINT nIDCaption/* = 0*/) : CPropertyPage(nIDTemplate, nIDCaption)
{
	//{{AFX_DATA_INIT(CCePropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	WideMode = InWideMode();
}

CCePropertyPage::~CCePropertyPage()
{
}

void CCePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCePropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCePropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCePropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCePropertyPage message handlers

void CCePropertyPage::ResizeControls() {
}

void CCePropertyPage::OnSize(UINT nType, int cx, int cy) {
	CPropertyPage::OnSize(nType, cx, cy);

	ResizeControls();
}
