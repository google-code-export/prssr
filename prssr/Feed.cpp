/**
 *  Feed.cpp
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
#endif

#ifdef PRSSR_TODAY
	#include "../prssrtoday/StdAfx.h"
#endif

#include "Feed.h"
#include "xml/FeedFile.h"
#include "Site.h"

#include "sha1/sha1.h"
#include "../share/reg.h"
#include "../share/helpers.h"
#include "www/LocalHtmlFile.h"

#ifdef PRSSR_APP
	#include "Config.h"
//	#include "WebDownloader.h"
//	#include "WebHtml.h"
	#include "net/Connection.h"
#elif defined PRSSR_TODAY || PRSSR_TODAYSTUB
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


//////////////////////////////////////////////////////////////////////

void FeedDiff(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff) {
	LOG0(5, "FeedDiff()");

	CCache cache;
	int i;

	if (second != NULL)
		for (i = 0; i < second->GetItemCount(); i++)
			cache.AddItem(second->GetItem(i)->Hash);

	if (first != NULL)
		for (i = 0; i < first->GetItemCount(); i++) {
			CFeedItem *fi = first->GetItem(i);
			if (!cache.InCache(fi->Hash))
//				diff->Add(new CFeedItem(*fi));
				diff->Add(fi);
		}
}

void FeedIntersection(CFeed *first, CFeed *second, CArray<CFeedItem *, CFeedItem *> *diff) {
	LOG0(5, "FeedIntersection()");

	CCache cache;
	int i;

	if (second != NULL)
		for (i = 0; i < second->GetItemCount(); i++) {
			CFeedItem *fi = second->GetItem(i);
//			if (fi->Hash.IsEmpty()) AfxMessageBox(_T("Jak to?"));
			if (!fi->Hash.IsEmpty())
				cache.AddItem(fi->Hash);
		}

	if (first != NULL)
		for (i = 0; i < first->GetItemCount(); i++) {
			CFeedItem *fi = first->GetItem(i);
			if (!fi->Hash.IsEmpty() && cache.InCache(fi->Hash))
//				diff->Add(new CFeedItem(*fi));
				diff->Add(fi);
		}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedItem::CFeedItem(CSiteItem *si) {
	SiteItem = si;
	Flags = MESSAGE_NEW;
#ifdef PRSSR_APP
	Hidden = FALSE;
	ZeroMemory(&Date, sizeof(Date));
#endif
	ZeroMemory(&PubDate, sizeof(PubDate));
}

CFeedItem::CFeedItem(const CFeedItem &o) {
	SiteItem = o.SiteItem;

	Hash = o.Hash;
	Flags = o.Flags;
	PubDate = o.PubDate;
	Title = o.Title;
	Link = o.Link;

#ifdef PRSSR_APP
	Description = o.Description;
	Hidden = o.Hidden;
	Author = o.Author;
	Date = o.Date;

	while (!Enclosures.IsEmpty())
		delete Enclosures.RemoveHead();

	POSITION pos = o.Enclosures.GetHeadPosition();
	while (pos != NULL) {
		CEnclosureItem *enclosure = o.Enclosures.GetNext(pos);
		Enclosures.AddTail(new CEnclosureItem(*enclosure));
	}

	for (int i = 0; i < o.KeywordPos.GetSize(); i++)
		KeywordPos.SetAtGrow(i, o.KeywordPos[i]);
#endif
}


CFeedItem::~CFeedItem() {
#ifdef PRSSR_APP
	while (!Enclosures.IsEmpty())
		delete Enclosures.RemoveHead();
#endif
}

void CFeedItem::ComputeHash(CString prefix) {
	LOG0(5, "CFeedItem::ComputeHash()");

	SHA1Context sha = { 0 };
	int err;
    uint8_t digest[20];

	CString strMessage;
	strMessage.Format(_T("%s%s%s"), prefix, Title, Link);

	err = SHA1Reset(&sha);
	err = SHA1Input(&sha, strMessage.GetBuffer(strMessage.GetLength()), strMessage.GetLength());
	strMessage.ReleaseBuffer();
	err = SHA1Result(&sha, digest);

	// convert to string
	LPTSTR hash = Hash.GetBufferSetLength(40);
	TCHAR hexTab[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	for (int i = 0; i < 20; i++) {
		hash[2 * i] = hexTab[digest[i] & 0x0F];
		hash[(2 * i) + 1] = hexTab[(digest[i] >> 4) & 0x0F];
	}
}

#if defined PRSSR_APP

void CFeedItem::GetItemImages(CStringList &list) {
	LOG0(5, "CFeedItem::GetItemImages()");

	CString html;
	html.Format(_T("<div>%s</div>"), Description);		// workaround for libsgml

	CLocalHtmlFile h;
	h.LoadFromMemory(html);
	h.ExtractImages(list);

/*	CHtmlExtractor extr(_T(""), _T(""), EXTRACT_IMAGES);
	if (extr.Parse(Description)) {
		POSITION pos = extr.Links.GetHeadPosition();
		while (pos != NULL) {
			CString s = extr.Links.GetNext(pos);
			list.AddTail(s);
		}
	}
*/
}

