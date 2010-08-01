/**
 *  MessageDlg.cpp
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
#include "MessageDlg.h"
#include "../share/helpers.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog


CMessageDlg::CMessageDlg(UINT nCaption, UINT nText, DWORD style, UINT btnID)
	: CCeDialog(InWideMode() ? CMessageDlg::IDD_WIDE : CMessageDlg::IDD)
{
	LOG0(5, "CMessageDlg::CMessageDlg()");
	//{{AFX_DATA_INIT(CMessageDlg)
	//}}AFX_DATA_INIT
	m_strCaption.LoadString(nCaption);
	m_strText.LoadString(nText);

	m_nStyle = style;
	if (style & MB_YESNO) m_nMenuID = IDR_YESNO;
	else m_nMenuID = IDR_DONE;

	m_strSKLeft.LoadString(btnID);

	HMODULE hGWES = LoadLibraryEx(L"gwes.exe", NULL, LOAD_LIBRARY_AS_DATAFILE);
	int ico = style & 0x70;
	switch (ico) {
		default:
		case 1: m_hIcon = LoadIcon(hGWES, MAKEINTRESOURCE(MB_ICONHAND)); break;
		case 2: m_hIcon = LoadIcon(hGWES, MAKEINTRESOURCE(MB_ICONQUESTION)); break;
		case 3: m_hIcon = LoadIcon(hGWES, MAKEINTRESOURCE(MB_ICONEXCLAMATION)); break;
		case 4: m_hIcon = LoadIcon(hGWES, MAKEINTRESOURCE(MB_ICONASTERISK)); break;
	}
}

CMessageDlg::~CMessageDlg() {
	LOG0(5, "CMessageDlg::~CMessageDlg()");

}

void CMessageDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CMessageDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_Control(pDX, IDC_ICO, m_ctlIcon);
	DDX_Control(pDX, IDC_TEXT, m_ctlText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDlg, CCeDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	ON_COMMAND(IDOK, OnOk)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(IDYES, OnYes)
	ON_COMMAND(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

void SetMenuCaption(HWND hwndCmdBar, UINT nCmd, const CString &text) {
	TBBUTTONINFO tbButtonInfo = { 0 };
	tbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask = TBIF_TEXT;

	tbButtonInfo.pszText = (LPTSTR) (LPCTSTR) text;
	tbButtonInfo.cchText = text.GetLength();
	::SendMessage(hwndCmdBar, TB_SETBUTTONINFO, nCmd, (LPARAM) &tbButtonInfo);
}

BOOL CMessageDlg::OnInitDialog() {
	LOG0(3, "CMessageDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

//	m_ctlIcon.SetIcon(m_hIcon, TRUE);
	SendDlgItemMessageW(GetSafeHwnd(), IDC_ICO, STM_SETIMAGE, IMAGE_ICON, (LPARAM) m_hIcon);

	if ((m_nStyle & MB_YESNO) && (!m_strSKLeft.IsEmpty())) SetMenuCaption(m_hwndCmdBar, IDYES, m_strSKLeft);

	m_ctlText.SetWindowText(m_strText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageDlg::ResizeControls() {
	LOG0(1, "CMessageDlg::ResizeControls()");

}

void CMessageDlg::OnOk() {
	EndDialog(IDOK);
}

void CMessageDlg::OnCancel() {
	EndDialog(IDCANCEL);
}

void CMessageDlg::OnYes() {
	EndDialog(IDYES);
}

void CMessageDlg::OnNo() {
	EndDialog(IDNO);
}
