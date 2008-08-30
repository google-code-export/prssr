// FeedSyncer.h: interface for the CFeedSyncer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEEDSYNCER_H__7EB2DE24_9B84_49C1_983F_A7779957FF02__INCLUDED_)
#define AFX_FEEDSYNCER_H__7EB2DE24_9B84_49C1_983F_A7779957FF02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDownloader;

class CFeedSyncer {
public:
	CFeedSyncer(CDownloader *downloader);
	virtual ~CFeedSyncer();

	virtual BOOL Authenticate(const CString &userName, const CString &password) = 0; 

protected:
	CDownloader *Downloader;
};

#endif // !defined(AFX_FEEDSYNCER_H__7EB2DE24_9B84_49C1_983F_A7779957FF02__INCLUDED_)
