/**
 *  httpsock.h
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

#if !defined(_HTTPSOCKET_H_)
#define _HTTPSOCKET_H_

class CHttpSocket {
public:
	// Constructors / Destructors
	CHttpSocket();
	virtual ~CHttpSocket();

	// methods
	BOOL Create();
	BOOL Connect(const CString &hostName, int port);
	BOOL Connect(const sockaddr_in *lpSockerAddr);
	int Shutdown(int how = 0x00);
	void Close();

	BOOL Send(LPCSTR pszBuf, int nBuf);
	int Receive(LPSTR pszBuf, int nBuf);
	int ReceiveBlock(LPSTR pszBuf, int nBuf);

	BOOL MakeSecure(const CString &serverName);
	BOOL SslDeferHandshake();
	BOOL SslPerformHandshake();

	void SetKeepAlive();

	void SetTimeout(DWORD timeout);

protected:
	SOCKET Socket;
	DWORD Timeout;
};

//////////////////////////////////////////////////////////////////////

u_long LookupAddress(char *serverName);

#endif