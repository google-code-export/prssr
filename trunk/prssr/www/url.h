// url.h: interface for the url related stuff.
//
//////////////////////////////////////////////////////////////////////
#ifndef _URL_H_INCLUDED_
#define _URL_H_INCLUDED_

#include <afxtempl.h>

#if defined PRSSR_APP

///

#define INET_SERVICE_UNK					0
#define INET_SERVICE_FTP					1
#define INET_SERVICE_GOPHER					1
#define INET_SERVICE_HTTP					2
#define INET_SERVICE_HTTPS					3
#define INET_SERVICE_FILE					4
#define INET_SERVICE_NNTP					5
#define INET_SERVICE_MAILTO					6

//////////////////////////////////////////////////////////////////////

struct CRewriteRule {
	CString Match;					// matching pattern
	CString Replace;				// replacing pattern
};

BOOL ParseURL(LPCTSTR pstrURL, DWORD &dwServiceType, CString &strServer, CString &strObject, INTERNET_PORT &nPort);

CString SanitizeUrl(const CString &url);

CString GetURLFilePath(const CString &strObject);
CString GetUrlFileName(const CString &strUrl);

CString MakeAbsoluteUrl(const CString &url, const CString &strServer, const CString &strPath);
CString MakeOfflineUrl(const CString &strRootDir, const CString &strFileName);
CString MakeOfflineUrl(const CString &strFileName);
CString UrlToFileName(const CString &url, BOOL addHTMLExt = FALSE);
CString RewriteUrl(const CString &url, CArray<CRewriteRule *, CRewriteRule *> &rules);
CString MakeHtmlOptimizerUrl(const CString &url, const CString &optUrl);
CString UrlEncode(const CString &url);

#endif

// HTML
void ReplaceHTMLEntities(CString &str);

#endif