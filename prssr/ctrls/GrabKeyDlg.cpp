/**
 *  GrabKeyDlg.cpp
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
#include "GrabKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrabKeyDlg dialog


CGrabKeyDlg::CGrabKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrabKeyDlg::IDD, pParent)
{
	m_bFullScreen = FALSE;
	//{{AFX_DATA_INIT(CGrabKeyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	VKey = 0;
}


void CGrabKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrabKeyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrabKeyDlg, CDialog)
	//{{AFX_MSG_MAP(CGrabKeyDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrabKeyDlg message handlers

typedef
	BOOL (__stdcall *UnregisterFunc1Proc) (UINT, UINT);

BOOL CGrabKeyDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	// register all HW buttons
	HINSTANCE hCoreDll;
	UnregisterFunc1Proc procUnregisterFunc;
	hCoreDll = LoadLibrary(_T("coredll.dll"));
	ASSERT(hCoreDll);
	procUnregisterFunc = (UnregisterFunc1Proc) GetProcAddress(hCoreDll, _T("UnregisterFunc1"));
	ASSERT(procUnregisterFunc);
	for (int i = 0xc1; i <= 0xcf; i++) {
		procUnregisterFunc(MOD_WIN, i);
		RegisterHotKey(GetSafeHwnd(), i, MOD_WIN, i);
	}
	FreeLibrary(hCoreDll);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CGrabKeyDlg::OnHotKey(WPARAM wParam, LPARAM lParam) {
	int idHotKey = (int) wParam; 
	UINT fuModifiers = (UINT) LOWORD(lParam); 
	UINT uVirtKey = (UINT) HIWORD(lParam);

	VKey = uVirtKey;
	EndDialog(IDOK);

	return 0;
}

void CGrabKeyDlg::OnDestroy() {
	for (int i = 0xc1; i <= 0xcf; i++) {
		UnregisterHotKey(GetSafeHwnd(), i);
	}

	CDialog::OnDestroy();	
}
