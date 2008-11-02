// GReaderSync.cpp: implementation of the CGReaderSync class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "../prssr.h"
#include "GReaderSync.h"
#include "../net/Download.h"
#include "../Config.h"
#include "../Site.h"
#include "../xml/FeedFile.h"
#include "../www/url.h"

CString CGReaderSync::BaseUrl = _T("http://www.google.com/reader");
CString CGReaderSync::Api0 = _T("http://www.google.com/reader/api/0");


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGReaderSync::CGReaderSync(CDownloader *downloader, const CString &userName, const CString &password) : CFeedSync(downloader) {
	UserName = userName;
	Password = password;

	InitializeCriticalSection(&CS);
}

CGReaderSync::~CGReaderSync() {
	DeleteCriticalSection(&CS);
}

BOOL CGReaderSync::Authenticate() {
	LOG0(1, "CGReaderSync::Authenticate()");

	EnterCriticalSection(&CS);

	SID.Empty();

	BOOL ret = FALSE;
	if (Downloader != NULL) {

		TCHAR tempFileName[MAX_PATH];
		GetTempFileName(Config.CacheLocation, L"rsr", 0, tempFileName);

		CString url, body, response;
		url.Format(_T("https://www.google.com/accounts/ClientLogin"), UserName, Password);
		body.Format(_T("Email=%s&Passwd=%s"), UserName, Password);

		if (Downloader->Post(url, body, response)) {
			int npos = response.Find('\n');
			int start = 0;
			while (npos != -1) {
				int eqpos = response.Find('=', start);
				if (eqpos != -1 && response.Mid(start, eqpos - start).Compare(_T("SID")) == 0) {
					SID = response.Mid(eqpos + 1, npos - eqpos - 1);
				}
				start = npos + 1;
				npos = response.Find('\n', start);
			}
			ret = !SID.IsEmpty();
		}

		DeleteFile(tempFileName);
	}

	LeaveCriticalSection(&CS);

	return ret;
}

BOOL CGReaderSync::SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly) {
	LOG0(1, "CGReaderSync::SyncFeed()");

	EnterCriticalSection(&CS);

	BOOL ret = FALSE;
	Downloader->Reset();
	Downloader->SetCookie(FormatSIDCookie(SID));

	CString sTmpFileName;
	LPTSTR tmpFileName = sTmpFileName.GetBufferSetLength(MAX_PATH + 1);
	GetTempFileName(Config.CacheLocation, _T("rsr"), 0, tmpFileName);

	int n = 0;
	if (si->Info->CacheLimit == CACHE_LIMIT_DEFAULT)
		n = Config.CacheLimit;
	else if (si->Info->CacheLimit > 0)
		n = si->Info->CacheLimit;
	// TODO: unlimited and disabled options
	else
		n = 25;			// fallback

	CString url;
	url.Format(_T("%s/atom/feed/%s?n=%d"), BaseUrl, UrlEncode(si->Info->XmlUrl), n);
	if (Downloader->SaveHttpObject(url, tmpFileName) && Downloader->Updated) {
		CFeedFile xml;
		if (xml.LoadFromFile(tmpFileName)) {
			if (xml.Parse(feed, si))
				ret = TRUE;
			else
				ErrorMsg.LoadString(IDS_ERROR_PARSING_FEED_FILE);
		}
		else {
			ErrorMsg.LoadString(IDS_INVALID_FEED_FILE);
		}
	}
	else {
		switch (Downloader->Error) {
			case DOWNLOAD_ERROR_GETTING_FILE:            ErrorMsg.LoadString(IDS_ERROR_GETTING_FILE); break;
			case DOWNLOAD_ERROR_RESPONSE_ERROR:          ErrorMsg.LoadString(IDS_RESPONSE_ERROR); break;
			case DOWNLOAD_ERROR_SENDING_REQUEST:         ErrorMsg.LoadString(IDS_ERROR_SENDING_REQUEST); break;
			case DOWNLOAD_ERROR_CONNECTION_ERROR:        ErrorMsg.LoadString(IDS_ERROR_CONNECT); break;
			case DOWNLOAD_ERROR_MALFORMED_URL:           ErrorMsg.LoadString(IDS_MALFORMED_URL); break;
			case DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE: ErrorMsg.LoadString(IDS_INVALID_FEED_FILE); break;
			case DOWNLOAD_ERROR_UNKNOWN_AUTH_SCHEME:     ErrorMsg.LoadString(IDS_UNKNOWN_AUTH_SCHEME); break;
			case DOWNLOAD_ERROR_NO_LOCATION_HEADER:      ErrorMsg.LoadString(IDS_NO_LOCATION_HEADER); break;
			case DOWNLOAD_ERROR_HTTP_ERROR:              ErrorMsg.Format(IDS_HTTP_ERROR, Downloader->HttpErrorNo); break;
			case DOWNLOAD_ERROR_AUTHORIZATION_FAILED:    ErrorMsg.LoadString(IDS_AUTHORIZATION_FAILED); break;
			case DOWNLOAD_ERROR_AUTHENTICATION_ERROR:    ErrorMsg.LoadString(IDS_AUTHENTICATION_ERROR); break;
			case DOWNLOAD_ERROR_DISK_FULL:               ErrorMsg.LoadString(IDS_DISK_FULL); break;
		}
	}

	DeleteFile(tmpFileName);

	LeaveCriticalSection(&CS);

	return ret;
}

