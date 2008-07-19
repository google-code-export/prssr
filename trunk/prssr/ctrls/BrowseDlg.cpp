// browsedlg.cpp : implementation file
//

#include "../stdafx.h"
#include "../prssr.h"
#include "browsedlg.h"
//#include "Session.h"
#include "../../share/helpers.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICON_DEVICE						0
#define ICON_DIR						1
#define ICON_FILE						2

/////////////////////////////////////////////////////////////////////////////
// CBrowseDlg dialog

//
// For sorting files and dirs (dirs go before files)
//
static int CALLBACK 
DirFileCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	LOG0(5, "DirFileCompareProc()");

	CTreeCtrl *pTreeCtrl = (CTreeCtrl *) lParamSort;
	HTREEITEM hItem1 = (HTREEITEM) lParam1;
	HTREEITEM hItem2 = (HTREEITEM) lParam2;

	int nImage1, nSelImage1;
	pTreeCtrl->GetItemImage(hItem1, nImage1, nSelImage1);
	int nImage2, nSelImage2;
	pTreeCtrl->GetItemImage(hItem2, nImage2, nSelImage2);

	if (nImage1 == ICON_DIR && nImage2 >= ICON_FILE)
		return -1;
	else if (nImage1 >= ICON_FILE && nImage2 == ICON_DIR)
		return 1;
	else {
		CString strItem1 = pTreeCtrl->GetItemText(hItem1);
		CString strItem2 = pTreeCtrl->GetItemText(hItem2);
		return strItem1.CompareNoCase(strItem2);
	}
}


CBrowseDlg::CBrowseDlg(BOOL bFiles, CWnd* pParent /*=NULL*/)
	: CCeDialog(InWideMode() ? CBrowseDlg::IDD_WIDE : CBrowseDlg::IDD, pParent)
{
//	SESSION_ADD(SESSION_BROWSE_DLG);

	//{{AFX_DATA_INIT(CBrowseDlg)
//	m_strCaption = _T("");
	m_strPath = _T("");
	//}}AFX_DATA_INIT

	m_bFiles = bFiles;
//	m_bNewFolderBtn = TRUE;
	m_bExpandDelete = TRUE;
	m_strExts = _T("");
	m_nMenuID = IDR_CANCEL;
}

CBrowseDlg::~CBrowseDlg() {
//	SESSION_REMOVE;
}


void CBrowseDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CBrowseDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowseDlg)
//	DDX_Control(pDX, IDC_CAPTION, m_ctlCaption);
	DDX_Control(pDX, IDC_DIRS, m_ctlDirs);
	DDX_Control(pDX, IDC_PATH, m_ctlPath);
//	DDX_Text(pDX, IDC_CAPTION, m_strCaption);
	DDX_Text(pDX, IDC_PATH, m_strPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowseDlg, CCeDialog)
	//{{AFX_MSG_MAP(CBrowseDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DIRS, OnSelchangedDirs)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_DIRS, OnEndlabeleditDirs)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_DIRS, OnItemexpandingDirs)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseDlg message handlers

