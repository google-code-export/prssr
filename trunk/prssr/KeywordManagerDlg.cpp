// KeywordManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "KeywordManagerDlg.h"
#include "../share/helpers.h"
#include "ProgressDlg.h"
#include "site.h"

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
// CKeywordScanProgressDlg dialog

class CKeywordScanProgressDlg : public CProgressDlg {
public:
	CKeywordScanProgressDlg(CWnd *parent);
	virtual ~CKeywordScanProgressDlg();

	CStringArray Keywords;

protected:
	BOOL Terminate;

    //{{AFX_MSG(CKeywordScanProgressDlg)
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();
//	virtual void PostNcDestroy();

	DWORD Thread();

	DECLARE_MESSAGE_MAP()

	friend DWORD WINAPI KeywordScanThreadProc(LPVOID lpParameter);
};

CKeywordScanProgressDlg::CKeywordScanProgressDlg(CWnd *parent) :
	CProgressDlg(parent)
{
	m_bFullScreen = FALSE;
}

CKeywordScanProgressDlg::~CKeywordScanProgressDlg() {
}

BEGIN_MESSAGE_MAP(CKeywordScanProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CKeywordScanProgressDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_DIALOG_CLOSE, OnClose)
END_MESSAGE_MAP()

BOOL CKeywordScanProgressDlg::OnInitDialog() {
	LOG0(3, "CKeywordScanProgressDlg::OnInitDialog()");

	CProgressDlg::OnInitDialog();

	CloseHandle(CreateThread(NULL, 0, KeywordScanThreadProc, this, 0, NULL));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeywordScanProgressDlg::OnCancel() {
	LOG0(3, "CKeywordScanProgressDlg::OnCancel()");

	Terminate = TRUE;
	EndDialog(IDCANCEL);
}

/*void CKeywordScanProgressDlg::PostNcDestroy() {
	LOG0(1, "CKeywordScanProgressDlg::PostNcDestroy()");
	// EMPTY: to replace functionality of CProgressDlg
}
*/

DWORD CKeywordScanProgressDlg::Thread() {
	LOG0(1, "CKeywordScanProgressDlg::Thread() - start");

	Terminate = FALSE;

	// update keyword flags in feed items
	for (int i = 0; !Terminate && i < SiteList.GetCount(); i++) {
		SetPos(0);

		CSiteItem *si = SiteList.GetAt(i);

		CString sText;
		sText.Format(IDS_SCANNING_FOR_KEYWORDS, si->Name);
		m_ctlLabel.SetWindowText(sText);

		si->EnsureSiteLoaded();

		CFeed *feed = si->Feed;
		if (feed != NULL) {
			SetTotalSize(0, feed->GetItemCount() + 1);

			if (feed != NULL) {
				for (int j = 0; !Terminate && j < feed->GetItemCount(); j++) {
					CFeedItem *fi = feed->GetItem(j);
					fi->SearchKeywords(Keywords);
					AddSize(1);
				}
			}
		}

		AddSize(1);
	}

	if (!Terminate)
		EndDialog(IDOK);

	LOG0(1, "CKeywordScanProgressDlg::Thread() - end");

	return 0;
}

//

DWORD WINAPI KeywordScanThreadProc(LPVOID lpParameter) {
	CKeywordScanProgressDlg *dlg = (CKeywordScanProgressDlg *) lpParameter;
	return dlg->Thread();
}


/////////////////////////////////////////////////////////////////////////////
// CKeywordManagerDlg dialog


CKeywordManagerDlg::CKeywordManagerDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(InWideMode() ? CKeywordManagerDlg::IDD_WIDE : CKeywordManagerDlg::IDD, pParent),
	m_ctlKeywords(IDS_NO_KEYWORDS_DEFINED)
{
	//{{AFX_DATA_INIT(CKeywordManagerDlg)
	//}}AFX_DATA_INIT
	SetMenu(IDR_KEYWORD_MANAGER);
	Mode = -1;
}

CKeywordManagerDlg::~CKeywordManagerDlg() {
}

void CKeywordManagerDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CKeywordManagerDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeywordManagerDlg)
	DDX_Control(pDX, IDC_KEYWORDS, m_ctlKeywords);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeywordManagerDlg, CCeDialog)
	//{{AFX_MSG_MAP(CKeywordManagerDlg)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_KEYDOWN, IDC_KEYWORDS, OnKeydownKeywords)
	ON_NOTIFY(NM_CLICK, IDC_KEYWORDS, OnClickKeywords)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYWORDS, OnItemchangedKeywords)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_KEYWORDS, OnEndlabeleditKeywords)
	//}}AFX_MSG_MAP
	ON_NOTIFY(GN_CONTEXTMENU, IDC_KEYWORDS, OnContextMenu)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_UPDATE_COMMAND_UI(ID_REMOVE, OnUpdateRemove)
	ON_COMMAND(ID_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_NEW, OnNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeywordManagerDlg message handlers

BOOL CKeywordManagerDlg::OnInitDialog() {
	LOG0(3, "CKeywordManagerDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();
	
	m_ctlKeywords.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// fill list control
	CRect rc;
	m_ctlKeywords.GetClientRect(&rc);

	CString sHeading;
	sHeading.LoadString(IDS_KEYWORD);
	m_ctlKeywords.InsertColumn(0, sHeading, LVCFMT_LEFT, rc.Width());

	// new keyword
	CString sText;
	sText.LoadString(IDS_NEW_KEYWORD);
	int item = m_ctlKeywords.InsertItem(0, sText);
	m_ctlKeywords.SetItemData(item, FALSE);

	// keywords
	for (int i = 0; i < Keywords.GetSize(); i++) {
		CString keyword = Keywords.GetAt(i);
		item = m_ctlKeywords.InsertItem(m_ctlKeywords.GetItemCount(), keyword);
		m_ctlKeywords.SetItemData(item, TRUE);
	}

	return TRUE;
}

void CKeywordManagerDlg::UpdateControls() {
}

void CKeywordManagerDlg::OnDestroy() {
	LOG0(3, "CKeywordManagerDlg::OnDestroy()");

	Keywords.RemoveAll();
	for (int i = 1; i < m_ctlKeywords.GetItemCount(); i++) {	// start with 1 to skip ("New Keyword")
		Keywords.Add(m_ctlKeywords.GetItemText(i, 0));
	}

	CCeDialog::OnDestroy();
}

void CKeywordManagerDlg::OnItemchangedKeywords(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(5, "CKeywordManagerDlg::OnItemchangedKeywords()");

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *) pNMHDR;

	UpdateControls();
		
	*pResult = 0;
}

void CKeywordManagerDlg::OnRemove() {
	LOG0(3, "CKeywordManagerDlg::OnRemove()");

	int nSelItem = m_ctlKeywords.GetNextItem(0, LVNI_SELECTED);
	if (nSelItem != -1 && m_ctlKeywords.GetItemData(nSelItem) == TRUE && AfxMessageBox(IDS_DELETE_KEYWORDS, MB_YESNO | MB_ICONQUESTION) == IDYES) {
		m_ctlKeywords.SetRedraw(FALSE);
		int nSelItem = m_ctlKeywords.GetNextItem(0, LVNI_SELECTED);
		while (nSelItem != -1) {
			m_ctlKeywords.DeleteItem(nSelItem);
			nSelItem = m_ctlKeywords.GetNextItem(nSelItem - 1, LVNI_SELECTED);
		}
		m_ctlKeywords.SetRedraw(TRUE);
	}
}

void CKeywordManagerDlg::OnUpdateRemove(CCmdUI *pCmdUI) {
	LOG0(5, "CKeywordManagerDlg::OnUpdateRemove()");

	int item = m_ctlKeywords.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1)
		pCmdUI->Enable(m_ctlKeywords.GetItemData(item) == TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CKeywordManagerDlg::OnEndlabeleditKeywords(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(5, "CKeywordManagerDlg::OnEndlabeleditKeywords()");

	LV_DISPINFO *pDispInfo = (LV_DISPINFO *) pNMHDR;

	BOOL keyword = m_ctlKeywords.GetItemData(pDispInfo->item.iItem);
	if (keyword) {
		if (pDispInfo->item.pszText != NULL) {
			if (lstrlen(pDispInfo->item.pszText) > 0)
				m_ctlKeywords.SetItemText(pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
			else
				m_ctlKeywords.DeleteItem(pDispInfo->item.iItem);
		}
	}
	else {
		if (pDispInfo->item.pszText != NULL && lstrlen(pDispInfo->item.pszText) > 0) {
			// add new
			int item = m_ctlKeywords.InsertItem(m_ctlKeywords.GetItemCount(), pDispInfo->item.pszText);
			m_ctlKeywords.SetItemData(item, 1);
		}

		CString sText;
		sText.LoadString(IDS_NEW_KEYWORD);
		m_ctlKeywords.SetItemText(pDispInfo->item.iItem, 0, sText);
	}

	*pResult = FALSE;
}

void CKeywordManagerDlg::EditKeyword(int item) {
	LOG0(3, "CKeywordManagerDlg::EditKeyword()");

	m_ctlKeywords.SetFocus();
	CEdit *edit = m_ctlKeywords.EditLabel(item);
	if (edit != NULL && m_ctlKeywords.GetItemData(item) == FALSE)	// "new keyword" tapped
		edit->Clear();
}

void CKeywordManagerDlg::OnKeydownKeywords(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(5, "CKeywordManagerDlg::OnKeydownKeywords()");

	LV_KEYDOWN *plvkd = (LV_KEYDOWN *) pNMHDR;
   
	if (plvkd->wVKey == VK_RETURN) {
		int item = m_ctlKeywords.GetNextItem(-1, LVNI_SELECTED);
		if (item != -1)
			EditKeyword(item);
	}

	*pResult = 0;
}

void CKeywordManagerDlg::OnClickKeywords(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(5, "CKeywordManagerDlg::OnClickKeywords()");

	NMLISTVIEW *pnmlv = (NMLISTVIEW *) (pNMHDR);

	int item = m_ctlKeywords.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1)
		EditKeyword(item);

	*pResult = 0;
}

void CKeywordManagerDlg::OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult) {
	LOG0(1, "CKeywordManagerDlg::OnContextMenu()");

	// GN_CONTEXTMENU sends NMRGINFO structure through notify struct parameter
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	CMenu popup;
	popup.LoadMenu(IDR_KEYWORD_MANAGER_CTX);
	CMenu *submenu = popup.GetSubMenu(0);
	submenu->TrackPopupMenu(TPM_LEFTALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);
	
	*pResult = TRUE; // This is important!
}

void CKeywordManagerDlg::OnEdit() {
	LOG0(1, "CKeywordManagerDlg::OnEdit()");

	int item = m_ctlKeywords.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1)
		EditKeyword(item);
}
	
