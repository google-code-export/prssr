// prssr.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "prssr.h"
#include "../share/uihelper.h"

#include "MainFrm.h"
#include "ctrls/CeDialog.h"
#include "ctrls/LinkCtrl.h"
#include "ctrls/InfoBar.h"

//#include "Site.h"
#include "Appearance.h"
#include "Config.h"
#include "../share/notif.h"

#include "net\ssl.h"

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
// CAboutDlg dialog used for App About

class CAboutDlg : public CCeDialog
{
public:
	CAboutDlg();
	virtual ~CAboutDlg();

// Dialog Data
	enum { 
		IDD = IDD_ABOUTBOX,
		IDD_WIDE = IDD_ABOUTBOX_WIDE
	};
	//{{AFX_DATA(CAboutDlg)
	CLinkCtrl m_ctlHomepageLink;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();		// Added for WCE apps
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHomePage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CCeDialog(InWideMode() ? CAboutDlg::IDD_WIDE : CAboutDlg::IDD) {
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_CANCEL;
}

CAboutDlg::~CAboutDlg() {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_HOMEPAGE_LINK, m_ctlHomepageLink);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CCeDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_HOMEPAGE_LINK, OnHomePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp commands
// Added for WCE apps

BOOL CAboutDlg::OnInitDialog() {
	LOG0(5, "CAboutDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	m_strCaption.Empty();

	return TRUE;
}

void CAboutDlg::OnSize(UINT nType, int cx, int cy) {
	CCeDialog::OnSize(nType, cx, cy);
	
	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (!InWideMode()) {
		m_ctlHomepageLink.SetWindowPos(NULL, 0, cy - SCALEY(20), cx - SCALEX(10), SCALEY(20), SWP_NOZORDER);
	}
}

void CAboutDlg::OnHomePage() { 
	SHELLEXECUTEINFO ei = { 0 };
	ei.cbSize = sizeof(ei);
	ei.fMask = SEE_MASK_NOCLOSEPROCESS;
	CString strLink;
	m_ctlHomepageLink.GetWindowText(strLink);
	ei.lpFile = strLink;
	ShellExecuteEx(&ei);
}

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

	CAboutDlg aboutDlg;
	aboutDlg.DoModal();

#if 0
	// testing url download
/*	CDownloadData dd(
		_T("http://digiarena.zive.cz/GetThumbNail.aspx?id_file=3224&amp;width=5000&amp;height=140&amp;q=100"),
		_T("\\program files\\pRSSreader\\a"),
		1,
		CDownloadData::Image
	);

	CWebDownloader wd(_T("\\"), 1);
	wd.DownloadUrl(&dd);
*/	

	// testing HTML translation for offline browsing
/*	CDownloadData dd(_T("url"), _T("\\digiarena.html"));
	CWebDownloader wd(_T("\\"), 1);
	wd.TranslateHtml(&dd);

	// testing link extraction
/*	CWebDownloader wd(_T("\\"), 1);
	CDownloadList list;
	CCache cache;
	wd.ExtractLinksHtml(&dd, list, cache);
	POSITION pos = list.GetHeadPosition();
	while (pos != NULL) {
		CDownloadData *d = list.GetNext(pos);
		LOG1(1, ":%S:", d->URL);
		delete d;
	}
*/

	// testing summary view
//	AfxGetMainWnd()->PostMessage(UWM_UPDATE_FEED, 0, (LPARAM) SiteList.GetAt(30));
//	AfxGetMainWnd()->PostMessage(UWM_UPDATE_FEED, 0, (LPARAM) SiteList.GetAt(25));
#endif
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

	m_hResDLL = LoadLibrary(_T("res.dll"));

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


//	SetupCradlingEvent(Config.CheckOnCradling);
//	SetupPeriodicNotification(Config.UpdateInterval > 0, Config.UpdateInterval);
//	SetupDailyNotification(Config.TimeUpdate, Config.UpdateAtTime);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp message handlers

int CPrssrApp::ExitInstance() {
	LOG0(1, "CPrssrApp::ExitInstance()");

	SiteList.Destroy();

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
