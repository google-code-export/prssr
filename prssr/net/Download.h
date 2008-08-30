/**
 *  Download.h
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

	void Reset();

	virtual BOOL SaveHttpObject(CString &strURL, const CString &strFileName, LPVOID context = NULL);
	virtual BOOL PartialDownload(CString &strURL, const CString &strFileName, DWORD startOffset, DWORD endOffset = 0, LPVOID context = NULL);
	virtual BOOL Post(CString &strURL, const CString &strBody, CString &response, LPVOID context = NULL);
	virtual void Terminate();
	virtual BOOL IsTerminated();

	EDownloadState State;
	EDownloadError Error;
	int HttpErrorNo;

	// status info
	DWORD GetTotalFileSize() { return HttpConnection.GetTotalFileSize(); }
	DWORD GetDownloadedFileSize() { return HttpConnection.GetDownloadedFileSize(); }

	CString GetCharset() { return Charset; }

	// for Conditional GET
	CString ETag;
	CString LastModified;
	BOOL Updated;

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
};

/////////////////////////////////////////////////////////////////////////////////////////

BOOL DownloadFavIcon(const CString &url, const CString &fileName);
void MakeOptimizedUrl(CString &url);

#endif // !defined(_DOWNLOAD_H_)
