/**
 *  Site.cpp
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
	#include "StdAfx.h"
	#include "prssr.h"
	#include "misc.h"
#endif

#if defined PRSSR_TODAY
	#include "../prssrtoday/StdAfx.h"
#endif

#include "Site.h"

#include "../share/reg.h"
#include "../share/defs.h"
#include "../share/fs.h"
#include "misc.h"

#include "xml/FeedFile.h"
#include "xml/OpmlFile.h"

#ifdef PRSSR_APP
	#include "Config.h"
#elif defined PRSSR_TODAY
	#include "../prssrtoday/Config.h"
#endif

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// registry
static LPCTSTR szName = _T("Name");
static LPCTSTR szIdx = _T("Idx");

static LPCTSTR szFileName = _T("File Name");
static LPCTSTR szXmlUrl = _T("XML URL");
static LPCTSTR szUseGlobalCacheOptions = _T("Use Global Cache Options");
static LPCTSTR szCacheItemImages = _T("Cache Item Images");
static LPCTSTR szCacheHtml = _T("Cache Html");
static LPCTSTR szCacheLimit = _T("Cache Limit");
static LPCTSTR szUpdateInterval = _T("Update Interval");
static LPCTSTR szCacheEnclosures = _T("Cache Enclosures");
static LPCTSTR szEnclosureLimit = _T("Enclosure Limit");
static LPCTSTR szETag = _T("ETag");
static LPCTSTR szLastModified = _T("LastModified");
static LPCTSTR szUserName = _T("Username");
static LPCTSTR szPassword = _T("Password");
static LPCTSTR szSort = _T("Sort");
static LPCTSTR szSortReversed = _T("SortReversed");

static LPCTSTR szUnreadCount = _T("Unread Count");
static LPCTSTR szFlaggedCount = _T("Flagged Count");
static LPCTSTR szCheckFavIcon = _T("Check FavIcon");

//#ifdef PRSSR_APP

CSiteList SiteList;
CSiteItem UnreadItems(NULL, CSiteItem::VFolder);
CSiteItem FlaggedItems(NULL, CSiteItem::VFolder);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedInfo::CFeedInfo() {
	TodayShow = TRUE;
#if defined PRSSR_APP
	UseGlobalCacheOptions = TRUE;
	CacheItemImages = FALSE;
	CacheHtml = FALSE;
	CacheLimit = CACHE_LIMIT_DEFAULT;
	UpdateInterval = UPDATE_INTERVAL_GLOBAL;
	CacheEnclosures = FALSE;
	EnclosureLimit = 0;
#endif
}

CFeedInfo &CFeedInfo::operator=(const CFeedInfo &o) {
	if (this != &o) {
		FileName = o.FileName;
		XmlUrl = o.XmlUrl;
		TodayShow = o.TodayShow;

#if defined PRSSR_APP
		UseGlobalCacheOptions = o.UseGlobalCacheOptions;
		CacheItemImages = o.CacheItemImages;
		CacheHtml = o.CacheHtml;
		CacheLimit = o.CacheLimit;
		UpdateInterval = o.UpdateInterval;
		CacheEnclosures = o.CacheEnclosures;
		EnclosureLimit = o.EnclosureLimit;
		ETag = o.ETag;
		LastModified = o.LastModified;
		UserName = o.UserName;
		Password = o.Password;
#endif
	}

	return *this;
}

CFeedInfo::~CFeedInfo() {
}

#ifdef PRSSR_APP

CString CFeedInfo::GenerateFileName(const CString &url) {
	return GetSha1Hash(url);
}

#endif

// CSiteItem //////////////////////////////////////////////////////////

CSiteItem::CSiteItem(CSiteItem *parent, eType type) {
	LOG1(5, "CSiteItem::CSiteItem(%d)", type);

	Parent = parent;
	Type = type;
	Info = NULL;
	Feed = NULL;
#ifdef PRSSR_APP
	Sort.Item = CSortInfo::Date;
	Sort.Type = CSortInfo::Descending;
#endif

	if (Type == Site)
		Status = Empty;
	else
		Status = Ok;

	ImageIdx = -1;
	CheckFavIcon = TRUE;

	Modified = FALSE;

	UnreadItems = 0;
	FlaggedItems = 0;
	CheckFavIcon = TRUE;

	InitializeCriticalSection(&CSLoadFeed);

#ifdef PRSSR_TODAY
	memset(&LastUpdate, 0, sizeof(FILETIME));
#endif
}


CSiteItem::CSiteItem(CSiteItem *parent, CSiteItem *siteItem) {
	LOG1(5, "CSiteItem::CSiteItem(%p)", siteItem);

	Parent = parent;
	Type = siteItem->Type;
	Name = siteItem->Name;
	ImageIdx = siteItem->ImageIdx;
	CheckFavIcon = siteItem->CheckFavIcon;
	Modified = siteItem->Modified;
#ifdef PRSSR_APP
	Sort = siteItem->Sort;
	Info = NULL;
#endif

	if (siteItem->Type == Site) {
		Status = Empty;
		Info = new CFeedInfo();
		*Info = *(siteItem->Info);
		Feed = NULL;

		UnreadItems = siteItem->UnreadItems;
		FlaggedItems = siteItem->FlaggedItems;
	}
	else if (siteItem->Type == VFolder) {
		Status = Empty;
		Feed = NULL;
		UnreadItems = 0;
		FlaggedItems = 0;
	}
	else {
		Status = Ok;
	}

	InitializeCriticalSection(&CSLoadFeed);

#ifdef PRSSR_TODAY
	LastUpdate = siteItem->LastUpdate;
#endif
}

CSiteItem::~CSiteItem() {
	LOG1(1, "CSiteItem::~CSiteItem(%S)", Name);

	DeleteCriticalSection(&CSLoadFeed);
}

void CSiteItem::Destroy() {
	LOG1(5, "CSiteItem::Destroy(%S)", Name);

	switch (Type) {
		case Site:
			delete Info; Info = NULL;
			if (Feed != NULL) Feed->Destroy();
			delete Feed; Feed = NULL;
			break;

		case VFolder:
			delete Info; Info = NULL;
			// we do not destroy feed, since vfolder is an array of links to already existing items that are deallocated elsewhere
			delete Feed; Feed = NULL;
			break;

		case Group:
			while (!SubItems.IsEmpty()) {
				CSiteItem *item = SubItems.RemoveHead();
				item->Destroy();
				delete item;
			}
			break;
	}
}

void CSiteItem::EnsureSiteLoaded() {
	LOG0(5, "CSiteItem::EnsureSiteLoaded()");

	if (Type == Site) {
		// if not loaded ->load
		EnterCriticalSection(&CSLoadFeed);
		if (Status == Empty) {
			CFeed *feed = new CFeed();

			CString pathName;
#if defined PRSSR_APP
			pathName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, Info->FileName);
#elif defined PRSSR_TODAY
			pathName.Format(_T("%s\\feeds\\%s"), Config.CacheLocation, Info->FileName);
#endif
			if (feed->Load(pathName, this)) {
				Status = Ok;

#ifdef PRSSR_APP
				feed->UpdateHiddenFlags();
//				feed->SetKeywordFlags(SiteList.GetKeywords());
#endif

				if (Feed != NULL) Feed->Destroy();
				delete Feed;
				Feed = feed;

				UpdateCachedCounts();
			}
			else {
				Status = Error;
#if defined PRSSR_APP
				// discard ETag and LastModified value (to allow update)
				Info->ETag.Empty();
				Info->LastModified.Empty();
#endif
				UpdateCachedCounts();

				if (feed != NULL) feed->Destroy();
				delete feed;
			}
		}
		LeaveCriticalSection(&CSLoadFeed);
	}
}

int CSiteItem::GetUnreadCount() const {
	LOG0(5, "CSiteItem::GetUnreadCount()");

	if (Type == Site) {
		if (Status == Ok && Feed != NULL)
			return Feed->GetUnreadCount() + Feed->GetNewCount();
		else
			return UnreadItems;
	}
	else if (Type == Group) {
		int unreadCount = 0;
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			unreadCount += si->GetUnreadCount();
		}

		return unreadCount;
	}
	else {
		return 0;
	}
}

int CSiteItem::GetFlaggedCount() const {
	LOG0(5, "CSiteItem::GetFlaggedCount()");

	if (Type == Site) {
		if (Status == Ok && Feed != NULL)
			return Feed->GetFlaggedCount();
		else
			return FlaggedItems;
	}
	else if (Type == Group) {
		int flaggedCount = 0;
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			flaggedCount += si->GetFlaggedCount();
		}

		return flaggedCount;
	}
	else {
		return 0;
	}
}

void CSiteItem::UpdateCachedCounts() {
	LOG0(5, "CSiteItem::UpdateCachedCounts()");

	if (Type == Site) {
		if (Feed != NULL)
			UnreadItems = Feed->GetNewCount() + Feed->GetUnreadCount();
		else
			UnreadItems = 0;
	}
	else if (Type == Group) {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			si->UpdateCachedCounts();
		}
	}
}

void CSiteItem::GetSites(CList<CSiteItem *, CSiteItem *> &sites) {
	if (Type == Site)
		sites.AddTail(this);
	else if (Type == Group) {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			sites.AddTail(si);
		}
	}
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiteList::CSiteList() {
	Root = NULL;
}

CSiteList::~CSiteList() {
}

void CSiteList::Detach() {
	Root = NULL;
	Data.RemoveAll();
}

void CSiteList::Destroy() {
	LOG0(5, "CSiteList::Destroy()");

	if (Root != NULL) {
		Root->Destroy();
		delete Root; Root = NULL;
	}

	Data.RemoveAll();
}

/*
void CSiteList::SetKeywords(CStringArray &kws) {
	Keywords.RemoveAll();

	for (int i = 0; i < kws.GetSize(); i++)
		Keywords.Add(kws.GetAt(i));
}
*/

