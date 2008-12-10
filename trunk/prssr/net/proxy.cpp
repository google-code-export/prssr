/**
 *  proxy.cpp
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
#include "proxy.h"
#include "httpsock.h"
#include "HttpConnection.h"
#include "../../share/helpers.h"
#include "../../share/str.h"
#include "../www/url.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CConnectionProvider::CConnectionProvider() {
	LOG0(3, "CConnectionProvider::CConnectionProvider()");
}

BOOL CConnectionProvider::MakeConnection(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port) {
	LOG4(3, "CConnectionProvider::MakeConnection(%p, %d, '%S', %d)", s, serviceType, hostName, port);

	// resolve the address
	char *host = WCharToChar(hostName);
	u_long nRemoteAddr = LookupAddress(host);
	delete [] host;

	if (nRemoteAddr == INADDR_NONE) {
		LOG1(1, "Lookup failed: %d", WSAGetLastError());
		return FALSE;
	}

	// fill in the structure
	sockaddr_in saServer = { 0 };
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = nRemoteAddr;
	saServer.sin_port = htons((u_short) port);

	if (serviceType == INET_SERVICE_HTTPS) {
		if (!s->MakeSecure(hostName) || !s->SslDeferHandshake())
			return FALSE;
	}

	return s->Connect(&saServer);
}

BOOL CConnectionProvider::Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port) {
	LOG4(3, "CConnectionProvider::Connect(%p, %d, '%S', %d)", s, serviceType, hostName, port);

	if (!MakeConnection(s, serviceType, hostName, port))
		return FALSE;

	// SSL
	if (serviceType == INET_SERVICE_HTTPS) {
		// switch to secure mode
		if (!s->SslPerformHandshake())
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

CSocks4ConnectionProvider::CSocks4ConnectionProvider(CProxy *proxy) {
	LOG1(3, "CSocks4ConnectionProvider::CSocks4ConnectionProvider(%p)", proxy);

	Proxy = proxy;
}

BOOL CSocks4ConnectionProvider::Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port) {
	LOG4(3, "CSocks4ConnectionProvider::Connect(%p, %d, '%S', %d)", s, serviceType, hostName, port);

	if (!CConnectionProvider::MakeConnection(s, serviceType, Proxy->HostName, Proxy->Port))
		return FALSE;

	// resolve destination
	char *host = WCharToChar(hostName);
	u_long nRemoteAddress = LookupAddress(host);
	delete [] host;
	if (nRemoteAddress == INADDR_NONE)
		return FALSE;

	char *userName = NULL;
	if (Proxy->NeedAuth) {
		userName = WCharToChar(Proxy->UserName);
	}
	else {
		// create empty user name
		userName = new char [1];
		userName[0] = '\0';
	}

	// send CONNECT
	u_short portNo = htons((u_short) port);
	int len = 8 + strlen(userName) + 1;
	BYTE *socksBase = new BYTE [len];
	socksBase[0] = 4;											// SOCKS4
	socksBase[1] = 1;											// 1 == CONNECT
	memcpy(socksBase + 2, &portNo, 2);							// destination port
	memcpy(socksBase + 4, &nRemoteAddress, 4);					// destination ip
	socksBase[8] = 0;
	strcpy((char *) socksBase + 8, userName);
	int sn = s->Send((const char *) (socksBase + 0), len);

	delete [] socksBase;
	delete [] userName;

	// receive a reply
	BYTE socksReply[8] = { 0 };
	int size = s->Receive((char *) socksReply, 8);
	if (size > 0 && size != SOCKET_ERROR) {
		if (socksReply[0] == 0 && socksReply[1] == 90) {
			// OK
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	// SSL
	if (serviceType == INET_SERVICE_HTTPS) {
		// switch to secure mode
		if (!s->SslPerformHandshake())
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

CSocks5ConnectionProvider::CSocks5ConnectionProvider(CProxy *proxy) {
	LOG1(3, "CSocks5ConnectionProvider::CSocks5ConnectionProvider(%p)", proxy);

	Proxy = proxy;
}

BOOL CSocks5ConnectionProvider::Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port) {
	LOG4(3, "CSocks5ConnectionProvider::Connect(%p, %d, '%S', %d)", s, serviceType, hostName, port);

	if (!CConnectionProvider::MakeConnection(s, serviceType, Proxy->HostName, Proxy->Port))
		return FALSE;

	int size;

	// method negotiation
	BYTE s5meth[4] = { 0 };
	s5meth[0] = 5;			// SOCKS5
	s5meth[2] = 0;			// no authentication
	if (Proxy->NeedAuth) {
		s5meth[1] = 2;		// 2 methods offered
		s5meth[3] = 2;		// USERNAME/PASSWORD authentication method
		s->Send((const char *) (s5meth + 0), 4);
	}
	else {
		s5meth[1] = 1;		// 1 method offered
		s->Send((const char *) (s5meth + 0), 3);
	}

	// get reply
	BYTE s5selmeth[2] = { 0 };
	size = s->Receive((char *) s5selmeth, 2);
	if (size > 0 && size != SOCKET_ERROR) {
		// error in reply
		if (s5selmeth[0] != 5)
			return FALSE;
	}
	else
		return FALSE;

	// authentication subnegotiation
	if (s5selmeth[1] == 0) {
		// no authentiaction
	}
	else if (s5selmeth[1] == 2) {
		// USERNAME/PASSWORD authentication method
		char *userName = WCharToChar(Proxy->UserName);
		char *password = WCharToChar(Proxy->Password);

		int userNameLen = strlen(userName);
		int passwordLen = strlen(password);
		int len = 3 + userNameLen + passwordLen;
		BYTE *s5auth = new BYTE [len];
		s5auth[0] = 1;												// VER = 1
		s5auth[1] = userNameLen;									// length(user name)
		memcpy(s5auth + 2, userName, userNameLen);					// user name
		s5auth[2 + userNameLen] = passwordLen;						// length(password)
		memcpy(s5auth + 3 + userNameLen, password, passwordLen);	// password
		s->Send((const char *) (s5auth + 0), len);

		delete [] s5auth;
		delete [] password;
		delete [] userName;

		// get reply
		BYTE s5authreply[2] = { 0 };
		WSASetLastError(0);
		size = s->Receive((char *) s5authreply, 2);
		if (size > 0 && size != SOCKET_ERROR) {
			// error in reply?
			// VER == 1 && STATUS == 0
			if (s5authreply[0] != 1 || s5authreply[1] != 0)
				return FALSE;
		}
		else
			return FALSE;
	}
	else {
		// error -> quit
		return FALSE;
	}

	// send request
	char *destServerName = WCharToChar(hostName);
	u_short portNo = htons((u_short) port);

	int reqLen = strlen(destServerName) + 7;
	BYTE *s5req = new BYTE [reqLen];
	s5req[0] = 5;			// VER  = SOCKS5
	s5req[1] = 1;			// CD   = CONNECT
	s5req[2] = 2;			// RSV  = reserved
	s5req[3] = 3;			// ATYP = 3 -> domain name
	s5req[4] = strlen(destServerName);								// length(domain name)
	memcpy(s5req + 5, destServerName, strlen(destServerName));		// domain name
	memcpy(s5req + reqLen - 2, &portNo, 2);							// destination port
	s->Send((char *) s5req, reqLen);
	delete [] s5req;
	delete [] destServerName;

	// get response
	BOOL ret = FALSE;
	BYTE *s5resp = new BYTE [reqLen];
	size = s->Receive((char *) s5resp, reqLen);
	if (size > 0 && size != SOCKET_ERROR) {
		if (s5resp[0] == 5 && s5resp[1] == 0 && s5resp[2] == 0)
			ret = TRUE;
	}

	delete [] s5resp;

	// SSL
	if (serviceType == INET_SERVICE_HTTPS) {
		// switch to secure mode
		if (!s->SslPerformHandshake())
			return FALSE;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////

CHttpProxyConnectionProvider::CHttpProxyConnectionProvider(CProxy *proxy) {
	LOG1(3, "CHttpProxyConnectionProvider::CHttpProxyConnectionProvider(%p)", proxy);

	Proxy = proxy;
}

BOOL CHttpProxyConnectionProvider::Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port) {
	LOG4(3, "CHttpProxyConnectionProvider::Connect(%p, %d, '%S', %d)", s, serviceType, hostName, port);

	if (serviceType == INET_SERVICE_HTTPS) {
		// HTTPS over HTTP is not possible (man-in-the-middle attacks)
		return FALSE;
	}

	return CConnectionProvider::MakeConnection(s, serviceType, Proxy->HostName, Proxy->Port);
}
