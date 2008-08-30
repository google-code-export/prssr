/**
 *  UpdateBar.cpp
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
#include "../share/UIHelper.h"

#include "UpdateBar.h"
#include "Appearance.h"
#include "../share/helpers.h"
#include "MainFrm.h"
#include "Config.h"
#include "Errors.h"
#include "SuspendKiller.h"

#include "net/Connection.h"
#include "net/Download.h"
#include "xml/FeedFile.h"
#include "Site.h"
#include "www/LocalHtmlFile.h"
#include "misc/shnotif.h"

#include "sync/GReaderSync.h"

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

#define BUFSIZE						8192

static BOOL TranslateForOfflineReading(const CString &srcFileName, const CString &destFileName, const CString &srcEncoding = _T("windows-1251")) {
	LOG2(5, "TranslateForOfflineReading('%S', '%S')", srcFileName, destFileName);

	BOOL translated = FALSE;

	CLocalHtmlFile file;
	file.LoadFromFile(srcFileName);
	file.DetectEncoding(srcEncoding);

	file.Filter();
	// TODO: find the content and drop everything else
	file.TranslateForOffline();
	file.Recode();
	file.Save(destFileName);

	return TRUE;
}

void ClearImages(CArray<CFeedItem *, CFeedItem *> &items) {
	LOG0(1, "ClearImages()");

	CStringList files;
	for (int i = 0; i < items.GetSize(); i++) {
		CFeedItem *fi = items.GetAt(i);

		CStringList links;
		// clear images
		fi->GetItemImages(links);
		while (!links.IsEmpty()) {
			CString url = links.RemoveHead();
			files.AddTail(GetCacheFile(FILE_TYPE_IMAGE, Config.CacheLocation, url));
		}
	}

	if (files.GetCount() > 0) {
		ClearCacheFiles(FILE_TYPE_IMAGE, Config.CacheLocation, files);
	}
}

void ClearHtmlPages(CArray<CFeedItem *, CFeedItem *> &items) {
	LOG0(1, "ClearHtmlPages()");

	CStringList files;
	for (int i = 0; i < items.GetSize(); i++) {
		CFeedItem *fi = items.GetAt(i);
		files.AddTail(GetCacheFile(FILE_TYPE_HTML, Config.CacheLocation, fi->Link));
	}

	if (files.GetCount() > 0)
		ClearCacheFiles(FILE_TYPE_HTML, Config.CacheLocation, files);
}

void ClearEnclosures(CArray<CFeedItem *, CFeedItem *> &items) {
	LOG0(1, "ClearEnclosures()");

	CStringList files;
	for (int i = 0; i < items.GetSize(); i++) {
		CFeedItem *fi = items.GetAt(i);

		CStringList links;
		// clear enclosures
		fi->GetEnclosures(links);
		while (!links.IsEmpty()) {
			CString url = links.RemoveHead();
			files.AddTail(GetCacheFile(FILE_TYPE_ENCLOSURE, Config.CacheLocation, url));
		}
	}

	if (files.GetCount() > 0)
		ClearCacheFiles(FILE_TYPE_ENCLOSURE, Config.CacheLocation, files);
}

//
// CDownloadQueue
//

CDownloadQueue::CDownloadQueue() {
	InitializeCriticalSection(&CS);
}

CDownloadQueue::~CDownloadQueue() {
	DeleteCriticalSection(&CS);
}

BOOL CDownloadQueue::IsEmpty() {
	EnterCriticalSection(&CS);
	BOOL b = Items.IsEmpty();
	LeaveCriticalSection(&CS);
	return b;
}

int CDownloadQueue::GetCount() {
	EnterCriticalSection(&CS);
	int cnt = Items.GetCount();
	LeaveCriticalSection(&CS);
	return cnt;
}

void CDownloadQueue::Enqueue(CDownloadItem *item) {
	EnterCriticalSection(&CS);
	Items.AddTail(item);
	LeaveCriticalSection(&CS);
//	SetEvent(HDownloadQNewItem);		// notify download thread that it has some work to do
}

CDownloadItem *CDownloadQueue::Dequeue() {
	EnterCriticalSection(&CS);
	CDownloadItem *item = Items.RemoveHead();
	LeaveCriticalSection(&CS);
	return item;
}


//
// CUpdateBar
//

DWORD WINAPI UpdateStubProc(LPVOID lpParameter) {
	CUpdateBar *bar = (CUpdateBar *) lpParameter;
	bar->UpdateThread();
	return 0;
}

CUpdateBar::CUpdateBar() {
	LOG0(5, "CUpdateBar::CUpdateBar()");

	HUpdateThread = NULL;
	Downloader = NULL;
	ErrorCount = 0;
	Terminate = FALSE;

	InitializeCriticalSection(&CSUpdateList);
	InitializeCriticalSection(&CSDownloader);
}

CUpdateBar::~CUpdateBar() {
	LOG0(5, "CUpdateBar::~CUpdateBar()");

	DeleteCriticalSection(&CSDownloader);
	DeleteCriticalSection(&CSUpdateList);
}

BOOL CUpdateBar::Create(CWnd *pParentWnd) {
//	DWORD dwStyle = WS_CHILD | CBRS_BOTTOM;
//	m_dwStyle = dwStyle;
	m_dwStyle = CBRS_BOTTOM;

	BOOL ret;

	CRect rect; rect.SetRectEmpty();
	ret = CWnd::Create(NULL, NULL, WS_CHILD, rect, pParentWnd, AFX_IDW_TOOLBAR + 2);

	m_ctlProgress.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, CRect(0, 0, 0, 0), this, IDC_UPDATE_PROGRESS);
	m_ctlText.Create(_T(""), WS_CHILD | SS_NOTIFY | SS_NOPREFIX, CRect(0, 0, 0, 0), this, IDC_UPDATE_TEXT);
	m_ctlText.SetFont(&Appearance.BaseFont);

	m_ctlStopBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(0, 0, 0, 0), this, IDC_UPDATE_STOP);

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ctlStopBtn.LoadBitmaps(IDB_CLOSE);
	AfxSetResourceHandle(AfxGetInstanceHandle());
	m_ctlStopBtn.SizeToContent();

	return ret;
}

void CUpdateBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) {
}

CSize CUpdateBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz) {
	LOG0(1, "CUpdateBar::CalcFixedLayout()");

	CDC *pDC = GetDC();
	int cx = pDC->GetDeviceCaps(HORZRES);
	ReleaseDC(pDC);

	return CSize(cx, SCALEY(21) - 1);
}

void CUpdateBar::Redraw() {
	UpdateProgressText();
	m_ctlProgress.Redraw(FALSE);
}

void CUpdateBar::EnqueueSite(CSiteItem *site, BOOL updateOnly) {
	LOG0(1, "CUpdateBar::EnqueueSites()");

	EnterCriticalSection(&CSUpdateList);
	UpdateList.AddTail(new CUpdateItem(site, updateOnly));
	LeaveCriticalSection(&CSUpdateList);
}

void CUpdateBar::EnqueueSites(CList<CSiteItem *, CSiteItem *> &sites, BOOL updateOnly) {
	LOG0(1, "CUpdateBar::EnqueueSites()");

	EnterCriticalSection(&CSUpdateList);
	POSITION pos = sites.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *si = sites.GetNext(pos);
		UpdateList.AddTail(new CUpdateItem(si, updateOnly));
	}
	LeaveCriticalSection(&CSUpdateList);
}

void CUpdateBar::EnqueueImages(CArray<CFeedItem *, CFeedItem *> &items) {
	LOG0(1, "CUpdateBar::EnqueueImages()");

	for (int i = 0; i < items.GetSize(); i++) {
		CFeedItem *fi = items.GetAt(i);

		//
		CStringList images;
		fi->GetItemImages(images);

		POSITION pos = images.GetHeadPosition();
		while (pos != NULL) {
			CString imgUrl = images.GetNext(pos);
			EnqueueItem(imgUrl, FILE_TYPE_IMAGE);
		}
	}
}

void CUpdateBar::EnqueueHtml(const CString &url, CSiteItem *siteItem) {
	// URL rewriting
	CString surl = SanitizeUrl(url);
	surl = RewriteUrl(surl, siteItem->Info->RewriteRules);
	if (Config.UseHtmlOptimizer)
		surl = MakeHtmlOptimizerUrl(surl, Config.HtmlOptimizerURL);

	CString strFileName = GetCacheFile(FILE_TYPE_HTML, Config.CacheLocation, surl);
	if (!FileExists(strFileName)) {
		CDownloadItem *di = new CDownloadItem();
		di->URL = surl;
		di->Type = FILE_TYPE_HTML;
		di->Try = DOWNLOAD_TRIES;
		di->FileName = strFileName;
		di->SiteIdx = SiteList.GetIndexOf(siteItem);
		DownloadQueue.Enqueue(di);
	}
}

void CUpdateBar::EnqueueHtmls(CArray<CFeedItem *, CFeedItem *> &items) {
	LOG0(1, "CUpdateBar::EnqueueHtml()");

	for (int i = 0; i < items.GetSize(); i++) {
		CFeedItem *fi = items.GetAt(i);
		EnqueueHtml(fi->Link, fi->SiteItem);
	}
}

void CUpdateBar::EnqueueEnclosures(CArray<CFeedItem *, CFeedItem *> &items, DWORD limit/* = 0*/) {
	LOG0(1, "CUpdateBar::EnqueueEnclosures()");

	for (int i = 0; i < items.GetSize(); i++) {
		CFeedItem *fi = items.GetAt(i);

		//
		CStringList encs;
		fi->GetEnclosures(encs);

		POSITION pos = encs.GetHeadPosition();
		while (pos != NULL) {
			CString encUrl = encs.GetNext(pos);
			EnqueueItem(encUrl, FILE_TYPE_ENCLOSURE);
		}
	}
}

