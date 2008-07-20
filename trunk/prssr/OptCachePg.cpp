// OptCachePg.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "OptCachePg.h"

#include "Config.h"
#include "Site.h"
#include "../share/helpers.h"
#include "ctrls/BrowseDlg.h"
#include "ProgressDlg.h"

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
// CMoveCacheProgressDlg dialog

class CMoveCacheProgressDlg : public CProgressDlg {
public:
	CMoveCacheProgressDlg(CWnd *parent);
	virtual ~CMoveCacheProgressDlg();

	CString SrcLocation;
	CString DestLocation;

protected:
	UINT Timer;
	BOOL Terminate;

    //{{AFX_MSG(CMoveCacheProgressDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();
	virtual void PostNcDestroy();

	DWORD Thread();

	DECLARE_MESSAGE_MAP()

	friend DWORD WINAPI MoveCacheThreadProc(LPVOID lpParameter);
};

CMoveCacheProgressDlg::CMoveCacheProgressDlg(CWnd *parent) :
	CProgressDlg(parent)
{
	Timer = 1;
	m_bFullScreen = FALSE;
}

CMoveCacheProgressDlg::~CMoveCacheProgressDlg() {
}

BEGIN_MESSAGE_MAP(CMoveCacheProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CMoveCacheProgressDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_DIALOG_CLOSE, OnClose)
END_MESSAGE_MAP()

BOOL CMoveCacheProgressDlg::OnInitDialog() {
	LOG0(3, "CMoveCacheProgressDlg::OnInitDialog()");

	SetTimer(Timer, 500, NULL);
	CProgressDlg::OnInitDialog();

	CloseHandle(CreateThread(NULL, 0, MoveCacheThreadProc, this, 0, NULL));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMoveCacheProgressDlg::OnDestroy() {
	KillTimer(Timer);
}

void CMoveCacheProgressDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == Timer) {
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CMoveCacheProgressDlg::OnCancel() {
	LOG0(3, "CMoveCacheProgressDlg::OnCancel()");

	Terminate = TRUE;
	EndDialog(IDCANCEL);
}

void CMoveCacheProgressDlg::PostNcDestroy() {
	LOG0(1, "CMoveCacheProgressDlg::PostNcDestroy()");
	delete this;
}

DWORD CMoveCacheProgressDlg::Thread() {
	LOG0(1, "CMoveCacheProgressDlg::Thread() - start");

	Terminate = FALSE;

	CString sText;
	sText.LoadString(IDS_READING_DIR_STRUCTURE);
	m_ctlLabel.SetWindowText(sText + _T("..."));

	// read files in the cache
	CList<CFileData *, CFileData *> list;
	ScanDirForFiles(SrcLocation + _T("\\enclosures"), _T("*.*"), list);
	ScanDirForFiles(SrcLocation + _T("\\favicons"), _T("*.*"), list);
	ScanDirForFiles(SrcLocation + _T("\\feeds"), _T("*.*"), list);
	ScanDirForFiles(SrcLocation + _T("\\html"), _T("*.*"), list);

	// calc total size
	DWORD totalSize = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos != NULL) {
		CFileData *fd = list.GetNext(pos);
		totalSize += fd->Size + 1;
	}

	CStringList copiedFiles;

	// I like to move it, move it
	sText.LoadString(IDS_MOVING);
	m_ctlLabel.SetWindowText(sText + _T("..."));

	SetTotalSize(0, totalSize + 1);

	BOOL bSuccess = TRUE;
	pos = list.GetHeadPosition();
	while (!Terminate && pos != NULL) {
		CFileData *fd = list.GetNext(pos);

		if (fd->Name.Left(SrcLocation.GetLength()).CompareNoCase(SrcLocation) == 0) {
			CString strDestFile;
			strDestFile = DestLocation + _T("\\") + fd->Name.Mid(SrcLocation.GetLength() + 1);

			if (CreatePath(strDestFile) && CopyFile(fd->Name, strDestFile, FALSE)) {
				// ok
				copiedFiles.AddTail(strDestFile);
			}
			else {
				// error
				bSuccess = FALSE;
			}
		}

		AddSize(fd->Size + 1);
	}

	if (Terminate) {
		// remove src files
		POSITION pos = copiedFiles.GetHeadPosition();
		while (pos != NULL) {
			CString fileName = copiedFiles.GetNext(pos);
			DeleteFile(fileName);
		}

		// deal only with our dirs
		RemoveDirectories(DestLocation + _T("\\enclosures"));
		RemoveDirectories(DestLocation + _T("\\favicons"));
		RemoveDirectories(DestLocation + _T("\\feeds"));
		RemoveDirectories(DestLocation + _T("\\html"));

		RemoveDirectory(DestLocation + _T("\\enclosures"));
		RemoveDirectory(DestLocation + _T("\\favicons"));
		RemoveDirectory(DestLocation + _T("\\feeds"));
		RemoveDirectory(DestLocation + _T("\\html"));
	}
	else {
		// remove src files
		POSITION pos = list.GetHeadPosition();
		while (pos != NULL) {
			CFileData *fd = list.GetNext(pos);
			DeleteFile(fd->Name);
		}

		// deal only with our dirs
		RemoveDirectories(SrcLocation + _T("\\enclosures"));
		RemoveDirectories(SrcLocation + _T("\\favicons"));
		RemoveDirectories(SrcLocation + _T("\\feeds"));
		RemoveDirectories(SrcLocation + _T("\\html"));

		RemoveDirectory(SrcLocation + _T("\\enclosures"));
		RemoveDirectory(SrcLocation + _T("\\favicons"));
		RemoveDirectory(SrcLocation + _T("\\feeds"));
		RemoveDirectory(SrcLocation + _T("\\html"));
	}

	AddSize(1);

	// clean up
	while (!list.IsEmpty())
		delete list.RemoveHead();

	if (!Terminate)
		EndDialog(IDOK);

	LOG0(1, "CMoveCacheProgressDlg::Thread() - end");

	return 0;
}

