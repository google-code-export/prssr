/**
 *  AddFeedDlg.cpp
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
#include "prssr.h"
#include "AddFeedDlg.h"
#include "prssr.h"
#include "Config.h"
#include "../share/helpers.h"

#include "net/HttpConnection.h"
#include "net/Connection.h"
#include "net/Download.h"

#include "Site.h"
#include "Feed.h"
#include "xml/FeedFile.h"
#include "ProgressDlg.h"
#include "ChooseFeedsDlg.h"
#include "www/HTMLFile.h"
#include "www/AutoFeedHtmlFile.h"

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

static CRITICAL_SECTION CSDownloader;

DWORD WINAPI AddThreadProc(LPVOID lpParameter) {
	CAddFeedDlg *dlg = (CAddFeedDlg *) lpParameter;
	return dlg->AddThread();
}

/////////////////////////////////////////////////////////////////////////////
// CAddProgressDlg dialog

class CAddProgressDlg : public CProgressDlg {
public:
	CAddProgressDlg(CDownloader *downloader, CAddFeedDlg *parent);
	virtual ~CAddProgressDlg();

protected:
	UINT Timer;
	UINT ProgressRefreshTimer;
	EDownloadState State;
	CDownloader *Downloader;
	CAddFeedDlg *Parent;

	int RangeHi, PosOffset;

    //{{AFX_MSG(CAddProgressDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	friend class CAddFeedDlg;
};

CAddProgressDlg::CAddProgressDlg(CDownloader *downloader, CAddFeedDlg *parent) {
	Downloader = downloader;
	State = DOWNLOAD_STATE_NONE;
	Timer = 1;
	ProgressRefreshTimer = 2;

	Parent = parent;
	RangeHi = 160000;
	PosOffset = 0;
}

CAddProgressDlg::~CAddProgressDlg() {
}

BEGIN_MESSAGE_MAP(CAddProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CAddProgressDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAddProgressDlg::OnInitDialog() {
	LOG0(3, "CAddProgressDlg::OnInitDialog()");

	SetTimer(Timer, 500, NULL);
	SetTimer(ProgressRefreshTimer, 700, NULL);
	CProgressDlg::OnInitDialog();

	SetRange(0, RangeHi);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddProgressDlg::OnDestroy() {
	KillTimer(Timer);
	KillTimer(ProgressRefreshTimer);

	CProgressDlg::OnDestroy();
}

void CAddProgressDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == Timer) {
/*		EDownloadState state = DOWNLOAD_STATE_NONE;

		EnterCriticalSection(&CSDownloader);
		if (Downloader != NULL)
			state = Downloader->State;
		LeaveCriticalSection(&CSDownloader);

		if (State != state) {
			State = state;

			CString sText;
			switch (state) {
				case DOWNLOAD_STATE_CONNECTING:         sText.LoadString(IDS_CONNECTING); break;
				case DOWNLOAD_STATE_CONNECTED:          sText.LoadString(IDS_CONNECTED); break;
				case DOWNLOAD_STATE_SENDING_REQUEST:    sText.LoadString(IDS_SENDING_REQUEST); break;
				case DOWNLOAD_STATE_RECEIVING_RESPONSE: sText.LoadString(IDS_GETTING_RESPONSE); break;
				case DOWNLOAD_STATE_DATA_TRANSFER:      sText.LoadString(IDS_DATA_TRANSFER); break;
				default:                                sText.LoadString(IDS_SEARCHING); break;
			}

			sText += _T("...");
			m_ctlLabel.SetWindowText(sText);
		}
*/
	}
	else if (nIDEvent == ProgressRefreshTimer) {
		EnterCriticalSection(&CSDownloader);
		if (Downloader != NULL) {
			int newPos = PosOffset + (int) Downloader->GetDownloadedFileSize();
			if (newPos > RangeHi) {
				RangeHi *= 10;
				SetRange(0, RangeHi);
			}
			SetPos(newPos);
		}
		LeaveCriticalSection(&CSDownloader);
	}

	CWnd::OnTimer(nIDEvent);
}

void CAddProgressDlg::OnCancel() {
	LOG0(3, "CAddProgressDlg::OnCancel()");

	KillTimer(ProgressRefreshTimer);
	KillTimer(Timer);

	EnterCriticalSection(&CSDownloader);
	if (Downloader != NULL)
		Downloader->Terminate();
	LeaveCriticalSection(&CSDownloader);

	CProgressDlg::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CAddFeedDlg dialog

CAddFeedDlg::CAddFeedDlg(CWnd *pParent/* = NULL*/)
	: CCeDialog(InWideMode() ? CAddFeedDlg::IDD_WIDE : CAddFeedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFeedDlg)
	//}}AFX_DATA_INIT
//	m_strCaption.LoadString(IDS_NEW_SUBSCRIPTION);
	m_nMenuID = IDR_ADD;

	m_bCheckOnline = TRUE;
	m_pProgress = NULL;
	Downloader = NULL;
	InitializeCriticalSection(&CSDownloader);
}

