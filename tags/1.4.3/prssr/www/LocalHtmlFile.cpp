/**
 *  LocalHtmlFile.cpp
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

#ifdef PRSSR_APP
	#include "../StdAfx.h"
	#include "../prssr.h"
#endif

#include "LocalHtmlFile.h"
#include "../../share/helpers.h"
#include "../Config.h"
#include "url.h"

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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalHtmlFile::CLocalHtmlFile() {
}

CLocalHtmlFile::CLocalHtmlFile(const CString &baseUrl) {
	DWORD serviceType;
	INTERNET_PORT port;
	ParseURL(baseUrl, serviceType, Server, ActivePath, port);
}

CLocalHtmlFile::~CLocalHtmlFile() {
}

///

void CLocalHtmlFile::FixTree(DOM_NODE *node) {
	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		const char *tag = domNodeGetName(child);		
		if (_strnicmp(tag, "!]]", 3) == 0) {
			// move all childs one level up in the tree
			DOM_NODE *next = domNodeGetNextSibling(node);

			for (DOM_NODE *curr = child->firstChild; curr; curr = curr->nextSibling)
				curr->parent = node->parent;

			DOM_NODE *firstChild = child->firstChild;
			if (firstChild != NULL) {
				child->firstChild->prevSibling = node;
				child->lastChild->nextSibling = node->nextSibling;

				if (node->nextSibling != NULL)
					node->nextSibling->prevSibling = child->lastChild;
				node->nextSibling = child->firstChild;

				child->firstChild = NULL;
			}
			else {
				// no nothing
			}
			child = domNodeGetNextSibling(child);
		}
		else {
			FixTree(child);
			child = domNodeGetNextSibling(child);
		}
	}
}

void CLocalHtmlFile::Filter(DOM_NODE *node) {
	LOG1(5, "CLocalHtmlFile::Filter(%p)", node);

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		const char *tag = domNodeGetName(child);
		if (_stricmp(tag, "script") == 0 ||
			_stricmp(tag, "noscript") == 0 ||
			_stricmp(tag, "style") == 0 ||
			_stricmp(tag, "link") == 0 ||
			_stricmp(tag, "iframe") == 0 ||
			_stricmp(tag, "object") == 0 ||
			_stricmp(tag, "param") == 0 ||
			_stricmp(tag, "embed") == 0 ||
			_stricmp(tag, "meta") == 0)
		{
			DOM_NODE *prev = domNodeGetPreviousSibling(child);
			domNodeDestroySpecific(child);
			if (prev == NULL)
				child = domNodeGetFirstChild(node);
			else
				child = domNodeGetNextSibling(prev);
		}
		else if (_strnicmp(tag, "![", 2) == 0) {
			DOM_NODE *prev = domNodeGetPreviousSibling(child);

			for (DOM_NODE *curr = child->firstChild; curr; curr = curr->nextSibling)
				curr->parent = node;

			DOM_NODE *firstChild = child->firstChild;
			if (firstChild != NULL) {
				child->firstChild->prevSibling = prev;
				child->lastChild->nextSibling = child->nextSibling;
				if (child->nextSibling != NULL)
					child->nextSibling->prevSibling = child->lastChild;
				else
					child->lastChild->nextSibling = NULL;

				if (prev == NULL)
					node->firstChild = child->firstChild;
				else
					prev->nextSibling = child->firstChild;


			}
			else {
				firstChild = child->nextSibling;

				if (prev == NULL)
					node->firstChild = child->nextSibling;
				else
					prev->nextSibling = child->nextSibling;

				if (child->nextSibling != NULL)
					child->nextSibling->prevSibling = prev;
				else
					node->firstChild = NULL;
			}

			domNodeDestroy(child->attributes);
			if (child->name) free(child->name);
			if (child->value) free(child->value);
			free(child);

			child = firstChild;
		}
		else {
			Filter(child);
			child = domNodeGetNextSibling(child);
		}
	}
}

void CLocalHtmlFile::Filter() {
	LOG0(3, "CLocalHtmlFile::Filter()");

	FixTree(GetDocumentNode());
	Filter(GetDocumentNode());
}

void CLocalHtmlFile::RewriteAttr(DOM_NODE *node, char *attr) {
	const char *attrVal = domElementGetAttribute(node, attr);
	if (attrVal != NULL) {
		CString value = CharToWChar(attrVal);
		CString url = MakeAbsoluteUrl(value, Server, ActivePath);
		char *s = WCharToChar(url);
		domElementSetAttribute(node, attr, s);
		delete [] s;
	}
}

void CLocalHtmlFile::RewriteRelativeUrls(DOM_NODE *node) {
	LOG1(5, "CLocalHtmlFile::RewriteRelativeUrls(%p)", node);

	const char *tag = domNodeGetName(node);
	if (tag != NULL && node != NULL) {
		if (_stricmp(tag, "img") == 0)
			RewriteAttr(node, "src");
		else if (_stricmp(tag, "a") == 0)
			RewriteAttr(node, "href");
	}

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		RewriteRelativeUrls(child);
		child = domNodeGetNextSibling(child);
	}
}

void CLocalHtmlFile::RewriteRelativeUrls() {
	LOG0(3, "CLocalHtmlFile::RewriteRelativeUrls()");

	RewriteRelativeUrls(GetDocumentNode());
}

void CLocalHtmlFile::TranslateForOffline(DOM_NODE *node) {
	LOG1(5, "CLocalHtmlFile::TranslateForOffline(%p)", node);

	//
	const char *tag = domNodeGetName(node);
	if (tag != NULL && node != NULL) {
		if (_stricmp(tag, "img") == 0) {
//			LOG1(1, "img: %s", domElementGetAttribute(node, "src"));
			const char *src = domElementGetAttribute(node, "src");
			if (src != NULL) {
				CString value = CharToWChar(src);

				CString url = MakeAbsoluteUrl(value, Server, ActivePath);
				if (IsHTMLCached(url, FALSE)) {
					CString fileName = UrlToFileName(url, FALSE);
					CString path = GetCachePath(FILE_TYPE_HTML, Config.CacheLocation);
					CString strFileName = MakeOfflineUrl(path, fileName);

					char *s = WCharToChar(strFileName);
					domElementSetAttribute(node, "src", s);
					delete [] s;
				}
				else
					domElementSetAttribute(node, "src", "");
			}
		}
	}

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		TranslateForOffline(child);
		child = domNodeGetNextSibling(child);
	}
}

void CLocalHtmlFile::TranslateForOffline() {
	LOG0(3, "CLocalHtmlFile::TranslateForOffline()");

	TranslateForOffline(GetDocumentNode());
}


void CLocalHtmlFile::ExtractImages(DOM_NODE *node, CStringList &list) {
	LOG1(5, "CLocalHtmlFile::ExtractImages(%p)", node);

	//
	const char *tag = domNodeGetName(node);
	if (tag != NULL && node != NULL) {
		if (_stricmp(tag, "img") == 0) {
			const char *src = domElementGetAttribute(node, "src");
//			LOG1(1, "img: %s", src);
			if (src != NULL) {
				CString value = CharToWChar(src);
				CString url = MakeAbsoluteUrl(value, Server, ActivePath);
				list.AddTail(url);
			}
		}
	}

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		ExtractImages(child, list);
		child = domNodeGetNextSibling(child);
	}
}

void CLocalHtmlFile::ExtractImages(CStringList &list) {
	LOG0(3, "CLocalHtmlFile::ExtractImages()");

	ExtractImages(GetDocumentNode(), list);
}