void CSiteList::CreateFrom(CSiteItem *root) {
	if (root == NULL)
		return;

	if (root->Type == CSiteItem::Site) {
		Data.Add(root);
	}
	else if (root->Type == CSiteItem::Group) {
		POSITION pos = root->SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = root->SubItems.GetNext(pos);
			CreateFrom(si);
		}
	}
}

int CSiteList::GetIndexOf(CSiteItem *item) {
	for (int i = 0; i < Data.GetSize(); i++)
		if (Data[i] == item)
			return i;
	return SITE_INVALID;
}

int CSiteList::GetCount() {
	return Data.GetSize();
}

CSiteItem *CSiteList::GetAt(int i) {
	if (i >= 0 && i < Data.GetSize())
		return Data.GetAt(i);
	else
		return NULL;
}

void CSiteList::SetRoot(CSiteItem *root) {
	Root = root;
}

// Save ///////

#if defined PRSSR_APP

BOOL SaveSiteItem(CSiteItem *item, int idx) {
	LOG2(5, "SaveSiteItem(%p, %d)", item, idx);

	if (item == NULL) return TRUE;

	CString sRegPath;
	sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx + 1);

	CRegistry::DeleteKey(HKEY_CURRENT_USER, sRegPath);
	CRegistry reg(HKEY_CURRENT_USER, sRegPath);

	reg.Write(szName, item->Name);
	if (item->Type == CSiteItem::Site && item->Info != NULL) {
		reg.Write(szFileName, item->Info->FileName);
		reg.Write(szXmlUrl, item->Info->XmlUrl);
		reg.Write(szTodayShow, item->Info->TodayShow);
		reg.Write(szUseGlobalCacheOptions, item->Info->UseGlobalCacheOptions);
		reg.Write(szCacheItemImages, item->Info->CacheItemImages);
		reg.Write(szCacheHtml, item->Info->CacheHtml);
		reg.Write(szCacheLimit, item->Info->CacheLimit);
		reg.Write(szUpdateInterval, item->Info->UpdateInterval);
		reg.Write(szCacheEnclosures, item->Info->CacheEnclosures);
		reg.Write(szEnclosureLimit, item->Info->EnclosureLimit);
		reg.Write(szETag, item->Info->ETag);
		reg.Write(szLastModified, item->Info->LastModified);
		reg.Write(szUserName, item->Info->UserName);
		reg.Write(szPassword, item->Info->Password);
	}

	reg.Write(szCheckFavIcon, item->CheckFavIcon);

	switch (item->Sort.Item) {
		case CSortInfo::Date: reg.Write(szSort, _T("date"));break;
		case CSortInfo::Read: reg.Write(szSort, _T("read"));break;
		case CSortInfo::Keyword: reg.Write(szSort, _T("keyword"));break;
	}

	if (item->Sort.Type == CSortInfo::Ascending)
		reg.Write(szSortReversed, FALSE);
	else
		reg.Write(szSortReversed, TRUE);

	return TRUE;
}