CAddFeedDlg::~CAddFeedDlg() {
	DeleteCriticalSection(&CSDownloader);
}


void CAddFeedDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CAddFeedDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFeedDlg)
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_URL, m_ctlURL);
	DDX_Text(pDX, IDC_URL, m_strURL);

	DDV_NotEmpty(pDX, m_strURL, IDS_URL_CAN_NOT_BE_EMPTY);
	if (pDX->m_bSaveAndValidate) {
		if (m_strURL.Find(_T("://")) == -1)
			m_strURL = CString(_T("http://")) + m_strURL;

		DWORD srvType;
		CString server, object;
		INTERNET_PORT port;
		if (!ParseURL(m_strURL, srvType, server, object, port)) {
			AfxMessageBox(IDS_INVALID_URL);
			pDX->Fail();
		}
	}

	DDX_Control(pDX, IDC_CHECK_FEED_ONLINE, m_ctlCheckOnline);
	DDX_Check(pDX, IDC_CHECK_FEED_ONLINE, m_bCheckOnline);

	DDX_Control(pDX, IDC_NAME, m_ctlName);
	DDX_Text(pDX, IDC_NAME, m_strName);
	if (pDX->m_bSaveAndValidate) {
		if (!m_bCheckOnline && m_strName.IsEmpty()) {
			AfxMessageBox(IDS_NAME_CAN_NOT_BE_EMPTY);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CAddFeedDlg, CCeDialog)
	//{{AFX_MSG_MAP(CAddFeedDlg)
	ON_COMMAND(ID_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFeedDlg message handlers

BOOL CAddFeedDlg::OnInitDialog() {
	LOG0(3, "CAddFeedDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

	m_ctlURL.SetWindowText(_T("http://"));
//	m_ctlURL.SetWindowText(_T("http://www.lamer.cz/"));
//	m_ctlURL.SetWindowText(_T("http://192.168.2.1/garfield-gz.php"));
//	m_ctlURL.SetWindowText(_T("http://www.kravicky.unas.cz/rss.xml"));
//	m_ctlURL.SetWindowText(_T("http://isnoop.net/rss/theater.php?loc=79912&th=Cinemark+El+Paso&rss=1"));
	m_ctlURL.PostMessage(EM_SETSEL, -1, 0);

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddFeedDlg::OnAdd() {
	LOG0(1, "CAddFeedDlg::OnAdd()");

	if (UpdateData())
		OnOK();
}

void CAddFeedDlg::UpdateControls() {
}

void CAddFeedDlg::ResizeControls() {
	LOG0(3, "CAddFeedDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));
}

void CAddFeedDlg::OnOK() {
	LOG0(1, "CAddFeedDlg::OnOK()");

	if (m_bCheckOnline) {
		if (m_pProgress == NULL) {
			EnterCriticalSection(&CSDownloader);
			Downloader = new CDownloader();
			LeaveCriticalSection(&CSDownloader);

			m_pProgress = new CAddProgressDlg(Downloader, this);
			m_pProgress->OpenDialog(IDS_DOWNLOADING_FEED, this);

			CloseHandle(CreateThread(NULL, 0, AddThreadProc, this, 0, NULL));
		}
		else
			m_pProgress->SetActiveWindow();
	}
	else {
		EndDialog(IDOK);
	}
}


DWORD CAddFeedDlg::AddThread() {
	LOG0(1, "CAddFeedDlg::AddThread() - start");
/*
	BOOL bOK = FALSE;

	CString fileName;
//	fileName.Format(_T("%s\\%s"), Config.CacheLocation, _T("rsr4521.tmp"));
//	fileName.Format(_T("%s\\%s"), Config.CacheLocation, _T("jp.xml"));

			// prepare data structures
			CSiteItem *item = new CSiteItem(NULL, CSiteItem::Site);

			// process downloaded feed file
			CFeedFile xml;
			CFeed *feed = new CFeed;
			if (xml.LoadFromFile(fileName) && xml.Parse(feed, item)) {
				CFeedInfo *info = new CFeedInfo();
				info->FileName = CFeedInfo::GenerateFileNameFromTitle(feed->Title);
				CFeedInfo::EnsureUniqueFileName(info->FileName);
				info->XmlUrl = m_strURL;
				// TODO: set auth info (if it was entered)

				item->Status = CSiteItem::Ok;
				item->Name = feed->Title;
				item->Feed = feed;
				item->Info = info;
				item->CheckFavIcon = FALSE;

				CString sFeedFileName = FormatFilePath(FILE_TYPE_FEED, Config.CacheLocation, info->FileName);
				item->Feed->Save(sFeedFileName);

				SiteItemsToAdd.AddTail(item);

				bOK = TRUE;
			}
			else {
				// TODO: start auto feed discovery

				// for now
				delete feed;
				delete item;
			}
*/

	CList<CHtmlFeed *, CHtmlFeed *> lFeeds;
	lFeeds.AddTail(new CHtmlFeed(_T(""), m_strURL));

	BOOL bOK = FALSE;
	BOOL disconnect;
	if (CheckConnection(Config.AutoConnect, disconnect)) {
		DWORD type;
		CString server, object;
		INTERNET_PORT port;
		ParseURL(m_strURL, type, server, object, port);

		int t = 0;
		while (lFeeds.GetCount() > 0 && !Downloader->IsTerminated()) {
			CHtmlFeed *htmlFeedItem = lFeeds.RemoveHead();

			// temp file name
			CString sFileName;
			LPTSTR fileName = sFileName.GetBufferSetLength(MAX_PATH + 1);
			GetTempFileName(Config.CacheLocation, _T("rsr"), 0, fileName);

			// TODO: get the feed from greader if we use it
			Downloader->Reset();
			if (Downloader->SaveHttpObject(htmlFeedItem->Url, fileName)) {
				// prepare data structures
				CSiteItem *item = new CSiteItem(NULL, CSiteItem::Site);

				// process downloaded file
				CFeedFile xml;
				CFeed *feed = new CFeed;
				if (xml.LoadFromFile(fileName) && xml.Parse(feed, item)) {
					CFeedInfo *info = new CFeedInfo();
					info->FileName = CFeedInfo::GenerateFileNameFromTitle(feed->Title);
					CFeedInfo::EnsureUniqueFileName(info->FileName);
					info->XmlUrl = htmlFeedItem->Url;
					// TODO: set auth info (if it was entered)

					item->Status = CSiteItem::Ok;
					item->Name = feed->Title;
					item->Feed = feed;
					item->Info = info;
					item->CheckFavIcon = FALSE;

					CString sFeedFileName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, info->FileName);
					item->Feed->Save(sFeedFileName);

					// try to get favicon
					CString sFaviconFileName = GetCacheFile(FILE_TYPE_FAVICON, Config.CacheLocation, info->FileName);
					if (DownloadFavIcon(feed->HtmlUrl, sFaviconFileName)) {
						// we've got a favicon
					}

					SiteItemsToAdd.AddTail(item);

					bOK = TRUE;
				}
				else {
					feed->Destroy();
					delete feed;
					delete item;

					// do auto discovery only in first iteration
					if (t == 0) {
						CDiscoverFeedHtmlFile html;
						if (html.LoadFromFile(fileName)) {
							html.DetectEncoding(Downloader->GetCharset());

							lFeeds.RemoveAll();

							CList<CHtmlFeed *, CHtmlFeed *> feeds;
							html.GetRSS(feeds);

							// make all feed URLs absolute
							POSITION pos = feeds.GetHeadPosition();
							while (pos != NULL) {
								CHtmlFeed *hf = feeds.GetNext(pos);
								if (hf->Url.Left(7).CompareNoCase(_T("http://")) == 0 || hf->Url.Left(8).CompareNoCase(_T("https://")) == 0) {
									// full URL -> do nothing
								}
								else if (hf->Url.GetAt(0) == '/') {
									hf->Url.Format(_T("http://%s%s"), server, hf->Url);
								}
								else {
									hf->Url.Format(_T("%s//%s"), m_strURL, hf->Url);
								}

								lFeeds.AddTail(hf);
							}

							if (feeds.GetCount() > 1) {
								CChooseFeedsDlg dlg(AfxGetMainWnd());
								while (!lFeeds.IsEmpty())
									dlg.Feeds.AddTail(lFeeds.RemoveHead());
								if (dlg.DoModal() == IDOK) {
									lFeeds.RemoveAll();
									while (!dlg.Feeds.IsEmpty())
										lFeeds.AddTail(dlg.Feeds.RemoveHead());
								}
							}
						}
					}
				}

				m_pProgress->PosOffset = m_pProgress->m_ctlProgress.GetPos();
			} // save http opbject

			// remove temporary feed file
			DeleteFile(fileName);
			delete htmlFeedItem;
			t++;
		} // while

		if (disconnect)
			Connection.HangupConnection();
	}
	else {
		// error: can not connect to the Internet
		bOK = FALSE;
	}

	m_pProgress->SetPos(m_pProgress->RangeHi);	// fill the progress bar

	// free feeds that might left in lFeeds from auto-discovery (user might cancel the operation)
	while (!lFeeds.IsEmpty())
		delete lFeeds.RemoveHead();

	// done
	DWORD error = 0;
	if (bOK) {
		error = 0;
		CDialog::OnOK();
	}
	else {
		if (Downloader->IsTerminated())
			error = 1;		// terminated
		else {
			error = 2;
			AfxMessageBox(IDS_FEED_NOT_ADDED);
			m_pProgress->CloseDialog();
		}

		SiteItemsToAdd.RemoveAll();
	}

	m_pProgress = NULL;

	EnterCriticalSection(&CSDownloader);
	delete Downloader;
	Downloader = NULL;
	LeaveCriticalSection(&CSDownloader);

	LOG0(1, "CAddFeedDlg::AddThread() - end");

	return error;
}
