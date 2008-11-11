/**
 *  http.h
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

#ifndef _HTTP_H_
#define _HTTP_H_

enum EHttpMethod {
	HTTP_METHOD_GET = 0,
	HTTP_METHOD_POST = 1
};

///////////////////////////////////////////////////////////////////////////

class CHttpSocket;

///////////////////////////////////////////////////////////////////////////
// CHttpHeader

class CHttpHeader {
public:
	CString Name;
	CString Value;

	CHttpHeader(const CString &name, const CString &value);
};

///////////////////////////////////////////////////////////////////////////
// CHttpRequest

class CHttpRequest {
public:

	CHttpRequest(EHttpMethod method, const CString &object, LPCTSTR hostName = NULL);
	virtual ~CHttpRequest();

	// add new header, if the header already exists, its value is replaced with the new value
	void SetHeader(const CString &name, const CString &value);
	void SetHeader(const CString &name, int value);
	void AddHeaders(CList<CHttpHeader *, CHttpHeader *> &headers);
	void AddCookies(CStringList &cookies);

	void Send(CHttpSocket *socket, CStringArray *addHttpHeaders = NULL);

	CString GetObject() const { return Object; }

	void SetBody(const CString &body) { Body = body; }

protected:
	EHttpMethod Method;			// GET, POST
	CString Object;			// requested object
	CString StartLine;		// start line
	CList<CHttpHeader *, CHttpHeader*> Headers;
	CString Body;			// Body for POST method
};

///////////////////////////////////////////////////////////////////////////
// CHttpResponse

class CHttpResponse {
public:
	CHttpResponse();
	virtual ~CHttpResponse();

	DWORD GetStatusCode() { return StatusCode; }
	BOOL GetHeader(const CString &name, CString &value);
	void SetStartLine(const CString &line);
	void AddHeader(const CString &name, const CString &value);

protected:
	DWORD StatusCode;
	CString StartLine;
	CList<CHttpHeader *, CHttpHeader*> Headers;
};


#endif