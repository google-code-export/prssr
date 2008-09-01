/**
 *  OpmlFile.h
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

#if !defined(AFX_OPMLFILE_H__95F58019_D218_445D_BD53_4BA646B8FB8A__INCLUDED_)
#define AFX_OPMLFILE_H__95F58019_D218_445D_BD53_4BA646B8FB8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XMLFile.h"

class CSiteList;
class CSiteItem;
struct CRewriteRule;

class COpmlFile : public CXmlFile {
public:
	COpmlFile();
	virtual ~COpmlFile();

	BOOL Parse(CSiteItem *item);

#ifdef PRSSR_APP
	BOOL Export(LPCTSTR fileName, CSiteList &siteList);
	virtual BOOL Save(LPCTSTR fileName, CSiteList &siteList);
#endif

protected:
	BOOL ParseOutline(CXmlNode *parent, CSiteItem *item);

#ifdef PRSSR_APP
	BOOL SaveGroup(CXmlNode *parent, CSiteItem *item);
	BOOL SaveSite(CXmlNode *parent, CSiteItem *item);
#endif
};

#endif // !defined(AFX_OPMLFILE_H__95F58019_D218_445D_BD53_4BA646B8FB8A__INCLUDED_)