BOOL CGReaderSync::MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean) {
	LOG0(1, "CGReaderSync::MergeFeed()");

	EnterCriticalSection(&CS);

	MarkReadItems.RemoveAll();
	MarkStarredItems.RemoveAll();

	BOOL ret = CFeedSync::MergeFeed(si, feed, newItems, itemsToClean);

	UpdateInGreader();

	LeaveCriticalSection(&CS);

	return TRUE;
}

void CGReaderSync::UpdateInGreader() {
	LOG0(1, "CGReaderSync::DownloadFeed()");

	int i;

	// get token for these operations
	if (!GetToken()) {
		ErrorMsg.LoadString(IDS_ERROR_GETTING_TOKEN);
		return;
	}

	// send status to Greader
	for (i = 0; i < MarkReadItems.GetSize(); i++)
		SyncItem(MarkReadItems[i], MESSAGE_READ_STATE);

	for (i = 0; i < MarkStarredItems.GetSize(); i++)
		SyncItem(MarkStarredItems[i], MESSAGE_FLAG);

	MarkReadItems.RemoveAll();
	MarkStarredItems.RemoveAll();
}

BOOL CGReaderSync::SyncItem(CFeedItem *fi, DWORD mask) {
	LOG0(1, "CGReaderSync::SyncItem()");

	EnterCriticalSection(&CS);

	CString url, body, response;
	BOOL ret = TRUE;

	if (SID.IsEmpty()) ret = Authenticate();
	if (ret) {
		if (Token.IsEmpty()) ret = GetToken();
		if (ret) {
			url.Format(_T("%s/edit-tag"), Api0);
			if (mask & MESSAGE_READ_STATE) {
				if (fi->IsRead())
					body.Format(_T("i=%s&a=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/read")), Token);
				else if (fi->IsUnread())
					body.Format(_T("i=%s&a=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/kept-unread")), Token);
				else if (fi->IsNew())
					body.Format(_T("i=%s&r=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/read")), Token);
				Downloader->Post(url, body, response);
			}

			if (mask & MESSAGE_FLAG) {
				if (fi->IsFlagged())
					body.Format(_T("i=%s&a=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/starred")), Token);
				else
					body.Format(_T("i=%s&r=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/starred")), Token);
				Downloader->Post(url, body, response);
			}

			// TODO: set this accoring to the return state of the POST operation
			fi->SetFlags(MESSAGE_SYNC, MESSAGE_SYNC);
		}
	}

	LeaveCriticalSection(&CS);

	return ret;
}

BOOL CGReaderSync::DownloadFeed(CString &url, const CString &fileName) {
	LOG0(1, "CGReaderSync::DownloadFeed()");

	EnterCriticalSection(&CS);

	Downloader->Reset();
	if (SID.IsEmpty()) Authenticate();

	CString u;
	u.Format(_T("%s/atom/feed/%s"), BaseUrl, url);

	Downloader->SetCookie(FormatSIDCookie(SID));
	BOOL ret = Downloader->SaveHttpObject(u, fileName);

	LeaveCriticalSection(&CS);

	return ret;
}

