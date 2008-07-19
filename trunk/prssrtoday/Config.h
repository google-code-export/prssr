// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CONFIG_H_)
#define _CONFIG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MODE_BRIEF								0
#define MODE_CYCLING							1

#define CYCLING_SUBMODE_SITES					0
#define CYCLING_SUBMODE_FEED_ITEMS				1

// default values
#define CONFIG_DEFAULT_SHOWNEWCHANNELS			TRUE
#define CONFIG_DEFAULT_MODE						MODE_BRIEF

#define CONFIG_DEFAULT_VOFFSET					4
#define CONFIG_DEFAULT_HIDE_PLUGIN				FALSE
#define CONFIG_DEFAULT_FONT_SIZE				9
#define CONFIG_DEFAULT_DISPLAY_ICON				TRUE

// minimal synchronization interval in minutes
//#define CONFIG_DEFAULT_MINIMALSYNCINTERVAL		0

//#define CONFIG_DEFAULT_SHOW_UNREAD_COUNT		TRUE
#define CONFIG_DEFAULT_CYCLING_SUBMODE			CYCLING_SUBMODE_SITES
#define CONFIG_DEFAULT_SHOW_SITE_NAME			TRUE
#define CONFIG_DEFAULT_SHOW_DATE_TIME			TRUE
#define CONFIG_DEFAULT_SHOW_ONLY_NEW			TRUE
#define CONFIG_DEFAULT_CYCLING_SPEED			1

//#define CONFIG_DEFAULT_SKIP_NONEWS_SITES	FALSE
//#define CONFIG_DEFAULT_CYCLE_TITLES			FALSE
//#define CONFIG_DEFAULT_CYCLE_FEEDS			TRUE

//#define CONFIG_DEFAULT_STARTUP_DELAY			0


#ifdef LOGGING
#define CONFIG_DEFAULT_LOGFILE						_T("\\prssr-today.log")
#define CONFIG_DEFAULT_LOGLEVEL						0
#endif


/////////////////////////////////////////////////////////////////////////////////////////

class CConfig {
public:
	CConfig();
	virtual ~CConfig();

	void Load();
	void Save();

	void LoadSharedConfig();

#ifdef LOGGING
	CString LogFile;
	DWORD LogLevel;
#endif

	// general
	BOOL ShowNewChannels;
	int Mode;
	int VOffset;
	BOOL DisplayIcon;
	BOOL HidePlugin;

	// apperance
	int FontSize;

	// cycling mode
	int CyclingSubMode;
	BOOL ShowSiteName;
	int CyclingSpeedIdx;
	BOOL ShowOnlyNew;
	BOOL ShowDateTime;

	// read-only (global for the whole application)
//	DWORD StartupDelay;
	CString InstallDir;
	CString CacheLocation;

	BOOL ShowRelativeDates;
	CString DateFormat;
	BOOL GMTTimes;
};

extern CConfig Config;

#endif // !defined(_CONFIG_H_)
