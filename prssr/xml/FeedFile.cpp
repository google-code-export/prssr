/**
 *  FeedFile.cpp
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
	#include "..\stdafx.h"
	#include "..\prssr.h"
#endif

#if defined PRSSR_TODAY
	#include "..\prssrtoday\stdafx.h"
#endif

#include "FeedFile.h"
#include "../../share/helpers.h"
#include "../Feed.h"

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


BOOL DcDateToSytemTime(LPCTSTR strTime, SYSTEMTIME *time) {
	LOG0(5, "DcDateToSytemTime()");

	SYSTEMTIME st = { 0 };

	TCHAR sign;
	double second = 0;
	int year, month, day, hour = 0, minute = 0, tzhour = 0, tzmin = 0;
	if (swscanf(strTime, _T("%d-%d-%dT%d:%d:%lf%c%d:%d"), 
		&year, &month, &day, &hour, &minute, &second, &sign, &tzhour, &tzmin) < 6)
		return FALSE;

	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	st.wHour = hour;
	st.wMinute = minute;
	st.wSecond = (int) second;

	if (sign == '+')
		*time = TimeToGMT(&st, -tzhour, tzmin);
	else
		*time = TimeToGMT(&st, tzhour, tzmin);

	return TRUE;
}

BOOL DateToSystemTime(LPCTSTR strTime, SYSTEMTIME *time) {
	LOG0(5, "DateToSytemTime()");

	SYSTEMTIME st = { 0 };

	int day, month, year;
	int hour = 0, minute = 0, second = 0;
	int tzhour = 0, tzmin = 0;

	// try format: yyyy-mm-dd hh:mm:ss (violates RSS standard)
	if (swscanf(strTime, _T("%d-%d-%d %d:%d:%d"), &year, &month, &day, &hour, &minute, &second) < 6)
		return FALSE;

	st.wDay = day;
	st.wMonth = month;
	st.wYear = year;
	st.wHour = hour;
	st.wMinute = minute;
	st.wSecond = second;

	// use default time zone
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);
	tzhour = -(tzi.Bias / 60);
	tzmin = -(tzi.Bias % 60);

	*time = TimeToGMT(&st, tzhour, tzmin);

	return TRUE;
}

BOOL DateRfc822ToSystemTime(LPCTSTR strTime, SYSTEMTIME *time) {
	LOG0(5, "DateRfc822ToSystemTime()");

	SYSTEMTIME st = { 0 };
	TCHAR buffer[1024] = { 0 };
	lstrcpy(buffer, strTime);

	int day, year;
	int hour = 0, minute = 0, second = 0;
	int tzhour = 0, tzmin = 0;


	TCHAR seps[] = _T(" ");
	TCHAR *token = NULL;

	token = wcstok(buffer, seps);
	if (token == NULL) return FALSE;
	if (wcschr(token, ',') != NULL)
		token = wcstok(NULL, seps);

	// day
	if (swscanf(token, _T("%d"), &day) != 1) return FALSE;
	st.wDay = day;
	
	// month
	token = wcstok(NULL, seps);
	if (token == NULL) return FALSE;

	if (_wcsnicmp(token, _T("Jan"), 3) == 0) st.wMonth = 1;
	else if (_wcsnicmp(token, _T("Feb"), 3) == 0) st.wMonth = 2;
	else if (_wcsnicmp(token, _T("Mar"), 3) == 0) st.wMonth = 3;
	else if (_wcsnicmp(token, _T("Apr"), 3) == 0) st.wMonth = 4;
	else if (_wcsnicmp(token, _T("May"), 3) == 0) st.wMonth = 5;
	else if (_wcsnicmp(token, _T("Jun"), 3) == 0) st.wMonth = 6;
	else if (_wcsnicmp(token, _T("Jul"), 3) == 0) st.wMonth = 7;
	else if (_wcsnicmp(token, _T("Aug"), 3) == 0) st.wMonth = 8;
	else if (_wcsnicmp(token, _T("Sep"), 3) == 0) st.wMonth = 9;
	else if (_wcsnicmp(token, _T("Oct"), 3) == 0) st.wMonth = 10;
	else if (_wcsnicmp(token, _T("Nov"), 3) == 0) st.wMonth = 11;
	else if (_wcsnicmp(token, _T("Dec"), 3) == 0) st.wMonth = 12;
	else return FALSE;

	// year
	token = wcstok(NULL, seps);
	if (token == NULL) return FALSE;

	if (swscanf(token, _T("%d"), &year) != 1) return FALSE;
	if (wcslen(token) == 4) {
		st.wYear = year;
	}
	else {
		// Note: ugly, but simple
		st.wYear = 2000 + year;
	}

	// time
	token = wcstok(NULL, seps);
	if (token == NULL) {
		// HACK: no time specified (violates RSS specification)
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;

		// make time zone correction
		TIME_ZONE_INFORMATION tzi;
		GetTimeZoneInformation(&tzi);
		tzhour = -(tzi.Bias / 60);
		tzmin = -(tzi.Bias % 60);
	}
	else {
		swscanf(token, _T("%d:%d:%d"), &hour, &minute, &second);
		st.wHour = hour;
		st.wMinute = minute;
		st.wSecond = second;

		// convert to time zone
		token = wcstok(NULL, seps);
		if (token != NULL) {
			TCHAR sign;
			int tzh = 0, tzm = 0;
			if (swscanf(token, _T("%c%02d%02d"), &sign, &tzh, &tzm) == 3) {
				if (sign == '+') {
					tzhour = tzh;
					tzmin = tzm;
				}
				else {
					tzhour = -tzh;
					tzmin = -tzm;
				}
			}
			// textual representation of time zone
			else if (wcscmp(token, _T("GMT")) == 0) {
				tzhour = 0; tzmin = 0;
			}
			else if (wcscmp(token, _T("UT")) == 0) {
				tzhour = 0; tzmin = 0;
			}
			else if (wcscmp(token, _T("EST")) == 0) {
				tzhour = -5; tzmin = 00;
			}
			else if (wcscmp(token, _T("EDT")) == 0) {
				tzhour = -4; tzmin = 00;
			}
			else if (wcscmp(token, _T("CST")) == 0) {
				tzhour = -6; tzmin = 00;
			}
			else if (wcscmp(token, _T("CDT")) == 0) {
				tzhour = -5; tzmin = 00;
			}
			else if (wcscmp(token, _T("MST")) == 0) {
				tzhour = -7; tzmin = 00;
			}
			else if (wcscmp(token, _T("MDT")) == 0) {
				tzhour = -6; tzmin = 00;
			}
			else if (wcscmp(token, _T("PST")) == 0) {
				tzhour = -8; tzmin = 00;
			}
			else if (wcscmp(token, _T("PDT")) == 0) {
				tzhour = -7; tzmin = 00;
			}
			else {
				// use default time zone
				TIME_ZONE_INFORMATION tzi;
				GetTimeZoneInformation(&tzi);
				tzhour = -(tzi.Bias / 60);
				tzmin = -(tzi.Bias % 60);
			}
		}
		else {
			// use default time zone
			TIME_ZONE_INFORMATION tzi;
			GetTimeZoneInformation(&tzi);
			tzhour = -(tzi.Bias / 60);
			tzmin = -(tzi.Bias % 60);
		}
	}

	*time = TimeToGMT(&st, tzhour, tzmin);

	return TRUE;
}

BOOL ConvertToSytemTime(LPCTSTR strTime, SYSTEMTIME *time) {
	LOG1(5, "ConvertToSytemTime('%S')", strTime);

	if (DcDateToSytemTime(strTime, time))
		return TRUE;

	if (DateToSystemTime(strTime, time))
		return TRUE;

	if (DateRfc822ToSystemTime(strTime, time))
		return TRUE;

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedFile::CFeedFile() {
}

CFeedFile::~CFeedFile() {
}

CFeedFile::EType CFeedFile::GetType() {
	LOG0(5, "CFeedFile::GetType()");

	CFeedFile::EType type = Unknown;

	CString tagName = RootNode->GetName();
	if (tagName.Compare(_T("rss")) == 0)
		type = RSS;
	else if (tagName.Compare(_T("rdf:RDF")) == 0)
//	else if (tagName.Compare(_T("rdf")) == 0)
		type = RDF;
	else if (tagName.Compare(_T("feed")) == 0)
		type = Atom;

	return type;
}

CString CFeedFile::GetVersion() {
	LOG0(5, "CFeedFile::GetVersion()");

	CString version = _T("???");

	CString tagName = RootNode->GetName();
	if (tagName.Compare(_T("rss")) == 0) {
		POSITION pos = RootNode->GetFirstAttrPos();
		while (pos != NULL) {
			CXmlAttr *attr = RootNode->GetNextAttr(pos);
			if (attr->GetName().CompareNoCase(_T("version")) == 0) {
				version = attr->GetValue();
			}
		}
	}
	else if (tagName.Compare(_T("rdf:RDF")) == 0)
		version = _T("1.0");
	else if (tagName.Compare(_T("feed")) == 0) {
		POSITION pos = RootNode->GetFirstAttrPos();
		while (pos != NULL) {
			CXmlAttr *attr = RootNode->GetNextAttr(pos);
			if (attr->GetName().CompareNoCase(_T("version")) == 0) {
				version = attr->GetValue();
			}
		}
	}

	return version;
}


BOOL CFeedFile::IsValid() {
	BOOL ret = TRUE;

	return GetType() != Unknown;
}

BOOL CFeedFile::Parse(CFeed *feed, CSiteItem *si) {
	LOG0(5, "CFeedFile::Parse()");

	BOOL ret = FALSE;
	EType type = GetType();

	switch (type) {
		case RSS:
			ret = RSSFill(feed, si);
			break;

		case RDF:
			ret = RDFFill(feed, si);
			break;

		case Atom:
			ret = AtomFill(feed, si);
			break;

		default:
			return FALSE;
	} // switch

#if defined PRSSR_APP
	// fill milisecond field (crucial for proper sorting)
	for (int i = feed->GetItemCount() - 1; i >= 0; i--) {
		CFeedItem *item = feed->GetItem(i);
		item->Date.wMilliseconds = (WORD) (GetTickCount() & 0xFFFF);
		Sleep(1);
	}
#endif


#if defined PRSSR_APP
	CString version = GetVersion();
	// set format
	CString strFormat;
	switch (type) {
		case RSS: strFormat.Format(IDS_FEED_FMT, _T("RSS"), version); break;
		case RDF: strFormat.Format(IDS_FEED_FMT, _T("RDF"), version); break;
		case Atom: strFormat.Format(IDS_FEED_FMT, _T("Atom"), version); break;
	} // switch
	feed->Format = strFormat;
#endif

	return TRUE;
}

BOOL CFeedFile::RSSFillInfo(CXmlNode *parent, CFeed *feed) {
	LOG0(5, "CFeedFile::RSSFillInfo()");

	BOOL ret = FALSE;

	CString updatePeriod;
	int updateFrequency = -1;
	int ttl = -1;

	POSITION pos = parent->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *child = parent->GetNextChild(pos);

		CString name = child->GetName();
		if (name.Compare(_T("title")) == 0) {
			feed->Title = child->GetValue();
			ret = TRUE;
		}
#if defined PRSSR_APP
		else if (name.Compare(_T("description")) == 0)
			feed->Description = child->GetValue();
		else if (name.Compare(_T("link")) == 0)
			feed->HtmlUrl = child->GetValue();
		else if (name.Compare(_T("language")) == 0 || name.Compare(_T("dc:language")) == 0)
			feed->Language = child->GetValue();
		else if (name.Compare(_T("copyright")) == 0 || name.Compare(_T("dc:rights")) == 0)
			feed->Copyright = child->GetValue();
#endif
#if defined PRSSR_APP
		else if (name.Right(13).Compare(_T(":updatePeriod")) == 0) {
			updatePeriod = child->GetValue();
		}
		else if (name.Right(16).Compare(_T(":updateFrequency")) == 0) {
			swscanf(child->GetValue(), _T("%d"), &updateFrequency);
		}
		else if (name.Compare(_T("ttl")) == 0) {
			swscanf(child->GetValue(), _T("%d"), &ttl);
		}
#endif
	}

#if defined PRSSR_APP
	if (ttl != -1) {
		feed->UpdateInterval = ttl;
	}
	else {
		int mins = 0;
		if (updatePeriod.CompareNoCase(_T("hourly")) == 0)
			mins = 60;
		else if (updatePeriod.CompareNoCase(_T("daily")) == 0)
			mins = 1440;
		else if (updatePeriod.CompareNoCase(_T("weekly")) == 0)
			mins = 10080;
		else if (updatePeriod.CompareNoCase(_T("monthly")) == 0)
			mins = 44640;
		else if (updatePeriod.CompareNoCase(_T("yearly")) == 0)
			mins = 525600;

		feed->UpdateInterval = mins / updateFrequency;
	}
#endif

	return ret;
}

BOOL CFeedFile::RSSFillItem(CXmlNode *xmlItem, CFeedItem *item) {
	LOG0(5, "CFeedFile::RSSFillItem()");

#if defined PRSSR_APP
	CString sDescription, sContent;
#endif

	POSITION pos = xmlItem->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *childNode = xmlItem->GetNextChild(pos);

		CString name = childNode->GetName();
		CString value = childNode->GetXML();
		if (name.Compare(_T("title")) == 0)
			item->Title = value;
#if defined PRSSR_APP
		else if (name.Compare(_T("description")) == 0)
			sDescription = value;
		else if (name.Compare(_T("content:encoded")) == 0)
			sContent = value;
#endif
#if defined PRSSR_APP
		else if (name.Compare(_T("dc:creator")) == 0 || name.Compare(_T("author")) == 0) {
			item->Author = value;
		}
#endif
		else if (name.Compare(_T("link")) == 0)
			item->Link = value;
		else if (name.Compare(_T("pubDate")) == 0 || name.Compare(_T("date")) == 0 || name.Compare(_T("dc:date")) == 0) {
			SYSTEMTIME st = { 0 };
			if (ConvertToSytemTime(value, &st))
				item->PubDate = st;
		}

#if defined PRSSR_APP
		// enclosure tag has no data in
		if (name.Compare(_T("enclosure")) == 0) {
			CEnclosureItem *enclosure = new CEnclosureItem;
			if (RSSFillEnclosure(childNode, enclosure)) {
				item->Enclosures.AddTail(enclosure);
			}
			else {
				delete enclosure;
			}
		}
#endif

	}

#if defined PRSSR_APP
	if (!sContent.IsEmpty())
		item->Description = sContent;
	else
		item->Description = sDescription;
#endif

#if defined PRSSR_APP
	// create date for proper sorting
	if (item->PubDate.wYear == 0)
		GetSystemTime(&item->PubDate);

	item->Date = item->PubDate;
	Sleep(1);
#endif

	return TRUE;
}

//
// RDF specific methods
//

BOOL CFeedFile::RDFFill(CFeed *feed, CSiteItem *si) {
	LOG0(5, "CFeedFile::RDFFill()");

	BOOL ret = FALSE;

	CList<CFeedItem *, CFeedItem *> feedList;

	POSITION pos = RootNode->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *child = RootNode->GetNextChild(pos);

		CString tagName = child->GetName();
		if (tagName.Compare(_T("channel")) == 0) {
			ret = RSSFillInfo(child, feed);
		}
		else if (tagName.Compare(_T("item")) == 0) {
			CFeedItem *item = new CFeedItem(si);
			if (RSSFillItem(child, item)) {
				item->ComputeHash();
				feedList.AddTail(item);
			}
			else
				delete item;
		}
	}

	// copy from temp list to feed
	feed->SetSize(feedList.GetCount());
	int idx = 0;
	while (!feedList.IsEmpty())
		feed->SetAt(idx++, feedList.RemoveHead());

	return ret;
}

//
// RSS specific methods
//

BOOL CFeedFile::RSSFill(CFeed *feed, CSiteItem *si) {
	LOG0(5, "CFeedFile::RSSFill()");

	BOOL ret = FALSE;

	// get channel tag
	POSITION pos = RootNode->GetFirstChildPos();
	if (pos != NULL) {
		CXmlNode *channel = RootNode->GetNextChild(pos);

		CString tagName = channel->GetName();
		if (tagName.Compare(_T("channel")) == 0) {
			ret = RSSFillInfo(channel, feed);

			// item list
			CList<CFeedItem *, CFeedItem *> feedList;
			POSITION posItem = channel->GetFirstChildPos();
			while (posItem != NULL) {
				CXmlNode *child = channel->GetNextChild(posItem);

				CString tagName = child->GetName();
				if (tagName.Compare(_T("item")) == 0) {
					CFeedItem *item = new CFeedItem(si);
					if (RSSFillItem(child, item)) {
						item->ComputeHash();
						feedList.AddTail(item);
					}
					else
						delete item;
				}
			}

			// copy from temp list to feed
			feed->SetSize(feedList.GetCount());
			int idx = 0;
			while (!feedList.IsEmpty())
				feed->SetAt(idx++, feedList.RemoveHead());
		}
	}

	return ret;
}

#if defined PRSSR_APP

BOOL CFeedFile::RSSFillEnclosure(CXmlNode *xmlElem, CEnclosureItem *enclosure) {
	LOG0(5, "CFeedFile::RSSFillEnclosure()");

	BOOL ret = FALSE;

	CString url, length, type;
	POSITION pos = xmlElem->GetFirstAttrPos();
	while (pos != NULL) {
		CXmlAttr *attr = xmlElem->GetNextAttr(pos);

		CString attrName = attr->GetName();
		if (attrName.Compare(_T("url")) == 0) {
			url = attr->GetValue();
		}
		else if (attrName.Compare(_T("length")) == 0) {
			length = attr->GetValue();
		}
		else if (attrName.Compare(_T("type")) == 0) {
			type = attr->GetValue();
		}

	}

	if (length.IsEmpty())
		length = _T("0");

	// set read values
	if (!url.IsEmpty() && !type.IsEmpty()) {
		enclosure->URL = url;
		long l;
		swscanf(length, _T("%ld"), &l);
		if (l == -1) l = 0;
		enclosure->Length = l;

		int nPos = type.Find('/');
		if (nPos != -1) {
			CString sType = type.Left(nPos);
			if (sType.CompareNoCase(_T("video")) == 0) enclosure->Type = ENCLOSURE_TYPE_VIDEO;
			else if (sType.CompareNoCase(_T("audio")) == 0) enclosure->Type = ENCLOSURE_TYPE_AUDIO;
			else if (sType.CompareNoCase(_T("image")) == 0) enclosure->Type = ENCLOSURE_TYPE_IMAGE;
			else enclosure->Type = ENCLOSURE_TYPE_UNKNOWN;
		}
		else enclosure->Type = ENCLOSURE_TYPE_UNKNOWN;

		ret = TRUE;
	}

	return ret;
}

#endif

//
// Atom specific methods
//

BOOL CFeedFile::AtomFill(CFeed *feed, CSiteItem *si) {
	LOG0(5, "CFeedFile::AtomFill()");

	BOOL ret = AtomFillInfo(RootNode, feed);

	// item list
	CList<CFeedItem *, CFeedItem *> feedList;
	POSITION posItem = RootNode->GetFirstChildPos();
	while (posItem != NULL) {
		CXmlNode *child = RootNode->GetNextChild(posItem);

		CString tagName = child->GetName();
		if (tagName.Compare(_T("entry")) == 0) {
			CFeedItem *item = new CFeedItem(si);
			if (AtomFillItem(child, item)) {
				item->ComputeHash();
				feedList.AddTail(item);
			}
			else
				delete item;
		}
	}

	// copy from temp list to feed
	feed->SetSize(feedList.GetCount());
	int idx = 0;
	while (!feedList.IsEmpty())
		feed->SetAt(idx++, feedList.RemoveHead());

	return ret;
}


BOOL CFeedFile::AtomFillInfo(CXmlNode *xmlElem, CFeed *feed) {
	LOG0(5, "CFeedFile::AtomFillInfo()");

	BOOL ret = FALSE;

	POSITION pos = xmlElem->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *child = xmlElem->GetNextChild(pos);

		CString name = child->GetName();
		if (name.Compare(_T("title")) == 0) {
			feed->Title = child->GetValue();
			ret = TRUE;
		}
#if defined PRSSR_APP
		else if (name.Compare(_T("copyright")) == 0)
			feed->Copyright = child->GetValue();
		else if (name.Compare(_T("tagline")) == 0 || name.Compare(_T("subtitle")) == 0)
			feed->Description = child->GetValue();
		else if (name.Compare(_T("link")) == 0) {
			CString rel, type, href;
			POSITION posAttr = child->GetFirstAttrPos();
			while (posAttr != NULL) {
				CXmlAttr *attr = child->GetNextAttr(posAttr);

				CString attrName = attr->GetName();
				if (attrName.Compare(_T("rel")) == 0)
					rel = attr->GetValue();
				else if (attrName.Compare(_T("type")) == 0)
					type = attr->GetValue();
				else if (attrName.Compare(_T("href")) == 0)
					href = attr->GetValue();
			}


			// href value of tag with attribute type="text/html" and rel="alternate" (0.3) or rel empty)
			if ((rel.Compare(_T("alternate")) == 0 || rel.IsEmpty()) &&
				type.Compare(_T("text/html")) == 0)
				feed->HtmlUrl = href;
		}
#endif
	}

	return ret;
}

#ifdef PRSSR_APP
BOOL CFeedFile::AtomFillPerson(CXmlNode *xmlItem, CString &name) {
	LOG0(5, "CFeedFile::AtomFillPerson()");

	POSITION pos = xmlItem->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *child = xmlItem->GetNextChild(pos);

		CString tag = child->GetName();
		if (tag.Compare(_T("name")) == 0) {
			name = child->GetValue();
		}
	}

	return TRUE;
}
#endif

BOOL CFeedFile::AtomFillItem(CXmlNode *xmlItem, CFeedItem *item) {
	LOG0(5, "CFeedFile::AtomFillItem()");

	SYSTEMTIME issued = { 0 };
	SYSTEMTIME created = { 0 };
	SYSTEMTIME modified = { 0 };
	SYSTEMTIME updated = { 0 };
	SYSTEMTIME published = { 0 };
	CString summary, content;

	POSITION pos = xmlItem->GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *child = xmlItem->GetNextChild(pos);

		CString name = child->GetName();

		if (name.Compare(_T("title")) == 0) {
			item->Title = child->GetValue();
		}
#if defined PRSSR_APP
		else if (wcscmp(name, _T("content")) == 0) {
			content = child->GetXML();
		}
		else if (wcscmp(name, _T("summary")) == 0) {
			summary = child->GetXML();
		}
#endif
#if defined PRSSR_APP
		else if (wcscmp(name, _T("author")) == 0) {
			AtomFillPerson(child, item->Author);
		}
#endif
		else if (wcscmp(name, _T("link")) == 0) {
			CString rel, type, href;
			POSITION posAttr = child->GetFirstAttrPos();
			while (posAttr != NULL) {
				CXmlAttr *attr = child->GetNextAttr(posAttr);

				CString attrName = attr->GetName();
				if (attrName.Compare(_T("rel")) == 0)
					rel = attr->GetValue();
				else if (attrName.Compare(_T("type")) == 0)
					type = attr->GetValue();
				else if (attrName.Compare(_T("href")) == 0)
					href = attr->GetValue();
			}


			// href value of tag with attributes rel="alternate" and type="text/html"
			if (rel.Compare(_T("alternate")) == 0 && type.Compare(_T("text/html")) == 0)
				item->Link = href;
		}
		else if (name.Compare(_T("issued")) == 0) {
			ConvertToSytemTime(child->GetValue(), &issued);
		}
		else if (name.Compare(_T("updated")) == 0) {
			ConvertToSytemTime(child->GetValue(), &updated);
		}
		else if (name.Compare(_T("modified")) == 0) {
			ConvertToSytemTime(child->GetValue(), &modified);
		}
		else if (name.Compare(_T("created")) == 0) {
			ConvertToSytemTime(child->GetValue(), &created);
		}
		else if (name.Compare(_T("published")) == 0) {
			ConvertToSytemTime(child->GetValue(), &published);
		}
	}

	if (issued.wYear != 0) {
		item->PubDate = issued;
	}
	else if (updated.wYear != 0) {
		item->PubDate = updated;
	}
	else if (modified.wYear != 0) {
		item->PubDate = modified;
	}
	else if (published.wYear != 0) {
		item->PubDate = updated;
	}
	else if (created.wYear != 0) {
		item->PubDate = created;
	}

#if defined PRSSR_APP
	// create date for proper sorting
	if (item->PubDate.wYear == 0)
		GetSystemTime(&item->PubDate);

	item->Date = item->PubDate;
	Sleep(1);
#endif

#if defined PRSSR_APP
	// if <content> is empty, then use <summary>
	if (!content.IsEmpty())
		item->Description = content;
	else
		item->Description = summary;
#endif

	return TRUE;
}

