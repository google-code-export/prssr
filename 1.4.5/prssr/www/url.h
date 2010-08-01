/**
 *  url.h
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

	CRewriteRule() { }
	CRewriteRule(const CString &match, const CString &replace) {
		Match = match;
		Replace = replace;
	}
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

#if defined PRSSR_APP
CString StripHtmlTags(const CString &str);
#endif

#endif
