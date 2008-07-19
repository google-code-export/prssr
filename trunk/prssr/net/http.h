// http.h
//
// HTTP related structures
//

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