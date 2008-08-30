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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGReaderSync::CGReaderSync(CDownloader *downloader) : CFeedSync(downloader) {
	BaseUrl = _T("http://www.google.com/reader/");
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

		CString url;
		url.Format(_T("https://www.google.com/accounts/ClientLogin?service=reader&Email=%s&Passwd=%s"), UserName, Password);

		if (Downloader->SaveHttpObject(url, tempFileName)) {
			CString response;
	
			HANDLE hFile = CreateFile(tempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				// suppose the response is short, so that we can read it at once
				DWORD read;
				DWORD size = GetFileSize(hFile, NULL);
				char *buffer = new char [size + 1];
				ReadFile(hFile, buffer, size, &read, NULL);
				buffer[size] = '\0';
				CloseHandle(hFile);

				// process the response
				char seps[] = "\r\n";
				char *token = strtok(buffer, seps);
				while (token != NULL) {
					char *eqSign = strchr(token, '=');
					if (eqSign != NULL) {
						*eqSign = '\0';					// separate the key and its value
						if (strcmp(token, "SID") == 0)
							SID.Format(_T("%S"), eqSign + 1);
					}

					token = strtok(NULL, seps);
				}

				delete [] buffer;
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

	CString url;
	url.Format(_T("http://www.google.com/reader/atom/feed/%s?n=%d"), si->Info->XmlUrl, n);

	CString sSIDCookie;
	sSIDCookie.Format(_T("SID=%s; expires=1600000000; path=/; domain=.google.com"), SID);

	Downloader->Reset();
	Downloader->SetCookie(sSIDCookie);
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
