/**
 *  Config.h
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

#if !defined(AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_)
#define AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ESyncSite {
	SYNC_SITE_NONE,
	SYNC_SITE_GOOGLE_READER,
	SYNC_SITE_GOOGLE_READERCL						// google clientlogin auth
};

enum ESelectedLanguage {
	SELECTED_LANGUAGE_RU_RU,
	SELECTED_LANGUAGE_UK_UA,
	SELECTED_LANGUAGE_CS_CZ,
	SELECTED_LANGUAGE_ET_EE,
	SELECTED_LANGUAGE_LV_LV,
	SELECTED_LANGUAGE_PL_PL,
	SELECTED_LANGUAGE_DE_DE,
	SELECTED_LANGUAGE_FR_FR,
	SELECTED_LANGUAGE_IT_IT,
	SELECTED_LANGUAGE_ES_SP,
	SELECTED_LANGUAGE_EN_US
};

static CString AcceptLanguage[] = {
	_T("ru-RU,ru;q=0.8"),
	_T("uk-UA,ua;q=0.8"),
	_T("cs-CZ,cs;q=0.8"),
	_T("et-EE,et;q=0.8"),
	_T("lv-LV,lv;q=0.8"),
	_T("pl-PL,pl;q=0.8"),
	_T("de-DE,de;q=0.8"),
    _T("fr-FR,fr;q=0.8"),
	_T("it-IT,fr;q=0.8"),
	_T("es-SP,es;q=0.8"),
	_T("en-US,en;q=0.8")
};

// Playlist format
#define PLAYLIST_FORMAT_ASX							0
#define PLAYLIST_FORMAT_M3U							1

// Main View
#define MAIN_VIEW_FEED_LIST							0
#define MAIN_VIEW_SUMMARY_VIEW						1
#define MAIN_VIEW_ARTICLE							2

#define NAVIGATION_TOUCH							0
#define NAVIGATION_NORMAL							1

// config
#define CONFIG_DEFAULT_CACHE_LIMIT					50

#define CONFIG_DEFAULT_CHECKONCRADLING				FALSE
#define CONFIG_DEFAULT_UPDATEINTERVAL				0
#define CONFIG_DEFAULT_AUTOCONNECT					FALSE
#define CONFIG_DEFAULT_USE_CONN_MANAGER				TRUE
#define CONFIG_DEFAULT_TIMEOUT						7000		// 7 seconds

#define CONFIG_DEFAULT_MOVETOUNREAD					FALSE

#define CONFIG_DEFAULT_NOTIFYNEW                    TRUE
#define CONFIG_DEFAULT_WRAPTITLES                   TRUE

// sync
#define CONFIG_DEFAULT_TRANSLATE_LANGUAGE			FALSE

// GUI
#define CONFIG_DEFAULT_MAIN_VIEW					MAIN_VIEW_SUMMARY_VIEW
#define CONFIG_DEFAULT_ACTSITEIDX					-1
#define CONFIG_DEFAULT_WORK_OFFLINE					FALSE
#define CONFIG_DEFAULT_HIDE_GROUPS					FALSE
#define CONFIG_DEFAULT_HIDE_READ_ITEMS				FALSE
#define CONFIG_DEFAULT_LEAVE_ONLY_KEYWORD			FALSE
#define CONFIG_DEFAULT_UPDATE_ONLY					FALSE
#define CONFIG_DEFAULT_BACKGROUNDUPDATE				TRUE
#define CONFIG_DEFAULT_NAVIGATION_TYPE				NAVIGATION_TOUCH
#define CONFIG_DEFAULT_EMAIL_ACCOUNT				_T("ActiveSync")
#define CONFIG_DEFAULT_GROUP_BY_FEED				FALSE
#define CONFIG_DEFAULT_DISABLE_GESTURES				FALSE

#define CONFIG_DEFAULT_CLEARERRORLOG				TRUE

#define CONFIG_DEFAULT_CACHE_IMAGES					FALSE
#define CONFIG_DEFAULT_CACHE_HTML					FALSE

#define CONFIG_DEFAULT_SYNCSITE						SYNC_SITE_NONE
#define CONFIG_DEFAULT_SELECTEDLANGUAGE				SELECTED_LANGUAGE_RU_RU

#define CONFIG_DEFAULT_GENERATEPLAYLISTS			FALSE
#define CONFIG_DEFAULT_ENCLOSUREPLAYLISTFORMAT		PLAYLIST_FORMAT_ASX

#define CONFIG_DEFAULT_SEARCHLIMIT					20

#define CONFIG_DEFAULT_USE_HTML_OPTIMIZER			FALSE

#define CONFIG_DEFAULT_TIME_UPDATE					FALSE

// today
#define CONFIG_DEFAULT_SHOWNEWCHANNELS				TRUE

// TWEAKS
#define CONFIG_DEFAULT_MINIMALSYNCINTERVAL			0
#define CONFIG_DEFAULT_SHOWMAINTOOLBAR				FALSE
#define CONFIG_DEFAULT_CHECKCERTIFICATES			TRUE
#define CONFIG_DEFAULT_STARTINSUMMARYVIEW			FALSE
#define CONFIG_DEFAULT_FEEDPRELOAD					FEED_PRELOAD_NEIGHBOURS
#define CONFIG_DEFAULT_REPORTERRORSAFTERUPDATE		TRUE
#define CONFIG_DEFAULT_OPENMSGWITHHTMLOPTIMIZER		TRUE
#define CONFIG_DEFAULT_HTMLOPTAFTERURLREWRITING		FALSE
#define CONFIG_DEFAULT_MSG_SCROLL_THROUGHT			TRUE
#define CONFIG_DEFAULT_SETPROPERTIESAFTERSITEADDED	TRUE
#define CONFIG_DEFAULT_WRAPAROUND					FALSE


#ifdef LOGGING
	#if defined PRSSR_APP
		#define CONFIG_DEFAULT_LOGFILE						_T("\\prssr.log")
		#define CONFIG_DEFAULT_LOGLEVEL						0
	#endif
#endif

#define USER_AGENT_STRING	_T("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)") //_T("Mozilla/4.0 (compatible; MSIE 4.01; Windows CE; PPC; %dx%d) pRSSreader/%d.%d.%d")

#include "net/proxy.h"
#include "../share/helpers.h"

struct CRewriteRule;

struct CProxyProfile {
	CString Name;
	CProxy ProxyConfig;
};


struct CSocialBookmarkSite {
	CString Name;
	CString Url;
};

//
//
//

class CConfig {
public:
#ifdef LOGGING
	CString LogFile;
	DWORD LogLevel;
#endif

	CString InstallDir;

	// GUI
	// main window
	int ActSiteIdx;
	int ActFeedItem;
	BOOL WorkOffline;
	int MainView;
	BOOL HideGroups;
	BOOL HideReadItems;
	BOOL LeaveOnlyKeyword;
	BOOL UpdateOnly;
	CString EmailAccount;					// email service used for sending emails (ActiveSync by default)
	BOOL DisableGestures; 
	
	////

	BOOL NotifyNew;
	BOOL WrapAround;
	BOOL WrapTitles;
	int NavigationType;
	BOOL GroupByFeed;

	// cache
	CString CacheLocation;
	int CacheLimit;

	BOOL MoveToUnread;

	//
	CString UserAgent;
	CStringArray AdditionalHttpHeaders;			// RO value

	BOOL ShowRelativeDates;

	//
	int UpdateInterval;
	BOOL AutoConnect;
	BOOL UseConnManager;
	BOOL CheckOnCradling;
	BOOL ClearErrorLog;
	BOOL BackgroundUpdate;

	BOOL TimeUpdate;
	SYSTEMTIME UpdateAtTime;

	//
	BOOL CacheImages;
	BOOL CacheHtml;

	// sync
	ESyncSite SyncSite;
	ESelectedLanguage SelectedLanguage;
	CString SyncUserName;
	CString SyncPassword;
	BOOL TranslateLanguage;

	// enclosures
	BOOL GeneratePlaylists;
	int EnclosurePlaylistFormat;

	// HTML optimizer
	BOOL UseHtmlOptimizer;
	CString HtmlOptimizerURL;
	BOOL AdvancedHtmlOptimizer;

	CStringArray Keywords;
	CArray<CRewriteRule *, CRewriteRule *> RewriteRules;

	// proxy profiles
	int ProxyProfileIdx;
	CArray<CProxyProfile *, CProxyProfile *> ProxyProfiles;

	CArray<CSocialBookmarkSite *, CSocialBookmarkSite *> SocialBookmarkSites;

	// TWEAKS
	CString DateFormat;						// custom date format
	int SearchLimit;						// number of items in search results
	BOOL MsgScrollThrought;					// scroll through when reading messages
	DWORD MinimalSyncInterval;				// minimal sync interval between cradling events
	BOOL SetPropertiesAfterSiteAdded;		// site man
	BOOL ShowMainToolbar;					// show toolbar on the main window
	BOOL CheckCertificates;					// check certificates when connection to secured sites
	DWORD Timeout;
	BOOL ReportErrorsAfterUpdate;			// Report errors after update if there are any
	BOOL OpenMsgWithHtmlOptimizer;
	BOOL HtmlOptAfterUrlRewriting;			// apply html optimizer on rewritten urls

#ifdef PRSSR_APP
	// HW keys
	CMap<int, int, UINT, UINT> HwKeysCmd;		// VirtualKey -> ID_COMMAND
	CMap<UINT, UINT, int, int> CmdsHwKey;		// ID_COMMAND -> Virtual Key
#endif

	CConfig();
	virtual ~CConfig();
	void Destroy();

	void Save();
	void Load();

	void SaveProxyProfiles();
	void LoadProxyProfiles();

	void SaveSocialBookmarkingSites();
	void LoadSocialBookmarkingSites();

	void SaveRewriteRules();
	void LoadRewriteRules();

	void SaveKeywords();
	void LoadKeywords();

	void SaveUI();
	void LoadUI();
};

extern CConfig Config;

#endif // !defined(AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_)