BOOL SaveSiteItemUnreadCount(CSiteItem *item, int idx) {
	LOG2(5, "SaveSiteItemUnreadCount(%p, %d)", item, idx);

	if (item == NULL)
		return TRUE;

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx + 1);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		reg.Write(szUnreadCount, item->GetUnreadCount());
	}

	return TRUE;
}

BOOL SaveSiteItemFlaggedCount(CSiteItem *item, int idx) {
	LOG2(5, "SaveSiteItemFlaggedCount(%p, %d)", item, idx);

	if (item == NULL)
		return TRUE;

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx + 1);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		reg.Write(szFlaggedCount, item->GetFlaggedCount());
	}

	return TRUE;
}

BOOL SaveSiteGroup(CSiteItem *item, CRegistry &reg) {
	LOG1(5, "SaveSiteGroup(%p)", item);

	// save sub items
	int k = 1;

	POSITION pos = item->SubItems.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *si = item->SubItems.GetNext(pos);

		CString sNum;
		sNum.Format(_T("%d"), k);
		CRegistry regItem(reg, sNum);
		SaveSiteGroup(si, regItem);

		k++;
	}

	// save item itself
	if (item->Type == CSiteItem::Site) {
		reg.Write(szIdx, SiteList.GetIndexOf(item));
	}
	else if (item->Type == CSiteItem::Group) {
		reg.Write(szName, item->Name);
	}

	return TRUE;
}