//

DWORD WINAPI MoveCacheThreadProc(LPVOID lpParameter) {
	CMoveCacheProgressDlg *dlg = (CMoveCacheProgressDlg *) lpParameter;
	return dlg->Thread();
}

/////////////////////////////////////////////////////////////////////////////
// COptCachePg property page

IMPLEMENT_DYNCREATE(COptCachePg, CPropertyPage)

COptCachePg::COptCachePg() : CPropertyPage(COptCachePg::IDD) {
	//{{AFX_DATA_INIT(COptCachePg)
	m_strLocation = Config.CacheLocation;
	m_nCacheLimit = Config.CacheLimit;
	m_bCacheImages = Config.CacheImages;
	m_bCacheHtml = Config.CacheHtml;
	//}}AFX_DATA_INIT
}

COptCachePg::~COptCachePg() {
}

void COptCachePg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptCachePg)
	DDX_Control(pDX, IDC_LOCATION, m_ctlLocation);
	DDX_Text(pDX, IDC_LOCATION, m_strLocation);
	DDX_Control(pDX, IDC_BROWSE, m_ctlBrowse);
	DDX_Control(pDX, IDC_FEED_ITEMS_SPIN, m_ctlCacheLimitSpin);
	DDX_Text(pDX, IDC_FEED_ITEMS, m_nCacheLimit);
	DDX_Check(pDX, IDC_STORE_IMAGES, m_bCacheImages);
	DDX_Check(pDX, IDC_CACHE_ONLINE_CONTENT, m_bCacheHtml);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptCachePg, CPropertyPage)
	//{{AFX_MSG_MAP(COptCachePg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptCachePg message handlers

BOOL COptCachePg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	m_ctlBrowse.SetFocus();

	m_ctlCacheLimitSpin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlCacheLimitSpin.SetRange(1, 30000);

	return TRUE;
}

void COptCachePg::OnBrowse() {
	CString strPrevLocation;
	m_ctlLocation.GetWindowText(strPrevLocation);

	CString s;
	s.LoadString(IDS_SELECT_CACHE_LOCATION);

	CBrowseDlg dlg(FALSE, this);
	dlg.SetCaption(s);
	dlg.SetMenu(IDR_BROWSE);
	dlg.m_strPath = strPrevLocation;
	if (dlg.DoModal() == IDOK) {
		if (strPrevLocation.CompareNoCase(dlg.m_strPath) != 0) {
			CreateDirectory(dlg.m_strPath + _T("\\enclosures"), NULL);
			CreateDirectory(dlg.m_strPath + _T("\\favicons"), NULL);
			CreateDirectory(dlg.m_strPath + _T("\\feeds"), NULL);
			CreateDirectory(dlg.m_strPath + _T("\\html"), NULL);

			//
			CMoveCacheProgressDlg *moveDlg = new CMoveCacheProgressDlg(this);
			moveDlg->SrcLocation = strPrevLocation;
			moveDlg->DestLocation = dlg.m_strPath;
			if (moveDlg->DoModal() == IDOK) {
				// set the new cache location
				m_ctlLocation.SetWindowText(dlg.m_strPath);
			}
		}
	}
}

BOOL COptCachePg::OnApply() {
	UpdateData();

	Config.CacheLocation = m_strLocation;
	Config.CacheLimit = m_nCacheLimit;
	Config.CacheHtml = m_bCacheHtml;
	Config.CacheImages = m_bCacheImages;

	return CPropertyPage::OnApply();
}
