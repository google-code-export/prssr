// NetworkSync.cpp: implementation of the CNetworkSync class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "../prssr.h"
#include "NetworkSync.h"
#include "../net/Download.h"
#include "../Config.h"
#include "../Site.h"
#include "../xml/FeedFile.h"
#include "../../share/cache.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkSync::CNetworkSync(CDownloader *downloader) : CFeedSync(downloader) {

}

CNetworkSync::~CNetworkSync() {

}

void CNetworkSync::FeedIntersection(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff) {
	LOG0(5, "CNetworkSync::FeedIntersection()");

	CCache cache;
	int i;

	if (second != NULL)
		for (i = 0; i < second->GetItemCount(); i++) {
			CFeedItem *fi = second->GetItem(i);
			if (!fi->Hash.IsEmpty())
				cache.AddItem(fi->Hash);
		}

	if (first != NULL)
		for (i = 0; i < first->GetItemCount(); i++) {
			CFeedItem *fi = first->GetItem(i);
			if (!fi->Hash.IsEmpty() && cache.InCache(fi->Hash))
				diff->Add(fi);
		}
}

BOOL CNetworkSync::SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly) {
	LOG0(1, "CNetworkSync::SyncFeed()");

	BOOL ret = FALSE;

	CString sTmpFileName;
	LPTSTR tmpFileName = sTmpFileName.GetBufferSetLength(MAX_PATH + 1);
	GetTempFileName(Config.CacheLocation, _T("rsr"), 0, tmpFileName);

	Downloader->Reset();
	Downloader->SetAuthenticationInfo(si->Info->UserName, si->Info->Password);
	Downloader->SetUAString(_T(""));
	if (Downloader->SaveHttpObject(si->Info->XmlUrl, tmpFileName) && Downloader->Updated) {
		CFeedFile xml;
		if (xml.LoadFromFile(tmpFileName)) {
			if (xml.Parse(feed, si)) {
				si->Info->ETag = Downloader->ETag;
				si->Info->LastModified = Downloader->LastModified;

				ret = TRUE;
			}
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

	return ret;
}

/*
BOOL CNetworkSync::MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean) {
}
*/

BOOL CNetworkSync::DownloadFeed(CString &url, const CString &fileName) {
	LOG0(1, "CNetworkSync::DownloadFeed()");

	Downloader->Reset();
	return Downloader->SaveHttpObject(url, fileName);
}

BOOL CNetworkSync::SyncItem(CFeedItem *fi, DWORD mask) {
	return TRUE;
}
