/**
 *  url.cpp
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

#include <afxwin.h>
#include "url.h"
#include "../../share/helpers.h"
#include "../../regex/regex.h"
#include "../debug/Logger.h"
#include "HTMLFile.h"

#include <wininet.h>

#if defined PRSSR_APP

// Taken from afxint.h

static BOOL AFXAPI _ParseURLWorker(LPCTSTR pstrURL,
	LPURL_COMPONENTS lpComponents, DWORD& dwServiceType,
	INTERNET_PORT& nPort, DWORD dwFlags)
{
	// this function will return bogus stuff if lpComponents
	// isn't set up to copy the components

	ASSERT(lpComponents != NULL && pstrURL != NULL);
	if (lpComponents == NULL || pstrURL == NULL)
		return FALSE;
	ASSERT(lpComponents->dwHostNameLength == 0 ||
			lpComponents->lpszHostName != NULL);
	ASSERT(lpComponents->dwUrlPathLength == 0 ||
			lpComponents->lpszUrlPath != NULL);
	ASSERT(lpComponents->dwUserNameLength == 0 ||
			lpComponents->lpszUserName != NULL);
	ASSERT(lpComponents->dwPasswordLength == 0 ||
			lpComponents->lpszPassword != NULL);

	ASSERT(AfxIsValidAddress(lpComponents, sizeof(URL_COMPONENTS), TRUE));

	LPTSTR pstrCanonicalizedURL;
	TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	BOOL bRetVal;
	BOOL bMustFree = FALSE;
	DWORD dwCanonicalizeFlags = dwFlags &
		(ICU_NO_ENCODE | ICU_DECODE | ICU_NO_META |
		ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);
	DWORD dwCrackFlags = dwFlags & (ICU_ESCAPE | ICU_USERNAME);

	bRetVal = InternetCanonicalizeUrl(pstrURL, szCanonicalizedURL,
		&dwNeededLength, dwCanonicalizeFlags);

	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		bMustFree = TRUE;
		bRetVal = InternetCanonicalizeUrl(pstrURL, pstrCanonicalizedURL,
			&dwNeededLength, dwCanonicalizeFlags);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return FALSE;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;

	// now that it's safely canonicalized, crack it

	bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0,
						dwCrackFlags, lpComponents);
	if (bMustFree)
		delete [] pstrCanonicalizedURL;

	// convert to MFC-style service ID

	if (!bRetVal)
		dwServiceType = INET_SERVICE_UNK;
	else
	{
		nPort = lpComponents->nPort;
		switch (lpComponents->nScheme)
		{
		case INTERNET_SCHEME_FTP:
			dwServiceType = INET_SERVICE_FTP;
			break;

		case INTERNET_SCHEME_GOPHER:
			dwServiceType = INET_SERVICE_GOPHER;
			break;

		case INTERNET_SCHEME_HTTP:
			dwServiceType = INET_SERVICE_HTTP;
			break;

		case INTERNET_SCHEME_HTTPS:
			dwServiceType = INET_SERVICE_HTTPS;
			break;

		case INTERNET_SCHEME_FILE:
			dwServiceType = INET_SERVICE_FILE;
			break;

		case INTERNET_SCHEME_NEWS:
			dwServiceType = INET_SERVICE_NNTP;
			break;

		case INTERNET_SCHEME_MAILTO:
			dwServiceType = INET_SERVICE_MAILTO;
			break;

		default:
			dwServiceType = INET_SERVICE_UNK;
		}
	}

	return bRetVal;
}

BOOL AFXAPI ParseURL(LPCTSTR pstrURL, DWORD &dwServiceType, CString &strServer, CString &strObject, INTERNET_PORT &nPort) {
	dwServiceType = INET_SERVICE_UNK;

	ASSERT(pstrURL != NULL);
	if (pstrURL == NULL)
		return FALSE;

	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

	urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName = strServer.GetBuffer(INTERNET_MAX_URL_LENGTH+1);
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = strObject.GetBuffer(INTERNET_MAX_URL_LENGTH+1);

	BOOL bRetVal = _ParseURLWorker(pstrURL, &urlComponents, dwServiceType, nPort, ICU_BROWSER_MODE);

	strServer.ReleaseBuffer();
	strObject.ReleaseBuffer();
	return bRetVal;
}

////

CString SanitizeUrl(const CString &url) {
	int nPos = url.Find(_T("://"));
	if (nPos != -1) {
		int nEndPos = url.Find(_T("/"), nPos + 3);

		CString sServer;
		if (nEndPos != -1)
			sServer = url.Mid(nPos + 3, nEndPos - nPos - 3);
		else
			sServer = url.Mid(nPos + 3);
		sServer.MakeLower();

		return url.Left(nPos + 3) + sServer + url.Mid(nEndPos);
	}
	else
		return url;
}

//
//
//
CString GetURLFilePath(const CString &strObject) {
	CString strObj;
	int qmarkPos = strObject.Find('?');
	if (qmarkPos != -1)
		strObj = strObject.Left(qmarkPos);
	else
		strObj = strObject;

	int slashPos = strObj.ReverseFind('/');
	CString strPath = strObj.Left(slashPos + 1);
//	if (strPath.GetLength() > 0 && strPath.GetAt(strPath.GetLength() - 1) != '/')
//		strPath += '/';

	return strPath;
}

CString GetUrlFileName(const CString &strUrl) {
	CString strObj;
	int qmarkPos = strUrl.Find('?');
	if (qmarkPos != -1)
		strObj = strUrl.Left(qmarkPos);
	else
		strObj = strUrl;

	int slashPos = strObj.ReverseFind('/');
	CString strPath = strObj.Mid(slashPos + 1);

	return strPath;
}

/*BOOL ParseURL(const CString &url, CString &strServer, CString &strObject) {
	LOG1(5, "ParseURL('%S')", url);

	DWORD dwServiceType;
	INTERNET_PORT nPort;

	AfxParseURL(url, dwServiceType, strServer, strObject, nPort);

	return TRUE;
}
*/

