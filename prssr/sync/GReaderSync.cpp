// GReaderSync.cpp: implementation of the CGReaderSync class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "../prssr.h"
#include "GReaderSync.h"
#include "../net/download.h"
#include "../Config.h"
#include "../Site.h"
#include "../xml/FeedFile.h"
#include "../www/url.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGReaderSync::CGReaderSync(CDownloader *downloader) : CFeedSync(downloader) {
	BaseUrl = _T("http://www.google.com/reader");
	Api0 = _T("http://www.google.com/reader/api/0");
}

CGReaderSync::~CGReaderSync() {

}

BOOL CGReaderSync::Authenticate(const CString &userName, const CString &password) {
	UserName = userName;
	Password = password;
	SID.Empty();

	if (Downloader != NULL) {
		BOOL ret = FALSE;

		TCHAR tempFileName[MAX_PATH];
		GetTempFileName(Config.CacheLocation, L"rsr", 0, tempFileName);

		CString url, body, response;
		url.Format(_T("https://www.google.com/accounts/ClientLogin"), UserName, Password);
		body.Format(_T("Email=%s&Passwd=%s"), UserName, Password);

		if (Downloader->Post(url, body, response)) {
			int npos = response.Find('\n');
			int start = 0;
			while (npos != -1) {
				int eqpos = response.Find('=', start);
				if (eqpos != -1 && response.Mid(start, eqpos - start).Compare(_T("SID")) == 0) {
					SID = response.Mid(eqpos + 1, npos - eqpos - 1);
				}
				start = npos + 1;
				npos = response.Find('\n', start);
			}
			ret = !SID.IsEmpty();
		}
		else
			ret =  FALSE;

		DeleteFile(tempFileName);

		return ret;
	}
	else
		return FALSE;
}

BOOL CGReaderSync::SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly) {
	LOG0(1, "CGReaderSync::SyncFeed()");

	BOOL ret = FALSE;

	// phase 1 ////////////////////////////////////////////////////////////////
	// what is read in prssr will be read in Greader
	// what is starred in prssr will be starred in Greader

	if (!GetToken()) {
//		ErrorMsg.LoadString(IDS_ERROR_GETTING_TOKEN);
		return FALSE;
	}

	Downloader->Reset();
	Downloader->SetCookie(FormatSIDCookie(SID));

	CString url, body, response;
	if (si->Feed != NULL) {
		for (int i = 0; i < si->Feed->GetItemCount(); i++) {
			CFeedItem *fi = si->Feed->GetItem(i);

			if (fi->IsRead()) {
				url.Format(_T("%s/edit-tag"), Api0);
				body.Format(_T("i=%s&a=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/read")), Token); 
				Downloader->Post(url, body, response);
			}
		
/*			if (fi->IsFlagged()) {
				url.Format(_T("%s/edit-tag"), Api0);
				body.Format(_T("i=%s&a=%s&T=%s"), fi->Hash, UrlEncode(_T("user/-/state/com.google/starred")), Token); 
				Downloader->Post(url, body, response);
			}
*/
		}
	}

/*
	url.Format(_T("%s/edit-tag"), Api0);
	body.Format(_T("i=%s&r=%s&T=%s"), _T("tag:google.com,2005:reader/item/b7a7a87e59e8a909"), UrlEncode(_T("user/-/state/com.google/starred")), Token); 

//	url.Format(_T("%s/subscription/edit"), Api0);
//	body.Format(_T("ac=subscribe&s=feed%%2Fhttp://blisty.cz/rss2.rb&T=%s"), Token); 
	LOG1(1, "url : %S", url);
	LOG1(1, "body: %S", body);

	Downloader->Reset();
	Downloader->SetCookie(FormatSIDCookie(SID));
	return Downloader->Post(url, body, response);
*/

	// phase 2 ////////////////////////////////////////////////////////////////

	CString sTmpFileName;
	LPTSTR tmpFileName = sTmpFileName.GetBufferSetLength(MAX_PATH + 1);
	GetTempFileName(Config.CacheLocation, _T("rsr"), 0, tmpFileName);

	int n = 0;
	if (si->Info->CacheLimit == CACHE_LIMIT_DEFAULT)
		n = Config.CacheLimit;
	else if (si->Info->CacheLimit > 0)
		n = si->Info->CacheLimit;
	// TODO: unlimited and disabled options
	else
		n = 25;			// fallback

	url.Format(_T("%s/atom/feed/%s?n=%d"), BaseUrl, si->Info->XmlUrl, n);
	if (Downloader->SaveHttpObject(url, tmpFileName) && Downloader->Updated) {
		CFeedFile xml;
		if (xml.LoadFromFile(tmpFileName)) {
			if (xml.Parse(feed, si))
				ret = TRUE;
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

CString CGReaderSync::FormatSIDCookie(const CString &sid) {
	CString sSIDCookie;
//	sSIDCookie.Format(_T("SID=%s; expires=1600000000; path=/; domain=.google.com"), sid);
	sSIDCookie.Format(_T("SID=%s"), sid);
	return sSIDCookie;
}

BOOL CGReaderSync::GetToken() {
	Downloader->Reset();
	Downloader->SetCookie(FormatSIDCookie(SID));

	Token.Empty();
	CString url;
	url.Format(_T("%s/token"), Api0);
	return Downloader->GetHttpObject(url, Token);
}

