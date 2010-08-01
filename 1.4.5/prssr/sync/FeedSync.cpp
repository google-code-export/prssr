// FeedSync.cpp: implementation of the CFeedSync class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "FeedSync.h"
#include "../net/Download.h"
#include "../Feed.h"
#include "../Site.h"
#include "../../share/defs.h"
#include "../Config.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedSync::CFeedSync(CDownloader *downloader) {
	Downloader = downloader;
}

CFeedSync::~CFeedSync() {

}

BOOL CFeedSync::MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean) {
	LOG0(1, "CFeedSync::MergeFeed()");

	int i;

	feed->Lock();

	FeedDiff(feed, si->Feed, &newItems);
	// set the SYNC flag for new items
	for (i = 0; i < newItems.GetSize(); i++) {
		CFeedItem *fi = newItems.GetAt(i);
		fi->SetFlags(MESSAGE_SYNC, MESSAGE_SYNC);
	}

	CArray<CFeedItem *, CFeedItem *> existingItems;
	FeedIntersection(feed, si->Feed, &existingItems);

	CList<CFeedItem *, CFeedItem *> items;
	if (si->Info->CacheLimit > 0 || si->Info->CacheLimit == CACHE_LIMIT_DEFAULT) {
		int limit;
		if (si->Info->CacheLimit > 0)
			limit = si->Info->CacheLimit;
		else
			limit = Config.CacheLimit;

		// add new items (up to the limit)
		for (i = 0; i < newItems.GetSize(); i++) {
			CFeedItem *fi = newItems.GetAt(i);
			if (limit > 0) {
				items.AddTail(fi);
				limit--;
			}
			else
				existingItems.Add(fi);	// add to existing items, that are deleted after the merge
		}

		// add flagged items (we do not want to lose them)
		for (i = 0; i < si->Feed->GetItemCount(); i++) {
			CFeedItem *fi = si->Feed->GetItem(i);
			if (fi->IsFlagged())
				items.AddTail(fi);
			else {
				if (limit > 0) {
					items.AddTail(fi);
					limit--;
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
