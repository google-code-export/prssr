/**
 *  prssrtoday.cpp : Defines the initialization routines for the DLL.
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
#include "prssrtoday.h"
#include "todaycmn.h"
#include "TodayPluginWnd.h"
#include "Config.h"

#ifdef LOGGING
	#include "../prssr/debug/Logger.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

//
// global variables
//
//HINSTANCE			DLLInstance;

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../prssr/debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

// forward function declarations
static INT InitilizeClasses();

SHACTIVATEINFO		g_sai;


/////////////////////////////////////////////////////////////////////////////
// CPrssrtodayApp

BEGIN_MESSAGE_MAP(CPrssrtodayApp, CWinApp)
	//{{AFX_MSG_MAP(CPrssrtodayApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrssrtodayApp construction

CPrssrtodayApp::CPrssrtodayApp() {
	Config.Load();
#ifdef LOGGING
	Logger = new CLogger(Config.LogFile, Config.LogLevel);
#endif
}

CPrssrtodayApp::~CPrssrtodayApp() {
#ifdef LOGGING
	delete Logger;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPrssrtodayApp object

CPrssrtodayApp theApp;

//
// Initilize the DLL by creating a new window
//
HWND InitializeCustomItem(TODAYLISTITEM *ptli, HWND hwndParent) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (FAILED(CoInitializeEx(NULL, 0)))
		return NULL;

	CTodayPluginWnd::Unregister();
	if (!CTodayPluginWnd::Register())
		return NULL;

	TodayPluginWnd.Create(hwndParent);
	TodayPluginWnd.ShowWindow(SW_SHOWNORMAL);
	TodayPluginWnd.UpdateWindow();

	return TodayPluginWnd.GetSafeHwnd();
}

BOOL APIENTRY CustomItemOptionsDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	switch (message) {
		case WM_INITDIALOG:
			TodayPluginWnd.OpenConfig();
//			PostQuitMessage(0);
			SendMessage(hDlg, WM_DESTROY, 0, 0);
			return TRUE;
	}

	return DefWindowProc(hDlg, message, wParam, lParam);

//	return ConfigDlg.WindowProc(message, wParam, lParam);
}


BOOL CPrssrtodayApp::InitInstance() {
	HIDPI_InitScaling();

	SHInitExtraControls();

	LoadSiteList(SiteList);

	return CWinApp::InitInstance();
}

int CPrssrtodayApp::ExitInstance() {
	Config.Save();
	CTodayPluginWnd::Unregister();

	return CWinApp::ExitInstance();
}
