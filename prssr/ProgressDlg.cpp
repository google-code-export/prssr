// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
//#include <wceres.h>

#include "../share/helpers.h"
//#include "BrowseDlg.h"
#include "ProgressDlg.h"
#include "Appearance.h"

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
// CProgressDlg dialog

CProgressDlg::CProgressDlg() {
	//{{AFX_DATA_INIT(CProgressDlg)
	//}}AFX_DATA_INIT
	Terminate = FALSE;
}

CProgressDlg::CProgressDlg(CWnd* parent) :
	CDialog(CProgressDlg::IDD, parent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	//}}AFX_DATA_INIT
	Terminate = FALSE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_TEXT, m_ctlLabel);
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDCANCEL, m_ctlCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

void CProgressDlg::PostNcDestroy() {
	delete this;
}

void CProgressDlg::OpenDialog(const CString &strText, CWnd *parent/* = NULL*/) {
	LOG2(3, "CProgressDlg::OpenDialog('%S', %p)", strText, parent);

	m_bFullScreen = FALSE;

	m_strLabel = strText;
	Create(CProgressDlg::IDD, parent);
}

void CProgressDlg::OpenDialog(UINT nID, CWnd *parent/* = NULL*/) {
	LOG2(3, "CProgressDlg::OpenDialog(%d, %p)", nID, parent);

	CString sText;
	sText.LoadString(nID);
	OpenDialog(sText, parent);
}

void CProgressDlg::SetTotalSize(DWORD start, DWORD size) {
	LOG2(3, "CProgressDlg::SetTotalSize(%d, %d)", start, size);

	m_ctlProgress.SetRange32(0, size);
	m_ctlProgress.SetPos(start);
}

BOOL CProgressDlg::AddSize(DWORD amount, BOOL delayedPaint/* = TRUE*/) {
	LOG1(3, "CProgressDlg::AddSize(%d)", amount);

	m_ctlProgress.SetStep(amount);
	m_ctlProgress.StepIt();

	if (Terminate)
		return FALSE;
	else {
		if (!delayedPaint) {
			m_ctlProgress.Invalidate();
			m_ctlProgress.UpdateWindow();
		}

		return TRUE;
	}
}

void CProgressDlg::CloseDialog() {
	LOG0(3, "CProgressDlg::Close()");

	PostMessage(WM_CLOSE);
}

void CProgressDlg::SetRange(int lo, int hi) {
	m_ctlProgress.SetRange32(lo, hi);
}

void CProgressDlg::SetPos(int pos) {
	m_ctlProgress.SetPos(pos);
}

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog() {
	LOG0(3, "CProgressDlg::OnInitDialog()");

	CDialog::OnInitDialog();

	m_ctlLabel.SetWindowText(m_strLabel);

	// remove caption
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) & (~WS_CAPTION));
	ModifyStyleEx(0, WS_EX_NODRAG | WS_EX_DLGMODALFRAME);

	CenterWindow();
	SetForegroundWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProgressDlg::OnCancel() {
	LOG0(3, "CProgressDlg::OnCancel()");

	Terminate = TRUE;
	DestroyWindow();
}

void CProgressDlg::OnSize(UINT nType, int cx, int cy) {
	LOG0(3, "CProgressDlg::OnSize()");

	// do nothing - no resizing
}

void CProgressDlg::OnClose() {
	DestroyWindow();
}

void CProgressDlg::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized) {
	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CProgressDlg::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) {
	CWnd::OnSettingChange(uFlags, lpszSection);
}

void CProgressDlg::ShowError(UINT nID) {
	CString s;
	s.LoadString(nID);
	ShowError(s);
}

void CProgressDlg::ShowError(const CString &strText) {
	m_ctlProgress.ShowWindow(SW_HIDE);
	m_ctlLabel.SetWindowText(strText);

	CString sText;
	sText.LoadString(IDS_CLOSE);
	m_ctlCancel.SetWindowText(sText);
}
