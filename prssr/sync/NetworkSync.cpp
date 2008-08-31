// NetworkSync.cpp: implementation of the CNetworkSync class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "../prssr.h"
#include "NetworkSync.h"
#include "../net/download.h"
#include "../Config.h"
#include "../Site.h"
#include "../xml/FeedFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkSync::CNetworkSync(CDownloader *downloader) : CFeedSync(downloader) {

}

CNetworkSync::~CNetworkSync() {

}

BOOL CNetworkSync::SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly) {
	LOG0(1, "CNetworkSync::SyncFeed()");

	BOOL ret = FALSE;

	CString sTmpFileName;
	LPTSTR tmpFileName = sTmpFileName.GetBufferSetLength(MAX_PATH + 1);
	GetTempFileName(Config.CacheLocation, _T("rsr"), 0, tmpFileName);

	Downloader->Reset();
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
		}
	}

	DeleteFile(tmpFileName);

	return ret;
}

BOOL CNetworkSync::MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean) {
	LOG0(1, "CNetworkSync::MergeFeed()");

	feed->Lock();

	FeedDiff(feed, si->Feed, &newItems);

	CArray<CFeedItem *, CFeedItem *> existingItems;
	FeedIntersection(feed, si->Feed, &existingItems);

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
		si->Feed->SetAt(i, fi);
		i++;
	}
	feed->Unlock();

	return TRUE;
}

BOOL CNetworkSync::DownloadFeed(CString &url, const CString &fileName) {
	LOG0(1, "CNetworkSync::DownloadFeed()");

	Downloader->Reset();
	return Downloader->SaveHttpObject(url, fileName);
}