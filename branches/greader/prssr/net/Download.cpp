/**
 *  Download.cpp
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

#if defined PRSSR_APP
	#include "../StdAfx.h"
	#include "../prssr.h"
#endif

#include "Download.h"

//#include "..\share\defs.h"
#include "../Config.h"
#include "HttpConnection.h"
#include "../Site.h"
#include "../Feed.h"
//#include "..\FeedFile.h"

#include "../../share/helpers.h"
#include "../base64/base64.h"

#include "Connection.h"
#if defined PRSSR_APP
//	#include "CePropertySheet.h"
	#include "../AuthenticationDlg.h"
//	#include "..\MainFrm.h"
#endif
#include "../digest/digcalc.h"
//#include "SuspendKiller.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include <afxinet.h>
#define INITGUID
#include <connmgr.h>
#include <connmgr_proxy.h>


BOOL DownloadFavIcon(const CString &url, const CString &fileName) {
	LOG0(3, "DownloadFavIcon()");

	BOOL ret = FALSE;
	CDownloader downloader;

	// url of favicon
	DWORD serviceType;
	CString sServer, sObject;
	INTERNET_PORT nPort;
	ParseURL(url, serviceType, sServer, sObject, nPort);

	CString faviconURL;
	faviconURL.Format(_T("%s://%s/favicon.ico"),
		serviceType == INET_SERVICE_HTTPS ? _T("https") : _T("http"), sServer);

	// temp file name
	CString sTmpFileName;
	LPTSTR tmpFileName = sTmpFileName.GetBufferSetLength(MAX_PATH + 1);
	GetTempFileName(Config.CacheLocation, _T("rsr"), 0, tmpFileName);

	if (downloader.SaveHttpObject(faviconURL, tmpFileName)) {
		DeleteFile(fileName);
		CreatePath(fileName);
		if (MoveFile(tmpFileName, fileName))
			return TRUE;					// ok, we have got a favicon
		else {
			DeleteFile(tmpFileName);
			return FALSE;
		}
	}
	else {
		// no favicon => delete temp file
		DeleteFile(tmpFileName);
		return FALSE;
	}
}

// CDownloader /////////////////////////////////

CDownloader::CDownloader() {
	LOG0(3, "CDownloader::CDownloader()");

	HTerminate = CreateEvent(NULL, FALSE, FALSE, NULL);

	HttpConnection.UserAgent = Config.UserAgent;

	// setup proxy for connection
	if (Config.ProxyProfileIdx != -1)
		HttpConnection.SetProxy(&(Config.ProxyProfiles[Config.ProxyProfileIdx]->ProxyConfig));

	AuthSet = FALSE;
}

CDownloader::~CDownloader() {
	LOG0(5, "CDownloader::~CDownloader()");
	FreeAdditionalHeaders();
}

void CDownloader::Reset() {
	ETag.Empty();
	LastModified.Empty();
	Charset.Empty();
	Cookies.RemoveAll();
}

void CDownloader::SaveHeaders(CHttpResponse *res) {
	LOG0(5, "CDownloader::SaveHeaders()");

	CString value;

	if (res->GetHeader(_T("Content-Type"), value)) {
		value.MakeLower();
		int nPosCharset = value.Find(_T("charset="));
		if (nPosCharset != -1) {
			int nPosSemiColon = value.Find(';', nPosCharset);
			Charset = nPosSemiColon == -1 ? value.Mid(nPosCharset + 8) : value.Mid(nPosCharset + 8, nPosSemiColon - nPosCharset - 8);
		}
	}
}

void CDownloader::OnConnection(LPVOID context) {
}

void CDownloader::OnFileDownloaded(LPVOID context) {
}

BOOL CDownloader::OnBeforeFileDownload(LPVOID context) {
	return TRUE;
}

BOOL CDownloader::OnAuthentication(LPVOID context) {
	LOG0(3, "CDownloader::OnAuthentication()");

	if (AuthSet) {
		State = DOWNLOAD_STATE_AUTHENTICATED;
		return TRUE;	// user predefined values
	}
	else {
/*
#if defined PRSSR_APP
		// in pRSSreader main app, we ask user for auth details
		// get username/password
		CCePropertySheet sheet(IDS_AUTHENTICATION, theApp.m_pMainWnd);
		CAuthenticationPg pgAuth;
		sheet.SetMenu(IDR_CANCEL);
		sheet.AddPage(&pgAuth);

		pgAuth.m_strSite = ServerName;
		pgAuth.m_strRealm = Realm;
		if (sheet.DoModal()) {
			UserName = pgAuth.m_strUserName;
			Password = pgAuth.m_strPassword;
			SavePassword = pgAuth.m_bSavePassword;

			return TRUE;
		}
		else
			return FALSE;
#elif defined PRSSR_SERVICE || defined PRSSR_SRVSTUB
		// in service, if we do not know how to authenticate -> do not disturb the user
		return FALSE;
#endif
*/
		return FALSE;
	}
}

