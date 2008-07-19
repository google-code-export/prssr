// ErrorsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "ErrorsDlg.h"
#include "../share\helpers.h"

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
// CErrorsDlg dialog


CErrorsDlg::CErrorsDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(InWideMode() ? CErrorsDlg::IDD_WIDE : CErrorsDlg::IDD),
	m_ctlErrors(IDS_NO_ERRORS_TO_DISPLAY)
{
	LOG0(5, "CErrorsDlg::CErrorsDlg()");
	//{{AFX_DATA_INIT(CErrorsDlg)
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_ERRORS;
	Retry = FALSE;
}

CErrorsDlg::~CErrorsDlg() {
	LOG0(5, "CErrorsDlg::~CErrorsDlg()");

}

void CErrorsDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CErrorsDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorsDlg)
	DDX_Control(pDX, IDC_ERRORS, m_ctlErrors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrorsDlg, CCeDialog)
	//{{AFX_MSG_MAP(CErrorsDlg)
	ON_WM_DESTROY()
	ON_COMMAND(ID_CLEANUP, OnCleanup)
	//}}AFX_MSG_MAP
	ON_NOTIFY(GN_CONTEXTMENU, IDC_ERRORS, OnContextMenu)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_RETRY, OnRetry)
	ON_UPDATE_COMMAND_UI(ID_RETRY, OnUpdateRetry)
	ON_UPDATE_COMMAND_UI(ID_CLEANUP, OnUpdateRetry)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrorsDlg message handlers

BOOL CErrorsDlg::OnInitDialog() {
	LOG0(3, "CErrorsDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	m_brEditBack.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	// setup of controls
	m_ctlErrors.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	RECT rcErrors;
	m_ctlErrors.GetClientRect(&rcErrors);
	
	int wd = rcErrors.right - rcErrors.left;
	CString strColName;

	// column
	strColName.LoadString(IDS_TIME);
	m_ctlErrors.InsertColumn(0, strColName, LVCFMT_LEFT, (UINT) (wd * 0.3));
	strColName.LoadString(IDS_ERROR);
	m_ctlErrors.InsertColumn(1, strColName, LVCFMT_LEFT, (UINT) (wd * 0.7));

	// fill with errors
	InsertErrors();
	if (m_ctlErrors.GetItemCount() > 0) {
		m_ctlErrors.SetColumnWidth(0, LVSCW_AUTOSIZE);
		m_ctlErrors.SetColumnWidth(1, LVSCW_AUTOSIZE);

		int col0wd = m_ctlErrors.GetColumnWidth(0);
		int col1wd = m_ctlErrors.GetColumnWidth(1);
		if (col0wd + col1wd < wd)
			m_ctlErrors.SetColumnWidth(1, wd - col0wd);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CErrorsDlg::OnDestroy() {
	LOG0(3, "CErrorsDlg::OnDestroy()");

	CCeDialog::OnDestroy();
}

void CErrorsDlg::InsertErrors() {
	Errors.Lock();
	POSITION pos = Errors.GetFirstPos();
	while (pos != NULL) {
		CErrorItem *ei = Errors.GetNext(pos);
		AddErrorItem(ei);
	}
	Errors.Unlock();
}

void CErrorsDlg::AddErrorItem(CErrorItem *item) {
	LOG0(5, "CErrorsDlg::AddErrorItem()");

	// format date
	int len = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &item->Time, NULL, NULL, 0);
	CString strTime;
	LPTSTR time = strTime.GetBufferSetLength(len);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &item->Time, NULL, time, len);
	// insert item
	int nItem = m_ctlErrors.InsertItem(m_ctlErrors.GetItemCount(), time);
	m_ctlErrors.SetItemText(nItem, 1, item->Message);
	m_ctlErrors.SetItemData(nItem, (DWORD) item);
}

void CErrorsDlg::OnItemchangedErrors(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(5, "CErrorsDlg::OnItemchangedErrors()");

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *) pNMHDR;
	
	if (m_ctlErrors.GetNextItem(-1, LVNI_SELECTED) == pNMListView->iItem) {
		CErrorItem *item = (CErrorItem *) pNMListView->lParam;
	}

	*pResult = 0;
}

