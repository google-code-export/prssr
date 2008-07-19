// AcceptCertificateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "AcceptCertificateDlg.h"

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
// CAcceptCertificateDlg dialog

CAcceptCertificateDlg::CAcceptCertificateDlg(CWnd *pParent /*=NULL*/)
	: CCeDialog(CAcceptCertificateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAcceptCertificateDlg)
	//}}AFX_DATA_INIT
	Trusted = TRUE;
	ValidDate = FALSE;
	ValidName = TRUE;

	SetMenu(IDR_YESNO);
}


void CAcceptCertificateDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CAcceptCertificateDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAcceptCertificateDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAcceptCertificateDlg, CCeDialog)
	//{{AFX_MSG_MAP(CAcceptCertificateDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDOK, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcceptCertificateDlg message handlers

void CAcceptCertificateDlg::OnPaint() {
	CPaintDC dc(this); // device context for painting
	
	int saveDC = dc.SaveDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	HICON hIcon;
	UINT nID;
	CRect rc;

	// not trusted
	GetDlgItem(IDC_STATIC2)->GetWindowRect(rc);
	ScreenToClient(rc);
	nID = Trusted ? IDI_OK : IDI_WARN;
	hIcon = (HICON) LoadImage(theApp.GetDPISpecificInstanceHandle(), MAKEINTRESOURCE(nID), IMAGE_ICON, 16, 16, 0);
	DrawIconEx(dc.GetSafeHdc(), SCALEX(50), rc.top, hIcon, SCALEX(16), SCALEY(16), 0, NULL, DI_NORMAL);

	// valid date
	GetDlgItem(IDC_STATIC3)->GetWindowRect(rc);
	ScreenToClient(rc);
	nID = ValidDate ? IDI_OK : IDI_WARN;
	hIcon = (HICON) LoadImage(theApp.GetDPISpecificInstanceHandle(), MAKEINTRESOURCE(nID), IMAGE_ICON, 16, 16, 0);
	DrawIconEx(dc.GetSafeHdc(), SCALEX(50), rc.top, hIcon, SCALEX(16), SCALEY(16), 0, NULL, DI_NORMAL);

	// invalid name
	GetDlgItem(IDC_STATIC4)->GetWindowRect(rc);
	ScreenToClient(rc);
	nID = ValidName ? IDI_OK : IDI_WARN;
	hIcon = (HICON) LoadImage(theApp.GetDPISpecificInstanceHandle(), MAKEINTRESOURCE(nID), IMAGE_ICON, 16, 16, 0);
	DrawIconEx(dc.GetSafeHdc(), SCALEX(50), rc.top, hIcon, SCALEX(16), SCALEY(16), 0, NULL, DI_NORMAL);

	dc.RestoreDC(saveDC);
	
	// Do not call CCeDialog::OnPaint() for painting messages
}

BOOL CAcceptCertificateDlg::OnInitDialog() {
	LOG0(3, "CAcceptCertificateDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	HICON hIcon;
	
	hIcon = (HICON) LoadImage(theApp.GetDPISpecificInstanceHandle(), MAKEINTRESOURCE(IDI_LOCK_ALERT), IMAGE_ICON, SCALEX(32), SCALEY(32), 0);
	SendDlgItemMessageW(GetSafeHwnd(), IDC_ALERT_ICON, STM_SETIMAGE, IMAGE_ICON, (LPARAM) hIcon);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAcceptCertificateDlg::UpdateControls() {
	LOG0(5, "CAcceptCertificateDlg::UpdateControls()");

}

void CAcceptCertificateDlg::OnYes() {
	LOG0(1, "CAcceptCertificateDlg::OnYes()");

	EndDialog(IDYES);
}

void CAcceptCertificateDlg::OnNo() {
	LOG0(1, "CAcceptCertificateDlg::OnNo()");

	EndDialog(IDNO);
}