void CFeedItem::GetEnclosures(CStringList &list, DWORD limit/* = 0*/) {
	LOG0(5, "CFeedItem::GetEnclosures()");

	POSITION pos = Enclosures.GetHeadPosition();
	while (pos != NULL) {
		CEnclosureItem *enclosure = Enclosures.GetNext(pos);

		if (limit == 0 || enclosure->Length <= limit)
			list.AddTail(enclosure->URL);
	}
}


void CFeedItem::UpdateHiddenFlag() {
/*	if (Config.HideReadItems) {
		if (Config.ShowFlaggedItems && IsFlagged())
			Hidden = FALSE;
		else {
			if (IsRead())
				Hidden = TRUE;
			else
				Hidden = FALSE;
		}
	}
	else {
		if (Config.ShowFlaggedItems) {
			if (IsFlagged())
				Hidden = FALSE;
			else
				Hidden = TRUE;
		}
		else
			Hidden = FALSE;
	}
*/
	Hidden = FALSE;
}
#endif

void CFeedItem::SetFlags(DWORD flags, DWORD mask) {
	Flags = (Flags & ~mask) | flags;
	SiteItem->SetModified();
}

#if defined PRSSR_APP

void CFeedItem::SearchKeywords(CStringArray &keywords) {
	LOG0(5, "CFeedItem::SearchKeywords()");

	KeywordPos.RemoveAll();

	for (int k = 0; k < keywords.GetSize(); k++) {
		CString kword = keywords.GetAt(k);

		CString text = StripHtmlTags(Description);
		if (text.Find(kword) != -1) {
			KeywordPos.Add(k);
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeed::CFeed() {
	FILETIME ft = { 0 };

#ifdef PRSSR_APP
	memset(&Published, 0, sizeof(Published));
	UpdateInterval = 0;
#endif

	InitializeCriticalSection(&CS);
}

CFeed::~CFeed() {
//	Empty();

	DeleteCriticalSection(&CS);
}

// Load/Save

struct CArchiveFileChunkHeader {
	char Id[4];
	DWORD Size;

	BOOL Load(CBufferedFile &file) {
		if (!file.Read(Id, 4))
			return FALSE;
		if (!file.Read(&Size, 4))
			return FALSE;

		return TRUE;
	}

	BOOL Save(CBufferedFile &file) {
		if (!file.Write(Id, 4))
			return FALSE;
		return file.Write(&Size, sizeof(DWORD));
	}
};

struct CArchiveFileChunk {
	CArchiveFileChunkHeader Header;
	char *Data;
	CList<CArchiveFileChunk *, CArchiveFileChunk*> Childs;

	CArchiveFileChunk(char id[]) {
//		assert(strlen(id) == 4);
		strncpy(Header.Id, id, 4);
		Header.Size = 0;

		Data = NULL;
	}

	void Add(CArchiveFileChunk *chunk) {
//		LOG1(5, "CArchiveFileChunk::Add(%p) - chunk", chunk);

		Childs.AddTail(chunk);
	}

	void Add(char id[], const CString &str) {
//		LOG2(5, "CArchiveFileChunk::Add('%s', '%S')", id, str);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);
		chunk->Data = WCharToChar(str, CP_UTF8);
		chunk->Header.Size = strlen(chunk->Data);

		Add(chunk);
	}

	void Add(char id[], int number) {
//		LOG2(5, "CArchiveFileChunk::Add('%s', %d)", id, number);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);
		chunk->Data = new char [sizeof(int)];
		memcpy(chunk->Data + 0, &number, sizeof(number));
		chunk->Header.Size = sizeof(int);
		Add(chunk);
	}

	void Add(char id[], SYSTEMTIME &st) {
//		LOG1(5, "CArchiveFileChunk::Add('%s', ) - systime", id);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);

		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);

		chunk->Data = new char [sizeof(DWORD) * 2];
		memcpy(chunk->Data + 0, &ft.dwHighDateTime, sizeof(DWORD));
		memcpy(chunk->Data + 4, &ft.dwLowDateTime, sizeof(DWORD));
		chunk->Header.Size = sizeof(DWORD) * 2;

		Add(chunk);
	}

	void Add(char id[], CArray<int, int> &arr) {
//		LOG2(5, "CArchiveFileChunk::Add('%s', %d)", id, number);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);
		chunk->Data = new char [sizeof(int) * arr.GetSize()];
		int *dest = (int *) chunk->Data;
		for (int i = 0; i < arr.GetSize(); i++)
			dest[i] = arr[i];
		chunk->Header.Size = sizeof(int) * arr.GetSize();
		Add(chunk);
	}


	~CArchiveFileChunk() {
		delete [] Data;
		while (!Childs.IsEmpty())
			delete Childs.RemoveHead();
	}

	void CalcSize() {
//		LOG0(5, "CArchiveFileChunk::CalcSize()");

		if (Childs.GetCount() > 0) {
			Header.Size = 0;
			POSITION pos = Childs.GetHeadPosition();
			while (pos != NULL) {
				CArchiveFileChunk *fc = Childs.GetNext(pos);
				fc->CalcSize();
				Header.Size += fc->Header.Size + 8;
			}
		}
	}

	void Save(CBufferedFile &file) {
//		LOG0(5, "CArchiveFileChunk::Save()");

		Header.Save(file);
		if (Childs.GetCount() > 0) {
			POSITION pos = Childs.GetHeadPosition();
			while (pos != NULL) {
				CArchiveFileChunk *fc = Childs.GetNext(pos);
				fc->Save(file);
			}
		}
		else {
			if (Data != NULL)
				file.Write(Data, Header.Size);
		}
	}
};

static BOOL ReadString(CBufferedFile &file, int len, CString &str) {
	if (len > 0) {
		char *buffer = new char[len + 1];
		memset(buffer, 0, sizeof(char) * (len + 1));
		if (!file.Read(buffer, len)) {
			delete [] buffer;
			return FALSE;
		}
		str = CharToWChar(buffer, CP_UTF8);
		delete [] buffer;
	}
	else
		str.Empty();

	return TRUE;
}

static BOOL ReadNumber(CBufferedFile &file, int &number) {
	int i;
	if (!file.Read(&i, sizeof(i)))
		return FALSE;
	number = i;
	return TRUE;
}

static BOOL ReadNumber(CBufferedFile &file, DWORD &number) {
	DWORD i;
	if (!file.Read(&i, sizeof(i)))
		return FALSE;
	number = i;
	return TRUE;
}

static BOOL ReadSystemTime(CBufferedFile &file, SYSTEMTIME &st) {
	FILETIME ft;
	char buffer[8];
	if (!file.Read(buffer, sizeof(buffer)))
		return FALSE;
	memcpy(&ft.dwHighDateTime, buffer + 0, sizeof(DWORD));
	memcpy(&ft.dwLowDateTime, buffer + 4, sizeof(DWORD));
	FileTimeToSystemTime(&ft, &st);
	return TRUE;
}

static BOOL ReadIntArray(CBufferedFile &file, int len, CArray<int, int> &array) {
	for (int i = 0; i < (int) (len / sizeof(int)); i++) {
		int number;
		if (!ReadNumber(file, number))
			return FALSE;
		array.Add(number);
	}

	return TRUE;
}

BOOL CFeed::Load(LPCTSTR fileName, CSiteItem *si) {
	LOG1(5, "CFeed::Load('%S')", fileName);

	CBufferedFile file;
	if (!file.Create(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL))
		return FALSE;

	DWORD remain;
	CArchiveFileChunkHeader header;
	if (!header.Load(file) || strncmp(header.Id, "PRSS", 4) != 0)
		return FALSE;

//	LOG0(1, "- loading header");

	// load HEAD
	if (!header.Load(file) || strncmp(header.Id, "HEAD", 4) != 0)
		return FALSE;
	remain = header.Size;
	while (remain > 0) {
		CArchiveFileChunkHeader hdr;
		if (!hdr.Load(file))
			return FALSE;
		remain -= 8;		// sizeof(hdr)

		if (strncmp(hdr.Id, "TITL", 4) == 0)
			ReadString(file, hdr.Size, Title);
#ifdef PRSSR_APP
		else if (strncmp(hdr.Id, "LINK", 4) == 0)
			ReadString(file, hdr.Size, HtmlUrl);
		else if (strncmp(hdr.Id, "DESC", 4) == 0)
			ReadString(file, hdr.Size, Description);
		else if (strncmp(hdr.Id, "PUBL", 4) == 0)
			ReadSystemTime(file, Published);
		else if (strncmp(hdr.Id, "LANG", 4) == 0)
			ReadString(file, hdr.Size, Language);
		else if (strncmp(hdr.Id, "FRMT", 4) == 0)
			ReadString(file, hdr.Size, Format);
		else if (strncmp(hdr.Id, "UPIN", 4) == 0)
			ReadNumber(file, UpdateInterval);
#endif
		else
			file.Seek(hdr.Size, FILE_CURRENT);

		remain -= hdr.Size;
	}

	// load ITMS
	if (!header.Load(file) || strncmp(header.Id, "ITMS", 4) != 0)
		return FALSE;

//	LOG0(5, "- loading items");

	BOOL ok = TRUE;
	DWORD remainItems = header.Size;
	while (remainItems > 0) {
		CArchiveFileChunkHeader hdrItem;
		if (!hdrItem.Load(file) || strncmp(hdrItem.Id, "ITEM", 4) != 0)
			return FALSE;
		remainItems -= 8;

//		LOG0(5, "- loading item");

		// load item
		BOOL itemOK = TRUE;
		CFeedItem feedItem(si);
		remain = hdrItem.Size;
		while (remain > 0 && itemOK) {
			CArchiveFileChunkHeader hdr;
			if (!hdr.Load(file)) {
				itemOK = FALSE;
				break;
			}
			remain -= 8;

			if (strncmp(hdr.Id, "TITL", 4) == 0)
				ReadString(file, hdr.Size, feedItem.Title);
			else if (strncmp(hdr.Id, "LINK", 4) == 0)
				ReadString(file, hdr.Size, feedItem.Link);
			else if (strncmp(hdr.Id, "FLGS", 4) == 0)
				ReadNumber(file, feedItem.Flags);
			else if (strncmp(hdr.Id, "TIME", 4) == 0)
				ReadSystemTime(file, feedItem.PubDate);
			else if (strncmp(hdr.Id, "ID  ", 4) == 0)
				ReadString(file, hdr.Size, feedItem.Hash);
#ifdef PRSSR_APP
			else if (strncmp(hdr.Id, "DESC", 4) == 0)
				ReadString(file, hdr.Size, feedItem.Description);
			else if (strncmp(hdr.Id, "AUTH", 4) == 0)
				ReadString(file, hdr.Size, feedItem.Author);
			else if (strncmp(hdr.Id, "ENCS", 4) == 0) {
				// read enclosures
				DWORD remainEncsItems = hdr.Size;
				while (remainEncsItems > 0) {
					CArchiveFileChunkHeader enitItem;
					if (enitItem.Load(file)) {
						remainEncsItems -= 8;
						if (strncmp(enitItem.Id, "ENIT", 4) == 0) {
							// load enclosures
							BOOL encOK = TRUE;
							CEnclosureItem ei;
							DWORD remainEnclosures = enitItem.Size;
							while (remainEnclosures > 0) {
								CArchiveFileChunkHeader hdrEnc;
								if (!hdrEnc.Load(file)) {
									encOK = FALSE;
									break;
								}
								remainEnclosures -= 8;			// sizeof(header)

								if (strncmp(hdrEnc.Id, "URL ", 4) == 0)
									ReadString(file, hdrEnc.Size, ei.URL);
								else if (strncmp(hdrEnc.Id, "TYPE", 4) == 0) {
									CString sMime;
									// convert from older format
									ReadString(file, hdrEnc.Size, sMime);
									int nPos = sMime.Find('/');
									if (nPos != -1) {
										CString sType = sMime.Left(nPos);
										if (sType.CompareNoCase(_T("video")) == 0) ei.Type = ENCLOSURE_TYPE_VIDEO;
										else if (sType.CompareNoCase(_T("audio")) == 0) ei.Type = ENCLOSURE_TYPE_AUDIO;
										else if (sType.CompareNoCase(_T("image")) == 0) ei.Type = ENCLOSURE_TYPE_IMAGE;
										else ei.Type = ENCLOSURE_TYPE_UNKNOWN;
									}
									else ei.Type = ENCLOSURE_TYPE_UNKNOWN;
								}
								else if (strncmp(hdrEnc.Id, "ETYP", 4) == 0) {
									DWORD encType;
									ReadNumber(file, encType);
									ei.Type = (EEnclosureType) encType;
								}
								else if (strncmp(hdrEnc.Id, "LENG", 4) == 0)
									ReadNumber(file, ei.Length);
								else
									file.Seek(hdrEnc.Size, FILE_CURRENT);

								remainEnclosures -= hdrEnc.Size;
							} // while

							if (encOK)
								feedItem.Enclosures.AddTail(new CEnclosureItem(ei));
						}
						else
							file.Seek(enitItem.Size, FILE_CURRENT);
					}
					else
						break;

					remainEncsItems -= enitItem.Size;
				}
			}
			else if (strncmp(hdr.Id, "KWDS", 4) == 0) {
				ReadIntArray(file, hdr.Size, feedItem.KeywordPos);
			}
#endif
			else
				file.Seek(hdr.Size, FILE_CURRENT);

			remain -= hdr.Size;
		} // while

		if (itemOK) {
			if (feedItem.Hash.IsEmpty())
				feedItem.ComputeHash();

#ifdef PRSSR_APP
			// fill Date
//			if (feedItem.PubDate.wYear == 0)
//				GetLocalTime(&feedItem.Date);
//			else
			feedItem.Date = feedItem.PubDate;
			// fill milisecond field (crucial for proper sorting)
			feedItem.Date.wMilliseconds = (WORD) (GetTickCount() & 0xFFFF);
			Sleep(1);
#endif

			Items.Add(new CFeedItem(feedItem));
		}
		else {
			ok = FALSE;
			break;
		}

		remainItems -= hdrItem.Size;
	}

	file.Close();

//	LOG1(5, "- state: %d", ok);

	return ok;
}

#ifdef PRSSR_APP

BOOL CFeed::Save(LPCTSTR fileName) {
	LOG1(5, "CFeed::Save('%S')", fileName);

	CBufferedFile file;
	if (!file.Create(fileName, GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL))
		return FALSE;

	// id (PRSS)
	CArchiveFileChunk *root = new CArchiveFileChunk("PRSS");

	// header (HEAD)
	CArchiveFileChunk *head = new CArchiveFileChunk("HEAD");
	head->Add("TITL", Title);
	head->Add("LINK", HtmlUrl);
	head->Add("DESC", Description);
	// TODO: image (IMAG)
	head->Add("UPIN", UpdateInterval);
	head->Add("PUBL", Published);
	head->Add("LANG", Language);
	head->Add("FRMT", Format);
	root->Add(head);

	if (Items.GetSize() > 0) {
		// items (ITMS)
		CArchiveFileChunk *items = new CArchiveFileChunk("ITMS");
		for (int i = 0; i < Items.GetSize(); i++) {
			CFeedItem *fi = Items.GetAt(i);
			CArchiveFileChunk *itm = new CArchiveFileChunk("ITEM");

			itm->Add("FLGS", fi->Flags);
			if (fi->IsDeleted()) {
				itm->Add("ID  ", fi->Hash);
			}
			else {
				itm->Add("TITL", fi->Title);
				itm->Add("DESC", fi->Description);
				itm->Add("LINK", fi->Link);
				itm->Add("AUTH", fi->Author);
				itm->Add("TIME", fi->PubDate);
				itm->Add("ID  ", fi->Hash);

				if (fi->Enclosures.GetCount() > 0) {
					// enclosures
					CArchiveFileChunk *enclosures = new CArchiveFileChunk("ENCS");
					POSITION pos = fi->Enclosures.GetHeadPosition();
					while (pos != NULL) {
						CEnclosureItem *ei = fi->Enclosures.GetNext(pos);
						CArchiveFileChunk *eitm = new CArchiveFileChunk("ENIT");

						eitm->Add("URL ", ei->URL);
						eitm->Add("ETYP", ei->Type);
						eitm->Add("LENG", ei->Length);

						enclosures->Add(eitm);
					}
					itm->Add(enclosures);
				}

				// keywords
				itm->Add("KWDS", fi->KeywordPos);
			}

			items->Add(itm);
		}
		root->Add(items);
	}

	root->CalcSize();
	root->Save(file);

	delete root;

	file.Close();

	return TRUE;
}

#endif

//

void CFeed::Detach() {
	LOG0(5, "CFeed::Detach()");

	Items.RemoveAll();
}

void CFeed::Destroy() {
	LOG0(5, "CFeed::Destroy()");

	for (int i = 0; i < Items.GetSize(); i++)
		delete Items.GetAt(i);
	Items.RemoveAll();
}

int CFeed::GetUnreadCount() const {
	int count = 0;
	for (int i = 0; i < GetItemCount(); i++) {
		CFeedItem *fi = Items.GetAt(i);
		if (!fi->IsDeleted() && fi->IsUnread())
			count++;
	}

	return count;
}

int CFeed::GetNewCount() const {
	int count = 0;
	for (int i = 0; i < GetItemCount(); i++) {
		CFeedItem *fi = Items.GetAt(i);
		if (!fi->IsDeleted() && fi->IsNew())
			count++;
	}

	return count;
}

int CFeed::GetFlaggedCount() const {
	int count = 0;
	for (int i = 0; i < GetItemCount(); i++) {
		CFeedItem *fi = Items.GetAt(i);
		if (!fi->IsDeleted() && fi->IsFlagged())
			count++;
	}

	return count;
}

#if defined PRSSR_APP

/*
void CFeed::GetPlaylistItems(CList<CPlaylistItem*, CPlaylistItem*> &list, DWORD limit/* = 0*//*) {
	LOG0(5, "CFeed::GetPlaylistItems()");

	// collect enclosures
	for (int i = 0; i < GetItemCount(); i++) {
		CFeedItem *fi = GetItem(i);

		POSITION pos = fi->Enclosures.GetHeadPosition();
		while (pos != NULL) {
			CEnclosureItem *enclosure = fi->Enclosures.GetNext(pos);

			if (enclosure->Type.Left(5).CompareNoCase(_T("audio")) == 0 ||
				enclosure->Type.Left(5).CompareNoCase(_T("video")) == 0)
			{
				if (limit == 0 || enclosure->Length <= limit) {
					CString url = MakeOnlineFile(enclosure->URL, _T(""), _T(""));
					CString outFileName = UrlToFileName(url, Config.EnclosuresLocation);

					if (FileExists(outFileName)) {
						// add only cached items
						list.AddTail(new CPlaylistItem(fi->Title, outFileName));
					}
				}
			}
		}
	}
}

BOOL CFeed::GenerateASX(const CString &strFileName, DWORD limit) {
	LOG2(5, "CFeed::GenerateASX('%S', %d)", strFileName, limit);

	// collect enclosures
	CList<CPlaylistItem *, CPlaylistItem *> list;
	GetPlaylistItems(list, limit);

	// got no enclosures -> exit
	if (list.GetCount() <= 0)
		return TRUE;

	// generate ASX
	CString strASXFileName = strFileName;
	if (strASXFileName.Right(4).CompareNoCase(_T(".xml")) == 0) {
		strASXFileName = strASXFileName.Left(strASXFileName.GetLength() - 4) + _T(".asx");
	}
	else {
		strASXFileName += _T(".asx");
	}

	BOOL ret = TRUE;

	CString strPathFileName = FormatFilePath(Config.EnclosuresLocation, strASXFileName);
	CBufferedFile file;
	if (file.Create(strPathFileName, GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL)) {
		ret = WriteFileString(file, _T("<asx version=\"3.0\">\n"), CP_ACP);

		// enclosures
		POSITION pos = list.GetHeadPosition();
		while (pos != NULL) {
			CPlaylistItem *pi = list.GetNext(pos);

			ret = ret ? WriteFileString(file, _T("\t<entry>\n"), CP_ACP) : FALSE;

			CString strTitle;
			strTitle.Format(_T("\t\t<title>%s</title>\n"), pi->Title);
			ret = ret ? WriteFileString(file, strTitle, CP_ACP) : FALSE;

			CString strRef;
			strRef.Format(_T("\t\t<ref href=\"%s\" />\n"), pi->FileName);
			ret = ret ? WriteFileString(file, strRef, CP_ACP) : FALSE;

			ret = ret ? WriteFileString(file, _T("\t</entry>\n"), CP_ACP) : FALSE;
		}

		ret = ret ? WriteFileString(file, _T("</asx>\n"), CP_ACP) : FALSE;

		file.Close();
	}
	else
		ret = FALSE;

	while (!list.IsEmpty())
		delete list.RemoveHead();

	return ret;
}

BOOL CFeed::GenerateM3U(const CString &strFileName, DWORD limit) {
	LOG2(5, "CFeed::GenerateM3U('%S', %d)", strFileName, limit);

	// collect enclosures
	CList<CPlaylistItem *, CPlaylistItem *> list;
	GetPlaylistItems(list, limit);

	// got no enclosures -> exit
	if (list.GetCount() <= 0)
		return TRUE;

	// generate M3U
	CString strM3UFileName = strFileName;
	if (strM3UFileName.Right(4).CompareNoCase(_T(".xml")) == 0) {
		strM3UFileName = strM3UFileName.Left(strM3UFileName.GetLength() - 4) + _T(".m3u");
	}
	else {
		strM3UFileName += _T(".m3u");
	}

	BOOL ret = TRUE;

	CString strPathFileName = FormatFilePath(Config.EnclosuresLocation, strM3UFileName);

	CBufferedFile file;
	if (file.Create(strPathFileName, GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL)) {
		ret = ret ? WriteFileString(file, _T("#EXTM3U\n"), CP_ACP) : FALSE;

		// enclosures
		POSITION pos = list.GetHeadPosition();
		while (pos != NULL) {
			CPlaylistItem *pi = list.GetNext(pos);

			CString strINF;
			strINF.Format(_T("#EXTINF:,%s\n"), pi->Title);
			ret = ret ? WriteFileString(file, strINF, CP_ACP) : FALSE;

			CString strFile;
			strFile.Format(_T("%s\n"), pi->FileName);
			ret = ret ? WriteFileString(file, strFile, CP_ACP) : FALSE;
		}

		file.Close();
	}
	else
		ret = FALSE;

	while (!list.IsEmpty())
		delete list.RemoveHead();

	return ret;
}
*/
#endif

#ifdef PRSSR_APP

/*
void CFeed::SetKeywordFlags(CStringArray &keywords) {
	LOG0(5, "CFeed::SetKeywordFlags()");

	for (int i = 0; i < GetItemCount(); i++) {
		CFeedItem *fi = GetItem(i);
		fi->SearchKeywords(keywords);
	}
}
*/

void CFeed::UpdateHiddenFlags() {
	LOG0(5, "CFeed::UpdateHiddenFlags()");

	for (int i = 0; i < GetItemCount(); i++)
		GetItem(i)->UpdateHiddenFlag();
}

#endif

/*
void CFeed::SetFlagsFromRegistry() {
	LOG0(5, "CFeed::SetFlagsFromRegistry()");

	HKEY hItems = RegOpenItemsKey();
	if (hItems != NULL) {
		for (int i = 0; i < GetItemCount(); i++)
			GetItem(i)->SetFlagsFromRegistry(hItems);
		RegCloseKey(hItems);
	}
}
*/

#if defined PRSSR_APP

void CFeed::MarkAll(DWORD flag, DWORD mask) {
	LOG1(5, "CFeed::MarkAll(%d)", flag);

	for (int i = 0; i < GetItemCount(); i++) {
		CFeedItem *fi = GetItem(i);
		fi->SetFlags(flag, mask);
		fi->UpdateHiddenFlag();
	}
}

#endif

#if defined PRSSR_APP

/*void CFeed::PurgeRegistryHashes() {
	LOG0(5, "CFeed::PurgeRegistryHashes()");

	HKEY hItems = RegOpenItemsKey();
	if (hItems != NULL) {
		for (int i = 0; i < GetItemCount(); i++)
			RegDeleteValue(hItems, GetItem(i)->Hash);
		RegCloseKey(hItems);
	}
}
*/

#endif
