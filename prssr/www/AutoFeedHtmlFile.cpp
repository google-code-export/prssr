/**
 *  AutoFeedHtmlFile.cpp
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

#include "AutoFeedHtmlFile.h"
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
