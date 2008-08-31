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
	CGReaderSync(CDownloader *downloader);
	virtual ~CGReaderSync();

	virtual BOOL NeedAuth() { return TRUE; }
	virtual BOOL Authenticate(const CString &userName, const CString &password);
	virtual BOOL SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly);

protected:
	// auth
	CString UserName, Password;		// username & password (needed to getting auth token)
	CString SID;					// session ID

	//
	CString BaseUrl;
	CString Api0;

	//
	CString Token;

	BOOL GetToken();
	CString FormatSIDCookie(const CString &sid);
};

#endif // !defined(AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_)
