/**
 *  OptAppearancePg.cpp
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
#include "prssr.h"
#include "OptAppearancePg.h"

#include "Config.h"
#include "Appearance.h"
#include "Site.h"
#include "../share/helpers.h"
#include "ctrls/BrowseDlg.h"
#include "ctrls/ColorPickerDlg.h"
#include "ProgressDlg.h"

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

int arFontSize[] = { 8, 9, 10, 11, 12, 14, 16, 18 };

struct CFontItem {
	CString Title;
	CFontConfig FontConfig;

	CFontItem() { }
	CFontItem(UINT nID, const CString &fontFace, int size) { 
		Title.LoadString(nID);
		FontConfig.Size = size;
		FontConfig.FontFace = fontFace;
	}

	CFontItem(UINT nID, const CFontConfig &fontCfg) {
		Title.LoadString(nID);
		FontConfig = fontCfg;
	}
};

#define FONT_ITEMS_COUNT					3

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg property page

IMPLEMENT_DYNCREATE(COptAppearancePg, CPropertyPage)

COptAppearancePg::COptAppearancePg() : CPropertyPage(COptAppearancePg::IDD) {
	//{{AFX_DATA_INIT(COptAppearancePg)
	m_bClearType = Appearance.ClearType;
	//}}AFX_DATA_INIT

	FontItems = new CFontItem[FONT_ITEMS_COUNT];
	FontItems[0] = CFontItem(IDS_MESSAGE_READING, Appearance.ArticleFontCfg);
	FontItems[1] = CFontItem(IDS_FEED_VIEW, Appearance.FeedViewFontCfg);
	FontItems[2] = CFontItem(IDS_SUMMARY_VIEW, Appearance.SummaryViewFontCfg);
}

COptAppearancePg::~COptAppearancePg() {
	delete [] FontItems;
}

void COptAppearancePg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptAppearancePg)
	DDX_Control(pDX, IDC_FONT_LBL, m_lblFont);
	DDX_Control(pDX, IDC_COLORS_LBL, m_lblColors);
	DDX_Control(pDX, IDC_CLEARTYPE_LBL, m_lblClearType);

	DDX_Control(pDX, IDC_ITEM_COLOR, m_ctlItemColor);
	DDX_Control(pDX, IDC_ITEM, m_ctlItems);
	DDX_Control(pDX, IDC_FONT_SIZE, m_ctlFontSize);
	DDX_Control(pDX, IDC_FONT_NAME, m_ctlFontName);
	DDX_Control(pDX, IDC_FONT_ITEM, m_ctlFontItem);
	DDX_Check(pDX, IDC_CLEARTYPE, m_bClearType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptAppearancePg, CPropertyPage)
	//{{AFX_MSG_MAP(COptAppearancePg)
	ON_BN_CLICKED(IDC_ITEM_COLOR, OnItemColor)
	ON_CBN_SELENDOK(IDC_ITEM, OnSelendokItem)
	ON_CBN_SELENDOK(IDC_FONT_ITEM, OnSelendokFontItem)
	ON_CBN_SELENDOK(IDC_FONT_NAME, OnSelendokFontName)
	ON_CBN_EDITUPDATE(IDC_FONT_SIZE, OnEditupdateFontSize)
	ON_CBN_SELENDOK(IDC_FONT_SIZE, OnSelendokFontSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg message handlers

int CALLBACK EnumFontProc(ENUMLOGFONT FAR *lpelf, TEXTMETRIC FAR *lpntm, int FontType, LPARAM lParam) {
	CComboBox *pCombo = (CComboBox *) lParam;
	pCombo->AddString(lpelf->elfLogFont.lfFaceName);
	return 1;
}

BOOL COptAppearancePg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	struct CItemColor {
		CString Title;
		COLORREF Clr;
		CItemColor(UINT nID, COLORREF clr) { Title.LoadString(nID); Clr = clr; }
	} arItems[] = {
		CItemColor(IDS_ARTICLE_FG, Appearance.ClrArticleFg),
		CItemColor(IDS_ARTICLE_BG, Appearance.ClrArticleBg),
		CItemColor(IDS_FEEDVIEW_FG, Appearance.ClrFeedViewFg),
		CItemColor(IDS_FEEDVIEW_BG, Appearance.ClrFeedViewBg),
		CItemColor(IDS_SUMMARY_FG, Appearance.ClrSummaryFg),
		CItemColor(IDS_SUMMARY_BG, Appearance.ClrSummaryBg),
		CItemColor(IDS_GROUP_FG, Appearance.ClrGroupFg),
		CItemColor(IDS_GROUP_BG, Appearance.ClrGroupBg),
		CItemColor(IDS_VFOLDER_FG, Appearance.ClrVFolderFg),
		CItemColor(IDS_VFOLDER_BG, Appearance.ClrVFolderBg),
		CItemColor(IDS_DATE, Appearance.ClrDate)
	};

	int i;

	// font items
	for (i = 0; i < FONT_ITEMS_COUNT; i++) {
		int item = m_ctlFontItem.AddString(FontItems[i].Title);
		m_ctlFontItem.SetItemData(item, (DWORD) &(FontItems[i].FontConfig));
	}

	// font families
	CDC *pDC = GetDC();
	EnumFontFamilies(pDC->GetSafeHdc(), NULL, (FONTENUMPROC) EnumFontProc, (LPARAM) &m_ctlFontName);
	ReleaseDC(pDC);

	// font sizes
	BOOL bSelected = FALSE;
	for (i = 0; i < sizeof(arFontSize) / sizeof(arFontSize[0]); i++) {
		CString strFontSize;
		strFontSize.Format(_T("%d"), arFontSize[i]);
		int newItem = m_ctlFontSize.AddString(strFontSize);
	}

	// select the first fotn item (this could be done after font and font sizes are inserted)
	m_ctlFontItem.SetCurSel(0);
	OnSelendokFontItem();


	// items
	for (i = 0; i < sizeof(arItems) / sizeof(arItems[0]); i++) {
		int item = m_ctlItems.AddString(arItems[i].Title);
		m_ctlItems.SetItemData(item, arItems[i].Clr);
	}
	m_ctlItems.SetCurSel(0);
	m_ctlItemColor.m_clrColor = m_ctlItems.GetItemData(0);
	

	return TRUE;
}


BOOL COptAppearancePg::OnApply() {
	UpdateData();

	// font items
	Appearance.ArticleFontCfg = *((CFontConfig *) m_ctlFontItem.GetItemData(0));
	Appearance.FeedViewFontCfg = *((CFontConfig *) m_ctlFontItem.GetItemData(1));
	Appearance.SummaryViewFontCfg = *((CFontConfig *) m_ctlFontItem.GetItemData(2));

	Appearance.ClearType = m_bClearType;

	// colors
	Appearance.ClrArticleFg = m_ctlItems.GetItemData(0);
	Appearance.ClrArticleBg = m_ctlItems.GetItemData(1);
	Appearance.ClrFeedViewFg = m_ctlItems.GetItemData(2);
	Appearance.ClrFeedViewBg = m_ctlItems.GetItemData(3);
	Appearance.ClrSummaryFg = m_ctlItems.GetItemData(4);
	Appearance.ClrSummaryBg = m_ctlItems.GetItemData(5);
	Appearance.ClrGroupFg = m_ctlItems.GetItemData(6);
	Appearance.ClrGroupBg = m_ctlItems.GetItemData(7);
	Appearance.ClrVFolderFg = m_ctlItems.GetItemData(8);
	Appearance.ClrVFolderBg = m_ctlItems.GetItemData(9);
	Appearance.ClrDate = m_ctlItems.GetItemData(10);

	return CPropertyPage::OnApply();
}

void COptAppearancePg::OnSelendokItem() {
	int item = m_ctlItems.GetCurSel();
	m_ctlItemColor.SetColor(m_ctlItems.GetItemData(item));
}

void COptAppearancePg::OnItemColor() {
	int item = m_ctlItems.GetCurSel();
	CColorPickerDlg dlg(m_ctlItems.GetItemData(item));
	if (dlg.DoModal() == IDOK) {
		m_ctlItems.SetItemData(item, dlg.GetColor());
		m_ctlItemColor.SetColor(dlg.GetColor());
	}
}

void COptAppearancePg::OnSelendokFontItem() {
	int item = m_ctlFontItem.GetCurSel();
	if (item == CB_ERR)
		return;
	CFontConfig *fontConfig = (CFontConfig *) m_ctlFontItem.GetItemData(item);

	// select the font face
	if (m_ctlFontName.SelectString(-1, fontConfig->FontFace) == CB_ERR)
		m_ctlFontName.SetCurSel(0);

	// select the font size
	CString size;
	size.Format(_T("%d"), fontConfig->Size);
	if (m_ctlFontSize.SelectString(-1, size) == CB_ERR) {
		m_ctlFontSize.SetWindowText(size);
	}
}

void COptAppearancePg::OnSelendokFontName() {
	int item = m_ctlFontItem.GetCurSel();
	if (item == CB_ERR)
		return;
	CFontConfig *fontConfig = (CFontConfig *) m_ctlFontItem.GetItemData(item);

	CString fontFace;
	m_ctlFontName.GetLBText(m_ctlFontName.GetCurSel(), fontFace);
	fontConfig->FontFace = fontFace;
}

void COptAppearancePg::OnSelendokFontSize() {
	int item = m_ctlFontItem.GetCurSel();
	if (item == CB_ERR) 
		return;
	CFontConfig *fontConfig = (CFontConfig *) m_ctlFontItem.GetItemData(item);

	CString size;
	m_ctlFontSize.GetLBText(m_ctlFontSize.GetCurSel(), size);
	int fontSize;
	if (swscanf(size.GetBuffer(size.GetLength()), _T("%d"), &fontSize) == 1) {
		fontConfig->Size = fontSize;
	}
	size.ReleaseBuffer();
}

void COptAppearancePg::OnEditupdateFontSize() {
	int item = m_ctlFontItem.GetCurSel();
	if (item == CB_ERR) 
		return;
	CFontConfig *fontConfig = (CFontConfig *) m_ctlFontItem.GetItemData(item);

	CString size;
	m_ctlFontSize.GetWindowText(size);
	int fontSize;
	if (swscanf(size.GetBuffer(size.GetLength()), _T("%d"), &fontSize) == 1) {
		fontConfig->Size = fontSize;
	}
	size.ReleaseBuffer();
}