void CKeywordManagerDlg::OnUpdateEdit(CCmdUI *pCmdUI) {
	LOG0(5, "CKeywordManagerDlg::OnUpdateEdit()");

	int item = m_ctlKeywords.GetNextItem(-1, LVNI_SELECTED);
	if (item != -1)
		pCmdUI->Enable(m_ctlKeywords.GetItemData(item) == TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CKeywordManagerDlg::OnNew() {
	LOG0(1, "CKeywordManagerDlg::OnNew()");

	EditKeyword(0);
}

void CKeywordManagerDlg::OnOK() {
	LOG0(3, "CKeywordManagerDlg::OnOK()");

	BOOL dirty = FALSE;
	if (Keywords.GetSize() == m_ctlKeywords.GetItemCount() - 1) {
		for (int i = 0; i < Keywords.GetSize(); i++) {
			CString oldKeyword = Keywords.GetAt(i);
			CString newKeyword = m_ctlKeywords.GetItemText(i + 1, 0);
			if (oldKeyword.CompareNoCase(newKeyword) != 0) {
				dirty = TRUE;
				break;
			}
		}
	}
	else
		dirty = TRUE;

	if (dirty) {
		CKeywordScanProgressDlg *dlg = new CKeywordScanProgressDlg(this);
		for (int i = 1; i < m_ctlKeywords.GetItemCount(); i++)	// start with 1 to skip ("New Keyword")
			dlg->Keywords.Add(m_ctlKeywords.GetItemText(i, 0));

		if (dlg->DoModal() == IDOK) {
			CCeDialog::OnOK();
		}
	}
	else
		EndDialog(IDCANCEL);
}

void CKeywordManagerDlg::ResizeControls() {
	LOG0(1, "CKeywordManagerDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (!InWideMode()) {
		CRect rc;
		GetWindowRect(&rc);
		CRect rcKey;
		m_ctlKeywords.GetWindowRect(&rcKey);
		m_ctlKeywords.SetWindowPos(NULL, 0, 0, rc.Width() - SCALEX(13), rc.bottom - rcKey.top - SCALEY(5), SWP_NOZORDER | SWP_NOMOVE);

		m_ctlKeywords.GetClientRect(&rcKey);
		m_ctlKeywords.SetColumnWidth(0, rcKey.Width());
	}
}