///

BOOL CDownloader::DoBasicAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context) {
	LOG0(5, "CDownloader::DoBasicAuthentication()");

	BOOL ret = FALSE;
	do {
		// force closing of the connection
		HttpConnection.Close();
		delete res; res = NULL;

		// ask for username and password
		if (OnAuthentication(context)) {
			// add authentication credentials to the original request
			CString userpass;
			userpass.Format(_T("%s:%s"), UserName, Password);
			CString authentication;
			authentication.Format(_T("Basic %s"), base64encode(userpass));
			req->SetHeader(AuthResponseHeader, authentication);

			// new connection with authentication credentials
			if (HttpConnection.Open(ServiceType, ServerName, Port)) {
				HttpConnection.SendRequest(req);
				res = HttpConnection.ReceiveResponse();
				if (res != NULL) {
					if (res->GetStatusCode() == HTTP_STATUS_OK) {
						State = DOWNLOAD_STATE_AUTHENTICATED;
					}
					else {
						if (AuthSet) {
							// authorization failed
							Error = DOWNLOAD_ERROR_AUTHORIZATION_FAILED;
							break;
						}
						else {
							// try re-enter the user name and password
						}
					}
				}
				else {
					// response error
					Error = DOWNLOAD_ERROR_RESPONSE_ERROR;
					break;
				}
			}
			else {
				// can not connect
				Error = DOWNLOAD_ERROR_CONNECTION_ERROR;
				break;
			}
		}
		else {
			// auth canceled
			Error = DOWNLOAD_ERROR_AUTHENTICATION_ERROR;
			break;
		}
	} while (State != DOWNLOAD_STATE_AUTHENTICATED);

	return State == DOWNLOAD_STATE_AUTHENTICATED;
}

void CDownloader::PrepareAuthorizationHeader(int nc, const CString &uri, const CString &userName, const CString &password, CString &header) {
	LOG0(5, "CDownloader::PrepareAuthorizationHeader()");

	HASHHEX hhHA1;
	HASHHEX hhHA2 = "";
	HASHHEX hhResponse;

	char szNonceCount[9] = { 0 };
	sprintf(szNonceCount, "%08d", nc);

	char *pszAlg = WCharToChar(Algorithm);
	char *pszUser = WCharToChar(userName);
	char *pszRealm = WCharToChar(Realm);
	char *pszPass = WCharToChar(password);
	char *pszNonce = WCharToChar(Nonce);
	// FIXME: generate something
	char *pszCNonce = "0a4f113b";
	char *pszQop = NULL;
	char *pszMethod = "GET";					// we use always GET method
	char *pszURI = WCharToChar(uri);

	if (!Qop.IsEmpty()) {
		int nComma = Qop.Find(',');
		if (nComma != -1) {
			int nStart = 0;
			CString sItem;
			while (nComma != -1) {
				sItem = Qop.Mid(nStart, nComma - nStart);
				if (sItem.CompareNoCase(_T("auth")) == 0) {
					pszQop = new char [5];
					strcpy(pszQop, "auth");
					break;
				}
				else if (sItem.CompareNoCase(_T("auth-int")) == 0) {
					pszQop = new char [9];
					strcpy(pszQop, "auth-int");
					break;
				}
				nStart = nComma + 1;
				nComma = Qop.Find(nStart, ',');
			}
			sItem = Qop.Mid(nStart);
			if (pszQop == NULL)
				pszQop = WCharToChar(sItem);	// use the last value in the qop list
		}
		else
			pszQop = WCharToChar(Qop);
	}

	// calc response
	DigestCalcHA1(pszAlg, pszUser, pszRealm, pszPass, pszNonce, pszCNonce, hhHA1);
	DigestCalcResponse(hhHA1, pszNonce, szNonceCount, pszCNonce, pszQop, pszMethod, pszURI, hhHA2, hhResponse);
	CString response = CharToWChar((char *) hhResponse);

	// format the header
	CString authorization;
	authorization.Format(
		_T("Digest username=\"%s\", ")
		_T("realm=\"%s\", ")
		_T("nonce=\"%s\", ")
		_T("uri=\"%s\", ")
		_T("response=\"%s\""),
		userName, Realm, Nonce, uri, response);

	if (!Opaque.IsEmpty()) {
		CString opaque;
		opaque.Format(_T(", opaque=\"%s\""), Opaque);
		authorization += opaque;
	}

	// if 'qop' is specified, other parameters must be sent
	if (!Qop.IsEmpty()) {
		CString sQop = CharToWChar(pszQop);
		CString sCNonce = CharToWChar(pszCNonce);
		CString sNonceCount = CharToWChar(szNonceCount);

		CString qopMust;
		qopMust.Format(
			_T(", qop=%s")
			_T(", cnonce=\"%s\"")
			_T(", nc=\"%08d\""),
			sQop, sCNonce, nc);

		authorization += qopMust;
	}

	delete [] pszAlg;
	delete [] pszUser;
	delete [] pszRealm;
	delete [] pszPass;
	delete [] pszNonce;
	delete [] pszURI;
	delete [] pszQop;

	header = authorization;
}

