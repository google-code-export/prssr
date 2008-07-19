// ssl.h
//
// SSL
//

#ifndef _SSL_H_
#define _SSL_H_

HRESULT LoadSSL();
HRESULT FreeSSL();

int certificateValidationCallback(DWORD dwType, LPVOID pvArg, DWORD dwChainLen, LPBLOB pCertChain, DWORD dwFlags);

#endif