CString CGReaderSync::FormatSIDCookie(const CString &sid) {
	CString sSIDCookie;
//	sSIDCookie.Format(_T("SID=%s; expires=1600000000; path=/; domain=.google.com"), sid);
	sSIDCookie.Format(_T("SID=%s"), sid);
	return sSIDCookie;
}

BOOL CGReaderSync::GetToken() {
	LOG0(5, "CGReaderSync::GetToken()");

	EnterCriticalSection(&CS);

	Downloader->Reset();
	Downloader->SetCookie(FormatSIDCookie(SID));

	Token.Empty();
	CString url;
	url.Format(_T("%s/token"), Api0);
	BOOL ret = Downloader->GetHttpObject(url, Token);

	LeaveCriticalSection(&CS);

	return ret;
}

void CGReaderSync::FeedIntersection(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff) {
	LOG0(5, "CGReaderSync::FeedIntersection()");

	CCache cache;
	int i;

	if (second != NULL)
		for (i = 0; i < second->GetItemCount(); i++) {
			CFeedItem *fi = second->GetItem(i);
			if (!fi->Hash.IsEmpty())
				cache.AddItem(fi->Hash, fi);
		}

	if (first != NULL)
		for (i = 0; i < first->GetItemCount(); i++) {
			CFeedItem *fa = first->GetItem(i);
			void *ptr;
			if (!fa->Hash.IsEmpty() && cache.InCache(fa->Hash, ptr)) {
				CFeedItem *fb = (CFeedItem *) ptr;
				// update read status
				if (fa->IsRead() || fb->IsRead()) {
					if (fb->IsRead() && !fa->IsRead())	// read in prssr, not read in Greader
						if (!fb->IsSynced())			// update status in Greader only if not synced yet
							MarkReadItems.Add(fb);
					fb->SetFlags(MESSAGE_READ, MESSAGE_READ_STATE);
				}

				// update starred status
				if (fa->IsFlagged() || fb->IsFlagged())  {
					if (fb->IsFlagged() && !fa->IsFlagged())	// flagged in prssr, not flagged in Greader
						if (!fb->IsSynced())					// update status in Greader only if not synced yet
							MarkStarredItems.Add(fb);
					fb->SetFlags(MESSAGE_FLAG, MESSAGE_FLAG);
				}

				diff->Add(fa);
			}
		}
}

static BOOL ParseSubscriptionObject(CXmlNode *object, CSiteItem *&siteItem) {
	LOG0(5, "ParseSubscriptionObject");

	CString url, title;

	POSITION posObject = object->GetFirstChildPos();
	while (posObject != NULL) {
		CXmlNode *string = object->GetNextChild(posObject);
		CString tagName = string->GetName();

		if (tagName.Compare(_T("string")) == 0) {
			// find attribute name=""
			CString attrName;
			POSITION posAttr = string->GetFirstAttrPos();
			while (posAttr != NULL) {
				CXmlAttr *attr = string->GetNextAttr(posAttr);
				if (attr->GetName().CompareNoCase(_T("name")) == 0)
					attrName = attr->GetValue();
			}

			if (attrName.CompareNoCase(_T("id")) == 0) {
				url = string->GetValue();
				if (url.Left(5).CompareNoCase(_T("feed/")) == 0)	// strip the feed/ prefix
					url = url.Mid(5);
			}
			else if (attrName.CompareNoCase(_T("title")) == 0) title = string->GetValue();
		}
	}

	if (!url.IsEmpty() && !title.IsEmpty()) {
		// prepare data structures
		CSiteItem *item = new CSiteItem(NULL, CSiteItem::Site);

		// add site offline
		CFeedInfo *info = new CFeedInfo();
		info->FileName = CFeedInfo::GenerateFileNameFromTitle(title);
		CFeedInfo::EnsureUniqueFileName(info->FileName);
		info->XmlUrl= url;
		info->TodayShow = CONFIG_DEFAULT_SHOWNEWCHANNELS;

		item->Status = CSiteItem::Error;
		item->Name = title;
		item->Info = info;
		item->CheckFavIcon = TRUE;

		siteItem = item;

		return TRUE;
	}
	else
		return FALSE;
}