BOOL CDownloader::DoDigestAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context) {
	LOG0(5, "CDownloader::DoDigestAuthentication()");

	int nc = 1;
	do {
		// force closing of the connection
		HttpConnection.Close();
		delete res; res = NULL;

		// ask for username and password
		if (OnAuthentication(context)) {
			// add authentication credentials to the original request
			CString authorization;
			PrepareAuthorizationHeader(nc, req->GetObject(), UserName, Password, authorization);
			req->SetHeader(AuthResponseHeader, authorization);

			// new connection with authentication credentials
			if (HttpConnection.Open(ServiceType, ServerName, Port)) {
				HttpConnection.SendRequest(req);
				res = HttpConnection.ReceiveResponse();
				if (res != NULL) {
					if (res->GetStatusCode() == HTTP_STATUS_OK) {
						State = DOWNLOAD_STATE_AUTHENTICATED;
					}
					else {
						if (AuthSet) {
							// authorization failed
							Error = DOWNLOAD_ERROR_AUTHORIZATION_FAILED;
							break;
						}
						else {
							// try re-enter the user name and password
						}
					}
				}
				else {
					// response error
					Error = DOWNLOAD_ERROR_RESPONSE_ERROR;
					break;
				}
			}
			else {
				// can not connect
				Error = DOWNLOAD_ERROR_CONNECTION_ERROR;
				break;
			}
		}
		else {
			// auth canceled
			Error = DOWNLOAD_ERROR_AUTHENTICATION_ERROR;
			break;
		}
	} while (State != DOWNLOAD_STATE_AUTHENTICATED);

	return State = DOWNLOAD_STATE_AUTHENTICATED;
}

BOOL CDownloader::WWWAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context) {
	LOG0(5, "CDownloader::WWWAuthentication()");

	CString authChallenge;
	if (!res->GetHeader(_T("WWW-Authenticate"), authChallenge)) {
		Error = DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE;
		return FALSE;
	}

	AuthResponseHeader = _T("Authorization");
	return DoAuthentication(authChallenge, req, res, context);
}

BOOL CDownloader::ProxyAuthentication(CHttpRequest *&req, CHttpResponse *&res, LPVOID context) {
	LOG0(5, "CDownloader::ProxyAuthentication()");

	CString authChallenge;
	if (!res->GetHeader(_T("Proxy-Authenticate"), authChallenge)) {
		Error = DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE;
		return FALSE;
	}

	AuthResponseHeader = _T("Proxy-Authorization");
	return DoAuthentication(authChallenge, req, res, context);
}

