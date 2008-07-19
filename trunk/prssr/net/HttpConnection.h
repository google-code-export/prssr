// HttpConnection.h: interface for the CHttpConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_HTTPCONNECTION_H_)
#define _HTTPCONNECTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "proxy.h"
#include "httpsock.h"
#include "connmgr.h"
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
};


#endif // !defined(_HTTPCONNECTION_H_)
