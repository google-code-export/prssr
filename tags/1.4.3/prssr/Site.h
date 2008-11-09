/**
 *  Site.h
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

#if !defined(AFX_SITEITEM_H__8D1A83CF_3901_43C4_A332_1D692C140810__INCLUDED_)
#define AFX_SITEITEM_H__8D1A83CF_3901_43C4_A332_1D692C140810__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../share/helpers.h"
#include "../share/reg.h"
#include "Feed.h"
//#include "share\IPCReadWriteLock.h"
#include "www/url.h"


#define SITE_UNREAD						-1
#define SITE_FLAGGED					-2
#define SITE_INVALID					-3


//////////////////////////////////////////////////////////////////////

class CFeedInfo {
public:
	CFeedInfo();
	virtual ~CFeedInfo();

#ifdef PRSSR_APP
	static CString GenerateFileNameFromTitle(const CString &title);
	static void EnsureUniqueFileName(CString &fileName);
#endif

public:
	CString FileName;
	CString XmlUrl;

	BOOL TodayShow;				// show on today screen

#if defined PRSSR_APP
	// cache options
	BOOL UseGlobalCacheOptions;
	BOOL CacheItemImages;
	BOOL CacheHtml;

	int CacheLimit;
	int UpdateInterval;			// overridden update interval (-1 = use global settings, -2 = no auto-update)

	// enclosures
	BOOL CacheEnclosures;
	int EnclosureLimit;

	// HTTP headers (for conditional GET)
	CString ETag;
	CString LastModified;

	// authentication
	CString UserName;			// if empty -> no auth done (and password is ignored)
	CString Password;
#endif

	CFeedInfo &operator=(const CFeedInfo &o);
};

//////////////////////////////////////////////////////////////////////

class CSiteItem {
public:
	enum eType {
		Site = 0,
		Group = 1,
		VFolder
	} Type;

	CSiteItem(CSiteItem *parent, eType type);
	CSiteItem(CSiteItem *parent, CSiteItem *siteItem);
	~CSiteItem();

	void Destroy();

	void SetUpdatedFeed(CFeed *feed);
	void UseUpdatedFeed();
	void EnsureSiteLoaded();

	int GetUnreadCount() const;
	int GetFlaggedCount() const;

	void UpdateCachedCounts();

	void AddItem(CSiteItem *siteItem)
		{ SubItems.AddTail(siteItem); }
	void AddItemHead(CSiteItem *item)
		{ SubItems.AddHead(item); }

	void GetSites(CList<CSiteItem *, CSiteItem *> &sites);

	int GetCount()
		{ return SubItems.GetCount(); }

	BOOL IsModified()
		{ return Modified == TRUE; }
	void SetModified(BOOL modified = TRUE)
		{ Modified = modified; }

	enum eState {
		Ok,
		Error,
		Empty			// not read in memory
	} Status;
	int ImageIdx;		// index to the image list
	BOOL CheckFavIcon;	// if TRUE -> check favicon on update

	CSiteItem *Parent;
	CString Name;
	CList<CSiteItem *, CSiteItem *> SubItems;
	// site
	CFeedInfo *Info;
	CFeed *Feed;
	CSortInfo Sort;					// sort
	DWORD FlagMask;					// for virtual folder

#ifdef PRSSR_TODAY
	FILETIME LastUpdate;
#endif

protected:
	CRITICAL_SECTION CSLoadFeed;

	int UnreadItems;
	int FlaggedItems;

	BOOL Modified;						// TRUE, if the site has to be saved

	friend BOOL LoadSiteItemUnreadCount(CSiteItem *item, int idx);
	friend BOOL LoadSiteItemFlaggedCount(CSiteItem *item, int idx);
};

//////////////////////////////////////////////////////////////////////

class CSiteList {
public:
	CSiteList();
	virtual ~CSiteList();

	void Destroy();
	void Detach();

	int GetCount();
	CSiteItem *GetAt(int i);
	BOOL SaveItem(int idx);

	CSiteItem *GetRoot() const
		{ return Root; }
	void SetRoot(CSiteItem *root);

	int GetIndexOf(CSiteItem *item);

	void CreateFrom(CSiteItem *root);

protected:
	BOOL SaveItem(CSiteItem *item, CRegistry &reg);
	BOOL LoadItem(CSiteItem *item, CRegistry &reg);

	// if TRUE, Data array is inconsistent (it is being loaded)
	CArray<CSiteItem*, CSiteItem*> Data;
	CSiteItem *Root;
};

extern CSiteList SiteList;
extern LPCTSTR GszRegItems;
extern LPCTSTR GszRegTemp;

extern CSiteItem FlaggedItems;
extern CSiteItem UnreadItems;

//////////////////////////////////////////////////////////////////////


int LoadSiteList(CSiteList &siteList);
BOOL LoadSiteItem(CSiteItem *item, int idx);
BOOL LoadSiteItemUnreadCount(CSiteItem *item, int idx);
BOOL LoadSiteItemFlaggedCount(CSiteItem *item, int idx);

#ifdef PRSSR_APP

BOOL SaveSiteList();
BOOL SaveSiteItemUnreadCount(CSiteItem *item, int idx);
BOOL SaveSiteItemFlaggedCount(CSiteItem *item, int idx);

#endif

BOOL SaveSiteItem(CSiteItem *item, int idx);

#endif // !defined(AFX_SITEITEM_H__8D1A83CF_3901_43C4_A332_1D692C140810__INCLUDED_)
