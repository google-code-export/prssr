// NetworkSync.h: interface for the CNetworkSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKSYNC_H__8EEA8104_760F_41AA_8494_4C49A0F198E7__INCLUDED_)
#define AFX_NETWORKSYNC_H__8EEA8104_760F_41AA_8494_4C49A0F198E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FeedSync.h"

class CNetworkSync : public CFeedSync {
public:
	CNetworkSync(CDownloader *downloader);
	virtual ~CNetworkSync();

	virtual BOOL NeedAuth() { return FALSE; }
	virtual BOOL Authenticate() { return TRUE; }
	virtual BOOL SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly);
//	virtual BOOL MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean);
	virtual BOOL DownloadFeed(CString &url, const CString &fileName);
	virtual BOOL SyncItem(CFeedItem *fi, DWORD mask);

protected:
	virtual void FeedIntersection(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);
};

#endif // !defined(AFX_NETWORKSYNC_H__8EEA8104_760F_41AA_8494_4C49A0F198E7__INCLUDED_)
