/**
 *  Appearance.h
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

#if !defined(AFX_APPEARANCE_H__5AC4BE0E_774D_49D7_9F6C_4BD735AD567A__INCLUDED_)
#define AFX_APPEARANCE_H__5AC4BE0E_774D_49D7_9F6C_4BD735AD567A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../share/reg.h"

/*// default config values
#define CONFIG_DEFAULT_FEED_VW_FONT_FAMILY					_T("Tahoma")
#define CONFIG_DEFAULT_FEED_VW_FONT_SIZE					9
#define CONFIG_DEFAULT_SUMM_VW_FONT_FAMILY					_T("Tahoma")
#define CONFIG_DEFAULT_SUMM_VW_FONT_SIZE					9
#define CONFIG_DEFAULT_ARTICLE_VW_FONT_FAMILY				_T("Tahoma")
#define CONFIG_DEFAULT_ARTICLE_VW_FONT_SIZE					9
*/

// colors
#define CONFIG_DEFAULT_CLR_ARTICLE_BACKGROUND		::GetSysColor(COLOR_WINDOW)
#define CONFIG_DEFAULT_CLR_ARTICLE_FOREGROUND		::GetSysColor(COLOR_WINDOWTEXT)
#define CONFIG_DEFAULT_CLR_SUMMARY_BACKGROUND		::GetSysColor(COLOR_WINDOW)
#define CONFIG_DEFAULT_CLR_SUMMARY_FOREGROUND		::GetSysColor(COLOR_WINDOWTEXT)
#define CONFIG_DEFAULT_CLR_GROUP_BACKGROUND			RGB(0xCC, 0xFF, 0xBB)
#define CONFIG_DEFAULT_CLR_GROUP_FOREGROUND			::GetSysColor(COLOR_WINDOWTEXT)
#define CONFIG_DEFAULT_CLR_VFOLDER_BACKGROUND		RGB(253, 249, 208)
#define CONFIG_DEFAULT_CLR_VFOLDER_FOREGROUND		::GetSysColor(COLOR_WINDOWTEXT)
#define CONFIG_DEFAULT_CLR_FEEDVIEW_BACKGROUND		::GetSysColor(COLOR_WINDOW)
#define CONFIG_DEFAULT_CLR_FEEDVIEW_FOREGROUND		::GetSysColor(COLOR_WINDOWTEXT)
#define CONFIG_DEFAULT_CLR_DATE						RGB(0x88, 0x88, 0x88)

#define CONFIG_DEFAULT_CLEARTYPE					FALSE


// font configuration (apppearance)
struct CFontConfig {
	int Size;
	CString FontFace;

	CFontConfig() { }
	CFontConfig(const CString &fontFace, int size) { 
		Size = size;
		FontFace = fontFace;
	}
};


class CAppearance {
public:
	CAppearance();
	virtual ~CAppearance();

	void Create();
	void Destroy();

	void Save();
	void Load();

	CFont BaseFont;
/*	CFont FeedViewFont;
	CFont SummaryViewFont;
*/
	CFontConfig ArticleFontCfg;
	CFontConfig FeedViewFontCfg;
	CFontConfig SummaryViewFontCfg;

	BOOL ClearType;

	COLORREF ClrArticleFg;
	COLORREF ClrArticleBg;
	COLORREF ClrFeedViewFg;
	COLORREF ClrFeedViewBg;
	COLORREF ClrSummaryFg;
	COLORREF ClrSummaryBg;
	COLORREF ClrGroupFg;
	COLORREF ClrGroupBg;
	COLORREF ClrVFolderFg;
	COLORREF ClrVFolderBg;
	COLORREF ClrDate;

protected:
	void SaveFont(CRegistry &reg, LPCTSTR szKey, CFontConfig *fntCfg);
	void LoadFont(CRegistry &reg, LPCTSTR szKey, CFontConfig *fntCfg);
};

extern CAppearance Appearance;

#endif // !defined(AFX_APPEARANCE_H__5AC4BE0E_774D_49D7_9F6C_4BD735AD567A__INCLUDED_)
