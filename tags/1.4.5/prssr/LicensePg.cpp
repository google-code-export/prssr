/**
 *  LicensePg.cpp
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
#include "LicensePg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFFER_SIZE							8192

/////////////////////////////////////////////////////////////////////////////
// CLicensePg property page

IMPLEMENT_DYNCREATE(CLicensePg, CCePropertyPage)

CLicensePg::CLicensePg() : CCePropertyPage(CLicensePg::IDD)
{
	//{{AFX_DATA_INIT(CLicensePg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLicensePg::~CLicensePg()
{
}

void CLicensePg::DoDataExchange(CDataExchange* pDX)
{
	CCePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicensePg)
	DDX_Control(pDX, IDC_LICENSE, m_ctlLicense);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLicensePg, CCePropertyPage)
	//{{AFX_MSG_MAP(CLicensePg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicensePg message handlers

void CLicensePg::OnOK() {
	CCePropertyPage::OnOK();
}

BOOL CLicensePg::OnInitDialog() {
	CCePropertyPage::OnInitDialog();

	// load the license
	CString license;

	CString fileName;
	fileName.Format(_T("%s\\%s"), Config.InstallDir, _T("COPYING"));

	HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		char buffer[BUFFER_SIZE];
		DWORD read;
		while (ReadFile(hFile, buffer, BUFFER_SIZE, &read, NULL) && read > 0) {
			int len = MultiByteToWideChar(CP_UTF8, 0, buffer, read, NULL, 0);
			CString s;
			LPTSTR lpStr = s.GetBufferSetLength(len);
			MultiByteToWideChar(CP_UTF8, 0, buffer, read, lpStr, len);
			license += s;
		}

		license.Replace(_T("\n"), _T("\r\n"));
		m_ctlLicense.SetWindowText(license);

		CloseHandle(hFile);
	}

	m_ctlLicense.SetReadOnly();

	return TRUE;
}

void CLicensePg::OnSize(UINT nType, int cx, int cy) {
	m_ctlLicense.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}
