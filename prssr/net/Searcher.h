//
// Searcher.cpp
//

#ifndef _SEARCHER_H_
#define _SEARCHER_H_

#include "..\xml\XMLFile.h"
#include "HttpConnection.h"

/////////////////////////////////////////////////////////////////////////////
// CSearcher abstract class

struct CSearchResultItem {
	CString SiteName;
	CString Description;
	CString XMLURL;

	CSearchResultItem() {
	}

	CSearchResultItem(const CString &siteName, const CString &xmlUrl) {
		SiteName = siteName;
		XMLURL = xmlUrl;
	}

	CSearchResultItem(const CSearchResultItem &item) {
		SiteName = item.SiteName;
		Description = item.Description;
		XMLURL = item.XMLURL;
	}
};

enum EError {
	ERROR_CONNECT,
	ERROR_SENDING_REQUEST,
	ERROR_GETTING_RESPONSE,
	ERROR_GETTING_FILE,
	ERROR_XML_RPC_RESPONSE
};

enum ESearchState {
	SEARCH_STATE_NONE,
	SEARCH_STATE_CONNECTING,
	SEARCH_STATE_CONNECTED,
	SEARCH_STATE_SENDING_REQUEST,
	SEARCH_STATE_RECEIVING_RESPONSE,
	SEARCH_STATE_DATA_TRANSFER,
	SEARCH_STATE_CLOSED
};

/////////////////////////////////////////////////////////////////////////////
// CSyndic8ComSearcher class (search on www.syndic8.com)

class CSyndic8ComSearcher {
public:
	EError Error;
	ESearchState State;

	CSyndic8ComSearcher(const CString &what, const CString &location);
	
	virtual BOOL Search(int limit, CList<CSearchResultItem *, CSearchResultItem *> &searchResult);
	virtual void Terminate();

	BOOL IsTerminated() { return HttpConnection.IsTerminated(); }

	DWORD GetDownloadedFileSize() { return PosOffset + HttpConnection.GetDownloadedFileSize(); } 

protected:
	CString What;														// what we are searching for
	CString ServerName;
	CString EndPoint;
	CString TempLocation;

	CHttpConnection HttpConnection;
	int PosOffset;

	// support function
	BOOL SendXMLRPCRequest(const CString &strRequest, char **buffer, DWORD *len);
	BOOL IsChildNode(CXmlNode *parent, CXmlNode *&child, const CString &tagName);
	BOOL ParseArray(CXmlNode *value, CList<CXmlNode *, CXmlNode *> &listValues);

	// XMLRPC function
	BOOL FindFeeds(char **buffer, DWORD *len, int limit);
	BOOL QueryFeeds(char **buffer, DWORD *len);
	BOOL GetFeedInfo(char **buffer, DWORD *len, CList<int, int> &feedIDs);

	// result parsing functions
	BOOL ParseFindResponse(char *xmlResponse, int responseLen, CList<int, int> &feedIDs);
	BOOL ParseFeedInfoResponse(char *xmlResponse, int responseLen, CList<CSearchResultItem *, CSearchResultItem *> &searchResult);

	BOOL ParseFeedInfoStruct(CXmlNode *node, CSearchResultItem *sritem);
};

#endif
