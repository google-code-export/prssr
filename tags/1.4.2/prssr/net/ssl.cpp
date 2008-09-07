/**
 *  ssl.cpp
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

#include "../StdAfx.h"
#include "../prssr.h"
#include "ssl.h"
#include "../Config.h"

#if defined PRSSR_APP
#include "../AcceptCertificateDlg.h"
#endif

#include <wincrypt.h>
#include <schnlsp.h>
#include <sslsock.h>

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif



// load SslCrackCertificate and SslFreeCertificate
#define SSL_CRACK_CERTIFICATE_NAME		TEXT("SslCrackCertificate")
#define SSL_FREE_CERTIFICATE_NAME		TEXT("SslFreeCertificate")

static HMODULE hSchannelDLL;

static SSL_CRACK_CERTIFICATE_FN gSslCrackCertificate;
static SSL_FREE_CERTIFICATE_FN gSslFreeCertificate;

static HCERTSTORE HCertStore;

HRESULT LoadSSL() {
	// already loaded?
	if (gSslCrackCertificate && gSslFreeCertificate) return S_OK;

	HCertStore = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, NULL, 0, NULL);
	if (HCertStore == NULL) {
		LOG0(1, "CertOpenStore failed");
	}

	hSchannelDLL = LoadLibrary(TEXT("schannel.dll"));
	if (!hSchannelDLL) {
		// error logging
		return E_FAIL;
	}

	gSslCrackCertificate = (SSL_CRACK_CERTIFICATE_FN) GetProcAddress(hSchannelDLL, SSL_CRACK_CERTIFICATE_NAME);
	gSslFreeCertificate = (SSL_FREE_CERTIFICATE_FN) GetProcAddress(hSchannelDLL, SSL_FREE_CERTIFICATE_NAME);

	if (!gSslCrackCertificate || !gSslFreeCertificate) {
		// error logging
		gSslCrackCertificate = NULL;
		gSslFreeCertificate = NULL;
		FreeLibrary(hSchannelDLL);
		hSchannelDLL = NULL;
		return E_FAIL;
	}
	else {
		return S_OK;
	}
}

HRESULT FreeSSL() {
	if (!CertCloseStore(HCertStore, CERT_CLOSE_STORE_CHECK_FLAG)) {
		LOG0(1, "CertCloseStore failed");
	}

	if (hSchannelDLL) {
		FreeLibrary(hSchannelDLL);
		hSchannelDLL = NULL;
	}
	return S_OK;
}

BOOL IsEarlierThan(FILETIME *ft1, FILETIME *ft2) {
	return CompareFileTime(ft1, ft2) < 0;
}

char *ParseCN(char *pchSubjLine) {
	char *pchCommonName = NULL;
	char *pchEnd = NULL;

	if (!pchSubjLine) return NULL;

	pchSubjLine = _strlwr(pchSubjLine);
	pchCommonName = strstr(pchSubjLine, "cn=");
	if (!pchCommonName) {
		return NULL;
	}

	pchCommonName += 3;
	pchEnd = pchCommonName;
	// find the next separator: , or \t or space, or apos
	while (*pchEnd != '\0'
		&& *pchEnd != ','
		&& *pchEnd != ' '
		&& *pchEnd != '\t'
		&& *pchEnd != '\'')
	{
		pchEnd++;
	}
	if (*pchEnd != '\0') *pchEnd = '\0';

	return pchCommonName;
}

// the certificate validation for SSL
int certificateValidationCallback(DWORD dwType, LPVOID pvArg, DWORD dwChainLen, LPBLOB pCertChain, DWORD dwFlags) {
	if (!Config.CheckCertificates)
		return SSL_ERR_OKAY;

	X509Certificate *pCert = NULL;
	int nRet = SSL_ERR_CERT_UNKNOWN;

	BOOL bMatched = FALSE;
	BOOL bValidDate = TRUE;
	BOOL bTrusted = !(dwFlags & SSL_CERT_FLAG_ISSUER_UNKNOWN);

	// dwType must be SSL_CERT_X.509
	if (dwType != SSL_CERT_X509) {
		// error logging
		return nRet;
	}

/*	if (dwFlags & SSL_CERT_FLAG_ISSUER_UNKNOWN) {
		// error logging
		return nRet;
	}
*/
	if (pCertChain == NULL) return nRet;
	ASSERT(dwChainLen == 1);

	if (!gSslCrackCertificate || !gSslFreeCertificate) {
		// error logging
		return nRet; // unable to crack
	}

	// crack X.509 Certificate
	if (!gSslCrackCertificate(pCertChain->pBlobData, pCertChain->cbSize, TRUE, &pCert)) {
		// error logging
		return SSL_ERR_BAD_DATA;
	}

	// Site check
	{
		char *pchSubject = NULL;
		char *pchCN = NULL;
		CString sRemoteHost((LPCTSTR) pvArg);
		char *pchRemoteHost = WCharToChar(sRemoteHost);

		pchSubject = pCert->pszSubject;

		// here you need to parse the subjec to retrieve the CN name
		pchCN = ParseCN(pchSubject);
		if (!pchCN) {
			goto FuncExit;
		}

		// CN comparison
		if ('*' == *pchCN && '.' == *(pchCN+1)) {
			while ('\0' != *pchRemoteHost && '.' != *pchRemoteHost)
				pchRemoteHost++;
			if ('.' == *pchRemoteHost) {
				bMatched = !(_stricmp(pchRemoteHost + 1, pchCN + 2));
			}
			// if there is no dot, bMatch remains FALSE
		}
		else {
			bMatched = !(_stricmp(pchRemoteHost, pchCN));
		}
//		if (!bMatched) {
//			// error logging
//			goto FuncExit;
//			// show certificate
//		}
	}

	// validFrom, validUntil check
	{
		SYSTEMTIME stNow;
		FILETIME ftNow;
		FILETIME ftValidFrom = pCert->ValidFrom;
		FILETIME ftValidUntil = pCert->ValidUntil;

		GetSystemTime(&stNow);
		SystemTimeToFileTime(&stNow, &ftNow);

		if (!(IsEarlierThan(&ftValidFrom, &ftNow) && IsEarlierThan(&ftNow, &ftValidUntil))) {
			// give user an option to continue or not
			// a little more lenient than Subject check
			bValidDate = FALSE;
		}
	}