void CUpdateBar::EnqueueItem(const CString &strUrl, EFileType type) {
	CString url = SanitizeUrl(strUrl);
	CString strFileName = GetCacheFile(type, Config.CacheLocation, url);
	if (!FileExists(strFileName)) {
		CDownloadItem *di = new CDownloadItem();
		di->URL = strUrl;
		di->Type = type;
		di->Try = DOWNLOAD_TRIES;
		di->FileName = strFileName;
		di->SiteIdx = SITE_INVALID;

		DownloadQueue.Enqueue(di);
	}
}

void CUpdateBar::Start() {
	LOG0(1, "CUpdateBar::Start()");

	if (HUpdateThread == NULL && (UpdateList.GetCount() > 0 || DownloadQueue.GetCount() > 0)) {
		if (Config.ClearErrorLog)
			Errors.Cleanup();

		DownloadQueue.FinishedItems = 0;
		HUpdateThread = CreateThread(NULL, 0, UpdateStubProc, this, 0, NULL);
//		SetThreadPriority(HUpdateThread, THREAD_PRIORITY_LOWEST);
	}
}

void CUpdateBar::UpdateFeeds() {
	LOG0(1, "CUpdateBar::UpdateFeeds()");

	State = UPDATE_STATE_RSS;
	UpdateProgressText();

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();

	POSITION pos = UpdateList.GetHeadPosition();
	while (!Terminate && pos != NULL) {
		EnterCriticalSection(&CSUpdateList);
		CUpdateItem *ui = UpdateList.GetNext(pos);
		CSiteItem *si = ui->SiteItem;
		LeaveCriticalSection(&CSUpdateList);

		SiteName = si->Name;
//		CString sTitle;
//		sTitle.Format(_T("%s: %s"), si->Name, sUpdating);
//		m_ctlProgress.SetText(sTitle, FALSE);
		Redraw();

		LOG1(3, "Updating %S", si->Name);

		EnterCriticalSection(&CSDownloader);
		Downloader = new CDownloader;
		Downloader->ETag = si->Info->ETag;
		Downloader->LastModified = si->Info->LastModified;
		LeaveCriticalSection(&CSDownloader);

		if (UpdateFeed(si, ui->UpdateOnly)) {
			si->Info->ETag = Downloader->ETag;
			si->Info->LastModified = Downloader->LastModified;

			if (si->CheckFavIcon) {
				// temp file name
				CString faviconFileName = GetCacheFile(FILE_TYPE_FAVICON, Config.CacheLocation, si->Info->FileName);
				// get favicon
				if (DownloadFavIcon(si->Feed->HtmlUrl, faviconFileName)) {
					// update favicon in GUI
					if (frame != NULL) frame->SendMessage(UWM_UPDATE_FAVICON, 0, (LPARAM) si);
				}

				si->CheckFavIcon = FALSE;
			}

			// send message to update the feed view
			if (frame != NULL) frame->SendMessage(UWM_UPDATE_FEED, 0, (LPARAM) si);

			SaveSiteItemUnreadCount(si, SiteList.GetIndexOf(si));
			SaveSiteItemFlaggedCount(si, SiteList.GetIndexOf(si));
			// process
		}

		EnterCriticalSection(&CSDownloader);
		delete Downloader;
		Downloader = NULL;
		LeaveCriticalSection(&CSDownloader);

		m_ctlProgress.SetStep(1);
		m_ctlProgress.StepIt();
		m_ctlProgress.Redraw(FALSE);
	}
}

