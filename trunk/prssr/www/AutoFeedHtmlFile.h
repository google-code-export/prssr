/**
 *  AutoFeedHtmlFile.h
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
