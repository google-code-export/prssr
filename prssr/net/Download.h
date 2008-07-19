#if !defined(_DOWNLOAD_H_)
#define _DOWNLOAD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HttpConnection.h"

/////////////////////////////////////////////////////////////////////////////////////////

enum EDownloadState {
	DOWNLOAD_STATE_NONE,
	DOWNLOAD_STATE_CONNECTING,
	DOWNLOAD_STATE_SENDING_REQUEST,
	DOWNLOAD_STATE_RECEIVING_RESPONSE,
	DOWNLOAD_STATE_DATA_TRANSFER,
	DOWNLOAD_STATE_AUTHENTICATING,
	DOWNLOAD_STATE_AUTHENTICATED,
};

enum EDownloadError {
	DOWNLOAD_ERROR_NONE,
	DOWNLOAD_ERROR_GETTING_FILE,
	DOWNLOAD_ERROR_RESPONSE_ERROR,
	DOWNLOAD_ERROR_SENDING_REQUEST,
	DOWNLOAD_ERROR_CONNECTION_ERROR,
	DOWNLOAD_ERROR_MALFORMED_URL,
	DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE,
	DOWNLOAD_ERROR_UNKNOWN_AUTH_SCHEME,
	DOWNLOAD_ERROR_NO_LOCATION_HEADER,
	DOWNLOAD_ERROR_HTTP_ERROR,
	DOWNLOAD_ERROR_AUTHORIZATION_FAILED,
	DOWNLOAD_ERROR_AUTHENTICATION_ERROR,
};

//
// Base class for downloading HTTP object
//
// to add a special functionality, derive new object from this class and implement
// the functionality in handlers (virtual On...())
//

class CDownloader {
public:
	CDownloader();
	virtual ~CDownloader();

	CString GetUserName() { return UserName; }
	CString GetPassword() { return Password; }
	BOOL GetSavePassword() { return SavePassword; }
	void SetAuthenticationInfo(const CString &userName, const CString &password) {
		if (userName.IsEmpty()) {
			AuthSet = FALSE;
		}
		else {
			AuthSet = TRUE;
			UserName = userName;
			Password = password;
		}
	}
	void ClearAuthenticationInfo() {
		AuthSet = FALSE;
		UserName.Empty();
		Password.Empty();
	}

	virtual BOOL SaveHttpObject(CString &strURL, const CString &strFileName, LPVOID context = NULL);
	virtual BOOL PartialDownload(CString &strURL, const CString &strFileName, DWORD startOffset, DWORD endOffset = 0, LPVOID context = NULL);
	virtual void Terminate();
	virtual BOOL IsTerminated();

	EDownloadState State;
	EDownloadError Error;
	int HttpErrorNo;

	// status info
	DWORD GetTotalFileSize() { return HttpConnection.GetTotalFileSize(); }
	DWORD GetDownloadedFileSize() { return HttpConnection.GetDownloadedFileSize(); }

	CString GetCharset() { return Charset; }

protected:
	CHttpConnection HttpConnection;

	virtual void OnConnection(LPVOID context);
	virtual void OnBeforeSendRequest(CHttpRequest *&req, LPVOID context);
	virtual BOOL OnResponse(CHttpRequest *&con, CHttpResponse *&res, LPVOID context);
	virtual BOOL OnBeforeFileDownload(LPVOID context);
	virtual void OnFileDownloaded(LPVOID context);
	virtual BOOL OnAuthentication(LPVOID context);

	BOOL WWWAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context);
	BOOL ProxyAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context);
	BOOL DoAuthentication(const CString &authChallenge, CHttpRequest *&req, CHttpResponse *&res, LPVOID context);

	BOOL DoBasicAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context);
	BOOL DoDigestAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context);

	void PrepareAuthorizationHeader(int nc, const CString &uri, const CString &userName, const CString &password, CString &header);

	void FreeAdditionalHeaders();

	void SaveHeaders(CHttpResponse *res);

	CString URL;			// URL that is processed
	CString Charset;		// charset of the content

	// host
	DWORD ServiceType;
	CString ServerName;
	INTERNET_PORT Port;
	// Authentication
	BOOL AuthSet;
	CString UserName, Password;
	BOOL SavePassword;
	CString Realm, Domain, Nonce, Opaque, Stale, Algorithm, Qop;
	CString AuthResponseHeader;				// authorization header used in response to authentication

	CList<CHttpHeader *, CHttpHeader*> AdditionalHeaders;

	HANDLE HTerminate;
//	friend class CWebDownloader;
};

/////////////////////////////////////////////////////////////////////////////////////////

//
// Downloading HTTP object via conditional GET
//

class CUpdateDownloader : public CDownloader {
public:
	CUpdateDownloader(const CString &etag, const CString &lastModified);
	virtual ~CUpdateDownloader();

protected:
	virtual void OnBeforeSendRequest(CHttpRequest *&req, LPVOID context);
	virtual BOOL OnResponse(CHttpRequest *&req, CHttpResponse *&res, LPVOID context);
	virtual BOOL OnBeforeFileDownload(LPVOID context);

	CString ETag;
	CString LastModified;

	BOOL Updated;

	friend class CUpdateBar;
};

/////////////////////////////////////////////////////////////////////////////////////////

BOOL DownloadFavIcon(const CString &url, const CString &fileName);

/*
DWORD WINAPI UpdateThreadProc(LPVOID lpParameter);
UINT ManualUpdateThreadProc(LPVOID lpParameter);
*/

void MakeOptimizedUrl(CString &url);

#endif // !defined(_DOWNLOAD_H_)