BOOL CUpdateBar::UpdateFeed(CSiteItem *si, BOOL updateOnly) {
	LOG0(1, "CUpdateBar::UpdateFeed()");

	BOOL ret = FALSE;

	si->EnsureSiteLoaded();

	CString sTmpFileName;
	LPTSTR tmpFileName = sTmpFileName.GetBufferSetLength(MAX_PATH + 1);
	GetTempFileName(Config.CacheLocation, _T("rsr"), 0, tmpFileName);

	CString sErrMsg;
	if (Downloader->SaveHttpObject(si->Info->XmlUrl, tmpFileName) && Downloader->Updated) {
		CFeedFile xml;
		if (xml.LoadFromFile(tmpFileName)) {
			CFeed *feed = new CFeed;
			if (xml.Parse(feed, si)) {
				si->Status = CSiteItem::Ok;

/*				SYSTEMTIME st;
				GetLocalTime(&st);
				SystemTimeToFileTime(&st, &(si->LastUpdate));
*/

				if (si->Feed == NULL) {
					si->Feed = new CFeed();

					si->Feed->Copyright = feed->Copyright;
					si->Feed->Description = feed->Description;
					si->Feed->HtmlUrl = feed->HtmlUrl;
					si->Feed->Language = feed->Language;
					si->Feed->Published = feed->Published;
					si->Feed->Title = feed->Title;
					si->Feed->UpdateInterval = feed->UpdateInterval;
				}

				// mark all new items in the old feed as unread
				for (int i = 0; i < si->Feed->GetItemCount(); i++) {
					CFeedItem *fi = si->Feed->GetItem(i);
					if (fi->IsNew())
						fi->SetFlags(MESSAGE_UNREAD, MESSAGE_READ_STATE);
				}

				MergeFeed(si, feed, updateOnly);

				ret = TRUE;
			}
			else {
				sErrMsg.LoadString(IDS_ERROR_PARSING_FEED_FILE);
			}

			delete feed;
		}
		else {
			sErrMsg.LoadString(IDS_INVALID_FEED_FILE);
		}
	}
	else {
		switch (Downloader->Error) {
			case DOWNLOAD_ERROR_GETTING_FILE:            sErrMsg.LoadString(IDS_ERROR_GETTING_FILE); break;
			case DOWNLOAD_ERROR_RESPONSE_ERROR:          sErrMsg.LoadString(IDS_RESPONSE_ERROR); break;
			case DOWNLOAD_ERROR_SENDING_REQUEST:         sErrMsg.LoadString(IDS_ERROR_SENDING_REQUEST); break;
			case DOWNLOAD_ERROR_CONNECTION_ERROR:        sErrMsg.LoadString(IDS_ERROR_CONNECT); break;
			case DOWNLOAD_ERROR_MALFORMED_URL:           sErrMsg.LoadString(IDS_MALFORMED_URL); break;
			case DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE: sErrMsg.LoadString(IDS_INVALID_FEED_FILE); break;
			case DOWNLOAD_ERROR_UNKNOWN_AUTH_SCHEME:     sErrMsg.LoadString(IDS_UNKNOWN_AUTH_SCHEME); break;
			case DOWNLOAD_ERROR_NO_LOCATION_HEADER:      sErrMsg.LoadString(IDS_NO_LOCATION_HEADER); break;
			case DOWNLOAD_ERROR_HTTP_ERROR:              sErrMsg.Format(IDS_HTTP_ERROR, Downloader->HttpErrorNo); break;
			case DOWNLOAD_ERROR_AUTHORIZATION_FAILED:    sErrMsg.LoadString(IDS_AUTHORIZATION_FAILED); break;
			case DOWNLOAD_ERROR_AUTHENTICATION_ERROR:    sErrMsg.LoadString(IDS_AUTHENTICATION_ERROR); break;
		}
	}

	if (!sErrMsg.IsEmpty()) {
		CString sMsg;
		sMsg.Format(_T("%s: %s"), si->Name, sErrMsg);
		CErrorItem *ei = new CErrorItem(sMsg);
		ei->Type = CErrorItem::Site;
		ei->SiteIdx = SiteList.GetIndexOf(si);
		ei->UpdateOnly = updateOnly;
		Errors.Add(ei);
	}

	DeleteFile(tmpFileName);

	return ret;
}

