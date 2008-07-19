// AutoFeedHTMLFile.cpp: implementation of the CDiscoverFeedHtmlFile class.
//
//////////////////////////////////////////////////////////////////////

#ifdef PRSSR_APP
	#include "..\stdafx.h"
	#include "..\prssr.h"
#endif

#include "AutoFeedHTMLFile.h"
#include "../../share/helpers.h"

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

CDiscoverFeedHtmlFile::CDiscoverFeedHtmlFile() {
//	LinkTag = FALSE;
//	IsRss = FALSE;
}

CDiscoverFeedHtmlFile::~CDiscoverFeedHtmlFile() {
}

void CDiscoverFeedHtmlFile::GetRSS(DOM_NODE *node, CList<CHtmlFeed *, CHtmlFeed *> &feeds) {
	LOG0(5, "CDiscoverFeedHtmlFile::GetRSS()");

	//
	const char *tag = domNodeGetName(node);
	if (tag != NULL && node != NULL) {
		if (_stricmp(tag, "link") == 0) {
			const char *type = domElementGetAttribute(node, "type");
			const char *title = domElementGetAttribute(node, "title");
			const char *href = domElementGetAttribute(node, "href");

			if (type != NULL && 
				(_stricmp(type, "application/rss+xml") == 0 || _stricmp(type, "application/atom+xml") == 0) &&
				title != NULL && href != NULL)
			{
				CString sTitle = recode(title, &EncInfo);
				feeds.AddTail(new CHtmlFeed(sTitle, CharToWChar(href)));
			}
		}
		else if (_stricmp(tag, "body") == 0) {
			return;
		}
	}

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		GetRSS(child, feeds);
		child = domNodeGetNextSibling(child);
	}
}

void CDiscoverFeedHtmlFile::GetRSS(CList<CHtmlFeed *, CHtmlFeed *> &feeds) {
	LOG0(3, "CDiscoverFeedHtmlFile::GetRSS()");

	setEncoding(Charset, &EncInfo);
	GetRSS(GetDocumentNode(), feeds);
}
