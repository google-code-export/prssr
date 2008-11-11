/**
 *  Feed.h
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

#if !defined(AFX_FEED_H__1DEB3F7C_FE75_4310_8D51_09C2FFB4DF9A__INCLUDED_)
#define AFX_FEED_H__1DEB3F7C_FE75_4310_8D51_09C2FFB4DF9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <afxmt.h>

#if defined PRSSR_APP

//////////////////////////////////////////////////////////////////////

/*
class CPlaylistItem {
public:
	CString Title;
	CString FileName;

	CPlaylistItem(const CString &title, const CString &fileName) {
		Title = title;
		FileName = fileName;
	}
};
*/

//////////////////////////////////////////////////////////////////////

enum EEnclosureType {
	ENCLOSURE_TYPE_UNKNOWN,
	ENCLOSURE_TYPE_AUDIO,
	ENCLOSURE_TYPE_VIDEO,
	ENCLOSURE_TYPE_IMAGE,
};


class CEnclosureItem {
public:
	CString URL;
	DWORD Length;
	EEnclosureType Type;

	CEnclosureItem() {
		Length = 0;
	}

	CEnclosureItem(const CEnclosureItem &o) {
		URL = o.URL;
		Length = o.Length;
		Type = o.Type;
	}
};

#endif

//////////////////////////////////////////////////////////////////////

class CSiteItem;

#define MESSAGE_MASK_ALL						0xFFFF
#define MESSAGE_READ_STATE						0x0003

#define MESSAGE_READ							0x0000
#define MESSAGE_UNREAD							0x0001
#define MESSAGE_NEW								0x0002
#define MESSAGE_FLAG							0x0004
//#define MESSAGE_STICKY						0x0008
#define MESSAGE_SYNC							0x0040

#define MESSAGE_DELETED							0x0080

class CFeedItem {
public:
	CFeedItem(CSiteItem *si);
	CFeedItem(const CFeedItem &o);
	virtual ~CFeedItem();
	void ComputeHash(CString prefix = _T(""));

#if defined PRSSR_APP
	void GetEnclosures(CStringList &list, DWORD limit = 0);
	BOOL HasEnclosure() { return Enclosures.GetCount() > 0; }
	CList<CEnclosureItem *, CEnclosureItem *> Enclosures;

	void GetItemImages(CStringList &list);
#endif

#if defined PRSSR_APP
	BOOL HasKeywordMatch() { return KeywordPos.GetSize() > 0; }
	void SearchKeywords(CStringArray &keywords);
	CArray<int, int> KeywordPos;		// position in keyword list (from SiteList)
#endif

	CSiteItem *SiteItem;			// site item where the feed item belongs

	// info from feed file
	CString Title;
	CString Link;
	SYSTEMTIME PubDate;

	CString Hash;

#if defined PRSSR_APP
	CString Description;
	CString Author;
	SYSTEMTIME Date;			// used for sorting

	BOOL Hidden;
	void UpdateHiddenFlag();
#endif

	// flags related
	void SetFlags(DWORD flags, DWORD mask);
//	void SetFlagsFromRegistry(HKEY hItems);

	DWORD GetFlags() { return Flags; }
	DWORD GetReadState() { return Flags & MESSAGE_READ_STATE; }
	BOOL IsRead()    { return (Flags & MESSAGE_READ_STATE) == MESSAGE_READ; }
	BOOL IsUnread()  { return (Flags & MESSAGE_READ_STATE) == MESSAGE_UNREAD; }
	BOOL IsNew()     { return (Flags & MESSAGE_READ_STATE) == MESSAGE_NEW; }

	BOOL IsDeleted() { return Flags & MESSAGE_DELETED; }
	BOOL IsFlagged() { return Flags & MESSAGE_FLAG; }
	BOOL IsSynced()  { return Flags & MESSAGE_SYNC; }

protected:
	// flag
	//
	// 00000000
	// |    |||
	// |    |++--- read state (0 - read, 1 - unread, 2 - new)
	// |    +----- flag (1 - flag set)
	// +---------- deleted (1 - deleted)
	//

	DWORD Flags;

	friend class CFeed;
};

//////////////////////////////////////////////////////////////////////

class CFeed {
public:
	// info from xml file (feed)
	CString Title;

#ifdef PRSSR_APP
	CString HtmlUrl;
	CString Description;

	CString Language;
	CString Copyright;
	CString Format;

	SYSTEMTIME Published;
	int UpdateInterval;			// update interval suggested by feed (0 = not specified)
#endif

	CFeed();
	~CFeed();

	int GetItemCount() const { return Items.GetSize(); }
	int GetNewCount() const;
	int GetUnreadCount() const;
	int GetFlaggedCount() const;
	CFeedItem *GetItem(int i) const { return Items.GetAt(i); }

	BOOL Load(LPCTSTR fileName, CSiteItem *si);
#ifdef PRSSR_APP
	BOOL Save(LPCTSTR fileName);
#endif

	void Detach();
	void Destroy();
	void Add(CFeedItem *item) { Items.Add(item); }
	void UpdateHiddenFlags();

	void SetSize(int count) { Items.SetSize(count); }
	void SetAt(int idx, CFeedItem *item) { Items.SetAtGrow(idx, item); }

#if defined PRSSR_APP
	BOOL GenerateASX(const CString &strFileName, DWORD limit);
	BOOL GenerateM3U(const CString &strFileName, DWORD limit);
//	void SetKeywordFlags(CStringArray &keywords);
#endif

//	void SetFlagsFromRegistry();
//	void PurgeRegistryHashes();

#ifdef PRSSR_APP
	void MarkAll(DWORD flag, DWORD mask);
#endif

	void Lock() { EnterCriticalSection(&CS); }
	void Unlock() { LeaveCriticalSection(&CS); }

protected:
	CArray<CFeedItem *, CFeedItem *> Items;
	CRITICAL_SECTION CS;

#if defined PRSSR_APP
//	void GetPlaylistItems(CList<CPlaylistItem*, CPlaylistItem*> &list, DWORD limit = 0);
#endif
};


//////////////////////////////////////////////////////////////////////

#if defined PRSSR_APP
int CompareFeedItemDate(CFeedItem *fi1, CFeedItem *fi2);
int CompareFeedItemFlag(CFeedItem *fi1, CFeedItem *fi2);
#endif

void WriteFeedItemFlagsToRegistry(CFeedItem* feedItem);
void WriteFeedItemFlagsToRegistry(CList<CFeedItem*, CFeedItem*> &feedItems);
void SetFeedItemFlags(CList<CFeedItem*, CFeedItem*> &feedItems, DWORD flag, DWORD mask);

void FeedDiff(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);
void FeedIntersection(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff);


#endif // !defined(AFX_FEED_H__1DEB3F7C_FE75_4310_8D51_09C2FFB4DF9A__INCLUDED_)