BOOL CBrowseDlg::OnInitDialog() {
	LOG0(3, "CBrowseDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	// make extension list
	int nStart = 0;
	int nSemiCol = m_strExts.Find(';');
	while (nSemiCol != -1) {
		m_arExts.Add(m_strExts.Mid(nStart, nSemiCol - nStart));
		nStart = nSemiCol + 1;
		nSemiCol = m_strExts.Find(';', nStart);
	}
	m_arExts.Add(m_strExts.Mid(nStart));

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_oImageList.Create(IDB_DIRS, SCALEX(16), 10, RGB(255, 0, 255));
	AfxSetResourceHandle(AfxGetInstanceHandle());

	SHFILEINFO sfi = { 0 };
	// folder
	ZeroMemory(&sfi, sizeof(sfi));
	SHGetFileInfo(_T("\\"), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);
	m_oImageList.Add(sfi.hIcon);

	m_ctlDirs.SetImageList(&m_oImageList, TVSIL_NORMAL);

//	m_ctlCaption.SetTitle(m_strCaption);

	// sanitize path
	if (m_strPath.Right(1).CompareNoCase(_T("\\")) != 0)
		m_strPath += _T("\\");

	FillMe();
	SelectPath(m_strPath);

	SetForegroundWindow();
//	SESSION_RESTORE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBrowseDlg::SelectPath(const CString &path) {
	LOG1(3, "CBrowseDlg::SelectPath('%S')", path);

	HTREEITEM hParent = m_hRootItem;
	int nStart = 1;
	int nPos = path.Find('\\', nStart);
	while (nPos != -1) {
		CString strDirName = path.Mid(nStart, nPos - nStart);
		// find and select
		if (m_ctlDirs.ItemHasChildren(hParent)) {
			BOOL bFound = FALSE;
			HTREEITEM hItem = m_ctlDirs.GetChildItem(hParent);
			while (hItem != NULL) {
				CString strItemText = m_ctlDirs.GetItemText(hItem);
				if (strItemText.CompareNoCase(strDirName) == 0) {
					bFound = TRUE;
					break;
				}
				hItem = m_ctlDirs.GetNextSiblingItem(hItem);
			}

			if (bFound) {
				hParent = hItem;
				m_ctlDirs.Expand(hItem, TVE_EXPAND);
			}
		}
		else
			return;

		nStart = nPos + 1;
		nPos = path.Find('\\', nStart);
	}

	m_ctlDirs.SelectItem(hParent);
	m_ctlDirs.EnsureVisible(hParent);
}

HTREEITEM CBrowseDlg::AddDirItem(HTREEITEM hParent, CString strItem, int nIcon, BOOL bPerm, HTREEITEM hInsertAfter/* = TVI_LAST*/) {
	LOG2(5, "CBrowseDlg::AddDirItem(, '%S', %d)", strItem, nIcon);

	HTREEITEM hItem = m_ctlDirs.InsertItem(strItem, nIcon, nIcon, hParent, hInsertAfter);
	m_ctlDirs.SetItemData(hItem, (DWORD) hItem);
	return hItem;
}

void CBrowseDlg::AddPossibleChild(HTREEITEM hParent, CString strPath) {
	LOG1(5, "CBrowseDlg::AddPossibleChild(, '%S')", strPath);

	WIN32_FIND_DATA fd;
	CString mask;
	mask.Format(_T("%s*.*"), strPath);
	HANDLE hFind = FindFirstFile(mask, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				AddDirItem(hParent, fd.cFileName, ICON_DIR, FALSE);
				break;
			}
			else if (m_bFiles) {
				CString strFileName = CString(fd.cFileName);
				if (HasExt(strFileName)) {
					AddDirItem(hParent, fd.cFileName, ICON_FILE, FALSE);
					break;
				}
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}

BOOL CBrowseDlg::HasExt(const CString &strFileName) {
	if (m_strExts.GetLength() == 0)
		return TRUE;

	for (int i = 0; i < m_arExts.GetSize(); i++) {
		CString sExt = m_arExts[i];
		if (strFileName.Right(sExt.GetLength()).CompareNoCase(sExt) == 0)
			return TRUE;
	}

	return FALSE;
}

void CBrowseDlg::AddPathItems(HTREEITEM hParent, CString strPath) {
	LOG1(5, "CBrowseDlg::AddPathItems(, '%S')", strPath);

	WIN32_FIND_DATA fd;
	CString mask;
	mask.Format(_T("%s*.*"), strPath);
	HANDLE hFind = FindFirstFile(mask, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				HTREEITEM hItem = AddDirItem(hParent, fd.cFileName, ICON_DIR, FALSE);

				// add one child, to display the plus sing
				CString strSubDir = strPath + fd.cFileName + _T("\\");
				AddPossibleChild(hItem, strSubDir);
			}
			else if (m_bFiles) {
				CString strFileName = CString(fd.cFileName);
				if (HasExt(strFileName)) {
					CString strFilePath = strPath + strFileName;
					SHFILEINFO sfi = { 0 };
					SHGetFileInfo(strFilePath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON); // | SHGFI_SYSICONINDEX);
					int nIdx = m_oImageList.Add(sfi.hIcon);
					if (nIdx == -1)
						nIdx = ICON_FILE;

					HTREEITEM hItem = AddDirItem(hParent, strFileName, nIdx, FALSE);
					DeleteObject(sfi.hIcon);
				}
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);


		// Sort the items in the tree control using my callback procedure.
		TVSORTCB tvs;
		tvs.hParent = hParent;
		tvs.lpfnCompare = DirFileCompareProc;
		tvs.lParam = (LPARAM) &m_ctlDirs;
		m_ctlDirs.SortChildrenCB(&tvs);
	}
}

void CBrowseDlg::FillMe() {
	LOG0(3, "CBrowseDlg::FillMe()");

	m_ctlDirs.DeleteAllItems();

	// TODO: GetDeviceName
	CString sMyDevice;
	sMyDevice.LoadString(IDS_MYDEVICE);

	m_hRootItem = AddDirItem(TVI_ROOT, sMyDevice, ICON_DEVICE, FALSE);
	AddPathItems(m_hRootItem, _T("\\"));
	m_ctlDirs.Expand(m_hRootItem, TVE_EXPAND);
}

void CBrowseDlg::OnSelchangedDirs(NMHDR* pNMHDR, LRESULT* pResult) {
	LOG0(5, "CBrowseDlg::OnSelchangedDirs()");

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	// set path
	m_ctlPath.SetWindowText(GetFullPath(pNMTreeView->itemNew.hItem));

	UpdateControls();

	*pResult = 0;
}

/*
BOOL CBrowseDlg::IsSelectedItemValid() {
	// check selected item
	CString path;
	m_ctlPath.GetWindowText(path);
	DWORD attrs = GetFileAttributes(path);
	if (m_bFiles) {
		if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
			UpdateData();
			return TRUE;
		}
		else {
			AfxMessageBox(IDS_NOT_FILE);
		}
	}
	else {
		if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
			UpdateData();
			return TRUE;
		}
		else {
			AfxMessageBox(IDS_LOCATION_NOT_DIR);
		}
	}

	return FALSE;
}

void CBrowseDlg::OnOk() {
	LOG0(1, "CBrowseDlg::OnOk()");

	// if we are in editing a directory name, end editing
	if (m_ctlDirs.GetEditControl() != NULL) {
		m_ctlDirs.SendMessage(TVM_ENDEDITLABELNOW, FALSE);
//		return;
	}

	if (IsSelectedItemValid())
		CCeDialog::OnOK();
}
*/

void CBrowseDlg::OnOk() {
	// if we are in editing a directory name, end editing
	if (m_ctlDirs.GetEditControl() != NULL) {
		m_ctlDirs.SendMessage(TVM_ENDEDITLABELNOW, FALSE);
		return;
	}

	// check selected item
	CString path;
	m_ctlPath.GetWindowText(path);
	DWORD attrs = GetFileAttributes(path);
	if (m_bFiles) {
		if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
			UpdateData();
			CCeDialog::OnOK();
		}
		else {
			AfxMessageBox(IDS_NOT_FILE);
		}
	}
	else {
		if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
			UpdateData();
			CCeDialog::OnOK();
		}
		else {
			AfxMessageBox(IDS_LOCATION_NOT_DIR);
		}
	}
}