void CUpdateBar::MergeFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly) {
	LOG0(1, "CUpdateBar::MergeFeed()");

	// NOTE: move this function to CSiteItem class (?)

	feed->Lock();
	// we need to preserve new items for later caching
	CArray<CFeedItem *, CFeedItem *> newItems;
	FeedDiff(feed, si->Feed, &newItems);

	CArray<CFeedItem *, CFeedItem *> existingItems;
	FeedIntersection(feed, si->Feed, &existingItems);

	CArray<CFeedItem *, CFeedItem *> itemsToClean;

	int i;
	CList<CFeedItem *, CFeedItem *> items;
	if (si->Info->CacheLimit > 0 || si->Info->CacheLimit == CACHE_LIMIT_DEFAULT) {
		// add new items
		for (i = 0; i < newItems.GetSize(); i++)
			items.AddTail(newItems.GetAt(i));

		int limit;
		if (si->Info->CacheLimit > 0)
			limit = si->Info->CacheLimit;
		else
			limit = Config.CacheLimit;

		// add flagged
		for (i = 0; i < si->Feed->GetItemCount(); i++) {
			CFeedItem *fi = si->Feed->GetItem(i);
			if (fi->IsFlagged())
				items.AddTail(fi);
		}

		// limit the cache
		int toAdd = limit - items.GetCount();
		for (i = 0; i < si->Feed->GetItemCount(); i++) {
			CFeedItem *fi = si->Feed->GetItem(i);
			if (!fi->IsFlagged()) {
				if (toAdd > 0) {
					items.AddTail(fi);
					toAdd--;
				}
				else
					itemsToClean.Add(fi);							// old item -> delete it!
			}
		}

		// free duplicate items
		for (i = 0; i < existingItems.GetSize(); i++)
			delete existingItems.GetAt(i);
	}
	else if (si->Info->CacheLimit == CACHE_LIMIT_DISABLED) {
		// add new items
		for (i = 0; i < newItems.GetSize(); i++)
			items.AddTail(newItems.GetAt(i));

		// add same items
		CArray<CFeedItem *, CFeedItem *> sameItems;
		FeedIntersection(si->Feed, feed, &sameItems);
		for (i = 0; i < sameItems.GetSize(); i++)
			items.AddTail(sameItems.GetAt(i));

		CArray<CFeedItem *, CFeedItem *> freeItems;
		FeedDiff(si->Feed, feed, &freeItems);
		for (i = 0; i < freeItems.GetSize(); i++) {
			CFeedItem *fi = freeItems.GetAt(i);
			if (fi->IsFlagged())
				items.AddTail(fi);
			else
				itemsToClean.Add(fi);							// old item -> delete it!
		}

		// free duplicate items
		for (i = 0; i < existingItems.GetSize(); i++)
			delete existingItems.GetAt(i);
	}
	else if (si->Info->CacheLimit == CACHE_LIMIT_UNLIMITED) {
		// add old items
		for (i = 0; i < si->Feed->GetItemCount(); i++)
			items.AddTail(si->Feed->GetItem(i));
		// add new items
		for (i = 0; i < newItems.GetSize(); i++)
			items.AddTail(newItems.GetAt(i));

		// free duplicate items
		for (i = 0; i < existingItems.GetSize(); i++)
			delete existingItems.GetAt(i);
	}

	// set items in the feed
	i = 0;
	si->Feed->SetSize(items.GetCount());
	while (!items.IsEmpty()) {
		CFeedItem *fi = items.RemoveHead();
//		// if the feed item has no date -> set it to the date of download
//		if (fi->PubDate.wYear == 0)
//			GetLocalTime(&fi->PubDate);

		si->Feed->SetAt(i, fi);
		i++;
	}
	feed->Unlock();

	// save feed
	CString feedPathName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, si->Info->FileName);
	CreatePath(feedPathName);
	si->Feed->Save(feedPathName);

	// set file time
	HANDLE hFile = CreateFile(feedPathName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		FILETIME ftNow;
		SYSTEMTIME stNow;
		GetLocalTime(&stNow);
		SystemTimeToFileTime(&stNow, &ftNow);
		SetFileTime(hFile, NULL, NULL, &ftNow);
		CloseHandle(hFile);
	}

	// notify today plugin
	NotifyTodayPlugin(CheckFeedsMessage);

	// clean items
	ClearImages(itemsToClean);
	ClearHtmlPages(itemsToClean);
	ClearEnclosures(itemsToClean);

	for (i = 0; i < itemsToClean.GetSize(); i++)
		delete itemsToClean[i];

	// check keywords in new items
	for (i = 0; i < newItems.GetSize(); i++)
		newItems.GetAt(i)->SearchKeywords(SiteList.GetKeywords());

	// cache
	if (!updateOnly) {
		// enqueue items to cache
		if (si->Info->UseGlobalCacheOptions) {
			// cache item images
			if (Config.CacheImages)
				EnqueueImages(newItems);

			// cache HTML content
			if (Config.CacheHtml)
				EnqueueHtmls(newItems);
		}
		else {
			// cache item images
			if (si->Info->CacheItemImages)
				EnqueueImages(newItems);

			// cache HTML content
			if (si->Info->CacheHtml)
				EnqueueHtmls(newItems);
		}

		// cache enclosures
		if (si->Info->CacheEnclosures)
			EnqueueEnclosures(newItems, si->Info->EnclosureLimit);
	}
}

