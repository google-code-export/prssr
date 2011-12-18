/**
 *  Config.cpp : implementation of the CConfig class.
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


#include "StdAfx.h"
#include "Config.h"

#include "../share/defs.h"
#include "../share/reg.h"

//LPCTSTR szToday = _T("Today");

// today plugin config
LPCTSTR szMode            = _T("Mode");

LPCTSTR szVerticalOffset  = _T("VertOffset");
LPCTSTR szDisplayIcon     = _T("DisplayIcon");
LPCTSTR szDisplayFavicon  = _T("DisplayFavicon");
LPCTSTR szReverseSwitch	  = _T("ReverseSwitch");
LPCTSTR szHidePlugin      = _T("HidePlugin");
LPCTSTR szFontSize        = _T("FontSize");

LPCTSTR szCyclingSubMode  = _T("CyclingSubMode");
LPCTSTR szShowSiteName    = _T("ShowSiteName");
LPCTSTR szShowDateTime    = _T("ShowDateTime");
LPCTSTR szShowDateTimeTail    =_T("ShowDateTimeTail");
LPCTSTR szShowOnlyNew     = _T("ShowOnlyNew");
LPCTSTR szCyclingSpeed    = _T("CyclingSpeed");
LPCTSTR szShowTodayWrap   = _T("ShowTodayWrap");
LPCTSTR szDisplayRows     = _T("DisplayRows");

//LPCTSTR szStartupDelay    = _T("Startup Delay");

#ifdef LOGGING
// logging feature
LPCTSTR szLogFile = _T("LogFile");
LPCTSTR szLogLevel = _T("LogLevel");
#endif

CConfig Config;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig() {
#ifdef LOGGING
	LogFile = CONFIG_DEFAULT_LOGFILE;
	LogLevel = CONFIG_DEFAULT_LOGLEVEL;
#endif

	Mode = CONFIG_DEFAULT_MODE;

	VOffset = CONFIG_DEFAULT_VOFFSET;
	DisplayIcon = CONFIG_DEFAULT_DISPLAY_ICON;
	DisplayFavicon = CONFIG_DEFAULT_DISPLAY_FAVICON;
	ReverseSwitch = CONFIG_DEFAULT_REVERSE_SWITCH;
	HidePlugin = CONFIG_DEFAULT_HIDE_PLUGIN;
	FontSize = CONFIG_DEFAULT_FONT_SIZE;

	CyclingSubMode = CONFIG_DEFAULT_CYCLING_SUBMODE;
	ShowSiteName = CONFIG_DEFAULT_SHOW_SITE_NAME;
	ShowDateTime = CONFIG_DEFAULT_SHOW_DATE_TIME;
	ShowDateTimeTail = CONFIG_DEFAULT_SHOW_DATETIME_TAIL;
	ShowOnlyNew = CONFIG_DEFAULT_SHOW_ONLY_NEW;
	CyclingSpeed = CONFIG_DEFAULT_CYCLING_SPEED;
	ShowTodayWrap = CONFIG_DEFAULT_SHOW_TODAY_WRAP;
	DisplayRows = CONFIG_DEFAULT_DISPLAY_ROWS;

//	StartupDelay = CONFIG_DEFAULT_STARTUP_DELAY;
}

CConfig::~CConfig() {
}

void CConfig::Save() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_TODAY);

	reg.Write(szShowNewChannels, ShowNewChannels);
	reg.Write(szMode, Mode);

	reg.Write(szVerticalOffset , VOffset);
	reg.Write(szHidePlugin, HidePlugin);
	reg.Write(szFontSize, FontSize);
	reg.Write(szDisplayIcon, DisplayIcon);
	reg.Write(szDisplayFavicon, DisplayFavicon);
	reg.Write(szReverseSwitch, ReverseSwitch);

	reg.Write(szCyclingSubMode, CyclingSubMode);
	reg.Write(szShowSiteName, ShowSiteName);
	reg.Write(szShowDateTime, ShowDateTime);
	reg.Write(szShowDateTimeTail, ShowDateTimeTail);
	reg.Write(szShowOnlyNew, ShowOnlyNew);
	reg.Write(szCyclingSpeed, CyclingSpeed);
	reg.Write(szShowTodayWrap, ShowTodayWrap);
	reg.Write(szDisplayRows, DisplayRows);

#ifdef LOGGING
	reg.Write(szLogFile, LogFile);
	reg.Write(szLogLevel, LogLevel);
#endif
}

void CConfig::Load() {
	CRegistry regApp(HKEY_CURRENT_USER, REG_KEY_APP);
	InstallDir = regApp.Read(szInstallDir, _T(""));

	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_TODAY);

	ShowNewChannels = reg.Read(szShowNewChannels, CONFIG_DEFAULT_SHOWNEWCHANNELS);

	Mode = reg.Read(szMode, CONFIG_DEFAULT_MODE);

	VOffset = reg.Read(szVerticalOffset, CONFIG_DEFAULT_VOFFSET);
	HidePlugin = reg.Read(szHidePlugin, CONFIG_DEFAULT_HIDE_PLUGIN);
	FontSize = reg.Read(szFontSize, CONFIG_DEFAULT_FONT_SIZE);
	DisplayIcon = reg.Read(szDisplayIcon, CONFIG_DEFAULT_DISPLAY_ICON);
	DisplayFavicon = reg.Read(szDisplayFavicon, CONFIG_DEFAULT_DISPLAY_FAVICON);
	ReverseSwitch = reg.Read(szReverseSwitch, CONFIG_DEFAULT_REVERSE_SWITCH);

	// cycling mode
	CyclingSubMode = reg.Read(szCyclingSubMode, CONFIG_DEFAULT_CYCLING_SUBMODE);
	ShowSiteName = reg.Read(szShowSiteName, CONFIG_DEFAULT_SHOW_SITE_NAME);
	ShowDateTime = reg.Read(szShowDateTime, CONFIG_DEFAULT_SHOW_DATE_TIME);
	ShowDateTimeTail = reg.Read(szShowDateTimeTail, CONFIG_DEFAULT_SHOW_DATETIME_TAIL);
	ShowOnlyNew = reg.Read(szShowOnlyNew, CONFIG_DEFAULT_SHOW_ONLY_NEW);
	CyclingSpeed = reg.Read(szCyclingSpeed, CONFIG_DEFAULT_CYCLING_SPEED);
	ShowTodayWrap = reg.Read(szShowTodayWrap, CONFIG_DEFAULT_SHOW_TODAY_WRAP);
	DisplayRows = reg.Read(szDisplayRows, CONFIG_DEFAULT_DISPLAY_ROWS);

//	StartupDelay = reg.Read(szStartupDelay, CONFIG_DEFAULT_STARTUP_DELAY);

#ifdef LOGGING
	LogFile = reg.Read(szLogFile, CONFIG_DEFAULT_LOGFILE);
	LogLevel = reg.Read(szLogLevel, CONFIG_DEFAULT_LOGLEVEL);
#endif

	LoadSharedConfig();
}


void CConfig::LoadSharedConfig() {
	CRegistry regConf(HKEY_CURRENT_USER, REG_KEY_CONFIGURATION);
	CacheLocation = regConf.Read(szCacheLocation, _T(""));

	ShowRelativeDates = regConf.Read(szShowRelativeDates, CONFIG_DEFAULT_SHOWRELATIVEDATES);
	GMTTimes = regConf.Read(szGMTTimes, CONFIG_DEFAULT_GMTTIMES);
//	DateFormat = regConf.Read(szDateFormat, CONFIG_DEFAULT_DATE_FORMAT);
	DateFormat = regConf.Read(szDateFormat, _T(""));
}
