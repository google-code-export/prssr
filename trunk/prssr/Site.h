// SiteItem.h: interface for the CSiteItem class.
//
//////////////////////////////////////////////////////////////////////

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

	// URL rewriting
	CArray<CRewriteRule *, CRewriteRule *> RewriteRules;
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
//	CSiteItem(CSiteItem *parent, CFeedInfo *siteInfo, CFeed *feed);
	CSiteItem(CSiteItem *parent, CSiteItem *siteItem);
//	CSiteItem(CSiteItem *parent, const CString &name);
	~CSiteItem();

	CSiteItem *Duplicate(CSiteItem *parent);
	void Destroy();

	void SetUpdatedFeed(CFeed *feed);
	void UseUpdatedFeed();
	void EnsureSiteLoaded();

//	int GetItemCount() const;
//	int GetNewCount() const;
	int GetUnreadCount() const;
	int GetFlaggedCount() const;

//	void ReadItemCountsFromCache();
//	void WriteItemCountsToCache();
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
//	CRITICAL_SECTION CSUpdateFeed;
	CRITICAL_SECTION CSLoadFeed;

//	int NewItems;
	int UnreadItems;
	int FlaggedItems;
//	int TotalItems;

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

//	int Load();
//	BOOL Save();

	int GetCount();
	CSiteItem *GetAt(int i);
	BOOL SaveItem(int idx);

	CSiteItem *GetRoot() const
		{ return Root; }
	void SetRoot(CSiteItem *root);

	int GetIndexOf(CSiteItem *item);

	CStringArray &GetKeywords() { return Keywords; }
	void SetKeywords(CStringArray &keywords);

	void CreateFrom(CSiteItem *root);

	CSiteItem *Flagged;
	CSiteItem *Unread;

protected:
	BOOL SaveItem(CSiteItem *item, CRegistry &reg);
	BOOL LoadItem(CSiteItem *item, CRegistry &reg);

//	CIPCReadWriteLock RWLock;
	// if TRUE, Data array is inconsistent (it is being loaded)
//	BOOL Dirty;
	CArray<CSiteItem*, CSiteItem*> Data;
	CSiteItem *Root;
	CStringArray Keywords;
};

extern CSiteList SiteList;
extern LPCTSTR GszRegItems;
extern LPCTSTR GszRegTemp;

//////////////////////////////////////////////////////////////////////


int LoadSiteList(CSiteList &siteList);
BOOL LoadSiteItem(CSiteItem *item, int idx);
BOOL LoadSiteItemUnreadCount(CSiteItem *item, int idx);
BOOL LoadSiteItemFlaggedCount(CSiteItem *item, int idx);

#ifdef PRSSR_APP

BOOL SaveSiteList();
void SaveSiteListKeywords();
BOOL SaveSiteItemUnreadCount(CSiteItem *item, int idx);
BOOL SaveSiteItemFlaggedCount(CSiteItem *item, int idx);

#endif

BOOL SaveSiteItem(CSiteItem *item, int idx);

#endif // !defined(AFX_SITEITEM_H__8D1A83CF_3901_43C4_A332_1D692C140810__INCLUDED_)
