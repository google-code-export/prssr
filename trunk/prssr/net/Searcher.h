/**
 *  Searcher.h
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

#ifndef _SEARCHER_H_
#define _SEARCHER_H_

#include "../xml/XMLFile.h"
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
	CString GetErrorMsg();

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