void CUpdateBar::SyncGReader() {
	LOG0(1, "CUpdateBar::SyncGReader()");

	EnterCriticalSection(&CSDownloader);
	Downloader = new CDownloader;
	LeaveCriticalSection(&CSDownloader);

	CGReaderSync sync(Downloader);

	// authenticate with greader
	State = UPDATE_STATE_AUTHENTICATING;
	UpdateProgressText();

	if (sync.Authenticate(Config.SyncUserName, Config.SyncPassword)) {
		State = UPDATE_STATE_RSS;
		UpdateProgressText();

		// sync
		POSITION pos = UpdateList.GetHeadPosition();
		while (!Terminate && pos != NULL) {
			EnterCriticalSection(&CSUpdateList);
			CUpdateItem *ui = UpdateList.GetNext(pos);
			CSiteItem *si = ui->SiteItem;
			LeaveCriticalSection(&CSUpdateList);

			LOG1(3, "Syncing %S", si->Name);

			// TODO: sync site
			Sleep(500);

			
			m_ctlProgress.SetStep(1);
			m_ctlProgress.StepIt();
			m_ctlProgress.Redraw(FALSE);
		}
	}
	else {
		Errors.Add(new CErrorItem(IDS_AUTHENTICATION_FAILED));
	}
}


void CUpdateBar::DownloadHtmlPage(CDownloadItem *di) {
	LOG0(1, "CUpdateBar::DownloadHtmlPage()");

/*	CString tmpFileName = di->FileName + _T(".part");
	if (FileExists(tmpFileName)) {
		TranslateForOfflineReading(tmpFileName, di->FileName);
	}
*/

	if (FileExists(di->FileName))
		return;						// file already exists

	CString tmpFileName = di->FileName + _T(".part");
	if (FileExists(tmpFileName))
		DeleteFile(tmpFileName);

	CString url = di->URL;

	//
	m_ctlProgress.SetRange(0, 150000);

	BOOL ok = FALSE;
	CreatePath(tmpFileName);
	if (Downloader->SaveHttpObject(url, tmpFileName))
		ok = TRUE;
	else {
		// optimizing failed -> use original url
		Downloader->Reset();
		if (Config.UseHtmlOptimizer && Downloader->SaveHttpObject(di->URL, tmpFileName))
			ok = TRUE;
	}


	if (ok) {
		if (!TranslateForOfflineReading(tmpFileName, di->FileName, Downloader->GetCharset())) {
			DeleteFile(di->FileName);
		}
		DeleteFile(tmpFileName);
	}
	else {
		CString sErrMsg;
		sErrMsg.Format(IDS_ERROR_DOWNLOADING_FILE, di->URL);
		CErrorItem *ei = new CErrorItem(sErrMsg);
		ei->Type = CErrorItem::File;
		ei->SiteIdx = di->SiteIdx;
		ei->Url = di->URL;
		ei->FileType = FILE_TYPE_HTML;
		Errors.Add(ei);

//		Errors.Add(new CErrorItem(sErrMsg));
	}
}

