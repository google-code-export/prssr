/**
 *  Searcher.cpp
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

#include "../StdAfx.h"
#include "../prssr.h"
#include "Searcher.h"
#include "../xml/XMLFile.h"
#include "../www/url.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CSyndic8ComSearcher class (search on www.syndic8.com)


CSyndic8ComSearcher::CSyndic8ComSearcher(const CString &what, const CString &location) {
	What = what;
	ServerName = _T("www.syndic8.com");
	EndPoint = _T("/xmlrpc.php");
	TempLocation = location;
}

// support functions

BOOL CSyndic8ComSearcher::SendXMLRPCRequest(const CString &strRequest, char **buffer, DWORD *len) {
	LOG0(5, "CSyndic8ComSearcher::SendXMLRPCRequest(,,)");

	State = SEARCH_STATE_CONNECTING;
	BOOL ret = FALSE;
	if (HttpConnection.Open(INET_SERVICE_HTTP, ServerName, 80)) {
		State = SEARCH_STATE_CONNECTED;
		CHttpRequest *request = HttpConnection.CreateRequest(EndPoint, HTTP_METHOD_POST);
		if (request != NULL) {
			// set the request body
			request->SetBody(strRequest);

			// add necessary headers
			request->SetHeader(_T("Content-Type"), _T("text/xml"));
			request->SetHeader(_T("Content-Length"), strRequest.GetLength());

			// send the request
			State = SEARCH_STATE_SENDING_REQUEST;
			HttpConnection.SendRequest(request);

			// get response
			State = SEARCH_STATE_RECEIVING_RESPONSE;
			CHttpResponse *response = HttpConnection.ReceiveResponse();
			if (response != NULL) {
				TCHAR tempFileName[MAX_PATH];
				GetTempFileName(TempLocation, L"rsr", 0, tempFileName);

				State = SEARCH_STATE_DATA_TRANSFER;
				if (HttpConnection.GetFile(response, tempFileName)) {
					// ok, we get the response
					ret = TRUE;
					// read the file
					HANDLE hFile = CreateFile(tempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						DWORD read;
						DWORD size = GetFileSize(hFile, NULL);
						*buffer = new char[size];
						ReadFile(hFile, *buffer, size, &read, NULL);
						*len = read;

						CloseHandle(hFile);
					}
				}
				else {
					// error getting file
					Error = ERROR_GETTING_FILE;
				}

				delete response;
				// delete the temp file
				DeleteFile(tempFileName);
			}
			else {
				// error getting response
				Error = ERROR_GETTING_RESPONSE;
			}

			HttpConnection.Close();
			delete request;
		}
		else {
			// error sending request
			Error = ERROR_SENDING_REQUEST;
		}
	}
	else {
		// can not connect to site
		Error = ERROR_CONNECT;
	}

	PosOffset += HttpConnection.GetDownloadedFileSize();

	return ret;
}

BOOL CSyndic8ComSearcher::IsChildNode(CXmlNode *parent, CXmlNode *&child, const CString &tagName) {
	LOG1(5, "CSyndic8ComSearcher::IsChildNode(%p, ,)", parent);

	if (parent == NULL)
		return FALSE;

	POSITION pos = parent->GetFirstChildPos();
	if (pos != NULL) {
		child = parent->GetNextChild(pos);
		if (child != NULL) {
			CString childTagName = child->GetName();
			if (childTagName.Compare(tagName) == 0)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CSyndic8ComSearcher::ParseArray(CXmlNode *value, CList<CXmlNode *, CXmlNode *> &listValues) {
	LOG1(5, "CSyndic8ComSearcher::ParseArray(%p,)", value);

	if (value == NULL)
		return FALSE;

	// check <array> tag
	CXmlNode *array = NULL;
	if (IsChildNode(value, array, _T("array"))) {
		// check <data> tag
		CXmlNode *data = NULL;
		if (IsChildNode(array, data, _T("data"))) {
			// get list of <value> tags
			// iterate the array
			POSITION pos = data->GetFirstChildPos();
			while (pos != NULL) {
				// <value> tag
				CXmlNode *value = data->GetNextChild(pos);
				if (value != NULL) {
					CString tagName = value->GetName();
					if (tagName.Compare(_T("value")) == 0) {
						// get child node of <value> tag
						POSITION pos = value->GetFirstChildPos();
						if (pos != NULL) {
							CXmlNode *node = value->GetNextChild(pos);
							// save it to the list
							listValues.AddTail(node);
						}
					}
				}

				PosOffset += 100;
			}
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}

// search functions

BOOL CSyndic8ComSearcher::FindFeeds(char **buffer, DWORD *len, int limit) {
	LOG0(5, "CSyndic8ComSearcher::FindFeeds()");

	// create XML-RPC request
	CString sRequest;

	sRequest += _T("<?xml version=\"1.0\"?>\r\n");
	sRequest += _T("<methodCall>\r\n");
	sRequest += _T("\t<methodName>syndic8.FindFeeds</methodName>\r\n");
	sRequest += _T("\t<params>\r\n");

	sRequest += _T("\t\t<param><value><string>\r\n");		// search string
	sRequest += What;
	sRequest += _T("</string></value></param>\r\n");
//	sRequest += _T("\t\t<param><value><string>headlines_rank</string></value></param>\r\n");	// sort by
	sRequest += _T("\t\t<param><value><string>views</string></value></param>\r\n");			// sort by
//	sRequest += _T("\t\t<param><value><string>sitename</string></value></param>\r\n");		// sort by

	// number of items in the result
	CString sLimit;
	sLimit.Format(_T("\t\t<param><value><i4>%d</i4></value></param>\r\n"), limit);
	sRequest += sLimit;
	sRequest += _T("\t</params>\r\n");

	sRequest += _T("</methodCall>\r\n");

	return SendXMLRPCRequest(sRequest, buffer, len);
}

BOOL CSyndic8ComSearcher::QueryFeeds(char **buffer, DWORD *len) {
/*	//
	// * string Feed field to match
	// * string Relational operator
	// * string Value to match
	// * (optional) string Field to sort results
	// * (optional) int Result Limit
	// * (optional) int Result Start
	//

	// create XML-RPC request
	CString sRequest;

	sRequest += _T("<?xml version=\"1.0\"?>\r\n");
	sRequest += _T("<methodCall>\r\n");
	sRequest += _T("\t<methodName>syndic8.QueryFeeds</methodName>\r\n");
	sRequest += _T("\t<params>\r\n");

	sRequest += _T("\t\t<param><value><string>description</string></value></param>\r\n");
	sRequest += _T("\t\t<param><value><string>like</string></value></param>\r\n");
	sRequest += _T("\t\t<param><value><string>\r\n");		// search string
	sRequest += What;
	sRequest += _T("</string></value></param>\r\n");

	sRequest += _T("\t\t<param><value><string>sitename</string></value></param>\r\n");		// sort by

//	number of items in the result
	CString sLimit;
	sLimit.Format(_T("\t\t<param><value><i4>%d</i4></value></param>\r\n"), Config.SearchLimit);
	sRequest += sLimit;
	sRequest += _T("\t</params>\r\n");

	sRequest += _T("</methodCall>\r\n");

	return SendXMLRPCRequest(sRequest, buffer, len);
*/
	return FALSE;
}

