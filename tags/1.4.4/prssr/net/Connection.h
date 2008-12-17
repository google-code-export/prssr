/**
 *  Connection.h
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

#if !defined(AFX_CONNECTION_H__33D8B1CD_31D7_409D_8565_D53240608E57__INCLUDED_)
#define AFX_CONNECTION_H__33D8B1CD_31D7_409D_8565_D53240608E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <connmgr.h>
#include <connmgr_proxy.h>
#pragma comment( lib, "cellcore.lib" )

extern const GUID IID_ConnPrv_IProxyExtension;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Connection manager helper class
//
class CConnection
{
public:
	CConnection();
	virtual ~CConnection();

//	HRESULT IsAvailable(LPCTSTR pszPath = NULL);
//	HRESULT AttemptConnect(LPCTSTR pszPath = NULL);
	HRESULT IsAvailable();
	HRESULT AttemptConnect();
	HRESULT HangupConnection();

	BOOL    IsProxyRequired() { return m_bProxyRequired; }

	//
	// Properties on this object
	//
	HANDLE  GetConnection() { return m_hConnection; }
	DWORD   GetStatus()     { return m_dwStatus; }
//	void    SetCache(BOOL b){ m_bCache=b; }
//	BOOL    GetCache() { return m_bCache; }
	GUID    GetNetworkGuid() { return m_gNetwork; }
//	BOOL    GetProxy() { return m_bProxy; }
//	void    SetProxy(BOOL bProxy) { m_bProxy = bProxy; }
//	LPTSTR  GetNetworkPath() { return m_pszPath; }
//	LPTSTR  GetProxyServer() { if(m_bProxyRequired) return m_ProxyInfo.szProxyServer; else return NULL; }

//	static LPCTSTR GetInternetPath() { return CConnection::s_szInternetPath; }
//	static LPTSTR GetCorpNetPath() { return CConnection::s_szCorpNetPath; }

	PROXY_CONFIG *GetProxyInfo() { return &m_ProxyInfo; }

protected:      // Methods to override.
	HRESULT GetNetworkFromPath(LPCTSTR pszPath);

private:
//	TCHAR   *m_pszPath;
//	BOOL    m_bProxy;
	HANDLE  m_hConnection;  // Connection Manager Handle
//	BOOL    m_bCache;       // should we cache this connection when we 'hangup'
	GUID    m_gNetwork;     // the GUID for the network we are connecting to.
	DWORD   m_dwStatus;      // last connection status
//	HANDLE  m_hThreadStop;  // Event
//	HANDLE  m_hConnectionThread;    // Thread
	BOOL    m_bProxyRequired;

	BOOL CheckForRequiredProxy(HANDLE hConn);

	// Rigged paths that will map  to the correct GUID.
//	static LPCTSTR s_szInternetPath;
//	static LPTSTR s_szCorpNetPath;

	PROXY_CONFIG    m_ProxyInfo;
};

extern CConnection Connection;


#endif // !defined(AFX_CONNECTION_H__33D8B1CD_31D7_409D_8565_D53240608E57__INCLUDED_)