void CUpdateBar::DownloadFile(CDownloadItem *di) {
	LOG0(1, "CUpdateBar::DownloadFile()");

	if (FileExists(di->FileName))
		return;						// file already exists

	BOOL ok = FALSE;

	CString tmpFileName = di->FileName + _T(".part");
	HANDLE file = CreateFile(tmpFileName, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		// not exists
		CreatePath(tmpFileName);
		if (Downloader->SaveHttpObject(di->URL, tmpFileName)) {
			ok = TRUE;
		}
	}
	else {
		// file exists => resume download
		DWORD fileSize = GetFileSize(file, NULL);
		CloseHandle(file);

		if (Downloader->PartialDownload(di->URL, tmpFileName, fileSize)) {
			ok = TRUE;
		}
	}

	if (ok) {
		MoveFile(tmpFileName, di->FileName);
	}
	else {
		CString sErrMsg;
		sErrMsg.Format(IDS_ERROR_DOWNLOADING_FILE, di->URL);
		CErrorItem *ei = new CErrorItem(sErrMsg);
		ei->Type = CErrorItem::File;
		ei->SiteIdx = SITE_INVALID;
		ei->Url = di->URL;
		ei->FileType = di->Type;
		Errors.Add(ei);
//		Errors.Add(new CErrorItem(sErrMsg));
	}
}

void CUpdateBar::DownloadItems() {
	LOG0(1, "CUpdateBar::DownloadItems()");

	BOOL disconnect;
	if (CheckConnection(Config.AutoConnect, disconnect)) {
		while (!Terminate && !DownloadQueue.IsEmpty()) {
			CDownloadItem *di = DownloadQueue.Dequeue();

			EnterCriticalSection(&CSDownloader);
			Downloader = new CDownloader();
			m_ctlProgress.SetDownloader(Downloader);
			LeaveCriticalSection(&CSDownloader);

			m_ctlProgress.SetPos(0);
			UpdateProgressText();
			m_ctlProgress.Redraw(TRUE);

			switch (di->Type) {
				case FILE_TYPE_HTML: DownloadHtmlPage(di); break;
				default: DownloadFile(di); break;
			}

			DownloadQueue.FinishedItems++;
			int lo, hi;
			m_ctlProgress.GetRange(lo, hi);
			m_ctlProgress.SetPos(hi);
			m_ctlProgress.Redraw(FALSE);

			EnterCriticalSection(&CSDownloader);
			m_ctlProgress.SetDownloader(NULL);
			delete Downloader;
			Downloader = NULL;
			LeaveCriticalSection(&CSDownloader);

			delete di;
		}
	}
	else {
		Terminate = TRUE;
		Errors.Add(new CErrorItem(IDS_NO_INTERNET_CONNECTION));
	}
}

