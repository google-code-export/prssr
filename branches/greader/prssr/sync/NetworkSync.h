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
	virtual BOOL Authenticate(const CString &userName, const CString &password);
	virtual BOOL SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly);

protected:
};

#endif // !defined(AFX_NETWORKSYNC_H__8EEA8104_760F_41AA_8494_4C49A0F198E7__INCLUDED_)
