// HttpSock.h: interface for the CHttpSocket class.
//
//////////////////////////////////////////////////////////////////////

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