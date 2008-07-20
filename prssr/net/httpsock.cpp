/**
 *  httpsock.cpp
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

#include "..\stdafx.h"
#include "..\prssr.h"
#include "httpsock.h"
#include "ssl.h"
#include <sslsock.h>

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CSocket

CHttpSocket::CHttpSocket() {
	Socket = INVALID_SOCKET;		// default to an invalid socket descriptor
	Timeout = 5000;					// 5 seconds
}

CHttpSocket::~CHttpSocket() {
	Close();
}

BOOL CHttpSocket::Create() {
	Socket = ::socket(AF_INET, SOCK_STREAM, 0);	//IPPROTO_TCP
	return (Socket != INVALID_SOCKET);
}

BOOL CHttpSocket::Connect(const sockaddr_in *lpSockerAddr) {
	if (Socket == INVALID_SOCKET)
		return FALSE;

	return ::connect(Socket, (sockaddr *) lpSockerAddr, sizeof(sockaddr_in)) != SOCKET_ERROR;
}

BOOL CHttpSocket::Send(LPCSTR pszBuf, int nBuf) {
	if (Socket == INVALID_SOCKET)
		return FALSE;

	return (::send(Socket, pszBuf, nBuf, 0) != SOCKET_ERROR);
}

void CHttpSocket::SetTimeout(DWORD timeout) {
	Timeout = timeout;
}

int CHttpSocket::Receive(LPSTR pszBuf, int nBuf) {
	if (Socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	fd_set readset;
	int result, iof = -1;
	struct timeval tv;

	// Initialize the set
	FD_ZERO(&readset);
	FD_SET(Socket, &readset);

	// Initialize time out struct
	tv.tv_sec = Timeout / 1000;
	tv.tv_usec = Timeout % 1000;
	// select()
	result = ::select(1, &readset, NULL, NULL, &tv);

	// Check status
	if (result < 0)
		return SOCKET_ERROR;
	else if (result > 0 && FD_ISSET(Socket, &readset)) {
		// receive
		result = ::recv(Socket, pszBuf, nBuf, 0);
		return result;
	}

	// timed out
	return SOCKET_ERROR;
}

int CHttpSocket::ReceiveBlock(LPSTR pszBuf, int nBuf) {
	return ::recv(Socket, pszBuf, nBuf, 0);
}

int CHttpSocket::Shutdown(int how/* = 0x00*/) {
	if (Socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	return ::shutdown(Socket, how);
}

void CHttpSocket::Close() {
	if (Socket != INVALID_SOCKET) {
		VERIFY(SOCKET_ERROR != ::closesocket(Socket));
		Socket = INVALID_SOCKET;
	}
}

void CHttpSocket::SetKeepAlive() {
	// set keep alive
	unsigned long keepAlive = 1;
	setsockopt(Socket, SOL_SOCKET, SO_KEEPALIVE, (const char*) &keepAlive, sizeof(keepAlive)); 
}

BOOL CHttpSocket::MakeSecure(const CString &serverName) {
	// make it secure
	DWORD dwOptVal = SO_SEC_SSL;
	DWORD dwBytes = 0;
	SSLVALIDATECERTHOOK sslValidateFunc;

	if (setsockopt(Socket, SOL_SOCKET, SO_SECURE, (LPSTR) &dwOptVal, sizeof(dwOptVal)) == SOCKET_ERROR) {
		return FALSE;
	}

	// register certificate validation callback
	sslValidateFunc.HookFunc = certificateValidationCallback;
	sslValidateFunc.pvArg = (void *) (LPCTSTR) serverName;

	if (WSAIoctl(Socket, SO_SSL_SET_VALIDATE_CERT_HOOK, &sslValidateFunc, sizeof(sslValidateFunc), NULL, 0, &dwBytes, NULL, NULL) == SOCKET_ERROR) {
		return FALSE;
	}

	return TRUE;
}

BOOL CHttpSocket::SslDeferHandshake() {
	DWORD dwFlags = SSL_FLAG_DEFER_HANDSHAKE;
	if (WSAIoctl(Socket, SO_SSL_SET_FLAGS, &dwFlags, sizeof(DWORD), NULL, 0, NULL, NULL, NULL) == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

BOOL CHttpSocket::SslPerformHandshake() {
	if (WSAIoctl(Socket, SO_SSL_PERFORM_HANDSHAKE, NULL, 0, NULL, 0, NULL, NULL, NULL) == SOCKET_ERROR)
		return FALSE;
	
	return TRUE;
}

//

u_long LookupAddress(char *serverName) {
	hostent *lpHostEntry = NULL;
    u_long nRemoteAddr = inet_addr(serverName);
	if (nRemoteAddr == INADDR_NONE) {
		// pServerName isn't a dotted IP, so resolve it through DNS
		lpHostEntry = gethostbyname(serverName);
		if (lpHostEntry == NULL) {
			nRemoteAddr = INADDR_NONE;
		}
		else {
			nRemoteAddr = *((u_long *) lpHostEntry->h_addr_list[0]);
		}
    }

    return nRemoteAddr;
}
