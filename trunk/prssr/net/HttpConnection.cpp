// HTTPClient.cpp: implementation of the CHttpConnection class.
//
//////////////////////////////////////////////////////////////////////

#if defined PRSSR_APP
	#include "..\stdafx.h"
	#include "..\prssr.h"
	#include "..\resource.h"
#endif

#include "HttpConnection.h"
//#include "Download.h"
#include "../../share/helpers.h"
//#include "..\Config.h"

#include "../../zlib/zlib.h"


#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////

// for shutdown (from winsock2.h)
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpConnection::CHttpConnection(DWORD timeout/* = 12000*/) {
	LOG0(5, "CHttpConnection::CHttpConnection()");

	Terminated = FALSE;
	Socket.SetTimeout(timeout);
	Proxy = NULL;

	ResponseBodySize = 0;
	ResponseBodyDownloaded = 0;
}

CHttpConnection::~CHttpConnection() {
	LOG0(5, "CHttpConnection::~CHttpConnection()");

	Close();
	delete Proxy;
}

void CHttpConnection::Terminate() {
	LOG0(3, "CHttpConnection::Terminate()");

	Close();
	Terminated = TRUE;
}

BOOL CHttpConnection::IsTerminated() {
	return Terminated;
}

BOOL CHttpConnection::ReceiveLine(CString &sLine) {
	LOG0(5, "CHttpConnection::ReceiveLine()");

	sLine.Empty();
	
	char ch;
	while (TRUE) {
		int size = Socket.Receive(&ch, sizeof(ch));
		if (size > 0 && size != SOCKET_ERROR) {
			sLine += (TCHAR) ch;
			// althought lines should be terminated by CRLF, there are WEB servers that do not follow this fact
			if (ch == _T('\n'))
				return TRUE;
		}
		else {
			ShutdownConnection();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CHttpConnection::ShutdownConnection() {
	LOG0(5, "CHttpConnection::ShutdownConnection()");

	Socket.Shutdown();
	Socket.Close();

	return TRUE;
}

void CHttpConnection::SetProxy(CProxy *px) {
	delete Proxy;
	Proxy = NULL;

	if (px != NULL) {
		Proxy = new CProxy;
		*Proxy = *px;		// copy it
	}
}

BOOL CHttpConnection::Open(DWORD serviceType, const CString &serverName, int port) {
	LOG3(3, "CHttpConnection::Open(%d, '%S', %d)", serviceType, serverName, port);

	if (!Socket.Create())
		return FALSE;

	CConnectionProvider *provider = NULL;
	if (Proxy != NULL) {
		switch (Proxy->Type) {
			case PROXY_TYPE_SOCKS4: provider = new CSocks4ConnectionProvider(Proxy); break;
			case PROXY_TYPE_SOCKS5: provider = new CSocks5ConnectionProvider(Proxy); break;
			case PROXY_TYPE_HTTP:   provider = new CHttpProxyConnectionProvider(Proxy); break;
			default:                provider = new CConnectionProvider(); break;
		}
	}
	else
		provider = new CConnectionProvider();

	if (provider != NULL && provider->Connect(&Socket, serviceType, serverName, port)) {
		HostName = serverName;

		delete provider;
		return TRUE;
	}
	else {
		delete provider;

		Socket.Close();
		return FALSE;
	}
}

BOOL CHttpConnection::Close() {
	LOG0(3, "CHttpConnection::Close()");

	ShutdownConnection();
	return TRUE;
}

CHttpRequest *CHttpConnection::CreateRequest(const CString &object, EHttpMethod method/* = HTTP_METHOD_GET*/) {
	LOG0(3, "CHttpConnection::CreateRequest()");

	CHttpRequest *req = NULL;
	if (Proxy != NULL && Proxy->Type == PROXY_TYPE_HTTP)
		req = new CHttpRequest(method, object, HostName);
	else
		req = new CHttpRequest(method, object);

	req->SetHeader(_T("Host"), HostName);
	req->SetHeader(_T("Connection"), _T("close"));
	req->SetHeader(_T("User-Agent"), UserAgent);
//	req->SetHeader(_T("Accept"), _T("text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5"));
//	req->SetHeader(_T("Accept-Language"), _T("en-us,en;q=0.5"));
	req->SetHeader(_T("Accept-Encoding"), _T("gzip,deflate"));
	// for testing
////	req->SetHeader(_T("Accept-Encoding"), _T("gzip"));
////	req->SetHeader(_T("Accept-Encoding"), _T("deflate"));
//	req->SetHeader(_T("Accept-Charset"), _T("ISO-8859-1,utf-8;q=0.7,*;q=0.7"));

	return req;
}


void CHttpConnection::SendRequest(CHttpRequest *req, CStringArray *addHttpHeaders/* = NULL*/) {
	LOG0(3, "CHttpConnection::SendRequest()");

	req->Send(&Socket, addHttpHeaders);
}

CHttpResponse *CHttpConnection::ReceiveResponse() {
	LOG0(3, "CHttpConnection::ReceiveResponse()");

	CString sLine;
	if (!ReceiveLine(sLine)) {
		return NULL;
	}

	CHttpResponse *res = new CHttpResponse();
	res->SetStartLine(sLine);

	do {
		if (!ReceiveLine(sLine)) {
			delete res;
			return NULL;
		}

		sLine.TrimLeft();
		sLine.TrimRight();

		int nPos = sLine.Find(':');
		if (nPos != -1) {
			CString value = sLine.Mid(nPos + 1);
			value.TrimLeft();
			value.TrimRight();

			res->AddHeader(sLine.Left(nPos), value);
		}
		else
			break;
	} while (!sLine.IsEmpty());

	return res;
}

BOOL CHttpConnection::ReceiveBlock(unsigned char *buffer, DWORD len, DWORD *received) {
	LOG0(5, "CHttpConnection::ReceiveBlock()");

	BOOL ret = TRUE;
	DWORD totalReceived = 0;

	if (ChunkedTransfer) {
		DWORD remain = len;
		char *buff = (char *) buffer;

		while (remain > 0) {
			if (ChunkRemain == 0) {
				// get chunk header (containing the length of the following chunk)
				CString chunkHead;
				if (ReceiveLine(chunkHead))
					swscanf(chunkHead, _T("%x"), &ChunkRemain);
				else
					return FALSE;

				if (ChunkRemain == 0) {
					// last chunk header read
					ret = TRUE;
					break;
				}
			}
			else {
				int toReceive = min(min(remain, ChunkRemain), SOCK_BUFFER_SIZE);
				int size = Socket.Receive(buff, toReceive);
				totalReceived += size;

				if (size == 0) {
					// end of transfer
					ret = TRUE;
					break;
				}
				else if (size == SOCKET_ERROR) {
					ret = FALSE;
					break;
				}
				else {
//					if (ResponseBodySize > 0)
						ResponseBodyDownloaded += size;

					buff += size;
					remain -= size;
					ChunkRemain -= size;

					if (ChunkRemain == 0) {
						// read to the end of line to get to the start of the next chunk header
						CString empty;
						ReceiveLine(empty);
					}
				}
			}
		}
	}
	else {
		DWORD remain = len;
		char *buff = (char *) buffer;
		while (remain > 0) {
			int toReceive = min(remain, SOCK_BUFFER_SIZE);
			int size = Socket.Receive(buff, toReceive);
			totalReceived += size;

			if (size == 0) {
				// end of transfer
				ret = TRUE;
				break;
			}
			else if (size == SOCKET_ERROR) {
				ret = FALSE;
				break;
			}
			else {
//				if (ResponseBodySize > 0)
					ResponseBodyDownloaded += size;

				buff += size;
				remain -= size;
			}
		}
	}

	if (received != NULL)
		*received = totalReceived;

	return ret;
}

#define CHUNK					2048

// plain ////

BOOL CHttpConnection::ReceiveResponseBodyPlain(CBufferedFile &file) {
	LOG0(5, "CHttpConnection::ReceiveResponseBodyPlain()");

	BOOL ret = FALSE;

	unsigned char in[CHUNK] = { 0 };
	DWORD read;
	while (ReceiveBlock(in, CHUNK, &read)) {
		if (read == 0) {
			ret = TRUE;			// end of transfer
			break;
		}
		else {
			if (!file.Write(in, read)) {
				ret = FALSE;
				break;
			}
		}
	}

	return ret;
}

// gzip /////////////////////////////////////////////////////////////////////

#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

static unsigned gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

int CHttpConnection::CheckGzipHeader(unsigned char *iStr, BOOL *rs) {
	LOG0(5, "CHttpConnection::CheckGzipHeader()");

	enum  { GZIP_INIT = 0, GZIP_OS, GZIP_EXTRA0, GZIP_EXTRA1, GZIP_EXTRA2, GZIP_ORIG, GZIP_COMMENT, GZIP_CRC };
	char c;
	int mSkipCount = 0, mFlags = 0, hMode = GZIP_INIT, mLen = 0;

	*rs = TRUE;

	int streamLen = 0;
	DWORD read;
	BOOL mCheckHeaderDone = FALSE;

	while (!mCheckHeaderDone) {
		if (streamLen == 0) {
			if (!ReceiveBlock(iStr, CHUNK, &read)) {
				*rs = FALSE;
				return 0;
			}

			streamLen = read;
		}

		switch (hMode) {
			case GZIP_INIT:
				c = *iStr++;
				streamLen--;

				if (mSkipCount == 0 && ((unsigned) c & 0377) != gz_magic[0]) {
					*rs = FALSE;
					return 0;
				}

				if (mSkipCount == 1 && ((unsigned)c & 0377) != gz_magic[1]) {
					*rs = FALSE;
					return 0;
				}

				if (mSkipCount == 2 && ((unsigned)c & 0377) != Z_DEFLATED) {
					*rs = FALSE;
					return 0;
				}

				mSkipCount++;
				if (mSkipCount == 4) {
					mFlags = (unsigned) c & 0377;
					if (mFlags & RESERVED) {
						*rs = FALSE;
						return 0;
					}
					hMode = GZIP_OS;
					mSkipCount = 0;
				}
				break;

			case GZIP_OS:
				c = *iStr++;
				streamLen--;
				mSkipCount++;

				if (mSkipCount == 6)
					hMode = GZIP_EXTRA0;
				break;

			case GZIP_EXTRA0:
				if (mFlags & EXTRA_FIELD) {
					c = *iStr++;
					streamLen--;
					mLen = (uInt) c & 0377;
					hMode = GZIP_EXTRA1;
				}
				else
					hMode = GZIP_ORIG;
				break;

			case GZIP_EXTRA1:
				c = *iStr++;
				streamLen--;

				mLen = ((uInt) c & 0377) << 8;
				mSkipCount = 0;
				hMode = GZIP_EXTRA2;
				break;

			case GZIP_EXTRA2:
				if (mSkipCount == mLen)
					hMode = GZIP_ORIG;
				else {
					c = *iStr++;
					streamLen--;
					mSkipCount++;
				}
				break;

			case GZIP_ORIG:
				if (mFlags & ORIG_NAME) {
					c = *iStr++;
					streamLen--;
					if (c == 0)
						hMode = GZIP_COMMENT;
				}
				else
					hMode = GZIP_COMMENT;
				break;

			case GZIP_COMMENT:
				if (mFlags & GZIP_COMMENT) {
					c = *iStr++;
					streamLen--;
					if (c == 0) {
						hMode = GZIP_CRC;
						mSkipCount = 0;
					}
				}
				else {
					hMode = GZIP_CRC;
					mSkipCount = 0;
				}
				break;

			case GZIP_CRC:
				if (mFlags & HEAD_CRC) {
					c = *iStr++;
					streamLen--;

					mSkipCount++;
					if (mSkipCount == 2) {
						mCheckHeaderDone = TRUE;
						return read - streamLen;
					}
				}
				else {
					mCheckHeaderDone = TRUE;
					return read - streamLen;
				}
				break;
		} // switch
	} // while

	return read - streamLen;
}

BOOL CHttpConnection::ReceiveResponseBodyCompressed(CBufferedFile &file, EContentEncoding contentEnc) {
	LOG1(5, "CHttpConnection::ReceiveResponseBodyCompressed(, %d)", contentEnc);

	BOOL mDummyStreamInitialised = FALSE;

	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	// allocate inflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	int hdrLen = 0;
	if (contentEnc == CONTENT_ENCODING_GZIP) {
		BOOL rv;
		hdrLen = CheckGzipHeader(in, &rv);
		if (!rv)
			return FALSE;

		if (inflateInit2(&strm, -15) != Z_OK)
			return FALSE;

		strm.next_in  = in + hdrLen;
		strm.avail_in = (uInt) CHUNK - hdrLen;
	}
	else {
		// deflate
		if (inflateInit(&strm) != Z_OK)
			return FALSE;

		strm.avail_in = 0;
		strm.next_in = Z_NULL;
	}


	DWORD read;
	// decompress until deflate stream ends or end of file
	do {
		if (strm.avail_in == 0) {
			if (!ReceiveBlock(in, CHUNK, &read)) {
				(void) inflateEnd(&strm);
				return FALSE;
			}

			strm.avail_in = read;
			if (strm.avail_in == 0) {
				break;
			}
			strm.next_in = in;
		}

		// run inflate() on input until output buffer not full
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);

			if (ret == Z_DATA_ERROR) {
				unsigned char *org_next_in = strm.next_in;
				uInt org_avail_in = strm.avail_in;

				// this is taken from firefox source code
				//
				// some servers (notably Apache with mod_deflate) don't generate zlib headers
				// insert a dummy header and try again
				static char dummy_head[2] = {
					0x8 + 0x7 * 0x10,
					(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
				};
				inflateReset(&strm);
				strm.next_in = (Bytef*) dummy_head;
				strm.avail_in = sizeof(dummy_head);

				ret = inflate(&strm, Z_NO_FLUSH);
				if (ret != Z_OK)
					return FALSE;

				// stop an endless loop caused by non-deflate data being labelled as deflate
				if (mDummyStreamInitialised) {
					// endless loop detected
					return FALSE;
				}

				mDummyStreamInitialised = TRUE;
				// reset stream pointers to our original data
				strm.next_in = org_next_in;
				strm.avail_in = org_avail_in;
			}
			else if (ret == Z_MEM_ERROR) {
				(void) inflateEnd(&strm);
				return FALSE;
			}
			else if (ret == Z_OK || ret == Z_STREAM_END) {
				have = CHUNK - strm.avail_out;
				DWORD written;
				if (!file.Write(out, have, &written) || written != have) {
					(void) inflateEnd(&strm);
					return FALSE;
				}
			}
		} while (strm.avail_out == 0);

		// done when inflate() says it's done
	} while (ret != Z_STREAM_END);

	// clean up and return
	(void) inflateEnd(&strm);

	return ret == Z_STREAM_END ? TRUE : FALSE;
}

BOOL CHttpConnection::GetFile(CHttpResponse *res, const CString &fileName) {
	LOG0(5, "CHttpConnection::GetFile()");

	BOOL ret = FALSE;

	// check response headers

	// Content-Encoding header
	EContentEncoding contentEncoding;
	CString sContentEncoding;
	if (res->GetHeader(_T("Content-Encoding"), sContentEncoding)) {
		if (sContentEncoding.Compare(_T("gzip")) == 0)
			contentEncoding = CONTENT_ENCODING_GZIP;
		else if (sContentEncoding.Compare(_T("deflate")) == 0)
			contentEncoding = CONTENT_ENCODING_DEFLATE;
		else
			contentEncoding = CONTENT_ENCODING_PLAIN;
	}
	else
		contentEncoding = CONTENT_ENCODING_PLAIN;

	// find Transfer-Encoding: chunked
	ChunkedTransfer = FALSE;
	CString sTransferEncoding;
	if (res->GetHeader(_T("Transfer-Encoding"), sTransferEncoding) &&
		sTransferEncoding.Compare(_T("chunked")) == 0) {
		ChunkedTransfer = TRUE;
		ChunkRemain = 0;
	}

	CString sContentLength;
	ResponseBodySize = 0;
	ResponseBodyDownloaded = 0;
	if (res->GetHeader(_T("Content-Length"), sContentLength)) {
		swscanf(sContentLength, _T("%d"), &ResponseBodySize);
	}

	BOOL append = res->GetStatusCode() == HTTP_STATUS_PARTIAL_CONTENT;

	// get file
	CBufferedFile file;
	DWORD dwCreationDispostion = append ? OPEN_EXISTING : CREATE_ALWAYS;
	if (file.Create(fileName, GENERIC_WRITE, FILE_SHARE_READ, dwCreationDispostion, FILE_ATTRIBUTE_NORMAL)) {
		if (append) file.Seek(0, FILE_END);

		switch (contentEncoding) {
			case CONTENT_ENCODING_GZIP:
			case CONTENT_ENCODING_DEFLATE:
				ret = ReceiveResponseBodyCompressed(file, contentEncoding);
				break;

			default:
				ret = ReceiveResponseBodyPlain(file);
				break;
		}

		file.Close();
	}

	return ret;
}

