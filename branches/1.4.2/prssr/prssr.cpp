/**
 *  prssr.cpp : Defines the class behaviors for the application.
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
 *  $Id$
 */

#include "StdAfx.h"
#include "prssr.h"
#include "../share/UIHelper.h"

#include "MainFrm.h"
//#include "ctrls/CeDialog.h"
#include "ctrls/InfoBar.h"
#include "ctrls/CePropertySheet.h"

//#include "Site.h"
#include "Appearance.h"
#include "Config.h"
#include "../share/notif.h"

#include "net/ssl.h"

#include "AboutPg.h"
#include "LicensePg.h"
#include "CreditsPg.h"

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

//HANDLE HUpdateFeedFlagsEvent = NULL;
UINT ReadConfigMessage = 0;
UINT ReloadSubscriptionsMessage = 0;
UINT CheckFeedsMessage = 0;
UINT UpdateFeedFlagsMessage = 0;

OSVERSIONINFO OSVI = { 0 };

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp

BEGIN_MESSAGE_MAP(CPrssrApp, CWinApp)
	//{{AFX_MSG_MAP(CPrssrApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp construction

CPrssrApp::CPrssrApp()
	: CWinApp()
{
	HIDPI_InitScaling();

	Appearance.Load();
	Appearance.Create();

	// have to load config very early to have configuration for logging
	Config.Load();
	Config.LoadUI();
	Config.LoadProxyProfiles();
	Config.LoadSocialBookmarkingSites();
	Config.LoadKeywords();
	Config.LoadRewriteRules();

#ifdef LOGGING
	Logger = new CLogger(Config.LogFile, Config.LogLevel);
#endif
#ifdef MYDEBUG
	gb = new garbageCollector();
#endif

	ReadConfigMessage = RegisterWindowMessage(READ_CONFIG_MESSAGE);
	ReloadSubscriptionsMessage = RegisterWindowMessage(RELOAD_SUBSCRIPTIONS_MESSAGE);
	CheckFeedsMessage = RegisterWindowMessage(CHECK_FEEDS_MESSAGE);
	UpdateFeedFlagsMessage = RegisterWindowMessage(UPDATE_FEED_FLAGS_MESSAGE);

	GetVersionEx(&OSVI);
}

CPrssrApp::~CPrssrApp() {
#ifdef MYDEBUG
	delete gb;
#endif
#ifdef LOGGING
	delete Logger;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPrssrApp object

CPrssrApp theApp;

// App command to run the dialog
void CPrssrApp::OnAppAbout() {
	LOG0(3, "CPrssrApp::OnAppAbout()");

	CAboutPg pgAbout;
	CLicensePg pgLicense;
	CCreditsPg pgCredits;

	CCePropertySheet sheet(IDS_ABOUT);
	sheet.AddPage(&pgAbout);
	sheet.AddPage(&pgLicense);
	sheet.AddPage(&pgCredits);
	sheet.SetMenu(IDR_CANCEL);
	sheet.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp initialization

BOOL CPrssrApp::InitInstance() {
	LOG0(1, "CPrssrApp::InitInstance()");

	LoadSSL();
	// init win sock
	WSADATA wsaData;
	WSAStartup(0x0101, &wsaData);

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("DaProfik"));

	// register wnd class
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = PRSSR_CLASS_NAME;
	if (!AfxRegisterClass(&wc))
		return FALSE;

	CFeedView::Register();
	CGroupView::Register();
	CTextProgressCtrl::Register();
	CInfoBar::Register();

	// get DPI
	HDC hdcScreen = ::GetDC(NULL);
	int nSystemDPI = ::GetDeviceCaps(hdcScreen, LOGPIXELSX);
	::ReleaseDC(NULL, hdcScreen);

	// load the resource library according to the system DPI
	CString resLibName;
	resLibName.Format(_T("res.%03d.dll"), nSystemDPI);
	m_hResDLL = LoadLibrary(resLibName);

	if (!InitHTMLControl(AfxGetInstanceHandle()))
		return 0;

	// Initialize the control
	INITCOMMONCONTROLSEX icce;
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_DATE_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_UPDOWN_CLASS | ICC_TREEVIEW_CLASSES | ICC_TOOLTIP_CLASSES | ICC_TAB_CLASSES;
	InitCommonControlsEx(&icce);

	SHInitExtraControls();

	// frame
	CMainFrame* pFrame = new CMainFrame();
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW, NULL, NULL);

	// process cmd line
	CString strCmdLine = m_lpCmdLine;

	// minimized ?
	if (strCmdLine.Find(_T("/minimized")) != -1) {
		pFrame->ShowWindow(SW_MINIMIZE);
	}
	else {
		pFrame->ShowWindow(m_nCmdShow);
		pFrame->UpdateWindow();
	}

	if (strCmdLine.Find(_T("/updateall")) != -1) {
		pFrame->PostMessage(UWM_UPDATE_ALL);
	}

	int npos;
	if ((npos = strCmdLine.Find(_T("/opensite"))) != -1) {
		CString strSite = strCmdLine.Mid(npos + 10);
		int site;
		if (swscanf(strSite.GetBuffer(strSite.GetLength()), _T("%d"), &site) == 1) {
			// after sites are loaded, site with this value is opened
			pFrame->PostMessage(UWM_OPEN_SITE, site, 0);
		}
	}

	if ((npos = strCmdLine.Find(_T("/openitem"))) != -1) {
		CString sPar = strCmdLine.Mid(npos + 10);

/*		CString sSite, sFeedIdx;

		int nSpacePos = sPar.Find(' ');
		if (nSpacePos != -1) {
			sSite = sPar.Left(nSpacePos);
			sFeedIdx = sPar.Mid(nSpacePos + 1);
		}
		else {
			sSite = sPar;
		}

		int site;
		if (swscanf(sSite.GetBuffer(sSite.GetLength()), _T("%d"), &site) == 1) {
			pFrame->PostMessage(UWM_OPEN_FEEDITEM, site, (LPARAM) (LPCTSTR) FeedItemHash);
		}
*/
		int site, feedIdx;
		if (swscanf(sPar.GetBuffer(sPar.GetLength()), _T("%d %d"), &site, &feedIdx) == 2) {
			pFrame->PostMessage(UWM_OPEN_FEEDITEM, site, (LPARAM) feedIdx);
		}
		sPar.ReleaseBuffer();
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp message handlers

int CPrssrApp::ExitInstance() {
	LOG0(1, "CPrssrApp::ExitInstance()");

	SiteList.Destroy();
	UnreadItems.Destroy();
	FlaggedItems.Destroy();

	WSACleanup();
	FreeSSL();

	Config.Destroy();

	return CWinApp::ExitInstance();
}


BOOL CPrssrApp::IsIdleMessage(MSG* pMsg) {
	if (CWinApp::IsIdleMessage(pMsg) == FALSE)
		return FALSE;

	return (pMsg->message != 0x3FC);
}
