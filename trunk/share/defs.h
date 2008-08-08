/**
 *  defs.h
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

#ifndef _DEFS_H_
#define _DEFS_H_

#define PRSSREADER_BIN						L"prssr.exe"
#define PRSSRNOT_BIN						L"\\windows\\prssrnot.exe"

#define UWM_DIALOG_CLOSE					(WM_USER + 0x0001)
#define UWM_SHOW_UPDATEBAR					(WM_USER + 0x0002)
#define UWM_UPDATE_FEED						(WM_USER + 0x0003)
#define UWM_UPDATE_FAVICON					(WM_USER + 0x0005)
#define UWM_UPDATE_ALL						(WM_USER + 0x0006)

#define UWM_UPDATE_FINISHED					(WM_USER + 0x0007)

// shared user-defined messages
#define UWM_OPEN_SITE						(WM_USER + 0x0008)
#define UWM_OPEN_FEEDITEM					(WM_USER + 0x0009)

//
#define UWM_GROUP_CHECKED					(WM_USER + 0x000A)

/*

// custom messages
#define UPDATE_LABEL_MESSAGE        (WM_USER + 0x0014)
#define INSERT_SITES_MESSAGE        (WM_USER + 0x0015)
#define SET_NEW_MESSAGE             (WM_USER + 0x0016)
#define SHOW_UPDATEBAR_MESSAGE      (WM_USER + 0x0018)
#define UPDATE_SORT_MESSAGE			(WM_USER + 0x0019)

#define UWM_SETDOWNLOADSTATUSTEXT	(WM_USER + 0x0020)
#define UWM_SETSTATUSTEXT			(WM_USER + 0x0021)
#define UWM_SETDONE					(WM_USER + 0x0022)
#define UWM_ADDERROR				(WM_USER + 0x0023)
#define UWM_SETITEMSTATE			(WM_USER + 0x0024)

#define UWM_STEP					(WM_USER + 0x0026)
#define UWM_SETRANGE				(WM_USER + 0x0027)
#define UWM_SETREMAIN				(WM_USER + 0x0028)
#define UWM_SETPOS					(WM_USER + 0x0029)
#define UWM_START					(WM_USER + 0x002A)
#define UWM_END						(WM_USER + 0x002B)
#define UWM_UPDATE_FEED				(WM_USER + 0x002C)
#define UWM_FINISH					(WM_USER + 0x002D)
#define UWM_ADDITEM					(WM_USER + 0x002E)
#define UWM_REFRESH_VIEW			(WM_USER + 0x002F)
#define UWM_CHECK_CHANGED			(WM_USER + 0x0030)

// today plugin
#define UWM_RELOAD_OPML				(WM_USER + 0x0050)
#define UWM_RELOAD_FEED				(WM_USER + 0x0051)
*/

#define CACHE_LIMIT_DEFAULT						-2
#define CACHE_LIMIT_DISABLED					0
#define CACHE_LIMIT_UNLIMITED					-1

#define UPDATE_INTERVAL_GLOBAL					-1
#define UPDATE_INTERVAL_NO_AUTO					-2

// messages
#define READ_CONFIG_MESSAGE						L"DaProfik::pRSSreader::Messages::ReadConfig"
#define RELOAD_SUBSCRIPTIONS_MESSAGE			L"DaProfik::pRSSreader::Messages::ReloadSubscriptions"
#define CHECK_FEEDS_MESSAGE						L"DaProfik::pRSSreader::Messages::CheckFeeds"
#define UPDATE_FEED_FLAGS_MESSAGE				L"DaProfik::pRSSreader::Messages::UpdateFeedFlags"

// events
//#define CRADLING_EVENT							L"DaProfik::pRSSreader::Events::CradlingEvent"

#define TIMER_KEY_CTX_MENU						250

// shared config items
#define CONFIG_DEFAULT_SHOWRELATIVEDATES		TRUE
#define CONFIG_DEFAULT_SHOWRELATIVETIMES		FALSE
#define CONFIG_DEFAULT_GMTTIMES					FALSE
//#define CONFIG_DEFAULT_DATE_FORMAT				_T("d. M. yyyy")

extern LPCTSTR szInstallDir;
extern LPCTSTR szCacheLocation;

extern LPCTSTR szShowRelativeDates;
extern LPCTSTR szShowRelativeTimes;
extern LPCTSTR szGMTTimes;
extern LPCTSTR szDateFormat;

extern LPCTSTR szNewItems;
extern LPCTSTR szUnreadItems;
extern LPCTSTR szTotalItems;

extern LPCTSTR szEnabled;
extern LPCTSTR szTodayShow;

extern LPCTSTR szShowNewChannels;

extern LPCTSTR szBackgroundUpdate;

extern LPCTSTR szBackgroundUpdate;
extern LPCTSTR szUpdateAtTime;
extern LPCTSTR szUpdateInterval;

#endif