void CBrowseDlg::OnCancel() {
	LOG0(1, "CBrowseDlg::OnCancel()");

	// if we are in editing a directory name, end editing
	if (m_ctlDirs.GetEditControl() != NULL) {
		m_ctlDirs.SendMessage(TVM_ENDEDITLABELNOW, TRUE);
		return;
	}
	
	CCeDialog::OnCancel();
}

void CBrowseDlg::UpdateControls() {
	LOG0(5, "CBrowseDlg::UpdateControls()");

}

void CBrowseDlg::OnNewFolder() {
	LOG0(3, "CBrowseDlg::OnNewFolder()");

	HTREEITEM hSelItem = m_ctlDirs.GetSelectedItem();
	if (hSelItem == NULL) return;

	m_ctlDirs.ModifyStyle(0, TVS_EDITLABELS);

	m_ctlDirs.Expand(hSelItem, TVE_EXPAND);
	m_bExpandDelete = FALSE;
	HTREEITEM hNewItem = AddDirItem(hSelItem, _T(""), ICON_DIR, FALSE, TVI_FIRST);
	m_ctlDirs.Expand(hSelItem, TVE_EXPAND);
	m_bExpandDelete = TRUE;

	m_ctlDirs.EnsureVisible(hNewItem);
	m_ctlDirs.EditLabel(hNewItem);
}