void CUpdateBar::UpdateThread() {
	LOG0(3, "CUpdateBar::UpdateThread() - begin");

	if (UpdateList.GetCount() <= 0)
		return;

	//////
	CSuspendKiller suspendKiller;

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();

	Terminate = FALSE;
	ErrorCount = 0;

/*	// show update bar
	m_ctlProgress.ShowWindow(SW_SHOW);
	m_ctlText.ShowWindow(SW_HIDE);

	CString sUpdating;
	sUpdating.Format(IDS_UPDATING);
	m_ctlProgress.SetRange(0, 100);
	m_ctlProgress.SetPos(0);
//	m_ctlProgress.SetText(sUpdating);
	m_ctlProgress.SetText(_T(""));
	m_ctlProgress.Redraw(FALSE);

	if (frame != NULL) frame->PostMessage(UWM_SHOW_UPDATEBAR, TRUE);
*/

	// update feeds

//	BOOL updated;
	BOOL disconnect;
	if (CheckConnection(Config.AutoConnect, disconnect)) {
		// show update bar
		m_ctlProgress.ShowWindow(SW_SHOW);
		m_ctlText.ShowWindow(SW_HIDE);
		if (frame != NULL) frame->PostMessage(UWM_SHOW_UPDATEBAR, TRUE);
	
		m_ctlProgress.SetRange(0, UpdateList.GetCount());

		switch (Config.SyncSite) {
			case SYNC_SITE_GOOGLE_READER: SyncGReader();  break;
			default: UpdateFeeds(); break;
		}

		// download items
		if (DownloadQueue.GetCount() > 0) {
			State = UPDATE_STATE_CACHING;
			DownloadItems();

			// empty download queue
			while (!DownloadQueue.IsEmpty())
				delete DownloadQueue.Dequeue();
		}

		// notify
//		if (Config.NotifyNew && GetForegroundWindow()->GetSafeHwnd() != frame->GetSafeHwnd() && updated) {
		if (Config.NotifyNew && GetForegroundWindow()->GetSafeHwnd() != frame->GetSafeHwnd()) {
			int newItems = 0;
			for (int i = 0; i < SiteList.GetCount(); i++) {
				CFeed *feed = SiteList.GetAt(i)->Feed;
				if (feed != NULL)
					newItems += feed->GetNewCount();
			}

			if (newItems > 0) {
				prssrNotificationRemove();
				prssrNotification(newItems);
			}
		}

		if (disconnect)
			Connection.HangupConnection();

//		// hide update bar
//		Sleep(250);

		// done
		if (Errors.GetCount() > 0) {
			CString strError;
			strError.Format(IDS_N_ERRORS, Errors.GetCount());
			ShowError(strError);
		}
		else {
			if (frame != NULL) frame->SendMessage(UWM_SHOW_UPDATEBAR, FALSE);
		}
	}
	else {
		Terminate = TRUE;
		Errors.Add(new CErrorItem(IDS_NO_INTERNET_CONNECTION));
//		updated = FALSE;
		if (frame != NULL) frame->PostMessage(UWM_SHOW_UPDATEBAR, TRUE);
	}

	while (!UpdateList.IsEmpty())
		delete UpdateList.RemoveHead();


/*	// download items

	if (!Terminate) {
		State = UPDATE_STATE_CACHING;
		DownloadItems();
	}

	// empty download queue
	while (!DownloadQueue.IsEmpty())
		delete DownloadQueue.Dequeue();

	// hide update bar
	Sleep(250);

	// done
	if (Errors.GetCount() > 0) {
		CString strError;
		strError.Format(IDS_N_ERRORS, Errors.GetCount());
		ShowError(strError);
	}
	else {
		if (frame != NULL) frame->SendMessage(UWM_SHOW_UPDATEBAR, FALSE);
	}

	// notify
	if (Config.NotifyNew && GetForegroundWindow()->GetSafeHwnd() != frame->GetSafeHwnd() && updated) {
		int newItems = 0;
		for (int i = 0; i < SiteList.GetCount(); i++) {
			CFeed *feed = SiteList.GetAt(i)->Feed;
			if (feed != NULL)
				newItems += feed->GetNewCount();
		}

		if (newItems > 0) {
			prssrNotificationRemove();
			prssrNotification(newItems);
		}
	}
*/

	// end
	CloseHandle(HUpdateThread);
	HUpdateThread = NULL;

	if (frame != NULL) frame->SendMessage(UWM_UPDATE_FINISHED);

	LOG0(3, "CUpdateBar::UpdateThread() - end");
}