#if defined PRSSR_APP
	{
		// find the certificate in the store
		PCCERT_CONTEXT pCertCtx = CertCreateCertificateContext(X509_ASN_ENCODING, pCertChain->pBlobData, pCertChain->cbSize);
		if (CertFindCertificateInStore(HCertStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertCtx, NULL)) {
			// Certificate found (was accepted previously) -> accept it
			nRet = SSL_ERR_OKAY;
		}
		else {
			if (bTrusted && bValidDate && bMatched)
				nRet = SSL_ERR_OKAY;
			else {
				// certificate not found -> ask if it should be accepted
				CAcceptCertificateDlg dlg(AfxGetMainWnd());
				dlg.Trusted = bTrusted;
				dlg.ValidDate = bValidDate;
				dlg.ValidName = bMatched;
				if (dlg.DoModal() == IDYES) {
					nRet = SSL_ERR_OKAY;
					// the certificate was accepted -> add it to the store
					if (!CertAddEncodedCertificateToStore(HCertStore, X509_ASN_ENCODING,
						pCertChain->pBlobData, pCertChain->cbSize, CERT_STORE_ADD_REPLACE_EXISTING, NULL))
					{
						LOG0(1, "CertAddEncodedCertificateToStore failed");
					}
				}
				else
					nRet = SSL_ERR_CERT_UNKNOWN;
			}
		}

		CertFreeCertificateContext(pCertCtx);
	}

#elif defined PRSSR_SERVICE || defined PRSSR_SRVSTUB
	if (bTrusted && bValidDate && bMatched)
		nRet = SSL_ERR_OKAY;
	else
		nRet = SSL_ERR_CERT_UNKNOWN;
#endif

FuncExit:
	gSslFreeCertificate(pCert);

	return nRet;
}

