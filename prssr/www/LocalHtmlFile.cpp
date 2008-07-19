// LocalHtmlFile.cpp: implementation of the CLocalHtmlFile class.
//
//////////////////////////////////////////////////////////////////////

#ifdef PRSSR_APP
	#include "..\stdafx.h"
	#include "..\prssr.h"
#endif

#include "LocalHtmlFile.h"
#include "../../share/helpers.h"
#include "../Config.h"
#include "url.h"

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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalHtmlFile::CLocalHtmlFile() {
}

CLocalHtmlFile::~CLocalHtmlFile() {
}

///

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
			_stricmp(tag, "meta") == 0)
		{
			DOM_NODE *prev = domNodeGetPreviousSibling(child);
			domNodeDestroySpecific(child);
			if (prev == NULL)
				child = domNodeGetFirstChild(node);
			else
				child = domNodeGetNextSibling(prev);
		}
		else {
			Filter(child);
			child = domNodeGetNextSibling(child);
		}
	}
}

void CLocalHtmlFile::Filter() {
	LOG0(3, "CLocalHtmlFile::Filter()");

	Filter(GetDocumentNode());
}

void CLocalHtmlFile::TranslateForOffline(DOM_NODE *node) {
	LOG1(3, "CLocalHtmlFile::TranslateForOffline(%p)", node);

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
				list.AddTail(value);
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