static  BOOL ParseList(CXmlNode *parent, CSiteList &siteList) {
	LOG0(5, "ParseList");

	CSiteItem *rootItem = new CSiteItem(NULL, CSiteItem::Group);

	CXmlNode *list = parent;

	// process <object> nodes
	POSITION pos = list->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *object = list->GetNextChild(pos);
		CString tagName = object->GetName();
		if (tagName.Compare(_T("object")) == 0) {
			// process <object> nodes
			CSiteItem *siteItem = NULL;
			if (ParseSubscriptionObject(object, siteItem));
				rootItem->AddItem(siteItem);
		}
	}

	siteList.SetRoot(rootItem);

	return TRUE;
}

static BOOL ParseSubscriptions(CXmlNode *parent, CSiteList &siteList) {
	LOG0(5, "ParseSubscriptions");

	// start with the root node
	POSITION posParent = parent->GetFirstChildPos();
	while (posParent != NULL) {
		CXmlNode *child = parent->GetNextChild(posParent);

		if (child->GetName().Compare(_T("list")) == 0) {
			// find attribute name=""
			CString attrName;
			POSITION posAttr = child->GetFirstAttrPos();
			while (posAttr != NULL) {
				CXmlAttr *attr = child->GetNextAttr(posAttr);
				if (attr->GetName().CompareNoCase(_T("name")) == 0)
					attrName = attr->GetValue();
			}

			if (attrName.CompareNoCase(_T("subscriptions")) == 0)
				ParseList(child, siteList);
		}
	}

	return TRUE;
}

BOOL CGReaderSync::GetSubscriptions(CSiteList &siteList) {
	LOG0(5, "CGReaderSync::GetSubscriptions()");

	EnterCriticalSection(&CS);

	Downloader->Reset();
	if (SID.IsEmpty()) Authenticate();

	CString url;
	url.Format(_T("%s/subscription/list?output=xml"), Api0);

	TCHAR fileName[MAX_PATH];
	GetTempFileName(Config.CacheLocation, L"rsr", 0, fileName);

	Downloader->SetCookie(FormatSIDCookie(SID));
	BOOL ret = FALSE;
	if (Downloader->SaveHttpObject(url, fileName)) {
		CXmlFile xml;
		if (xml.LoadFromFile(fileName))
			ret = ParseSubscriptions(xml.GetRootNode(), siteList);
	}

	DeleteFile(fileName);

	LeaveCriticalSection(&CS);

	return ret;
}

BOOL CGReaderSync::AddSubscription(const CString &feedUrl, const CString &title) {
	LOG0(1, "CGReaderSync::AddSubscription()");

	EnterCriticalSection(&CS);

	CString url, body, response;
	BOOL ret = TRUE;

	if (SID.IsEmpty()) ret = Authenticate();
	if (ret) {
		if (Token.IsEmpty()) ret = GetToken();
		if (ret) {
			url.Format(_T("%s/subscription/edit"), Api0);
			body.Format(_T("s=feed/%s&ac=subscribe&t=%s&T=%s"), feedUrl, title, Token);
			ret = Downloader->Post(url, body, response);
		}
	}

	LeaveCriticalSection(&CS);

	return ret;
}

BOOL CGReaderSync::RemoveSubscription(const CString &feedUrl) {
	LOG0(1, "CGReaderSync::RemoveSubscription()");

	EnterCriticalSection(&CS);

	CString url, body, response;
	BOOL ret = TRUE;

	if (SID.IsEmpty()) ret = Authenticate();
	if (ret) {
		if (Token.IsEmpty()) ret = GetToken();
		if (ret) {
			url.Format(_T("%s/subscription/edit"), Api0);
			body.Format(_T("s=feed/%s&ac=unsubscribe&T=%s"), feedUrl, Token);
			ret = Downloader->Post(url, body, response);
		}
	}

	LeaveCriticalSection(&CS);

	return ret;
}