BOOL CDownloader::DoAuthentication(const CString &authChallenge, CHttpRequest *&req, CHttpResponse *&res, LPVOID context) {
	LOG0(5, "CDownloader::DoAuthentication()");

	State = DOWNLOAD_STATE_AUTHENTICATING;

	// scheme realm,auth-pars
	int pos = authChallenge.Find(' ');
	CString scheme = authChallenge.Left(pos);

	// parse realm and parameters
	while (pos < authChallenge.GetLength()) {
		int eqPos = authChallenge.Find('=', pos);
		if (eqPos == -1) {
			Error = DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE;
			return FALSE;
		}

		CString name = authChallenge.Mid(pos, eqPos - pos);
		name.TrimLeft();

		int endPos = -1;
		CString value;
		if (authChallenge[eqPos + 1] == '"') {
			endPos = authChallenge.Find('"', eqPos + 1);
			if (endPos == -1) {
				Error = DOWNLOAD_ERROR_AUTHENTICATION_RESPONSE;
				return FALSE;
			}
			endPos = authChallenge.Find(',', endPos);
		}
		else {
			endPos = authChallenge.Find(',', eqPos + 1);
		}

		// last item
		if (endPos == -1)
			endPos = authChallenge.GetLength();

		value = authChallenge.Mid(eqPos + 1, endPos - eqPos - 1);
		// strip "
		value.TrimLeft('"');
		value.TrimRight('"');

		if (name.Compare(_T("realm")) == 0)
			Realm = value;
		else if (name.Compare(_T("domain")) == 0)
			Domain = value;
		else if (name.Compare(_T("nonce")) == 0)
			Nonce = value;
		else if (name.Compare(_T("opaque")) == 0)
			Opaque = value;
		else if (name.Compare(_T("stale")) == 0)
			Stale = value;
		else if (name.Compare(_T("algorithm")) == 0)
			Algorithm = value;
		else if (name.Compare(_T("qop")) == 0)
			Qop = value;

		pos = endPos + 1;
	}

	if (scheme.CompareNoCase(_T("basic")) == 0) {
		// Basic authentication
		return DoBasicAuthentication(req, res, context);
	}
	else if (scheme.CompareNoCase(_T("digest")) == 0) {
		// Digest authentication
		return DoDigestAuthentication(req, res, context);
	}
	else {
		Error = DOWNLOAD_ERROR_UNKNOWN_AUTH_SCHEME;
		return FALSE;
	}
}

///

void CDownloader::OnBeforeSendRequest(CHttpRequest *&req, LPVOID context) {
	if (!ETag.IsEmpty())
		req->SetHeader(_T("If-None-Match"), (LPCTSTR) ETag);
	if (!LastModified.IsEmpty())
		req->SetHeader(_T("If-Modified-Since"), (LPCTSTR) LastModified);
}

BOOL CDownloader::OnResponse(CHttpRequest *&req, CHttpResponse *&res, LPVOID context) {
	LOG0(3, "CDownloader::OnResponse()");

	int redirections = 0;	// number of redirections
	DWORD statusCode;
	CString newAddress, headers;
	CString strETag, strLastModified;

	Updated = FALSE;
	do {
		statusCode = res->GetStatusCode();
		LOG1(1, "StausCode: %d", statusCode);
		switch (statusCode) {
			case HTTP_STATUS_OK:
			case HTTP_STATUS_PARTIAL_CONTENT:
				SaveHeaders(res);
				if (res->GetHeader(_T("ETag"), strETag)) ETag = strETag;
				if (res->GetHeader(_T("Last-Modified"), strLastModified)) LastModified = strLastModified;
				Updated = TRUE;
				return TRUE;

			case HTTP_STATUS_NOT_MODIFIED:						// 304
				LOG0(3, "- NOT updated");
				Updated = FALSE;	// do not download the file (it is not changed)
				return FALSE;

			case HTTP_STATUS_MOVED:								// 301
			case HTTP_STATUS_REDIRECT:							// 302
			case HTTP_STATUS_REDIRECT_KEEP_VERB:				// 307
			case HTTP_STATUS_REDIRECT_METHOD:					// 303
				redirections++;

				if (res->GetHeader(_T("Location"), newAddress)) {
					CString object;
					if (ParseURL(newAddress, ServiceType, ServerName, object, Port)) {
						// absolute URL
						URL = newAddress;
					}
					else {
						// relative URL
						object = newAddress;
						if (ServiceType == INET_SERVICE_HTTPS)
							URL.Format(_T("https://%s%s"), ServerName, object);
						else
							URL.Format(_T("http://%s%s"), ServerName, object);
					}

					delete res;
					res = NULL;

					delete req;
					req = NULL;

					HttpConnection.Close();

					if (HttpConnection.Open(ServiceType, ServerName, Port)) {
						req = HttpConnection.CreateRequest(object);
						if (req != NULL) {
							req->AddHeaders(AdditionalHeaders);
							HttpConnection.SendRequest(req);
							res = HttpConnection.ReceiveResponse();
							if (res != NULL) {
							}
							else {
								Error = DOWNLOAD_ERROR_RESPONSE_ERROR;
								return FALSE;
							}
						}
						else {
							Error = DOWNLOAD_ERROR_SENDING_REQUEST;
							return FALSE;
						}
					}
					else {
						Error = DOWNLOAD_ERROR_CONNECTION_ERROR;
						return FALSE;
					}
				}
				else {
					Error = DOWNLOAD_ERROR_NO_LOCATION_HEADER;
					return FALSE;
				}

				break;

			case HTTP_STATUS_DENIED:
				// 401 Authorization required
				return WWWAuthentication(req, res, context);

/*			case 407:
				// 407 Proxy Authentication required
//				if (HttpConnection.Proxy->NeedAuth) {
//					AuthSet = TRUE;
//					UserName = HttpConnection.Proxy->UserName;
//					Password = HttpConnection.Proxy->Password;
//					ret = ProxyAuthentication(req, res, context);
//					end = TRUE;
//				}
//				else {
//					// proxy needs authentication
//					end = TRUE;
//				}
				end = TRUE;
				break;
*/

			default:
				Error = DOWNLOAD_ERROR_HTTP_ERROR;
				HttpErrorNo = statusCode;
				return FALSE;
		}
	} while (redirections < 6);

	return FALSE;
}

