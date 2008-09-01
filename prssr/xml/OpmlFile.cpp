/**
 *  OpmlFile.cpp
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
#endif

#include "OpmlFile.h"

#include "../Site.h"
#include "../Config.h"
#include "../../share/defs.h"

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

COpmlFile::COpmlFile() {
	RootNode = NULL;
}

COpmlFile::~COpmlFile() {
	delete RootNode;
}

BOOL COpmlFile::ParseOutline(CXmlNode *parent, CSiteItem *item) {
	LOG0(5, "COpmlFile::ParseOutline()");

	// go thru all child nodes
	POSITION pos = parent->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *outline = parent->GetNextChild(pos);
		// process only outline nodes
		if (outline->GetName().Compare(_T("outline")) == 0) {
			CString strTitle, strXMLurl, strFileName;
			CString strETag, strLastModified;
			CString strUserName, strPassword;
			BOOL bSpecificCaching = FALSE;
			BOOL bCacheItemImages = FALSE;
			BOOL bCacheHtml = FALSE;
			BOOL bTodayShow = FALSE;
			int nCacheLimit = CACHE_LIMIT_DEFAULT;
			int nUpdateInterval = UPDATE_INTERVAL_GLOBAL;
			BOOL bCacheEnclosures = FALSE;
			int nEnclosureLimit = 0;
			CSortInfo sort;

			BOOL bGroup = TRUE;

			// read attributes and fill variables
			POSITION posAttr = outline->GetFirstAttrPos();
			while (posAttr != NULL) {
				CXmlAttr *attr = outline->GetNextAttr(posAttr);

				CString name = attr->GetName();
				CString value = attr->GetValue();

				if (name.CompareNoCase(_T("title")) == 0 ||
					name.CompareNoCase(_T("text")) == 0)
				{
					strTitle = value;
				}
				else if (name.CompareNoCase(_T("filename")) == 0) {
					strFileName = value;
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("xmlurl")) == 0) {
					strXMLurl = value;
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("today")) == 0) {
					swscanf(value, _T("%d"), &bTodayShow);
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("type")) == 0 && value.CompareNoCase(_T("rss")) == 0) {
				}
#if defined PRSSR_APP
				else if (name.CompareNoCase(_T("cacheHTML")) == 0) {
					swscanf(value, _T("%d"), &bCacheHtml);
					bGroup = FALSE;
					if (Config.CacheHtml != bCacheHtml)
						bSpecificCaching = TRUE;
				}
				else if (name.CompareNoCase(_T("store_images")) == 0 ||
						name.CompareNoCase(_T("cacheImages")) == 0) {
					swscanf(value, _T("%d"), &bCacheItemImages);
					bGroup = FALSE;
					if (Config.CacheImages != bCacheItemImages)
						bSpecificCaching = TRUE;
				}
				else if (name.CompareNoCase(_T("occlvl")) == 0) {
					// compatibility with older versions
					int level = 0;
					swscanf(value, _T("%d"), &level);
					bCacheHtml = level > 0;
					bGroup = FALSE;
					if (Config.CacheHtml != bCacheHtml)
						bSpecificCaching = TRUE;
				}
				else if (name.CompareNoCase(_T("updateInterval")) == 0) {
					int ui = 0;
					swscanf(value, _T("%d"), &ui);
					nUpdateInterval = UpdateIntervalToMinutes(ui);
				}
				else if (name.CompareNoCase(_T("updateMins")) == 0) {
					swscanf(value, _T("%d"), &nUpdateInterval);
				}
				else if (name.CompareNoCase(_T("cacheEnclosures")) == 0) {
					swscanf(value, _T("%d"), &bCacheEnclosures);
				}
				else if (name.CompareNoCase(_T("enclosureLimit")) == 0) {
					swscanf(value, _T("%d"), &nEnclosureLimit);
				}
				else if (name.CompareNoCase(_T("cacheLimit")) == 0) {
					swscanf(value, _T("%d"), &nCacheLimit);
				}
				else if (name.CompareNoCase(_T("etag")) == 0) {
					strETag = value;
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("lastmodified")) == 0) {
					strLastModified = value;
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("userName")) == 0) {
					strUserName = value;
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("password")) == 0) {
					strPassword = value;
					bGroup = FALSE;
				}
				else if (name.CompareNoCase(_T("sort")) == 0) {
					if (value.CompareNoCase(_T("read")) == 0)
						sort.Item = CSortInfo::Read;
					else
						sort.Item = CSortInfo::Date;
				}
				else if (name.CompareNoCase(_T("sortReversed")) == 0 && value.CompareNoCase(_T("false")) == 0) {
					sort.Type = CSortInfo::Ascending;
				}
#endif
			}

			// process read info
			if (bGroup) {
				// Bloglines groups everything under root group named 'Subscriptions' (we need only childs)
				if (item->GetCount() == 0 && strTitle.CompareNoCase(_T("subscriptions")) == 0) {
					ParseOutline(outline, item);
				}
				else {
					CSiteItem *subGroup = new CSiteItem(item, CSiteItem::Group);
					subGroup->Name = strTitle;
					ParseOutline(outline, subGroup);
					item->AddItem(subGroup);
				}
			}
			else {
				if (strTitle.GetLength() > 0 &&
					strXMLurl.GetLength() > 0) {

#ifdef PRSSR_APP
					if (strFileName.GetLength() == 0) {
						strFileName = CFeedInfo::GenerateFileNameFromTitle(strTitle);
						CFeedInfo::EnsureUniqueFileName(strFileName);
					}
#endif

					CFeedInfo *info = new CFeedInfo();
					info->FileName = strFileName;
					info->TodayShow = bTodayShow;
#if defined PRSSR_APP
					info->XmlUrl = strXMLurl;

					if (bSpecificCaching)
						info->UseGlobalCacheOptions = FALSE;
					else
						info->UseGlobalCacheOptions = TRUE;

					info->CacheItemImages = bCacheItemImages;
					info->CacheHtml = bCacheHtml;
					info->UpdateInterval = nUpdateInterval;
					info->CacheEnclosures = bCacheEnclosures;
					info->EnclosureLimit = nEnclosureLimit;
					info->CacheLimit = nCacheLimit;

					info->ETag = strETag;
					info->LastModified = strLastModified;

					info->UserName = strUserName;
					info->Password = strPassword;

					item->Sort = sort;
#endif
					CSiteItem *newSite = new CSiteItem(item, CSiteItem::Site);
					newSite->Name = strTitle;
					newSite->Info = info;
					// TODO: last update time

					item->AddItem(newSite);
				}
			}
		}
	}

	return TRUE;
}

BOOL COpmlFile::Parse(CSiteItem *item) {
	LOG0(5, "COpmlFile::Parse()");

	BOOL ret = FALSE;

	if (RootNode->GetName().Compare(_T("opml")) == 0) {
		POSITION pos = RootNode->GetFirstChildPos();
		while (pos != NULL) {
			CXmlNode *child = RootNode->GetNextChild(pos);
			if (child->GetName().Compare(_T("body")) == 0) {
				ret = ParseOutline(child, item);
			}
		}
	}

	return ret;
}


#ifdef PRSSR_APP

BOOL COpmlFile::SaveSite(CXmlNode *parent, CSiteItem *item) {
	LOG0(5, "COpmlFile::SaveSite()");

	CFeedInfo *info = item->Info;

	// build up the node
	CList<CXmlAttr *, CXmlAttr *> siteAttrs;
	siteAttrs.AddTail(new CXmlAttr(_T("title"), item->Name));
	siteAttrs.AddTail(new CXmlAttr(_T("xmlUrl"), info->XmlUrl));
	siteAttrs.AddTail(new CXmlAttr(_T("type"), _T("rss")));
	siteAttrs.AddTail(new CXmlAttr(_T("filename"), info->FileName));

	if (info->CacheLimit != CACHE_LIMIT_DEFAULT)
		siteAttrs.AddTail(new CXmlAttr(_T("cacheLimit"), info->CacheLimit));

	if (!info->UseGlobalCacheOptions) {
		siteAttrs.AddTail(new CXmlAttr(_T("cacheImages"), info->CacheItemImages));
		siteAttrs.AddTail(new CXmlAttr(_T("cacheHTML"), info->CacheHtml));
	}

	if (info->TodayShow)
		siteAttrs.AddTail(new CXmlAttr(_T("today"), 1));

	TCHAR buf[32];
	swprintf(buf, _T("%d"), info->UpdateInterval);
	siteAttrs.AddTail(new CXmlAttr(_T("updateMins"), buf));

	if (info->CacheEnclosures)
		siteAttrs.AddTail(new CXmlAttr(_T("cacheEnclosures"), 1));

	if (info->CacheEnclosures) {
		TCHAR buf[32];
		swprintf(buf, _T("%d"), info->EnclosureLimit);
		siteAttrs.AddTail(new CXmlAttr(_T("enclosureLimit"), buf));
	}

	// HTTP headers
	siteAttrs.AddTail(new CXmlAttr(_T("lastModified"), info->LastModified));
	siteAttrs.AddTail(new CXmlAttr(_T("ETag"), info->ETag));

	// auth info
	if (!info->UserName.IsEmpty()) {
		siteAttrs.AddTail(new CXmlAttr(_T("userName"), info->UserName));
		siteAttrs.AddTail(new CXmlAttr(_T("password"), info->Password));
	}

	// sort
	switch (item->Sort.Item) {
		case CSortInfo::Date: siteAttrs.AddTail(new CXmlAttr(_T("sort"), _T("date"))); break;
		case CSortInfo::Read: siteAttrs.AddTail(new CXmlAttr(_T("sort"), _T("read"))); break;
	}

	if (item->Sort.Type == CSortInfo::Ascending) {
		siteAttrs.AddTail(new CXmlAttr(_T("sortReversed"), _T("false")));
	}

	// <outline>
	CXmlNode *outline = new CXmlNode(CXmlNode::Tag, parent, _T("outline"), siteAttrs);

	// add the <outline> to parent
	parent->AddChild(outline);

	return TRUE;
}

BOOL COpmlFile::SaveGroup(CXmlNode *parent, CSiteItem *item) {
	LOG0(5, "COpmlFile::SaveGroup()");

	POSITION pos = item->SubItems.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *si = item->SubItems.GetNext(pos);

		if (si->Type == CSiteItem::Site)
			SaveSite(parent, si);
		else {
			CList<CXmlAttr *, CXmlAttr *> groupAttrs;
			groupAttrs.AddTail(new CXmlAttr(_T("title"), si->Name));
			CXmlNode *outline = new CXmlNode(CXmlNode::Tag, parent, _T("outline"), groupAttrs);
			parent->AddChild(outline);

			SaveGroup(outline, si);
		}
	}

	return TRUE;
}

BOOL COpmlFile::Save(LPCTSTR fileName, CSiteList &siteList) {
	LOG1(5, "COpmlFile::Save('%S')", fileName);

	delete RootNode;
	RootNode = NULL;

	//
	// build the structure
	//

	// root node
	CList<CXmlAttr *, CXmlAttr *> opmlAttrs;
	opmlAttrs.AddTail(new CXmlAttr(_T("version"), _T("1.0")));
	RootNode = new CXmlNode(CXmlNode::Tag, NULL, _T("opml"), opmlAttrs);

	// body
	CXmlNode *body = new CXmlNode(CXmlNode::Tag, RootNode, _T("body"));
	RootNode->AddChild(body);

	if (!SaveGroup(body, siteList.GetRoot()))
		return FALSE;

	if (!CXmlFile::Save(fileName))
		return FALSE;

	return TRUE;
}

BOOL COpmlFile::Export(LPCTSTR fileName, CSiteList &siteList) {
	LOG1(5, "COpmlFile::Export('%S')", fileName);

	return Save(fileName, siteList);
}

#endif