//
// Translates url to file path (<root>/<sever>/<object>)
//
CString UrlToFileName(const CString &url, BOOL addHTMLExt/* = FALSE*/) {
	LOG1(5, "UrlToFileName('%S')", url);

	if (url.Left(7).CompareNoCase(_T("file://")) == 0) {
		CString fileName = url.Mid(7);
		fileName.Replace('/', '\\');
		return fileName;
	}

	DWORD serviceType;
	INTERNET_PORT port;
	CString strServer, strObject;
	ParseURL(url, serviceType, strServer, strObject, port);

	// remove '?'s
//	strObject.Remove('?');
	// remove #text
	int hashPos = strObject.ReverseFind('#');
	CString strHash;
	if (hashPos != -1) {
		strHash = strObject.Right(strObject.GetLength() - hashPos);
		strObject = strObject.Left(hashPos);
	}

	CString strPath = GetURLFilePath(strObject);
	CString strFileName = strObject.Right(strObject.GetLength() - strPath.GetLength());

	// get params
	int qmPos = strFileName.ReverseFind('?');
	CString strPars;
	if (qmPos != -1) {
		strPars = strFileName.Right(strFileName.GetLength() - qmPos);
		strFileName = strFileName.Left(qmPos);
	}

	int dotPos = strFileName.ReverseFind('.');
	CString strExt;
	if (dotPos != -1) {
		strExt = strFileName.Right(strFileName.GetLength() - dotPos);
		strFileName = strFileName.Left(dotPos);
	}

	if (strFileName.GetLength() == 0) {
		strFileName = _T("index");
		strExt = _T(".html");
	}

	// add html extension to files, which have none (to open in Pocket IE successfully)
	if (addHTMLExt) {
		if (strExt.CompareNoCase(_T(".html")) != 0 && strExt.CompareNoCase(_T(".htm")) != 0)
			strExt = _T(".html");		// add html one
	}

	//
	strPars.Replace('?', '_');
	strPars.Replace(',', '_');

	strPars.Remove('&');
	strPars.Remove('=');
	strPars.Remove('%');
	strPars.Remove('"');
	strPars.Remove('\'');
	strPars.Remove(';');
	strPars.Remove(':');
	strPars.Remove('/');
	strPars.Remove('\\');

//	strFileName += strHash;

	strPath.Replace('/', '\\');

	CString strFileNamePath;
	strFileNamePath.Format(_T("%s%s%s%s%s"), strServer, strPath, strFileName, strPars, strExt);

	// truncate the file name to MAX_PATH (limit for filenames), because some URLs are longer than MAX_PATH
	strFileNamePath = strFileNamePath.Left(MAX_PATH - 10);

	return strFileNamePath;
}