void CDownloader::Terminate() {
	LOG0(3, "CDownloader::Terminate()");

	HttpConnection.Terminate();
	SetEvent(HTerminate);
}

BOOL CDownloader::IsTerminated() {
	return HttpConnection.IsTerminated();
}

//
// SaveHttpObject
//
BOOL CDownloader::SaveHttpObject(CString &url, const CString &strFileName, LPVOID context/* = NULL*/) {
	LOG1(3, "CDownloader::SaveHttpObject('%S')", url);

	Error = DOWNLOAD_ERROR_NONE;
	URL = url;

	ReplaceHTMLEntities(url);			// there might be entities in the url

	OnConnection(context);
	CString objectName;
	if (ParseURL(url, ServiceType, ServerName, objectName, Port)) {
		State = DOWNLOAD_STATE_CONNECTING;

		DWORD timeout = 3000;			// we start with 3 seconds
		for (int tries = 0; tries < 3; tries++) {
			if (HttpConnection.Open(ServiceType, ServerName, Port)) {
				CHttpRequest *req = HttpConnection.CreateRequest(objectName);
				if (req != NULL) {
					State = DOWNLOAD_STATE_SENDING_REQUEST;
					req->AddHeaders(AdditionalHeaders);
					req->AddCookies(Cookies);
					OnBeforeSendRequest(req, context);
					HttpConnection.SendRequest(req, &Config.AdditionalHttpHeaders);

					State = DOWNLOAD_STATE_RECEIVING_RESPONSE;
					CHttpResponse *res = HttpConnection.ReceiveResponse();
					if (res != NULL) {
						if (OnResponse(req, res, context)) {
							if (OnBeforeFileDownload(context)) {
								State = DOWNLOAD_STATE_DATA_TRANSFER;
								if (HttpConnection.GetFile(res, strFileName)) {
									OnFileDownloaded(context);
								}
								else {
									Error = DOWNLOAD_ERROR_GETTING_FILE;
								}
							}
						}
						delete res;
					}
					else
						Error = DOWNLOAD_ERROR_RESPONSE_ERROR;

					delete req;
				}
				else
					Error = DOWNLOAD_ERROR_SENDING_REQUEST;

				HttpConnection.Close();

				return Error == DOWNLOAD_ERROR_NONE;
			}
			else {
				// wait for a while
				DWORD dwResult = WaitForSingleObject(HTerminate, timeout);
				if (dwResult == WAIT_OBJECT_0)
					break;			// terminated
				else
					timeout *= 2;	// timed out
			}
		}

		Error = DOWNLOAD_ERROR_CONNECTION_ERROR;
	}
	else
		Error = DOWNLOAD_ERROR_MALFORMED_URL;

	return Error == DOWNLOAD_ERROR_NONE;
}

