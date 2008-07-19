// AutoFeedHTMLFile.h: interface for the CHtmlFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DISCOVER_FEED_HTMLFILE_H__INCLUDED_)
#define _DISCOVER_FEED_HTMLFILE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HTMLFile.h"

/////////////////////////////////////////////////////////////////////////////

// Feed found in HTML file

struct CHtmlFeed {
	CString Url;
	CString Name;

	CHtmlFeed(const CString &name, const CString &url) {
		Name = name;
		Url = url;
	}
};


class CDiscoverFeedHtmlFile : public CHtmlFile {
public:
	CDiscoverFeedHtmlFile();
	virtual ~CDiscoverFeedHtmlFile();

	void GetRSS(CList<CHtmlFeed *, CHtmlFeed *> &feeds);

protected:
	void GetRSS(DOM_NODE *node, CList<CHtmlFeed *, CHtmlFeed *> &feeds);
};

#endif // !defined(_DISCOVER_FEED_HTMLFILE_H__INCLUDED_)