CString MakeAbsoluteUrl(const CString &url, const CString &strServer, const CString &strPath) {
	LOG1(5, "MakeAbsoluteUrl('%S')", url);

	CString strAbsUrl;

	if (url.Left(7).CompareNoCase(_T("http://")) == 0 ||
		url.Left(8).CompareNoCase(_T("https://")) == 0 ||
		url.Left(7).CompareNoCase(_T("file://")) == 0) {
		// absolute link
		strAbsUrl = url;
	}
	else {
		// relative link
		if (url.GetLength() > 0 && url.GetAt(0) == '/')
			strAbsUrl.Format(_T("http://%s%s"), strServer, url);
		else
			strAbsUrl.Format(_T("http://%s%s%s"), strServer, strPath, url);
	}

	return strAbsUrl;
}


//
// Creates url for offline browsing in form (file://<url>)
// Keeps #part
//
CString MakeOfflineUrl(const CString &strRootDir, const CString &strFileName) {
	LOG0(5, "MakeOfflineUrl()");

	CString strOfflineUrl;
	strOfflineUrl.Format(_T("file://localhost%s/%s"), strRootDir, strFileName);
	strOfflineUrl.Replace('\\', '/');

	return strOfflineUrl;
}

CString MakeOfflineUrl(const CString &strFileName) {
	LOG0(5, "MakeOfflineUrl()");

	CString strOfflineUrl;
	strOfflineUrl.Format(_T("file://localhost%s"), strFileName);
	strOfflineUrl.Replace('\\', '/');

	return strOfflineUrl;
}

/*
//
// Create file url from object (#part is removed)
// For caching purposes
//
CString MakeOnlineFile(const CString &strObject, const CString &strServer, const CString &strActivePath) {
	LOG0(5, "MakeOnlineFile()");

	CString strAbsUrl;
	if (strObject.Left(7).Compare(_T("file://")) == 0)
		strAbsUrl = strObject;
	else
		strAbsUrl = MakeAbsoluteUrl(strObject, strServer, strActivePath);

	CString strObjectFileName;
	int hashPos = strAbsUrl.ReverseFind('#');
	if (hashPos != -1)
		strObjectFileName = strAbsUrl.Left(hashPos);
	else
		strObjectFileName = strAbsUrl;

	return strObjectFileName;
}
*/


//
// Rewrite URL
//
CString RewriteUrl(const CString &sUrl, CArray<CRewriteRule *, CRewriteRule *> &rules) {
	LOG1(5, "RewriteUrl('%S', )", sUrl);

	CString newUrl = sUrl;
	char *url = WCharToChar(sUrl, CP_ACP);

	BOOL found = FALSE;
	for (int i = 0; i < rules.GetSize() && !found; i++) {
		// prepare regexp
		char *matchPattern = WCharToChar(rules[i]->Match, CP_ACP);

		regex_t re;
		if (regcomp(&re, matchPattern, REG_EXTENDED) == 0) {
			#define NSUBS				10
			regmatch_t subs[NSUBS];
			if (regexec(&re, url, NSUBS, subs, 0) == 0) {
				// match(!) -> rewrite
				newUrl = rules[i]->Replace;

				for (int k = 1; k < NSUBS; k++) {
					if (subs[k].rm_so != -1) {
						CString sNum;
						sNum.Format(_T("\\%d"), k);
						CString sSub = sUrl.Mid(subs[k].rm_so, subs[k].rm_eo - subs[k].rm_so);
						newUrl.Replace(sNum, sSub);
					}
				}

				found = TRUE;
			}
		}

		regfree(&re);

		delete [] matchPattern;
	}

	delete [] url;

	LOG1(5, "- rewritten to: %S", newUrl);
	return newUrl;
}

CString MakeHtmlOptimizerUrl(const CString &url, const CString &optUrl) {
	LOG1(5, "MakeHtmlOptimizerUrl('%S')", url);

	CString u = optUrl;
	u.Replace(_T("[%URL%]"), UrlEncode(url));
	return u;
}