void CErrorsDlg::OnCleanup() {
	LOG0(3, "CErrorsDlg::OnCleanup()");

	Errors.Cleanup();

	m_ctlErrors.SetRedraw(FALSE);
	m_ctlErrors.DeleteAllItems();
	m_ctlErrors.SetRedraw(TRUE);
}

HBRUSH CErrorsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CCeDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	return hbr;
}

void CErrorsDlg::ResizeControls() {
	LOG0(1, "CErrorsDlg::ResizeControls()");

	CRect rc;
	GetClientRect(&rc);
	m_ctlErrors.SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height() - SCALEY(24), SWP_NOZORDER | SWP_NOMOVE);

	// set column size
	CRect rcErrors;
	m_ctlErrors.GetClientRect(&rcErrors);
	int wd = rcErrors.right - rcErrors.left;
	if (m_ctlErrors.GetItemCount() > 0) {
		m_ctlErrors.SetColumnWidth(1, LVSCW_AUTOSIZE);

		int col0wd = m_ctlErrors.GetColumnWidth(0);
		int col1wd = m_ctlErrors.GetColumnWidth(1);
		if (col0wd + col1wd < wd)
			m_ctlErrors.SetColumnWidth(1, wd - col0wd);
	}
}


void CErrorsDlg::OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(1, "CErrorsDlg::OnContextMenu()");

	// GN_CONTEXTMENU sends NMRGINFO structure through notify struct parameter
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	// TODO: Add refresh item

	CMenu mnu;
	mnu.LoadMenu(IDR_EDIT);

	CMenu *popup = mnu.GetSubMenu(0);
	popup->DeleteMenu(ID_EDIT_PASTE, MF_BYCOMMAND);
	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);

	*pResult = TRUE; // This is important!
}

void CErrorsDlg::OnEditCut() {
	LOG0(1, "CErrorsDlg::OnEditCut()");

	int nItem = m_ctlErrors.GetNextItem(-1, LVNI_SELECTED);
	while (nItem != -1) {
		CErrorItem *item = (CErrorItem *) m_ctlErrors.GetItemData(nItem);
		Errors.Delete(item);
		m_ctlErrors.DeleteItem(nItem);

		nItem = m_ctlErrors.GetNextItem(-1, LVNI_SELECTED);
	}
}

void CErrorsDlg::OnEditCopy() {
	LOG0(1, "CErrorsDlg::OnEditCopy()");

	CString errors;

	int nItem = m_ctlErrors.GetNextItem(-1, LVNI_SELECTED);
	while (nItem != -1) {
		CErrorItem *item = (CErrorItem *) m_ctlErrors.GetItemData(nItem);
		errors += item->Message;
		errors += _T("\r\n");

		nItem = m_ctlErrors.GetNextItem(nItem, LVNI_SELECTED);
	}

	if (!errors.IsEmpty())
		CopyTextToClipboard(GetSafeHwnd(), errors);
}

void CErrorsDlg::OnUpdateEditCut(CCmdUI *pCmdUI) {
	LOG0(3, "CErrorsDlg::OnUpdateEditCut()");

	int nItem = m_ctlErrors.GetNextItem(-1, LVNI_SELECTED);
	pCmdUI->Enable(nItem != -1);
}

void CErrorsDlg::OnUpdateEditCopy(CCmdUI *pCmdUI) {
	LOG0(3, "CErrorsDlg::OnUpdateEditCut()");

	int nItem = m_ctlErrors.GetNextItem(-1, LVNI_SELECTED);
	pCmdUI->Enable(nItem != -1);
}

void CErrorsDlg::OnRefresh() {
	m_ctlErrors.SetRedraw(FALSE);
	m_ctlErrors.DeleteAllItems();
	InsertErrors();
//	m_ctlErrors.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
//	m_ctlErrors.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
//	int col0wd = m_ctlErrors.GetColumnWidth(0);
//	m_ctlErrors.SetColumnWidth(1, nWd - col0wd);
	m_ctlErrors.SetRedraw(TRUE);
}

void CErrorsDlg::OnRetry() {
	LOG0(1, "CErrorsDlg::OnRetry()");

	Retry = TRUE;
	EndDialog(IDOK);
}

void CErrorsDlg::OnUpdateRetry(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_ctlErrors.GetItemCount() > 0);
}