void CBrowseDlg::OnEndlabeleditDirs(NMHDR* pNMHDR, LRESULT* pResult) {
	LOG0(5, "CBrowseDlg::OnEndlabeleditDirs()");

	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	if (pTVDispInfo->item.pszText != NULL &&
		lstrlen(pTVDispInfo->item.pszText) > 0) {

		m_ctlDirs.SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);
		CString strPath = GetFullPath(pTVDispInfo->item.hItem);

		if (CreateDirectory(strPath, NULL)) {
			m_ctlDirs.SelectItem(pTVDispInfo->item.hItem);
			*pResult = TRUE;
		}
		else {
			SysError(IDS_DIRECTORY_NOT_CREATED, GetLastError(), FALSE);
			m_ctlDirs.DeleteItem(pTVDispInfo->item.hItem);
			*pResult = FALSE;
		}

	}
	else {
		m_ctlDirs.DeleteItem(pTVDispInfo->item.hItem);
		*pResult = FALSE;
	}

	m_ctlDirs.ModifyStyle(TVS_EDITLABELS, 0);
}

CString CBrowseDlg::GetFullPath(HTREEITEM hItem) {
	LOG0(5, "CBrowseDlg::GetFullPath()");

	CString path = _T("");
	HTREEITEM hParent = hItem;
	while (hParent != m_hRootItem) {
		path = _T("\\") + m_ctlDirs.GetItemText(hParent) + path;
		hParent = m_ctlDirs.GetParentItem(hParent);
	} 

	if (path.GetLength() == 0)
		path = _T("\\");

	return path;
}

void CBrowseDlg::OnItemexpandingDirs(NMHDR* pNMHDR, LRESULT* pResult) {
	LOG0(3, "CBrowseDlg::OnItemexpandingDirs()");

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (m_bExpandDelete && pNMTreeView->action & TVE_EXPAND) {
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		// remove child that caused displaying of plus sign
		if (m_ctlDirs.ItemHasChildren(hItem)) {
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_ctlDirs.GetChildItem(hItem);

			while (hChildItem != NULL) {
				hNextItem = m_ctlDirs.GetNextItem(hChildItem, TVGN_NEXT);
				m_ctlDirs.DeleteItem(hChildItem);
				hChildItem = hNextItem;
			}
		}

		AfxGetApp()->DoWaitCursor(1);
		CString strPath = GetFullPath(hItem);
		if (strPath.Compare(_T("\\")) != 0)
			strPath += _T("\\");
		AddPathItems(hItem, strPath);
		AfxGetApp()->DoWaitCursor(-1);
	}

	*pResult = 0;
}

/*
void CBrowseDlg::Restore() {
	LOG0(3, "CBrowseDlg::Restore()");
	Session.Next();
}
*/

void CBrowseDlg::OnSize(UINT nType, int cx, int cy) {
	LOG0(3, "CBrowseDlg::OnSize()");

	CCeDialog::OnSize(nType, cx, cy);

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));

	if (!InWideMode()) {
		m_ctlDirs.SetWindowPos(NULL, 0, 0, cx - SCALEX(12), cy - SCALEY(26 + 32), SWP_NOZORDER | SWP_NOMOVE);
	}
}
