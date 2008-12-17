/**
 *  Config.cpp
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

#if defined PRSSR_APP
	#include "StdAfx.h"
	#include "prssr.h"
	#include "../share/resource.h"
	#include "resource.h"
#endif


#include "Config.h"
#include "Appearance.h"
#include "../share/defs.h"
#include "../share/reg.h"
#include "net/proxy.h"
#include "www/url.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#define USER_AGENT_STRING										_T("Mozilla/4.0 (compatible; MSIE 4.01; Windows CE; PPC; %dx%d) pRSSreader/%d.%d.%d")

CConfig Config;

LPCTSTR szAdditionalHttpHeaders = _T("Additional HTTP Headers");
LPCTSTR szHTTPRefer = _T("HTTP Refer");
LPCTSTR szStoreItemImages = _T("Store Item Images");
LPCTSTR szCacheLimit = _T("Cache Limit");

// GUI
LPCTSTR szActSiteIdx = _T("ActiveSiteIdx");
LPCTSTR szActFeedItem = _T("ActiceFeedItem");

LPCTSTR szHideReadItems = _T("Hide Read Items");
LPCTSTR szHideReadFeeds = _T("Hide Read Feeds");
LPCTSTR szSortType = _T("Sort Type");
LPCTSTR szSortItem = _T("Sort Item");
LPCTSTR szCacheManPage = _T("CacheManPage");
LPCTSTR szHideGroups = _T("Hide Groups");
LPCTSTR szShowFlaggedItems = _T("Show Flagged Items");
LPCTSTR szMainView = _T("Main View");
LPCTSTR szWorkOffline = _T("Work Offline");
LPCTSTR szUpdateOnly = _T("Update Only");

LPCTSTR szSetPropertiesAfterSiteAdded = _T("Set Properties After Site Added");

// general
LPCTSTR szNotifyNew = _T("Notify New");
LPCTSTR szWrapTitles = _T("Wrap Titles");
LPCTSTR szWrapAround = _T("Wrap Around");

LPCTSTR szTruncateItemTitles = _T("Truncate Item Titles");
LPCTSTR szDownloadThreads = _T("Download Threads");
LPCTSTR szMoveToUnread = _T("Move To Unread");
LPCTSTR szMoveChannel = _T("Move Channel");
LPCTSTR szWarnFeedMove = _T("Warn Feed Move");
LPCTSTR szNavigationType = _T("Navigation Type");
LPCTSTR szEmailAccount = _T("Email Account");
LPCTSTR szGroupByFeed = _T("Group By Feed");

// read options
LPCTSTR szNewSeconds = _T("New Seconds");
LPCTSTR szShowSiteNames = _T("Show Site Name");
LPCTSTR szActionButton = _T("Action Button");

LPCTSTR szUpdateIntervalIdx = _T("Update Interval");
LPCTSTR szAutoConnect = _T("Auto Connect");
LPCTSTR szUseConnMan = _T("Use Conn Manager");
LPCTSTR szClearErrorLog = _T("Clear Error Log");
LPCTSTR szMarkNewUnread = _T("Mark New Unread");
LPCTSTR szUserAgent = _T("User-Agent");
LPCTSTR szCheckOnCradling = _T("Check On Cradling");

LPCTSTR szTimeUpdate = _T("Time Update");

//
LPCTSTR szCacheImages = _T("Cache Images");
LPCTSTR szCacheHtml = _T("Cache Html");

// sync
LPCTSTR szSyncSite = _T("Sync Site");
LPCTSTR szSyncUserName = _T("Sync UserName");
LPCTSTR szSyncPassword = _T("Sync Password");

// enclosures
LPCTSTR szGeneratePlaylists = _T("Generate Playlists");
LPCTSTR szEnclosurePlaylistFormat = _T("Enclosure Playlist Format");

// html optimizer
LPCTSTR szUseHtmlOptimizer = _T("Use HTML Optimizer");
LPCTSTR szHtmlOptimizerURL = _T("HTML Optimizer URL");

static LPCTSTR szMatch = _T("Match");
static LPCTSTR szReplace = _T("Replace");


#ifdef LOGGING
// logging feature
LPCTSTR szLogFile = _T("LogFile");
LPCTSTR szLogLevel = _T("LogLevel");
#endif

// tweaks
LPCTSTR szMinimalSyncInterval = _T("Minimal Sync Interval");
LPCTSTR szShowMainToolbar = _T("Show Main Toolbar");
LPCTSTR szSearchLimit = _T("Search Limit");
LPCTSTR szMsgScrollThrought = _T("Msg Scroll Throught");
LPCTSTR szCheckCertificates = _T("Check Certificates");
LPCTSTR szStartInSummaryView = _T("Start In Summary View");
LPCTSTR szCacheHtmlImages = _T("Cache Html Images");
LPCTSTR szFeedPreload = _T("Feed Preload");
LPCTSTR szTimeout = _T("Timeout");
LPCTSTR szReportErrorsAfterUpdate = _T("Report Errors After Update");
LPCTSTR szOpenMsgWithHtmlOptimizer = _T("Open Msg With Html Optimizer");
LPCTSTR szHtmlOptAfterUrlRewriting = _T("HtmlOpt After Url Rewriting");

#ifdef PRSSR_APP
// hw keys
LPCTSTR szHwKeys = _T("HwKeys");
LPCTSTR szOpenEnclosure = _T("Open Enclosure");
LPCTSTR szOpenSiteList = _T("Open Site List");
LPCTSTR szSummaryView = _T("Summary View");
LPCTSTR szUpdateChannels = _T("Update Channels");
#endif

// proxy profile
LPCTSTR szProxyProfiles = _T("Proxy Profiles");

LPCTSTR szActive = _T("Active");
LPCTSTR szName = _T("Name");
LPCTSTR szHostName = _T("Host Name");
LPCTSTR szPort = _T("Port");
LPCTSTR szUserName = _T("User Name");
LPCTSTR szPassword = _T("Password");
LPCTSTR szType = _T("Type");
LPCTSTR szNeedAuth = _T("Need Authentication");

// social bookmarking
LPCTSTR szUrl = _T("Url");


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_APP);

	InstallDir = reg.Read(szInstallDir, _T(""));
#ifdef LOGGING
	LogFile = reg.Read(szLogFile, CONFIG_DEFAULT_LOGFILE);
	LogLevel = reg.Read(szLogLevel, CONFIG_DEFAULT_LOGLEVEL);
//	LogFile = _T("\\storage card\\program files\\prssreader\\prssr.log");
//	LogLevel = 1;
#endif

	// ////

	UserAgent.Format(USER_AGENT_STRING, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN), VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	NotifyNew = CONFIG_DEFAULT_NOTIFYNEW;
	WrapTitles = CONFIG_DEFAULT_WRAPTITLES;
	NavigationType = CONFIG_DEFAULT_NAVIGATION_TYPE;
	EmailAccount = CONFIG_DEFAULT_EMAIL_ACCOUNT;
	GroupByFeed = CONFIG_DEFAULT_GROUP_BY_FEED;

	CacheLocation = _T("");
	CacheLimit = CONFIG_DEFAULT_CACHE_LIMIT;

	ShowRelativeDates = CONFIG_DEFAULT_SHOWRELATIVEDATES;

	MoveToUnread = CONFIG_DEFAULT_MOVETOUNREAD;

	UpdateInterval = CONFIG_DEFAULT_UPDATEINTERVAL;
	AutoConnect = CONFIG_DEFAULT_AUTOCONNECT;
	UseConnManager = CONFIG_DEFAULT_USE_CONN_MANAGER;
	CheckOnCradling = CONFIG_DEFAULT_CHECKONCRADLING;
	ClearErrorLog = CONFIG_DEFAULT_CLEARERRORLOG;
	TimeUpdate = CONFIG_DEFAULT_TIME_UPDATE;
	ZeroMemory(&UpdateAtTime, sizeof(UpdateAtTime));
	UpdateAtTime.wHour = 8;
	UpdateAtTime.wMinute = 0;
	UpdateAtTime.wSecond = 0;

	SyncSite = CONFIG_DEFAULT_SYNCSITE;

	// enclosure
	GeneratePlaylists = CONFIG_DEFAULT_GENERATEPLAYLISTS;
	EnclosurePlaylistFormat = CONFIG_DEFAULT_ENCLOSUREPLAYLISTFORMAT;

	// HTML optimizer
	UseHtmlOptimizer = CONFIG_DEFAULT_USE_HTML_OPTIMIZER;

	// TWEAKS
//	DateFormat = CONFIG_DEFAULT_DATE_FORMAT;
	SearchLimit = CONFIG_DEFAULT_SEARCHLIMIT;
	MsgScrollThrought = CONFIG_DEFAULT_MSG_SCROLL_THROUGHT;
	MinimalSyncInterval = CONFIG_DEFAULT_MINIMALSYNCINTERVAL;
	SetPropertiesAfterSiteAdded = CONFIG_DEFAULT_SETPROPERTIESAFTERSITEADDED;
	ShowMainToolbar = CONFIG_DEFAULT_SHOWMAINTOOLBAR;
	CheckCertificates = CONFIG_DEFAULT_CHECKCERTIFICATES;
	Timeout = CONFIG_DEFAULT_TIMEOUT;
	ReportErrorsAfterUpdate = CONFIG_DEFAULT_REPORTERRORSAFTERUPDATE;
	OpenMsgWithHtmlOptimizer = CONFIG_DEFAULT_OPENMSGWITHHTMLOPTIMIZER;
	HtmlOptAfterUrlRewriting = CONFIG_DEFAULT_HTMLOPTAFTERURLREWRITING;
	WrapAround = CONFIG_DEFAULT_WRAPAROUND;

	MainView = CONFIG_DEFAULT_MAIN_VIEW;
	WorkOffline = CONFIG_DEFAULT_WORK_OFFLINE;
	HideGroups = CONFIG_DEFAULT_HIDE_GROUPS;
	HideReadItems = CONFIG_DEFAULT_HIDE_READ_ITEMS;
	UpdateOnly = CONFIG_DEFAULT_UPDATE_ONLY;
}

CConfig::~CConfig() {
	for (int i = 0; i < RewriteRules.GetSize(); i++)
		delete RewriteRules[i];
}

void CConfig::Destroy() {
	int i;

	for (i = 0; i < ProxyProfiles.GetSize(); i++)
		delete ProxyProfiles[i];

	for (i = 0; i < SocialBookmarkSites.GetSize(); i++)
		delete SocialBookmarkSites[i];
}

void CConfig::Save() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_CONFIGURATION);

	reg.Write(szNavigationType, NavigationType);

	reg.Write(szCacheLocation, CacheLocation);
	reg.Write(szCacheLimit, CacheLimit);

	// general
	reg.Write(szNotifyNew, NotifyNew);
	reg.Write(szWrapTitles, WrapTitles);
	reg.Write(szShowRelativeDates, ShowRelativeDates);
	reg.Write(szMoveToUnread, MoveToUnread);
	reg.Write(szEmailAccount, EmailAccount);
	reg.Write(szGroupByFeed, GroupByFeed);

	// retrieve
	reg.Write(szUpdateInterval, UpdateInterval);

	reg.Write(szAutoConnect, AutoConnect);
	reg.Write(szUseConnMan, UseConnManager);
	reg.Write(szCheckOnCradling, CheckOnCradling);
	reg.Write(szClearErrorLog, ClearErrorLog);

	reg.Write(szBackgroundUpdate, BackgroundUpdate);

	reg.Write(szTimeUpdate, TimeUpdate);
	reg.Write(szUpdateAtTime, &UpdateAtTime, sizeof(UpdateAtTime));

	//
	reg.Write(szCacheImages, CacheImages);
	reg.Write(szCacheHtml, CacheHtml);

	// sync
	reg.Write(szSyncSite, SyncSite);
	reg.Write(szSyncUserName, SyncUserName);
	reg.Write(szSyncPassword, SyncPassword);

	// enclosures
	reg.Write(szGeneratePlaylists, GeneratePlaylists);
	reg.Write(szEnclosurePlaylistFormat, EnclosurePlaylistFormat);

	// html optimizer
	reg.Write(szUseHtmlOptimizer, UseHtmlOptimizer);
	reg.Write(szHtmlOptimizerURL, HtmlOptimizerURL);
}

void CConfig::Load() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_CONFIGURATION);

	NavigationType = reg.Read(szNavigationType, CONFIG_DEFAULT_NAVIGATION_TYPE);

	// cache
	CacheLocation = reg.Read(szCacheLocation, _T("\\My Documents\\pRSSreader"));
	CacheLimit = reg.Read(szCacheLimit, CONFIG_DEFAULT_CACHE_LIMIT);

	// general
	NotifyNew = reg.Read(szNotifyNew, CONFIG_DEFAULT_NOTIFYNEW);
	WrapTitles = reg.Read(szWrapTitles, CONFIG_DEFAULT_WRAPTITLES);
	ShowRelativeDates = reg.Read(szShowRelativeDates, CONFIG_DEFAULT_SHOWRELATIVEDATES);
	MoveToUnread = reg.Read(szMoveToUnread, CONFIG_DEFAULT_MOVETOUNREAD);
	EmailAccount = reg.Read(szEmailAccount, CONFIG_DEFAULT_EMAIL_ACCOUNT);
	GroupByFeed = reg.Read(szGroupByFeed, CONFIG_DEFAULT_GROUP_BY_FEED);

	// retrieve
	UpdateInterval = reg.Read(szUpdateInterval, CONFIG_DEFAULT_UPDATEINTERVAL);

	//
	TimeUpdate = reg.Read(szTimeUpdate, CONFIG_DEFAULT_TIME_UPDATE);
	SYSTEMTIME tm;
	if (reg.Read(szUpdateAtTime, &tm, sizeof(tm)))
		UpdateAtTime = tm;
	else {
		UpdateAtTime.wHour = 8;
		UpdateAtTime.wMinute = 0;
		UpdateAtTime.wSecond = 0;
	}

	AutoConnect = reg.Read(szAutoConnect, CONFIG_DEFAULT_AUTOCONNECT);
	UseConnManager = reg.Read(szUseConnMan, CONFIG_DEFAULT_USE_CONN_MANAGER);
	CheckOnCradling = reg.Read(szCheckOnCradling, CONFIG_DEFAULT_CHECKONCRADLING);
	ClearErrorLog = reg.Read(szClearErrorLog, CONFIG_DEFAULT_CLEARERRORLOG);
	CString ua = reg.Read(szUserAgent, UserAgent);
	UserAgent = ua;

	// global caching options
	CacheImages = reg.Read(szCacheImages, CONFIG_DEFAULT_CACHE_IMAGES);
	CacheHtml = reg.Read(szCacheHtml, CONFIG_DEFAULT_CACHE_HTML);

	// sync
	SyncSite = (ESyncSite) reg.Read(szSyncSite, CONFIG_DEFAULT_SYNCSITE);
	SyncUserName = reg.Read(szSyncUserName, _T(""));
	SyncPassword = reg.Read(szSyncPassword, _T(""));

	// enclosures
	GeneratePlaylists = reg.Read(szGeneratePlaylists, CONFIG_DEFAULT_GENERATEPLAYLISTS);
	EnclosurePlaylistFormat = reg.Read(szEnclosurePlaylistFormat, CONFIG_DEFAULT_ENCLOSUREPLAYLISTFORMAT);

	// html optimizer
	UseHtmlOptimizer = reg.Read(szUseHtmlOptimizer, CONFIG_DEFAULT_USE_HTML_OPTIMIZER);
	HtmlOptimizerURL = reg.Read(szHtmlOptimizerURL, _T(""));

	///////////////////////////////////////////////////////////////////////////

	// TWEAKS ////

	AdditionalHttpHeaders.RemoveAll();
	CString strRawHeaders = reg.Read(szAdditionalHttpHeaders, _T(""));
	int start = 0;
	int nullPos = strRawHeaders.Find((TCHAR) '\0', start);
	while (nullPos != -1 && start < nullPos && start < strRawHeaders.GetLength()) {
		CString strHeader = strRawHeaders.Mid(start, nullPos - start);
		AdditionalHttpHeaders.Add(strHeader);
		start = nullPos + 1;
		nullPos = strRawHeaders.Find((TCHAR) '\0', start);
	}

//	DateFormat = reg.Read(szDateFormat, CONFIG_DEFAULT_DATE_FORMAT);						// custom date format
	DateFormat = reg.Read(szDateFormat, _T(""));						// custom date format
	SearchLimit = reg.Read(szSearchLimit, CONFIG_DEFAULT_SEARCHLIMIT);					// search
	MsgScrollThrought = reg.Read(szMsgScrollThrought, CONFIG_DEFAULT_MSG_SCROLL_THROUGHT);
	MinimalSyncInterval = reg.Read(szMinimalSyncInterval, CONFIG_DEFAULT_MINIMALSYNCINTERVAL);
	SetPropertiesAfterSiteAdded = reg.Read(szSetPropertiesAfterSiteAdded, CONFIG_DEFAULT_SETPROPERTIESAFTERSITEADDED);
	ShowMainToolbar = reg.Read(szShowMainToolbar, CONFIG_DEFAULT_SHOWMAINTOOLBAR);
	CheckCertificates = reg.Read(szCheckCertificates, CONFIG_DEFAULT_CHECKCERTIFICATES);
	Timeout = reg.Read(szTimeout, CONFIG_DEFAULT_TIMEOUT);
	ReportErrorsAfterUpdate = reg.Read(szReportErrorsAfterUpdate, CONFIG_DEFAULT_REPORTERRORSAFTERUPDATE);
	OpenMsgWithHtmlOptimizer = reg.Read(szOpenMsgWithHtmlOptimizer, CONFIG_DEFAULT_OPENMSGWITHHTMLOPTIMIZER);
	HtmlOptAfterUrlRewriting = reg.Read(szHtmlOptAfterUrlRewriting, CONFIG_DEFAULT_HTMLOPTAFTERURLREWRITING);
	BackgroundUpdate = reg.Read(szBackgroundUpdate, CONFIG_DEFAULT_BACKGROUNDUPDATE);
	WrapAround = reg.Read(szWrapAround, CONFIG_DEFAULT_WRAPAROUND);
}

// UI ////

void CConfig::SaveUI() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_UI);

	// main widow
	reg.Write(szActSiteIdx, ActSiteIdx);
	reg.Write(szActFeedItem, ActFeedItem);
	reg.Write(szMainView, MainView);
	reg.Write(szWorkOffline, WorkOffline);
	reg.Write(szHideGroups, HideGroups);
	reg.Write(szHideReadItems, HideReadItems);
	reg.Write(szUpdateOnly, UpdateOnly);
}

void CConfig::LoadUI() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_UI);

	ActSiteIdx = reg.Read(szActSiteIdx, CONFIG_DEFAULT_ACTSITEIDX);
	ActFeedItem = reg.Read(szActFeedItem, -1);
	MainView = reg.Read(szMainView, CONFIG_DEFAULT_MAIN_VIEW);
	WorkOffline = reg.Read(szWorkOffline, CONFIG_DEFAULT_WORK_OFFLINE);
	HideGroups = reg.Read(szHideGroups, CONFIG_DEFAULT_HIDE_GROUPS);
	HideReadItems = reg.Read(szHideReadItems, CONFIG_DEFAULT_HIDE_READ_ITEMS);
	UpdateOnly = reg.Read(szUpdateOnly, CONFIG_DEFAULT_UPDATE_ONLY);
}

// Proxy Profiles ////

void CConfig::SaveProxyProfiles() {
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_PROXY_PROFILES);

	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_PROXY_PROFILES);
	for (int i = 0; i < ProxyProfiles.GetSize(); i++) {
		CString sNum;
		sNum.Format(_T("%d"), i);

		CProxyProfile *profile = ProxyProfiles[i];
		CRegistry regProfile(reg, sNum);
		regProfile.Write(szName, profile->Name);
		regProfile.Write(szType, (DWORD) profile->ProxyConfig.Type);
		regProfile.Write(szHostName, profile->ProxyConfig.HostName);
		regProfile.Write(szPort, profile->ProxyConfig.Port);
		regProfile.Write(szNeedAuth, profile->ProxyConfig.NeedAuth);
		regProfile.Write(szUserName, profile->ProxyConfig.UserName);
		regProfile.Write(szPassword, profile->ProxyConfig.Password);
	}

	reg.Write(szActive, ProxyProfileIdx);
}

void CConfig::LoadProxyProfiles() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_PROXY_PROFILES);

	DWORD cSubKeys = 0;
	reg.QuerySubKeyNumber(&cSubKeys);
	ProxyProfiles.SetSize(cSubKeys);

	for (DWORD i = 0; i < cSubKeys; i++) {
		CString sNum;
		sNum.Format(_T("%d"), i);

		CRegistry regProfile(reg, sNum);
		CProxyProfile *profile = new CProxyProfile();

		profile->Name = regProfile.Read(szName, _T(""));
		profile->ProxyConfig.Type = (EProxyType) regProfile.Read(szType, 0);
		profile->ProxyConfig.HostName = regProfile.Read(szHostName, _T(""));
		profile->ProxyConfig.Port = regProfile.Read(szPort, 0);
		profile->ProxyConfig.NeedAuth = regProfile.Read(szNeedAuth, FALSE);
		profile->ProxyConfig.UserName = regProfile.Read(szUserName, _T(""));
		profile->ProxyConfig.Password = regProfile.Read(szPassword, _T(""));

		ProxyProfiles[i] = profile;
	}

	ProxyProfileIdx = reg.Read(szActive, -1);
	if (ProxyProfileIdx < 0 || ProxyProfileIdx > ProxyProfiles.GetUpperBound())
		ProxyProfileIdx = -1;
}

// Social Bookmarking Sites ////

void CConfig::SaveSocialBookmarkingSites() {
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_SOCIAL_BOOKMARKING_SITES);

	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_SOCIAL_BOOKMARKING_SITES);
	for (int i = 0; i < SocialBookmarkSites.GetSize(); i++) {
		CString sNum;
		sNum.Format(_T("%d"), i);

		CRegistry regProfile(reg, sNum);

		CSocialBookmarkSite *sbs = SocialBookmarkSites[i];
		regProfile.Write(szName, sbs->Name);
		regProfile.Write(szUrl, sbs->Url);
	}
}

void CConfig::LoadSocialBookmarkingSites() {
	{	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_SOCIAL_BOOKMARKING_SITES);

		DWORD cSubKeys = 0;
		reg.QuerySubKeyNumber(&cSubKeys);
		SocialBookmarkSites.SetSize(cSubKeys);

		int j = 0;
		for (DWORD i = 0; i < cSubKeys; i++) {
			CString sNum;
			sNum.Format(_T("%d"), i);

			CRegistry regProfile(reg, sNum);

			CSocialBookmarkSite *sbs = new CSocialBookmarkSite();
			sbs->Name = regProfile.Read(szName, _T(""));
			sbs->Url = regProfile.Read(szUrl, _T(""));

			if (!sbs->Name.IsEmpty())
				SocialBookmarkSites[j++] = sbs;
		}

		// destructor of reg is called here
	}

	if (SocialBookmarkSites.GetSize() <= 0) {
		// no sites - someone want to fuck up with us -> create del.icio.us
		CSocialBookmarkSite *sbs = new CSocialBookmarkSite();
		sbs->Name = _T("del.icio.us");
		sbs->Url = _T("http://del.icio.us/post?url=[%URL%]&title=[%TITLE%]");

		SocialBookmarkSites.SetAtGrow(0, sbs);

		// social bookmarking
		SaveSocialBookmarkingSites();
	}
}

void CConfig::SaveKeywords() {
	LOG0(3, "CConfig::SaveKeywords()");

	// Keywords
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_KEYWORDS);
	CRegistry regKeywords(HKEY_CURRENT_USER, REG_KEY_KEYWORDS);

	for (int i = 0; i < Keywords.GetSize(); i++) {
		CString sNum;
		sNum.Format(_T("%d"), i + 1);

		CString kw = Keywords.GetAt(i);
		regKeywords.Write(sNum, kw);
	}
}

void CConfig::LoadKeywords() {
	LOG0(3, "CConfig::LoadKeywords()");

	// read keywords
	DWORD cKeywords;
	CRegistry regKeywords(HKEY_CURRENT_USER, REG_KEY_KEYWORDS);
	regKeywords.QueryValueNumber(&cKeywords);

	for (DWORD k = 1; k <= cKeywords; k++) {
		CString sNum;
		sNum.Format(_T("%d"), k);

		CString kword = regKeywords.Read(sNum, _T(""));
		if (!kword.IsEmpty())
			Keywords.Add(kword);
	}
}

void CConfig::SaveRewriteRules() {
	LOG0(3, "CConfig::SaveRewriteRules()");

	CRegistry regRewriteRules(HKEY_CURRENT_USER, REG_KEY_REWRITE_RULES);
	for (int i = 0; i < RewriteRules.GetSize(); i++) {
		CRewriteRule *rr = RewriteRules[i];

		CString sNum;
		sNum.Format(_T("%02d"), i);
		CRegistry regRule(regRewriteRules, sNum);
		regRule.Write(szMatch, rr->Match);
		regRule.Write(szReplace, rr->Replace);
	}
}

void CConfig::LoadRewriteRules() {
	LOG0(3, "CConfig::LoadRewriteRules()");

	CRegistry regRewriteRules(HKEY_CURRENT_USER, REG_KEY_REWRITE_RULES);
	DWORD cSubKeys = 0;
	regRewriteRules.QuerySubKeyNumber(&cSubKeys);
	RewriteRules.SetSize(cSubKeys);

	for (DWORD i = 0; i < cSubKeys; i++) {
		CString sNum;
		sNum.Format(_T("%02d"), i);

		CRegistry regRule(regRewriteRules, sNum);
		CRewriteRule *rule = new CRewriteRule();

		rule->Match = regRule.Read(szMatch, _T(""));
		rule->Replace = regRule.Read(szReplace, _T(""));

		RewriteRules.SetAtGrow(i, rule);
	}
}
