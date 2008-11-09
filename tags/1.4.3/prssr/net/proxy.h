/**
 *  proxy.h
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

#ifndef _PROXY_H_
#define _PROXY_H_

enum EProxyType {
	PROXY_TYPE_SOCKS4 = 0,
	PROXY_TYPE_SOCKS5 = 1,
	PROXY_TYPE_HTTP = 2
};


// Proxy settings

struct CProxy {
	// proxy
	EProxyType Type;
	CString HostName;
	UINT Port;
	BOOL NeedAuth;
	CString UserName;
	CString Password;
};

//////////////////////////////////////////////////////////////////////

class CHttpSocket;

class CConnectionProvider {
public:
	CConnectionProvider();

	BOOL MakeConnection(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port);

	virtual BOOL Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port);
};

//////////////////////////////////////////////////////////////////////

class CSocks4ConnectionProvider : public CConnectionProvider {
public:
	CSocks4ConnectionProvider(CProxy *proxy);

	virtual BOOL Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port);

protected:
	CProxy *Proxy;
};

//////////////////////////////////////////////////////////////////////

class CSocks5ConnectionProvider : public CConnectionProvider {
public:
	CSocks5ConnectionProvider(CProxy *proxy);

	virtual BOOL Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port);

protected:
	CProxy *Proxy;
};

//////////////////////////////////////////////////////////////////////

class CHttpProxyConnectionProvider : public CConnectionProvider {
public:
	CHttpProxyConnectionProvider(CProxy *proxy);

	virtual BOOL Connect(CHttpSocket *s, DWORD serviceType, const CString &hostName, int port);

protected:
	CProxy *Proxy;
};

#endif