BOOL SaveSiteList() {
	LOG0(3, "SaveSiteList()");

	// sites
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_SUBSCRIPTIONS);
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_SUBSCRIPTIONS);
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *item = SiteList.GetAt(i);
		SaveSiteItem(item, i);
		SaveSiteItemUnreadCount(item, i);
		SaveSiteItemFlaggedCount(item, i);
	}

	// save hierarchy
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_GROUPS);
	CRegistry regGroups(HKEY_CURRENT_USER, REG_KEY_GROUPS);
	SaveSiteGroup(SiteList.GetRoot(), regGroups);

	return TRUE;
}


#endif

// Load //////////

BOOL LoadSiteGroup(CSiteItem *item, CRegistry &reg, CArray<CSiteItem*, CSiteItem*> &sites) {
	LOG1(5, "LoadSiteGroup(%p, ,)", item);

	// load sub items
	DWORD cSubKeys;
	reg.QuerySubKeyNumber(&cSubKeys);
	for (DWORD k = 1; k <= cSubKeys; k++) {
		CString sNum;
		sNum.Format(_T("%d"), k);
		CRegistry regItem(reg, sNum);

		CString name = regItem.Read(szName, _T(""));
		int idx = regItem.Read(szIdx, -1);

		CSiteItem *si = NULL;
		if (idx == -1) {
			si = new CSiteItem(item, CSiteItem::Group);
			si->Name = name;
			item->AddItem(si);

			LoadSiteGroup(si, regItem, sites);
		}
		else {
			sites[idx]->Parent = item;
			item->AddItem(sites[idx]);
		}
	}

	return TRUE;
}