//
// ResumeDownload
//
BOOL CDownloader::PartialDownload(CString &url, const CString &strFileName, DWORD startOffset, DWORD endOffset/* = 0*/, LPVOID context/* = NULL*/) {
	LOG1(3, "CDownloader::partialDownload('%S')", url);

	FreeAdditionalHeaders();
	CString sRange;
	if (endOffset == 0)
		sRange.Format(_T("bytes=%d-"), startOffset);
	else
		sRange.Format(_T("bytes=%d-%d"), startOffset, endOffset);
	AdditionalHeaders.AddTail(new CHttpHeader(_T("Range"), sRange));

	return SaveHttpObject(url, strFileName, context);
}

BOOL CDownloader::Post(CString &url, const CString &strBody, CString &response, LPVOID context/* = NULL*/) {
	LOG1(3, "CDownloader::Post('%S')", url);

	Error = DOWNLOAD_ERROR_NONE;
	URL = url;

	ReplaceHTMLEntities(url);			// there might be entities in the url

	OnConnection(context);
	CString objectName;
	if (ParseURL(url, ServiceType, ServerName, objectName, Port)) {
		State = DOWNLOAD_STATE_CONNECTING;

		DWORD timeout = 3000;			// we start with 3 seconds
		for (int tries = 0; tries < 3; tries++) {
			if (HttpConnection.Open(ServiceType, ServerName, Port)) {
//				CHttpRequest *req = HttpConnection.CreateRequest(objectName, HTTP_METHOD_POST);
				CHttpRequest *req = HttpConnection.CreateRequest(objectName, HTTP_METHOD_GET);
				if (req != NULL) {
					// set the request body
					req->SetBody(strBody);

					// add necessary headers
//					req->SetHeader(_T("Content-Type"), _T("appliction/x-www-form-urlencoded"));
//					req->SetHeader(_T("Content-Length"), strBody.GetLength());
//					LOG1(1, "Len: %d", strBody.GetLength());

					State = DOWNLOAD_STATE_SENDING_REQUEST;
					req->AddHeaders(AdditionalHeaders);
					OnBeforeSendRequest(req, context);
					HttpConnection.SendRequest(req, &Config.AdditionalHttpHeaders);

					State = DOWNLOAD_STATE_RECEIVING_RESPONSE;
					CHttpResponse *res = HttpConnection.ReceiveResponse();
					if (res != NULL) {
						if (OnResponse(req, res, context)) {
							if (OnBeforeFileDownload(context)) {
								State = DOWNLOAD_STATE_DATA_TRANSFER;

								TCHAR tempFileName[MAX_PATH];
								GetTempFileName(Config.CacheLocation, L"rsr", 0, tempFileName);
								if (HttpConnection.GetFile(res, tempFileName)) {
									// ok, we get the response in the file -> read it
									HANDLE hFile = CreateFile(tempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
									if (hFile != INVALID_HANDLE_VALUE) {
										// suppose the response is short, so that we can read it at once
										DWORD read;
										DWORD size = GetFileSize(hFile, NULL);
										char *buffer = new char [size + 1];
										ReadFile(hFile, buffer, size, &read, NULL);
										buffer[size] = '\0';
										response.Format(_T("%S"), buffer);
										CloseHandle(hFile);
									}

									OnFileDownloaded(context);
								}
								else {
									Error = DOWNLOAD_ERROR_GETTING_FILE;
								}
//								DeleteFile(tempFileName);
							}
						}
						delete res;
					}
					else
						Error = DOWNLOAD_ERROR_RESPONSE_ERROR;

					delete req;
				}
				else
					Error = DOWNLOAD_ERROR_SENDING_REQUEST;

				HttpConnection.Close();

				return Error == DOWNLOAD_ERROR_NONE;
			}
			else {
				// wait for a while
				DWORD dwResult = WaitForSingleObject(HTerminate, timeout);
				if (dwResult == WAIT_OBJECT_0)
					break;			// terminated
				else
					timeout *= 2;	// timed out
			}
		}

		Error = DOWNLOAD_ERROR_CONNECTION_ERROR;
	}
	else
		Error = DOWNLOAD_ERROR_MALFORMED_URL;

	return Error == DOWNLOAD_ERROR_NONE;
}

void CDownloader::FreeAdditionalHeaders() {
	while (!AdditionalHeaders.IsEmpty())
		delete AdditionalHeaders.RemoveHead();
}

void CDownloader::SetCookie(const CString &cookie) {
	Cookies.AddTail(cookie);	
}
