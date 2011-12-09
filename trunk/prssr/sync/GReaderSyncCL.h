// GReaderSyncCL.h: interface for the CGReaderSyncCL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GREADERSYNCCL_H__95111EC3_435C_4442_AA24_7D7D82303636__INCLUDED_)
#define AFX_GREADERSYNCCL_H__95111EC3_435C_4442_AA24_7D7D82303636__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FeedSync.h"

class CGReaderSyncCL : public CFeedSync  
{
public:
	CGReaderSyncCL(CDownloader *downloader, const CString &userName, const CString &password);
	virtual ~CGReaderSyncCL();
	virtual BOOL NeedAuth() { return TRUE; }
	virtual BOOL Authenticate();
	virtual BOOL SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly);
	virtual BOOL MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean);
	virtual BOOL DownloadFeed(CString &url, const CString &fileName);
	virtual BOOL SyncItem(CFeedItem *fi, DWORD mask);

	virtual BOOL GetSubscriptions(CSiteList &siteList);
	virtual BOOL AddSubscription(const CString &url, const CString &title);
	virtual BOOL RemoveSubscription(const CString &url);

protected:
	// auth
	CString UserName, Password;		// username & password (needed to getting auth token)
	CString Auth;					// authentication marker
	//
	static CString BaseUrl;
	static CString Api0;

	//
	CString Token;

	BOOL GetToken();
	CString FormatAuthMarker(const CString &auth);

	// merging stuff
	CArray<CFeedItem *, CFeedItem *> MarkReadItems;
	CArray<CFeedItem *, CFeedItem *> MarkStarredItems;
//	void FeedIntersectionEx(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);
	virtual void FeedIntersection(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);
	void UpdateInGreader();

	CRITICAL_SECTION CS;

};

#endif // !defined(AFX_GREADERSYNCCL_H__95111EC3_435C_4442_AA24_7D7D82303636__INCLUDED_)