CString UrlEncode(const CString &url) {
	CString encoded;
	for (int i = 0; i < url.GetLength(); i++) {
		TCHAR ch = url.GetAt(i);
		if ((ch >= 0x00 && ch <= 0x20) || (ch >= 0x22 && ch <= 0x26) ||
			ch == 0x2B || ch == 0x2C || ch == 0x2F ||
			(ch >= 0x3A && ch <= 0x40) || (ch >= 0x7B && ch <= 0x7E) || (ch >= 0x5B && ch <= 0x5E) ||
			ch == 0x60 || (ch >= 0x80 && ch <= 0xFF))
		{
			CString hex;
			hex.Format(_T("%%%02X"), ch);
			encoded += hex;
		}
		else {
			encoded += ch;
		}
	}

	return encoded;
}

#endif

//
//
//
//CString ReplaceHTMLEntities(LPCTSTR lpString) {
void ReplaceHTMLEntities(CString &str) {
	LOG1(5, "ReplaceHTMLEntities('%S')", str);

	struct {
		LPCTSTR Ent;
		int Ord;
	} ents[] = {
		{ _T("nbsp"), 160 },
		{ _T("iexcl"), 161 },
		{ _T("cent"), 162 },
		{ _T("pound"), 163 },
		{ _T("curren"), 164 },
		{ _T("yen"), 165 },
		{ _T("brvbar"), 166 },
		{ _T("sect"), 167 },
		{ _T("uml"), 168 },
		{ _T("copy"), 169 },
		{ _T("ordf"), 170 },
		{ _T("laquo"), 171 },
		{ _T("not"), 172 },
		{ _T("shy"), 173 },
		{ _T("reg"), 174 },
		{ _T("macr"), 175 },
		{ _T("deg"), 176 },
		{ _T("plusmn"), 177 },
		{ _T("sup2"), 178 },
		{ _T("sup3"), 179 },
		{ _T("acute"), 180 },
		{ _T("micro"), 181 },
		{ _T("para"), 182 },
		{ _T("middot"), 183 },
		{ _T("cedil"), 184 },
		{ _T("sup1"), 185 },
		{ _T("ordm"), 186 },
		{ _T("raquo"), 187 },
		{ _T("frac14"), 188 },
		{ _T("frac12"), 189 },
		{ _T("frac34"), 190 },
		{ _T("iquest"), 191 },
		{ _T("Agrave"), 192 },
		{ _T("Aacute"), 193 },
		{ _T("Acirc"), 194 },
		{ _T("Atilde"), 195 },
		{ _T("Auml"), 196 },
		{ _T("Aring"), 197 },
		{ _T("AElig"), 198 },
		{ _T("Ccedil"), 199 },
		{ _T("Egrave"), 200 },
		{ _T("Eacute"), 201 },
		{ _T("Ecirc"), 202 },
		{ _T("Euml"), 203 },
		{ _T("Igrave"), 204 },
		{ _T("Iacute"), 205 },
		{ _T("Icirc"), 206 },
		{ _T("Iuml"), 207 },
		{ _T("ETH"), 208 },
		{ _T("Ntilde"), 209 },
		{ _T("Ograve"), 210 },
		{ _T("Oacute"), 211 },
		{ _T("Ocirc"), 212 },
		{ _T("Otilde"), 213 },
		{ _T("Ouml"), 214 },
		{ _T("times"), 215 },
		{ _T("Oslash"), 216 },
		{ _T("Ugrave"), 217 },
		{ _T("Uacute"), 218 },
		{ _T("Ucirc"), 219 },
		{ _T("Uuml"), 220 },
		{ _T("Yacute"), 221 },
		{ _T("THORN"), 222 },
		{ _T("szlig"), 223 },
		{ _T("agrave"), 224 },
		{ _T("aacute"), 225 },
		{ _T("acirc"), 226 },
		{ _T("atilde"), 227 },
		{ _T("auml"), 228 },
		{ _T("aring"), 229 },
		{ _T("aelig"), 230 },
		{ _T("ccedil"), 231 },
		{ _T("egrave"), 232 },
		{ _T("eacute"), 233 },
		{ _T("ecirc"), 234 },
		{ _T("euml"), 235 },
		{ _T("igrave"), 236 },
		{ _T("iacute"), 237 },
		{ _T("icirc"), 238 },
		{ _T("iuml"), 239 },
		{ _T("eth"), 240 },
		{ _T("ntilde"), 241 },
		{ _T("ograve"), 242 },
		{ _T("oacute"), 243 },
		{ _T("ocirc"), 244 },
		{ _T("otilde"), 245 },
		{ _T("ouml"), 246 },
		{ _T("divide"), 247 },
		{ _T("oslash"), 248 },
		{ _T("ugrave"), 249 },
		{ _T("uacute"), 250 },
		{ _T("ucirc"), 251 },
		{ _T("uuml"), 252 },
		{ _T("yacute"), 253 },
		{ _T("thorn"), 254 },
		{ _T("yuml"), 255 },
		{ _T("fnof"), 402 },
		{ _T("Alpha"), 913 },
		{ _T("Beta"), 914 },
		{ _T("Gamma"), 915 },
		{ _T("Delta"), 916 },
		{ _T("Epsilon"), 917 },
		{ _T("Zeta"), 918 },
		{ _T("Eta"), 919 },
		{ _T("Theta"), 920 },
		{ _T("Iota"), 921 },
		{ _T("Kappa"), 922 },
		{ _T("Lambda"), 923 },
		{ _T("Mu"), 924 },
		{ _T("Nu"), 925 },
		{ _T("Xi"), 926 },
		{ _T("Omicron"), 927 },
		{ _T("Pi"), 928 },
		{ _T("Rho"), 929 },
		{ _T("Sigma"), 931 },
		{ _T("Tau"), 932 },
		{ _T("Upsilon"), 933 },
		{ _T("Phi"), 934 },
		{ _T("Chi"), 935 },
		{ _T("Psi"), 936 },
		{ _T("Omega"), 937 },
		{ _T("alpha"), 945 },
		{ _T("beta"), 946 },
		{ _T("gamma"), 947 },
		{ _T("delta"), 948 },
		{ _T("epsilon"), 949 },
		{ _T("zeta"), 950 },
		{ _T("eta"), 951 },
		{ _T("theta"), 952 },
		{ _T("iota"), 953 },
		{ _T("kappa"), 954 },
		{ _T("lambda"), 955 },
		{ _T("mu"), 956 },
		{ _T("nu"), 957 },
		{ _T("xi"), 958 },
		{ _T("omicron"), 959 },
		{ _T("pi"), 960 },
		{ _T("rho"), 961 },
		{ _T("sigmaf"), 962 },
		{ _T("sigma"), 963 },
		{ _T("tau"), 964 },
		{ _T("upsilon"), 965 },
		{ _T("phi"), 966 },
		{ _T("chi"), 967 },
		{ _T("psi"), 968 },
		{ _T("omega"), 969 },
		{ _T("thetasym"), 977 },
		{ _T("upsih"), 978 },
		{ _T("piv"), 982 },
		{ _T("bull"), 8226 },
		{ _T("hellip"), 8230 },
		{ _T("prime"), 8242 },
		{ _T("Prime"), 8243 },
		{ _T("oline"), 8254 },
		{ _T("frasl"), 8260 },
		{ _T("weierp"), 8472 },
		{ _T("image"), 8465 },
		{ _T("real"), 8476 },
		{ _T("trade"), 8482 },
		{ _T("alefsym"), 8501 },
		{ _T("larr"), 8592 },
		{ _T("uarr"), 8593 },
		{ _T("rarr"), 8594 },
		{ _T("darr"), 8595 },
		{ _T("harr"), 8596 },
		{ _T("crarr"), 8629 },
		{ _T("lArr"), 8656 },
		{ _T("uArr"), 8657 },
		{ _T("rArr"), 8658 },
		{ _T("dArr"), 8659 },
		{ _T("hArr"), 8660 },
		{ _T("forall"), 8704 },
		{ _T("part"), 8706 },
		{ _T("exist"), 8707 },
		{ _T("empty"), 8709 },
		{ _T("nabla"), 8711 },
		{ _T("isin"), 8712 },
		{ _T("notin"), 8713 },
		{ _T("ni"), 8715 },
		{ _T("prod"), 8719 },
		{ _T("sum"), 8721 },
		{ _T("minus"), 8722 },
		{ _T("lowast"), 8727 },
		{ _T("radic"), 8730 },
		{ _T("prop"), 8733 },
		{ _T("infin"), 8734 },
		{ _T("ang"), 8736 },
		{ _T("and"), 8743 },
		{ _T("or"), 8744 },
		{ _T("cap"), 8745 },
		{ _T("cup"), 8746 },
		{ _T("int"), 8747 },
		{ _T("there4"), 8756 },
		{ _T("sim"), 8764 },
		{ _T("cong"), 8773 },
		{ _T("asymp"), 8776 },
		{ _T("ne"), 8800 },
		{ _T("equiv"), 8801 },
		{ _T("le"), 8804 },
		{ _T("ge"), 8805 },
		{ _T("sub"), 8834 },
		{ _T("sup"), 8835 },
		{ _T("nsub"), 8836 },
		{ _T("sube"), 8838 },
		{ _T("supe"), 8839 },
		{ _T("oplus"), 8853 },
		{ _T("otimes"), 8855 },
		{ _T("perp"), 8869 },
		{ _T("sdot"), 8901 },
		{ _T("lceil"), 8968 },
		{ _T("rceil"), 8969 },
		{ _T("lfloor"), 8970 },
		{ _T("rfloor"), 8971 },
		{ _T("lang"), 9001 },
		{ _T("rang"), 9002 },
		{ _T("loz"), 9674 },
		{ _T("spades"), 9824 },
		{ _T("clubs"), 9827 },
		{ _T("hearts"), 9829 },
		{ _T("diams"), 9830 },
		{ _T("quot"), 34 },
		{ _T("amp"), 38 },
		{ _T("lt"), 60 },
		{ _T("gt"), 62 },
		{ _T("OElig"), 338 },
		{ _T("oelig"), 339 },
		{ _T("Scaron"), 352 },
		{ _T("scaron"), 353 },
		{ _T("Yuml"), 376 },
		{ _T("circ"), 710 },
		{ _T("tilde"), 732 },
		{ _T("ensp"), 8194 },
		{ _T("emsp"), 8195 },
		{ _T("thinsp"), 8201 },
		{ _T("zwnj"), 8204 },
		{ _T("zwj"), 8205 },
		{ _T("lrm"), 8206 },
		{ _T("rlm"), 8207 },
		{ _T("ndash"), 8211 },
		{ _T("mdash"), 8212 },
		{ _T("lsquo"), 8216 },
		{ _T("rsquo"), 8217 },
		{ _T("sbquo"), 8218 },
		{ _T("ldquo"), 8220 },
		{ _T("rdquo"), 8221 },
		{ _T("bdquo"), 8222 },
		{ _T("dagger"), 8224 },
		{ _T("Dagger"), 8225 },
		{ _T("permil"), 8240 },
		{ _T("lsaquo"), 8249 },
		{ _T("rsaquo"), 8250 },
		{ _T("euro"), 8364}
	};

	int pos, start;
	pos = str.Find('&');
	while (pos != -1) {
		CString s = str.Mid(pos + 1);
		int end = pos + 1 + s.FindOneOf(_T("; \t\n"));
		if (end != -1 && end > pos + 1) {
			CString e = str.Mid(pos + 1, end - pos - 1);
			if (e.GetAt(0) == '#') {
				if (str.GetAt(end) == ';')
					end++;

				if (e.GetAt(1) == 'x') {
					// hexa entity
					TCHAR ch;
					swscanf(e, _T("#x%x"), &ch);
					str = str.Left(pos) + CString(ch) + str.Mid(end);
				}
				else {
					// decimal entity
					TCHAR ch;
					swscanf(e, _T("#%d"), &ch);
					str = str.Left(pos) + CString(ch) + str.Mid(end);
				}
			}
			else {
				// string entity
				TCHAR buff[16];
				if (swscanf(e, _T("%s"), buff) == 1) {
					// find entity
					for (int i = 0; i < sizeof(ents) / sizeof(ents[0]); i++) {
						CString entity;
						if (wcscmp(buff, ents[i].Ent) == 0) {
							TCHAR ch = ents[i].Ord;
							str = str.Left(pos) + CString(ch) + str.Mid(end + 1);
							break;
						}
					}
				}
			}
		}

		start = pos + 1;
		pos = str.Find('&', start);
	}
}

#if defined PRSSR_APP

CString StripHtmlTags(const CString &str) {
	CHtmlFile html;
	html.SetFlags(SGML_EXTENSION_HTML_FLAG_STRIPELEMENT | SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT);

	CString s;
	s.Format(_T("<div>%s</div>"), str);		// libsgml workaround
	html.LoadFromMemory(s);

	return html.ToString();
}

#endif