BOOL CSyndic8ComSearcher::GetFeedInfo(char **buffer, DWORD *len, CList<int, int> &feedIDs) {
	LOG0(5, "CSyndic8ComSearcher::GetFeedInfo()");

	// create XML-RPC request
	CString sRequest;

	sRequest += _T("<?xml version=\"1.0\"?>\r\n");
	sRequest += _T("<methodCall>\r\n");
	sRequest += _T("\t<methodName>syndic8.GetFeedInfo</methodName>\r\n");
	sRequest += _T("\t<params>\r\n");

	// array
	sRequest += _T("\t\t<param><value><array><data>\r\n");
	// items

	POSITION pos = feedIDs.GetHeadPosition();
	while (pos != NULL) {
		int id = feedIDs.GetNext(pos);

		CString s;
		s.Format(_T("\t\t<value><int>%d</int></value>\r\n"), id);
		sRequest += s;
	}
	sRequest += _T("</data></array></value></param>\r\n");

	sRequest += _T("\t</params>\r\n");
	sRequest += _T("</methodCall>\r\n");

	return SendXMLRPCRequest(sRequest, buffer, len);
}

BOOL CSyndic8ComSearcher::ParseFindResponse(char *xmlResponse, int responseLen, CList<int, int> &feedIDs) {
	LOG0(5, "CSyndic8ComSearcher::ParseFindResponse()");

	BOOL ret = FALSE;

	CXmlFile xml;
	xml.LoadFromMemory(xmlResponse, responseLen);

	CXmlNode *rootNode = xml.GetRootNode();
	if (rootNode != NULL && rootNode->GetName().Compare(_T("methodResponse")) == 0) {
		// check <params> tag
		CXmlNode *params = NULL;
		if (IsChildNode(rootNode, params, _T("params"))) {
			// check <param> tag
			CXmlNode *param = NULL;
			if (IsChildNode(params, param, _T("param"))) {
				// check <value> tag
				CXmlNode *value = NULL;
				if (IsChildNode(param, value, _T("value"))) {
					CList<CXmlNode *, CXmlNode *> listValues;
					if (ParseArray(value, listValues)) {
						// iterate through list
						POSITION pos = listValues.GetHeadPosition();
						while (pos != NULL) {
							CXmlNode *node = listValues.GetNext(pos);

							int id = 0;
							if (swscanf(node->GetValue(), _T("%d"), &id) == 1)
								feedIDs.AddTail(id);

							PosOffset += 1000;
						}

						ret = TRUE;
					}
				}
			}
		}
	}

	return ret;
}

