// GReaderSync.h: interface for the CGReaderSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_)
#define AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FeedSync.h"

class CGReaderSync : public CFeedSync {
public:
	CGReaderSync(CDownloader *downloader, const CString &userName, const CString &password);
	virtual ~CGReaderSync();

	virtual BOOL NeedAuth() { return TRUE; }
	virtual BOOL Authenticate();
	virtual BOOL SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly);
	virtual BOOL MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean);
	virtual BOOL DownloadFeed(CString &url, const CString &fileName);

protected:
	// auth
	CString UserName, Password;		// username & password (needed to getting auth token)
	CString SID;					// session ID

	//
	static CString BaseUrl;
	static CString Api0;

	//
	CString Token;

	BOOL GetToken();
	CString FormatSIDCookie(const CString &sid);

	// merging stuff
	CArray<CFeedItem *, CFeedItem *> MarkReadItems;
	CArray<CFeedItem *, CFeedItem *> MarkStarredItems;
	void FeedIntersectionEx(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);
	void UpdateInGreader();
};

#endif // !defined(AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_)