void CUpdateBar::UpdateProgressText() {
	CString sText;
	CString sTitle;

	switch (State) {
		case UPDATE_STATE_CACHING:
			sText.Format(IDS_DOWNLOADING);
			sTitle.Format(_T("%d / %d: %s..."), DownloadQueue.FinishedItems + 1, DownloadQueue.FinishedItems + DownloadQueue.GetCount() + 1, sText);
			m_ctlProgress.SetText(sTitle);
			break;

		case UPDATE_STATE_RSS:
			sText.Format(IDS_UPDATING);
			sTitle.Format(_T("%s: %s"), SiteName, sText);
			m_ctlProgress.SetText(sTitle);
			break;

		case UPDATE_STATE_AUTHENTICATING:
			sText.Format(IDS_AUTHENTICATING);
			m_ctlProgress.SetText(sText);
			break;

		default:
			sText.Format(IDS_UPDATING);
			m_ctlProgress.SetText(sText);
			break;
	}

}

void CUpdateBar::ShowError(UINT nID) {
	LOG1(5, "CUpdateBar::ShowError(%d)", nID);

	CString str;
	str.LoadString(nID);
	ShowError(str);
}

void CUpdateBar::ShowError(const CString &str) {
	LOG1(3, "CUpdateBar::ShowError(%S)", str);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();

	// error
	m_ctlProgress.ShowWindow(SW_HIDE);
	m_ctlText.ShowWindow(SW_SHOW);
	m_ctlText.SetWindowText(str);

	if (frame != NULL) frame->SendMessage(UWM_SHOW_UPDATEBAR, TRUE);
}

BEGIN_MESSAGE_MAP(CUpdateBar, CControlBar)
	//{{AFX_MSG_MAP(CUpdateBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_UPDATE_STOP, OnStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_UPDATE_TEXT, OnTextClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUpdateBar::OnStop() {
	LOG0(1, "CUpdateBar::OnStop()");

	EnterCriticalSection(&CSDownloader);
	if (Downloader != NULL)
		Downloader->Terminate();
	LeaveCriticalSection(&CSDownloader);
	Terminate = TRUE;

	// hide update bar
	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	if (frame != NULL) frame->SendMessage(UWM_SHOW_UPDATEBAR, FALSE);
	if (frame != NULL) frame->SendMessage(UWM_UPDATE_FINISHED);
}

void CUpdateBar::OnTextClicked() {
	LOG0(1, "CUpdateBar::OnTextClicked()");

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	if (frame != NULL) {
		frame->SendMessage(UWM_SHOW_UPDATEBAR, FALSE);
		frame->PostMessage(WM_COMMAND, ID_TOOLS_ERRORS, NULL);		// show errors
	}
}

void CUpdateBar::OnPaint() {
	CControlBar::OnPaint();

	CDC *pDC = GetDC();
	if (pDC) {
		CRect	rc;
		GetClientRect(&rc);
		pDC->FillSolidRect(rc, ::GetSysColor(COLOR_3DFACE));

//		if (ErrorCount <= 0) {
//			m_ctlProgress.Invalidate();
//		}
//		else {
//			m_ctlText.Invalidate();
//		}
//		m_ctlStopBtn.Invalidate();
	}

	ReleaseDC(pDC);
}

void CUpdateBar::OnSize(UINT nType, int cx, int cy) {
	LOG0(1, "CUpdateBar::OnSize()");

	CControlBar::OnSize(nType, cx, cy);

	// reposition controls
	if (IsWindow(m_ctlProgress.GetSafeHwnd())) {
		m_ctlProgress.SetWindowPos(NULL, SCALEX(2), SCALEY(3),
			cx - SCALEX(5) - SCALEX(16) - 1, SCALEY(16) - 1, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if (IsWindow(m_ctlText.GetSafeHwnd())) {
		m_ctlText.SetWindowPos(NULL, SCALEX(2), SCALEY(4),
			cx - SCALEX(5) - SCALEX(16) - 1, SCALEY(16) - 1, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if (IsWindow(m_ctlStopBtn.GetSafeHwnd()))
		m_ctlStopBtn.SetWindowPos(NULL, cx - SCALEX(2) - SCALEX(16) + 1, SCALEY(3),
			SCALEX(16) - 1, SCALEY(16) - 1, SWP_NOACTIVATE | SWP_NOZORDER);
}

