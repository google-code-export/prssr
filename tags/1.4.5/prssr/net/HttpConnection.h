/**
 *  HttpConnection.h
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

#if !defined(_HTTPCONNECTION_H_)
#define _HTTPCONNECTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "proxy.h"
#include "httpsock.h"
#include <connmgr.h>
#include "http.h"
#include "../../share/file.h"

///////////////////////////////////////////////////////////////////////////

enum EConnectionState {
	CONNECTION_STATE_CLOSED,
	CONNECTION_STATE_CONNECTING,
	CONNECTION_STATE_CONNECTED,
	CONNECTION_STATE_SENDING_REQUEST,
	CONNECTION_STATE_DATA_TRANSFER,
};

enum EContentEncoding {
	CONTENT_ENCODING_GZIP,
	CONTENT_ENCODING_DEFLATE,
	CONTENT_ENCODING_PLAIN
};

///////////////////////////////////////////////////////////////////////////

#define SOCK_BUFFER_SIZE					511

class CHttpConnection {
public:
	CHttpConnection(DWORD timeout = 12000);		// 12 sec timeout (for slow connections)
	virtual ~CHttpConnection();

	virtual void Terminate();
	virtual BOOL IsTerminated();

	void SetProxy(CProxy *proxy);
	BOOL Open(DWORD serviceType, const CString &server, int port);
	CHttpRequest *CreateRequest(const CString &object, EHttpMethod method = HTTP_METHOD_GET);
	void SendRequest(CHttpRequest *req, CStringArray *addHttpHeaders = NULL);
	CHttpResponse *ReceiveResponse();
	BOOL GetFile(CHttpResponse *res, const CString &fileName);
	BOOL Close();

	CString UserAgent;

	DWORD GetTotalFileSize() { return ResponseBodySize; }
	DWORD GetDownloadedFileSize() { return ResponseBodyDownloaded; }
	DWORD GetSysError() { return SysError; }

protected:
	BOOL ReceiveLine(CString &sLine);
	BOOL ShutdownConnection();

	int CheckGzipHeader(unsigned char *iStr, BOOL *rs);
	BOOL ReceiveResponseBodyPlain(CBufferedFile &file);
	BOOL ReceiveResponseBodyCompressed(CBufferedFile &file, EContentEncoding contentEnc);
	BOOL ReceiveBlock(unsigned char *buffer, DWORD len, DWORD *received);

	CHttpSocket Socket;
	CString HostName;
	BOOL Terminated;

	DWORD ResponseBodySize;
	DWORD ResponseBodyDownloaded;

	CProxy *Proxy;

	BOOL ChunkedTransfer;
	DWORD ChunkRemain;

	DWORD SysError;
};


#endif // !defined(_HTTPCONNECTION_H_)
