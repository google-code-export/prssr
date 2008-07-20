/**
 *  Appearance.cpp
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

#include "stdafx.h"
#include "Appearance.h"

#include "Appearance.h"
#include "../share/reg.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAppearance Appearance;

//static LPCTSTR szAppearance = _T("Appearance");
static LPCTSTR szFonts = _T("Fonts");

static LPCTSTR szFontFamily = _T("FontFamily");
static LPCTSTR szFontSize = _T("FontSize");

static LPCTSTR szArticleView = _T("ArticleView");
static LPCTSTR szFeedView = _T("FeedView");
static LPCTSTR szSummaryView = _T("SummaryView");

static LPCTSTR szColors = _T("Colors");

static LPCTSTR szClrArticleBackground = _T("Article Bg");
static LPCTSTR szClrArticleForeground = _T("Article Fg");
static LPCTSTR szClrFeedViewBackground = _T("FeedView Bg");
static LPCTSTR szClrFeedViewForeground = _T("FeedView Fg");
static LPCTSTR szClrSummaryBackground = _T("Summary Bg");
static LPCTSTR szClrSummaryForeground = _T("Summary Fg");
static LPCTSTR szClrGroupBackground = _T("Group Bg");
static LPCTSTR szClrGroupForeground = _T("Group Fg");
static LPCTSTR szClrVFolderBackground = _T("VFolder Bg");
static LPCTSTR szClrVFolderForeground = _T("VFolder Fg");
static LPCTSTR szClrDate = _T("Date");

static LPCTSTR szClearType= _T("ClearType");


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppearance::CAppearance() {
}

CAppearance::~CAppearance() {
}


void CAppearance::Create() {
	HGDIOBJ hSysFont = ::GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	::GetObject(hSysFont, sizeof(LOGFONT), (LPVOID) &lf);
    lf.lfHeight = SCALEY(12) + 1;
	BaseFont.CreateFontIndirect(&lf);

//	FeedViewFont.CreatePointFont(FeedViewFontCfg.Size * 10, FeedViewFontCfg.FontFace);
//	SummaryViewFont.CreatePointFont(SummaryViewFontCfg.Size * 10, SummaryViewFontCfg.FontFace);
}

void CAppearance::Destroy() {
	BaseFont.DeleteObject();
//	FeedViewFont.DeleteObject();
//	SummaryViewFont.DeleteObject();
}

void CAppearance::Save() {
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_APPEARANCE);
	
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_APPEARANCE);
	reg.Write(szClearType, ClearType);

	CRegistry regColors(reg, szColors);
	regColors.Write(szClrArticleBackground, ClrArticleBg);
	regColors.Write(szClrArticleForeground, ClrArticleFg);
	regColors.Write(szClrFeedViewBackground, ClrFeedViewBg);
	regColors.Write(szClrFeedViewForeground, ClrFeedViewFg);
	regColors.Write(szClrSummaryBackground, ClrSummaryBg);
	regColors.Write(szClrSummaryForeground, ClrSummaryFg);
	regColors.Write(szClrGroupBackground, ClrGroupBg);
	regColors.Write(szClrGroupForeground, ClrGroupFg);
	regColors.Write(szClrVFolderBackground, ClrVFolderBg);
	regColors.Write(szClrVFolderForeground, ClrVFolderFg);
	regColors.Write(szClrDate, ClrDate);

	// fonts
	CRegistry regFonts(reg, szFonts);
	SaveFont(regFonts, szArticleView, &ArticleFontCfg);
	SaveFont(regFonts, szFeedView, &FeedViewFontCfg);
	SaveFont(regFonts, szSummaryView, &SummaryViewFontCfg);
}

void CAppearance::SaveFont(CRegistry &reg, LPCTSTR szKey, CFontConfig *fntCfg) {
	CRegistry regFont(reg, szKey);
	regFont.Write(szFontFamily, fntCfg->FontFace);
	regFont.Write(szFontSize, fntCfg->Size);
}

void CAppearance::Load() {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_APPEARANCE);
	ClearType = reg.Read(szClearType, CONFIG_DEFAULT_CLEARTYPE);

	CRegistry regColors(reg, szColors);
	ClrArticleBg = regColors.Read(szClrArticleBackground, CONFIG_DEFAULT_CLR_ARTICLE_BACKGROUND);
	ClrArticleFg = regColors.Read(szClrArticleForeground, CONFIG_DEFAULT_CLR_ARTICLE_FOREGROUND);
	ClrFeedViewBg = regColors.Read(szClrFeedViewBackground, CONFIG_DEFAULT_CLR_FEEDVIEW_BACKGROUND);
	ClrFeedViewFg = regColors.Read(szClrFeedViewForeground, CONFIG_DEFAULT_CLR_FEEDVIEW_FOREGROUND);
	ClrSummaryBg = regColors.Read(szClrSummaryBackground, CONFIG_DEFAULT_CLR_SUMMARY_BACKGROUND);
	ClrSummaryFg = regColors.Read(szClrSummaryForeground, CONFIG_DEFAULT_CLR_SUMMARY_FOREGROUND);
	ClrGroupBg = regColors.Read(szClrGroupBackground, CONFIG_DEFAULT_CLR_GROUP_BACKGROUND);
	ClrGroupFg = regColors.Read(szClrGroupForeground, CONFIG_DEFAULT_CLR_GROUP_FOREGROUND);
	ClrVFolderBg = regColors.Read(szClrVFolderBackground, CONFIG_DEFAULT_CLR_VFOLDER_BACKGROUND);
	ClrVFolderFg = regColors.Read(szClrVFolderForeground, CONFIG_DEFAULT_CLR_VFOLDER_FOREGROUND);
	ClrDate = regColors.Read(szClrDate, CONFIG_DEFAULT_CLR_DATE);

	// fonts
	CRegistry regFonts(reg, szFonts);
	LoadFont(regFonts, szArticleView, &ArticleFontCfg);
	LoadFont(regFonts, szFeedView, &FeedViewFontCfg);
	LoadFont(regFonts, szSummaryView, &SummaryViewFontCfg);
}

void CAppearance::LoadFont(CRegistry &reg, LPCTSTR szKey, CFontConfig *fntCfg) {
	CRegistry regFont(reg, szKey);
	fntCfg->FontFace = regFont.Read(szFontFamily, _T("Tahoma"));
	fntCfg->Size = regFont.Read(szFontSize, 9);
}
