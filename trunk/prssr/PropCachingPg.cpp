/**
 *  PropCachingPg.cpp
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
#include "PropCachingPg.h"

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
// CPropCachingPg property page

IMPLEMENT_DYNCREATE(CPropCachingPg, CPropertyPage)

CPropCachingPg::CPropCachingPg() : CPropertyPage(CPropCachingPg::IDD) {
	//{{AFX_DATA_INIT(CPropCachingPg)
	m_bSpecificCaching = FALSE;
	m_bCacheEnclosures = FALSE;
	m_bCacheOnlineContent = FALSE;
	m_bStoreImages = FALSE;
	m_nMB = 0;
	m_bEnclosureLimit = FALSE;
	m_bSpecificCaching = FALSE;
	//}}AFX_DATA_INIT
}

CPropCachingPg::~CPropCachingPg() {
}

void CPropCachingPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropCachingPg)
	DDX_Control(pDX, IDC_SPECIFIC_CACHING, m_ctlSpecificCaching);
	DDX_Control(pDX, IDC_STORE_IMAGES, m_ctlStoreImages);
	DDX_Control(pDX, IDC_CACHE_ONLINE_CONTENT, m_ctlCacheOnlineContent);
	DDX_Control(pDX, IDC_DONT_CACHE, m_ctlEnclosureLimit);
	DDX_Control(pDX, IDC_ENCLOSURE_LIMIT, m_ctlEnclosureLimitSpin);
	DDX_Control(pDX, IDC_MB, m_ctlMB);
	DDX_Control(pDX, IDC_MB_LBL, m_ctlMBLbl);
	DDX_Control(pDX, IDC_CACHE_ENCLOSURES, m_ctlCacheEnclosures);
	//}}AFX_DATA_MAP

	DDX_Check(pDX, IDC_SPECIFIC_CACHING, m_bSpecificCaching);
	DDX_Check(pDX, IDC_STORE_IMAGES, m_bStoreImages);
	DDX_Check(pDX, IDC_CACHE_ONLINE_CONTENT, m_bCacheOnlineContent);
	DDX_Check(pDX, IDC_CACHE_ENCLOSURES, m_bCacheEnclosures);
	DDX_Check(pDX, IDC_DONT_CACHE, m_bEnclosureLimit);
	DDX_Text(pDX, IDC_MB, m_nMB);
}


BEGIN_MESSAGE_MAP(CPropCachingPg, CPropertyPage)
	//{{AFX_MSG_MAP(CPropCachingPg)
	ON_BN_CLICKED(IDC_SPECIFIC_CACHING, OnSpecificCaching)
	ON_BN_CLICKED(IDC_CACHE_ENCLOSURES, OnCacheEnclosures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropCachingPg message handlers

void CPropCachingPg::UpdateControls() {
	LOG0(3, "CPropCachingPg::UpdateControls()");

	if (m_ctlSpecificCaching.GetCheck() == BST_CHECKED) {
		m_ctlStoreImages.EnableWindow();
		m_ctlCacheOnlineContent.EnableWindow();
	}
	else {
		m_ctlStoreImages.EnableWindow(FALSE);
		m_ctlCacheOnlineContent.EnableWindow(FALSE);
	}

	if (m_ctlCacheEnclosures.GetCheck() == BST_CHECKED) {
		m_ctlEnclosureLimit.EnableWindow();
		m_ctlLargerLbl.EnableWindow();
		m_ctlEnclosureLimitSpin.EnableWindow();
		m_ctlMB.EnableWindow();
		m_ctlMBLbl.EnableWindow();
	}
	else {
		m_ctlEnclosureLimit.EnableWindow(FALSE);
		m_ctlLargerLbl.EnableWindow(FALSE);
		m_ctlEnclosureLimitSpin.EnableWindow(FALSE);
		m_ctlMB.EnableWindow(FALSE);
		m_ctlMBLbl.EnableWindow(FALSE);
	}
}

BOOL CPropCachingPg::OnInitDialog() {
	LOG0(3, "CPropCachingPg::OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
	m_ctlEnclosureLimitSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlEnclosureLimitSpin.SetRange(0, 30000);
	UpdateControls();

	return TRUE;
}

void CPropCachingPg::OnCacheEnclosures() {
	LOG0(3, "CPropCachingPg::OnCacheEnclosures()");

	UpdateControls();
}

void CPropCachingPg::OnSpecificCaching() {
	LOG0(3, "CPropCachingPg::OnSpecificCaching()");

	UpdateControls();
}

