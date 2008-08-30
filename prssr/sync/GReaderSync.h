// GReaderSync.h: interface for the CGReaderSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_)
#define AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FeedSyncer.h"

class CDownloader;

class CGReaderSync : public CFeedSyncer {
public:
	CGReaderSync(CDownloader *downloader);
	virtual ~CGReaderSync();

	virtual BOOL Authenticate(const CString &userName, const CString &password);

protected:
	// auth
	CString UserName, Password;		// username & password (needed to getting auth token)
	CString SID;					// session ID

	BOOL GetAuthToken();
};

#endif // !defined(AFX_GREADERSYNC_H__3F7370B7_087C_4D04_9D9A_0F770437B61E__INCLUDED_)