BOOL CSyndic8ComSearcher::ParseFeedInfoStruct(CXmlNode *structNode, CSearchResultItem *sritem) {
	LOG0(5, "CSyndic8ComSearcher::ParseFeedInfoStruct()");

	if (structNode == NULL || sritem == NULL)
		return FALSE;

	POSITION pos = structNode->GetFirstChildPos();
	while (pos != NULL) {
		// <member> tag
		CXmlNode *member = structNode->GetNextChild(pos);
		if (member != NULL && member->GetName().Compare(_T("member")) == 0) {
			CString strMemberName;
			CString strMemberValue;

			// iterate <member> child nodes (obtain <name> and <value> tag values)
			POSITION pos = member->GetFirstChildPos();
			while (pos != NULL) {
				CXmlNode *child = member->GetNextChild(pos);
				if (child != NULL) {
					CString tagName = child->GetName();
					if (tagName.Compare(_T("name")) == 0) {
						strMemberName = child->GetValue();
					}
					else if (tagName.Compare(_T("value")) == 0) {
						POSITION posV = child->GetFirstChildPos();
						if (posV != NULL) {
							CXmlNode *type = child->GetNextChild(posV);
							if (type != NULL) {
								strMemberValue = type->GetValue();
							}
						}
						else {
							strMemberValue = child->GetValue();
						}
					}
				}

				PosOffset += 1000;
			}

			// did we find all?
			if (!strMemberValue.IsEmpty()) {
				if (strMemberName.Compare(_T("sitename")) == 0) {
					sritem->SiteName = strMemberValue;
				}
				else if (strMemberName.Compare(_T("dataurl")) == 0) {
					sritem->XMLURL = strMemberValue;
				}
				else if (strMemberName.Compare(_T("description")) == 0) {
					sritem->Description = strMemberValue;
				}
			}
		}
	}

	// we need at least site name and feed url
	if (sritem->XMLURL.IsEmpty() || sritem->SiteName.IsEmpty())
		return FALSE;
	else
		return TRUE;
}

BOOL CSyndic8ComSearcher::ParseFeedInfoResponse(char *xmlResponse, int responseLen, CList<CSearchResultItem *, CSearchResultItem *> &searchResult) {
	LOG0(5, "CSyndic8ComSearcher::ParseFeedInfoResponse()");

	BOOL ret = FALSE;

	CXmlFile xml;
	xml.LoadFromMemory(xmlResponse, responseLen);

	CXmlNode *rootNode = xml.GetRootNode();
	if (rootNode != NULL && rootNode->GetName().Compare(_T("methodResponse")) == 0) {
		// check <params> tag
		CXmlNode *params = NULL;
		if (IsChildNode(rootNode, params, _T("params"))) {
			// check <param> tag
			CXmlNode *param = NULL;
			if (IsChildNode(params, param, _T("param"))) {
				// check <value> tag
				CXmlNode *value = NULL;
				if (IsChildNode(param, value, _T("value"))) {
					CList<CXmlNode *, CXmlNode *> listValues;
					if (ParseArray(value, listValues)) {
						// iterate through list
						POSITION pos = listValues.GetHeadPosition();
						while (pos != NULL) {
							CXmlNode *node = listValues.GetNext(pos);

							CSearchResultItem *srItem = new CSearchResultItem;
							if (ParseFeedInfoStruct(node, srItem)) {
								// ok, we got it!
								searchResult.AddTail(srItem);
							}
							else {
								delete srItem;
							}

							PosOffset += 1000;
						}

						ret = TRUE;
					}
				}
			}
		}
	}

	return ret;
}

BOOL CSyndic8ComSearcher::Search(int limit, CList<CSearchResultItem *, CSearchResultItem *> &searchResult) {
	LOG0(1, "CSyndic8ComSearcher::Search()");

	BOOL ret = FALSE;

	PosOffset = 0;

	// find feeds
	char *findFeedResponse = NULL;		// XML file (start with some amount)
	DWORD findFeedResponseLen = 0;

	if (FindFeeds(&findFeedResponse, &findFeedResponseLen, limit)) {
		// parse the result
		CList<int, int> feedIDs;
		if (ParseFindResponse(findFeedResponse, findFeedResponseLen, feedIDs)) {
			// get info about found feeds
			char *getFeedInfoResponse = NULL;			// XML file  (start with some amount)
			DWORD getFeedInfoResponseLen = 0;

			if (feedIDs.GetCount() > 0 && GetFeedInfo(&getFeedInfoResponse, &getFeedInfoResponseLen, feedIDs)) {
				// parse the result (and fill the 'searchResult' variable)
				if (ParseFeedInfoResponse(getFeedInfoResponse, getFeedInfoResponseLen, searchResult)) {
					ret = TRUE;
					State = SEARCH_STATE_CLOSED;
				}
				else {
					Error = ERROR_XML_RPC_RESPONSE;
				}
			}

			if (getFeedInfoResponse != NULL)
				delete [] getFeedInfoResponse;
		}
		else {
			Error = ERROR_XML_RPC_RESPONSE;
		}
	}

	if (findFeedResponse != NULL)
		delete [] findFeedResponse;

	return ret;
}

void CSyndic8ComSearcher::Terminate() {
	HttpConnection.Terminate();
}
