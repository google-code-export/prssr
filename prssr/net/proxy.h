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