BOOL LoadSiteItem(CSiteItem *item, int idx) {
	LOG2(5, "LoadSiteItem(%p, %d)", item, idx);

	CString sRegPath;
	sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx);

	CRegistry reg(HKEY_CURRENT_USER, sRegPath);

	item->Name = reg.Read(szName, _T(""));
	if (item->Type == CSiteItem::Site) {
		CFeedInfo *info = new CFeedInfo();

		info->FileName = reg.Read(szFileName, _T(""));
		info->XmlUrl = reg.Read(szXmlUrl, _T(""));
		info->TodayShow = reg.Read(szTodayShow, TRUE);
#ifdef PRSSR_APP
		info->UseGlobalCacheOptions = reg.Read(szUseGlobalCacheOptions, TRUE);
		info->CacheItemImages = reg.Read(szCacheItemImages, FALSE);
		info->CacheHtml = reg.Read(szCacheHtml, FALSE);
		info->CacheLimit = reg.Read(szCacheLimit, CACHE_LIMIT_DEFAULT);
		info->UpdateInterval = reg.Read(szUpdateInterval, UPDATE_INTERVAL_GLOBAL);
		info->CacheEnclosures = reg.Read(szCacheEnclosures, FALSE);
		info->EnclosureLimit = reg.Read(szEnclosureLimit, 0);
		info->ETag = reg.Read(szETag, _T(""));
		info->LastModified = reg.Read(szLastModified, _T(""));
		info->UserName = reg.Read(szUserName, _T(""));
		info->Password = reg.Read(szPassword, _T(""));

		CString sortItem = reg.Read(szSort, _T(""));
		if (sortItem.CompareNoCase(_T("read")) == 0)
			item->Sort.Item = CSortInfo::Read;
		else
			item->Sort.Item = CSortInfo::Date;

		BOOL sortReversed = reg.Read(szSortReversed, FALSE);
		if (sortReversed)
			item->Sort.Type = CSortInfo::Descending;
		else
			item->Sort.Type = CSortInfo::Ascending;

		//
		item->CheckFavIcon = reg.Read(szCheckFavIcon, TRUE);
#endif
		item->Info = info;
		item->Feed = NULL;
		item->Status = CSiteItem::Empty;
	}

	return TRUE;
}

BOOL LoadSiteItemUnreadCount(CSiteItem *item, int idx) {
	LOG2(5, "LoadSiteItemUnreadCount(%p, %d)", item, idx);

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		item->UnreadItems = reg.Read(szUnreadCount, 0);
	}

	return TRUE;
}

BOOL LoadSiteItemFlaggedCount(CSiteItem *item, int idx) {
	LOG2(5, "LoadSiteItemFlaggedCount(%p, %d)", item, idx);

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		item->FlaggedItems = reg.Read(szFlaggedCount, 0);
	}

	return TRUE;
}

int LoadSiteList(CSiteList &siteList) {
	LOG0(3, "LoadSiteList()");

	CSiteItem *rootItem = new CSiteItem(NULL, CSiteItem::Group);

	// read sites
	DWORD cSites;

	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_SUBSCRIPTIONS);
	reg.QuerySubKeyNumber(&cSites);

	CArray<CSiteItem*, CSiteItem*> sites;
	if (cSites > 0) {
		sites.SetSize(cSites);
		for (DWORD i = 1; i <= cSites; i++) {
			CSiteItem *item = new CSiteItem(NULL, CSiteItem::Site);
			LoadSiteItem(item, i);
			LoadSiteItemUnreadCount(item, i);
			LoadSiteItemFlaggedCount(item, i);
			sites.SetAt(i - 1, item);
		}

		// read hierarchy
		CRegistry regGroup(HKEY_CURRENT_USER, REG_KEY_GROUPS);
		LoadSiteGroup(rootItem, regGroup, sites);
	}

	//
	siteList.CreateFrom(rootItem);
	siteList.SetRoot(rootItem);
//	siteList.SetKeywords(keywords);

	return 0;
}
