/**
 *  http.cpp
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

#include "../stdafx.h"
#include "../prssr.h"
#include "httpsock.h"
#include "http.h"
#include "../../share/helpers.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////

static CString MethodStr[] = {
	_T("GET"),
	_T("POST")
};


////////////////////////////////////////////////////////////////////////////////////
// HttpHeader

CHttpHeader::CHttpHeader(const CString &name, const CString &value) {
	Name = name;
	Value = value;
}

////////////////////////////////////////////////////////////////////////////////////
// CHttpRequest

CHttpRequest::CHttpRequest(EHttpMethod method, const CString &object, LPCTSTR hostName/* = NULL*/) {
	Method = method;
	Object = object;

	if (hostName == NULL)
		StartLine.Format(_T("%s %s HTTP/1.1\r\n"), MethodStr[method], object);
	else
		StartLine.Format(_T("%s http://%s%s HTTP/1.1\r\n"), MethodStr[method], hostName, object);
}

CHttpRequest::~CHttpRequest() {
	while (!Headers.IsEmpty())
		delete Headers.RemoveHead();
}

void CHttpRequest::SetHeader(const CString &name, const CString &value) {
	POSITION pos = Headers.GetHeadPosition();
	while (pos != NULL) {
		CHttpHeader *hdr = Headers.GetNext(pos);
		if (hdr->Name.Compare(name) == 0) {
			hdr->Value = value;
			return;
		}
	}

	// not found -> add new
	Headers.AddTail(new CHttpHeader(name, value));
}

void CHttpRequest::SetHeader(const CString &name, int value) {
	CString sValue;
	sValue.Format(_T("%d"), value);

	SetHeader(name, sValue);
}

void CHttpRequest::AddHeaders(CList<CHttpHeader *, CHttpHeader *> &headers) {
	POSITION pos = headers.GetHeadPosition();
	while (pos != NULL) {
		CHttpHeader *hdr = headers.GetNext(pos);
		Headers.AddTail(new CHttpHeader(hdr->Name, hdr->Value));
	}
}

void CHttpRequest::Send(CHttpSocket *socket, CStringArray *addHttpHeaders/* = NULL*/) {
	char *buffer;

	// start line
	buffer = WCharToChar(StartLine);
	socket->Send(buffer, strlen(buffer));
	delete [] buffer;

	// headers
	POSITION pos = Headers.GetHeadPosition();
	while (pos != NULL) {
		CHttpHeader *header = Headers.GetNext(pos);

		CString sLine;
		sLine.Format(_T("%s: %s\r\n"), header->Name, header->Value);
		buffer = WCharToChar(sLine);
		socket->Send(buffer, strlen(buffer));
		delete [] buffer;
	}
	// additional HTTP headers
	if (addHttpHeaders != NULL) {
		for (int i = 0; i < addHttpHeaders->GetSize(); i++) {
			CString sLine;
			sLine.Format(_T("%s\r\n"), addHttpHeaders->GetAt(i));
			buffer = WCharToChar(sLine);
			socket->Send(buffer, strlen(buffer));
			delete [] buffer;
		}
	}
	// and CRLF 
	socket->Send("\r\n", 2);

	// send POST body
	if (Method == HTTP_METHOD_POST) {
		// send the body
		buffer = WCharToChar(Body);
		socket->Send(buffer, strlen(buffer));
		delete [] buffer;
	}
}


////////////////////////////////////////////////////////////////////////////////////
// CHttpResponse

CHttpResponse::CHttpResponse() {
}

CHttpResponse::~CHttpResponse() {
	while (!Headers.IsEmpty())
		delete Headers.RemoveHead();
}

BOOL CHttpResponse::GetHeader(const CString &name, CString &value) {
	POSITION pos = Headers.GetHeadPosition();
	while (pos != NULL) {
		CHttpHeader *header = Headers.GetNext(pos);
		if (header->Name.CompareNoCase(name) == 0) {
			value = header->Value;
			return TRUE;
		}
	}

	return FALSE;
}

void CHttpResponse::SetStartLine(const CString &line) {
	StartLine = line;

	if (StartLine.Left(5).Compare(_T("HTTP/")) == 0 &&
		iswdigit(StartLine.GetAt(5)) &&
		StartLine.GetAt(6) == '.' &&
		iswdigit(StartLine.GetAt(7)))
	{
		swscanf(StartLine.Mid(8), _T("%d"), &StatusCode);
	}
	else
		StatusCode = 0;
}

void CHttpResponse::AddHeader(const CString &name, const CString &value) {
	Headers.AddTail(new CHttpHeader(name, value));
}
