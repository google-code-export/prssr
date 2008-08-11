/**
 *  FeedFile.h
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

#if !defined(AFX_FEEDFILE_H__557E1736_30F4_4A2B_9317_B4FF4A57DC74__INCLUDED_)
#define AFX_FEEDFILE_H__557E1736_30F4_4A2B_9317_B4FF4A57DC74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XMLFile.h"
#include "../Feed.h"
#include "../Site.h"

//class CFeedItem;

class CFeedFile : public CXmlFile {
public:
	BOOL IsValid();
	CFeedFile();
	virtual ~CFeedFile();

	BOOL Parse(CFeed *feed, CSiteItem *si);

protected:
	enum EType {
		RSS,
		RDF,
		Atom,
		Unknown
	};

	CString BaseUrl;

private:
	EType GetType();
	CString GetVersion();

	BOOL RSSFillInfo(CXmlNode *parent, CFeed *feed);
	BOOL RSSFillItem(CXmlNode *xmlItem, CFeedItem *item);

	BOOL RSSFill(CFeed *feed, CSiteItem *si);
	BOOL RDFFill(CFeed *feed, CSiteItem *si);
	BOOL AtomFill(CFeed *feed, CSiteItem *si);

#if defined PRSSR_APP
	BOOL RSSFillEnclosure(CXmlNode *xmlElem, CEnclosureItem *enclosure);
	BOOL AtomFillPerson(CXmlNode *xmlItem, CString &name);
#endif

	BOOL AtomFillInfo(CXmlNode *xmlElem, CFeed *feed);
	BOOL AtomFillItem(CXmlNode *xmlItem, CFeedItem *item);
};

#if defined PRSSR_APP
CString SanitizeHtml(const CString &html, const CString &baseUrl);
#endif

#endif // !defined(AFX_FEEDFILE_H__557E1736_30F4_4A2B_9317_B4FF4A57DC74__INCLUDED_)
