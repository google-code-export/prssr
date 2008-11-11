/**
 *  ArticleDlg.cpp
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
#include "prssr.h"
#include "../share/UIHelper.h"
#include "ArticleDlg.h"

#include "ctrls/CeDialog.h"
#include "../share/helpers.h"
#include "FeedView.h"
#include "Config.h"
#include "Feed.h"
#include "MainFrm.h"
#include "Appearance.h"
#include "www/LocalHtmlFile.h"
#include "www/url.h"

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


static DWORD MakeRGBVal(COLORREF clr) {
	return (GetRValue(clr) << 16) | (GetGValue(clr) << 8) | GetBValue(clr);
}

///

void OpenOnlineMessage(const CString &link, CSiteItem *si) {
	LOG1(1, "OpenOnlineMessage('%S')", link);

	CString url = SanitizeUrl(link);
	if (IsHTMLCached(url, TRUE)) {
		CString fileName = UrlToFileName(MakeAbsoluteUrl(url, _T(""), _T("")), TRUE);
		CString path = GetCachePath(FILE_TYPE_HTML, Config.CacheLocation);
		url = MakeOfflineUrl(path, fileName);
	}
	else {
		// first try to rewrite the URL
		if (Config.UseHtmlOptimizer && Config.OpenMsgWithHtmlOptimizer)
			url = MakeHtmlOptimizerUrl(url, Config.HtmlOptimizerURL);
		else
			url = RewriteUrl(url, Config.RewriteRules);
	}

	OpenUrlExt(url);
}

///////

// icons for enclosures
#define ICON_SITE						0
#define ICON_AUDIO						4
#define ICON_VIDEO						6
#define ICON_IMAGE						8
#define ICON_OTHER						10

/////////////////////////////////////////////////////////////////////////////
// CArticleDlg dialog

CArticleDlg::CArticleDlg() {
	//{{AFX_DATA_INIT(CArticleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	HotSpot = FALSE;
	m_nMenuID = IDR_ARTICLE;

	m_pFeedItem = NULL;

	HtmlCached = FALSE;
	InFullScreen = FALSE;
}

CArticleDlg::~CArticleDlg() {
	delete m_pFeedItem;
}

BOOL CArticleDlg::Create(CFeedView *view, CWnd *pParentWnd/* = NULL*/) {
	LOG0(3, "CArticleDlg::Create()");

	View = view;
	return CCeDialog::Create(CArticleDlg::IDD, pParentWnd);
}


void CArticleDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CArticleDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArticleDlg)
	DDX_Control(pDX, IDC_BANNER, m_ctlBanner);
	DDX_Control(pDX, IDC_ENCLOSUREBAR, m_ctlEnclosureBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArticleDlg, CCeDialog)
	//{{AFX_MSG_MAP(CArticleDlg)
	ON_WM_KEYDOWN()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEY, OnHotKey)

	ON_COMMAND(ID_ITEM_FLAG, OnItemFlag)
	ON_UPDATE_COMMAND_UI(ID_ITEM_FLAG, OnUpdateItemFlag)
	ON_COMMAND(ID_ITEM_OPEN, OnItemOpen)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_COPY_URL, OnCopyUrl)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)

	ON_COMMAND(ID_ENCLOSURE_OPEN, OnEnclosureOpen)
	ON_UPDATE_COMMAND_UI(ID_ENCLOSURE_OPEN, OnUpdateEnclosureOpen)
	ON_COMMAND(ID_ENCLOSURE_GET, OnEnclosureGet)
	ON_UPDATE_COMMAND_UI(ID_ENCLOSURE_GET, OnUpdateEnclosureGet)
	ON_COMMAND(ID_ENCLOSURE_DELETE, OnEnclosureDelete)
	ON_UPDATE_COMMAND_UI(ID_ENCLOSURE_DELETE, OnUpdateEnclosureDelete)

	ON_COMMAND(ID_OPEN_LINK, OnLinkOpen)
	ON_COMMAND(ID_DOWNLOAD_LINK, OnLinkDownload)

	ON_COMMAND_RANGE(ID_SOCIAL_BOOKMARK_BASE, ID_SOCIAL_BOOKMARK_BASE + 100, OnBookmarkLink)
	ON_COMMAND(ID_SEND_BY_EMAIL, OnSendByEmail)
	ON_COMMAND(ID_FULLSCREEN, OnFullscreen)

	ON_COMMAND(ID_VIEW_IMAGE, OnViewImage)
	ON_COMMAND(ID_COPY_IMAGE_LOCATION, OnCopyImageLocation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArticleDlg message handlers

BOOL CArticleDlg::OnInitDialog() {
	LOG0(3, "CArticleDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();
	m_strCaption.Empty();

	CRect rc;

	m_ctlInfoBar.Create(WS_CHILD, rc, this, IDC_INFO_BAR);

	m_ctlHTML.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, rc, GetSafeHwnd(), 0);

	::SetWindowLong(m_ctlHTML.GetHwnd(), GWL_ID, 12321);
	::SetFocus(m_ctlHTML.GetHwnd());
	::SendMessage(m_ctlHTML.GetHwnd(), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) _T(""));

	AfxSetResourceHandle(theApp.GetDPISpecificInstanceHandle());
	m_ilIcons.Create(IDB_CACHE_ITEMS, SCALEX(16), 0, RGB(255, 0, 255));
	AfxSetResourceHandle(AfxGetInstanceHandle());

	// create menu for the right Softkey
	TBBUTTON tb;
	::SendMessage(m_hwndCmdBar, TB_GETBUTTON, 1, (LPARAM) &tb);

	CMenu mnu;
	mnu.Attach((HMENU) tb.dwData);

	if (m_pFeedItem->HasEnclosure()) {
		AppendMenuFromResource(&mnu, IDR_ENCLOSURES);
	}
	mnu.AppendMenu(MF_SEPARATOR);
	AppendMenuFromResource(&mnu, IDR_ITEM_FLAG);
	AppendMenuFromResource(&mnu, IDR_COPY_URL);
	// bookmark
	AppendBookmarkMenu(&mnu);
	AppendMenuFromResource(&mnu, IDR_SEND_BY_EMAIL);
	AppendMenuFromResource(&mnu, IDR_FULLSCREEN);
	mnu.AppendMenu(MF_SEPARATOR);
	AppendMenuFromResource(&mnu, IDR_REFRESH);

	mnu.Detach();

	//
	ShowFeedItem();

	SetForegroundWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CArticleDlg::OnDestroy() {
	LOG0(5, "CArticleDlg::OnDestroy()");

	CCeDialog::OnDestroy();
}

void CArticleDlg::PostNcDestroy() {
	LOG0(5, "CArticleDlg::PostNcDestroy()");

	if ((View->SiteItem->Type != CSiteItem::VFolder && Config.HideReadItems) ||
		(View->SiteItem->Type == CSiteItem::VFolder && View->SiteItem->FlagMask == MESSAGE_READ_STATE))
	{
		for (int i = View->GetItemCount() - 1; i >= 0; i--) {
			if (View->GetItem(i)->IsRead())
				View->DeleteItem(i);
		}
	}

	CCeDialog::PostNcDestroy();
	View->m_pArticleDlg = NULL;
	AfxGetMainWnd()->SetFocus();

	delete this;
}

void CArticleDlg::OnOK() {
	LOG0(1, "CArticleDlg::OnOK()");

	ToNormalMode();
	DestroyWindow();
}

void CArticleDlg::OnCancel() {
	LOG0(1, "CArticleDlg::OnCancel()");

	ToNormalMode();
	DestroyWindow();
}

void CArticleDlg::NoNewMessage() {
	// info bar
	m_ctlInfoBar.SetText(IDS_NO_MORE_MESSAGES);

	CRect rcClient;
	GetClientRect(&rcClient);

//	CRect rcInfo;
	if (::IsWindow(m_ctlInfoBar.GetSafeHwnd())) {
//		m_ctlInfoBar.GetClientRect(rcInfo);
		m_ctlInfoBar.SetWindowPos(&wndTopMost, rcClient.left, rcClient.bottom - SCALEX(20),
			rcClient.Width(), SCALEX(20), SWP_SHOWWINDOW);
		m_ctlInfoBar.Invalidate();

		rcClient.bottom -= SCALEX(20);
		// reposition the html control
		::SetWindowPos(m_ctlHTML.GetHwnd(), NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
	}

//	m_ctlInfoBar.ShowWindow(SW_SHOW);
	m_ctlInfoBar.StartTimer();
}

void CArticleDlg::SetupEnclosureBar(CEnclosureItem *ei) {
	CString name = GetUrlFileName(ei->URL);
	if (name.IsEmpty())
		name.LoadString(IDS_ENCLOSURE);
	m_ctlEnclosureBar.SetName(name);
	BOOL cached = IsEnclosureCached(ei->URL);
	m_ctlEnclosureBar.SetCached(cached);
	m_ctlEnclosureBar.SetSize(ei->Length);
	// set icon according to the file type
	int nImage;
	switch (ei->Type) {
		case ENCLOSURE_TYPE_AUDIO: nImage = ICON_AUDIO; break;
		case ENCLOSURE_TYPE_VIDEO: nImage = ICON_VIDEO; break;
		case ENCLOSURE_TYPE_IMAGE: nImage = ICON_IMAGE; break;
		default: nImage = ICON_OTHER; break;
	}

	if (cached)
		m_ctlEnclosureBar.SetIcon(m_ilIcons.ExtractIcon(nImage));
	else
		m_ctlEnclosureBar.SetIcon(m_ilIcons.ExtractIcon(nImage - 1));
}

void CArticleDlg::ShowFeedItem() {
	LOG0(1, "CArticleDlg::ShowFeedItem()");

	// reposition controls before displaying the article (enclosure bar may get hidden or visible)
	ResizeControls();

	CString sText, sTemp;

	::SendMessage(m_ctlHTML.GetHwnd(), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) _T(""));
	m_ctlHTML.Clear();
	m_ctlHTML.EnableContextMenu(TRUE);

	m_ctlHTML.EnableClearType(Appearance.ClearType);
	m_ctlHTML.EnableScripting(FALSE);
	m_ctlHTML.ZoomLevel(1);

	m_ctlHTML.AddText(L"<html>");
	m_ctlHTML.AddText(L"<head>");
	CString strCSS;
	strCSS.Format(
		L"<style type=\"text/css\"><!-- "
		L"body { font-family: \"%s\"; font-size: %dpt; } "
		L"--></style>",
		Appearance.ArticleFontCfg.FontFace, Appearance.ArticleFontCfg.Size
	);
	m_ctlHTML.AddText(strCSS);

	m_ctlHTML.AddText(L"</head>");

	sText.Format(_T("<body bgcolor=#%x text=#%x>"),
		MakeRGBVal(Appearance.ClrArticleBg), MakeRGBVal(Appearance.ClrArticleFg));
	m_ctlHTML.AddText(sText);

	sText.Format(_T("<font face=\"%s\">"), Appearance.ArticleFontCfg.FontFace);
	m_ctlHTML.AddText(sText);

	// FIXME: critical section
	if (m_pFeedItem != NULL) {

		m_ctlHTML.AddText(L"<p><strong>");
		m_ctlHTML.AddText(m_pFeedItem->Title);
		m_ctlHTML.AddText(L"</strong></p>");

		// translate
		CString description ;
		description.Format(_T("<div>%s</div>"), m_pFeedItem->Description);		// workaround for libsgml
		m_ctlHTML.AddText(L"<p>");
		m_ctlHTML.AddText(description);
		m_ctlHTML.AddText(L"</p>");

		// author
		sText.Empty();
		if (!m_pFeedItem->Author.IsEmpty())
			sText += m_pFeedItem->Author + _T(" | ");
		// date/time
		CString sDateTime;
		SYSTEMTIME st = TimeToTimeZone(&m_pFeedItem->PubDate);		// convert to local time zone
		FormatDateTime(sDateTime, st, Config.ShowRelativeDates);
		sText += sDateTime;

		// keywords
		if (m_pFeedItem->HasKeywordMatch()) {
			m_ctlHTML.AddText(L"<p>Keywords: ");
			for (int i = 0; i < m_pFeedItem->KeywordPos.GetSize(); i++) {
				if (i > 0) m_ctlHTML.AddText(L", ");
				m_ctlHTML.AddText(Config.Keywords.GetAt(i));
			}
			m_ctlHTML.AddText(L"</p>");
		}

		m_ctlHTML.AddText(L"<p><font color=\"#aaa\">");
		m_ctlHTML.AddText(sText);
		m_ctlHTML.AddText(L"</font></p>");

		// horz divider
		m_ctlHTML.AddText(L"<hr/>");

		HtmlCached = IsHTMLCached(m_pFeedItem->Link, TRUE);
		sTemp.LoadString(IDS_LINK_TO_ARTICLE);
		if (HtmlCached)
			sText.Format(_T("<a href=\"%s\"><strong>%s</strong></a>"), m_pFeedItem->Link, sTemp);
		else
			sText.Format(_T("<a href=\"%s\">%s</a>"), m_pFeedItem->Link, sTemp);

		m_ctlHTML.AddText(L"<p>");
		m_ctlHTML.AddText(sText);
		m_ctlHTML.AddText(L"</p>");

		// enclosure bar
		if (m_pFeedItem->HasEnclosure()) {
			CEnclosureItem *ei = m_pFeedItem->Enclosures.GetHead();

			SetupEnclosureBar(ei);
			m_ctlEnclosureBar.ShowWindow(SW_SHOW);
			m_ctlEnclosureBar.Invalidate();
			m_ctlEnclosureBar.UpdateWindow();
		}
		else {
			m_ctlEnclosureBar.ShowWindow(SW_HIDE);
		}

		m_ctlHTML.AddText(L"</font></p>");
	}
	else {
		sTemp.Format(IDS_NOTHING_TO_DISPLAY);
		m_ctlHTML.AddText(L"<p>");
		m_ctlHTML.AddText(sTemp);
		m_ctlHTML.AddText(L"</p>");
	}
	m_ctlHTML.AddText(L"</font>");

	m_ctlHTML.AddText(L"</body>");
	m_ctlHTML.AddText(L"</html>");

	m_ctlHTML.EndOfSource();
}

BOOL CArticleDlg::PreTranslateMessage(MSG* pMsg) {
	LOG0(5, "CArticleDlg::PreTranslateMessage()");

	static BOOL bDoIdle = TRUE;

	MSG msg;
	if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && bDoIdle) {
//		m_wndCommandBar.OnUpdateCmdUI((CFrameWnd *) this, TRUE);
		bDoIdle = FALSE;
	}
	else {
		if (AfxGetApp()->IsIdleMessage(pMsg) && pMsg->message != 0x3FC) {
			bDoIdle = TRUE;
		}
	}

	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN) {
			HotSpot = TRUE;
		}

		if (pMsg->wParam == VK_LEFT ||
			pMsg->wParam == VK_RIGHT)
		{
			TranslateMessage(pMsg);
			SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
		else if (Config.MsgScrollThrought && (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN)) {
			TranslateMessage(pMsg);
			SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
		else if (pMsg->wParam == VK_RETURN) {
			TranslateMessage(pMsg);
			SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			if (HotSpot) {
				return CCeDialog::PreTranslateMessage(pMsg);
			}
			else {
				return TRUE;
			}
		}
		else {
			return CCeDialog::PreTranslateMessage(pMsg);
		}
	}
	else {
		return CCeDialog::PreTranslateMessage(pMsg);
	}
}

void CArticleDlg::OnScrollUp() {
	LOG0(1, "CArticleDlg::OnScrollUp()");

	SCROLLINFO si;
	HWND hSB = ::GetWindow(m_ctlHTML.GetHwnd(), GW_CHILD);
	if (hSB == NULL)
		hSB = m_ctlHTML.GetHwnd();

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	::GetScrollInfo(hSB, SB_VERT, &si);

	if (si.nPos == 0) {
		// at the top of the message move to the previous one
		OnItemPrev();
	}
	else {
		// scroll the message up
		::SendMessage(hSB, WM_VSCROLL, SB_LINEUP, NULL);
	}
}

void CArticleDlg::OnScrollDown() {
	LOG0(1, "CArticleDlg::OnScrollDown()");

	SCROLLINFO si;
	HWND hSB = ::GetWindow(m_ctlHTML.GetHwnd(), GW_CHILD);
	if (hSB == NULL)
		hSB = m_ctlHTML.GetHwnd();

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	::GetScrollInfo(hSB, SB_VERT, &si);

	if (si.nPos + (int) si.nPage > si.nMax) {
		// at the top of the message move to the previous one
		OnItemNext();
	}
	else {
		// scroll the message up
		::SendMessage(hSB, WM_VSCROLL, SB_LINEDOWN, NULL);
	}
}

void CArticleDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	LOG3(5, "CArticleDlg::OnKeyDown(%d, %d, %d)", nChar, nRepCnt, nFlags);

	switch (nChar) {
		case VK_RETURN:
			OnItemOpen();
			break;

		case VK_LEFT:
			// Move to previous item
			OnItemPrev();
			break;

		case VK_RIGHT:
			// Move to next item
			OnItemNext();
			break;

		case VK_UP:
			OnScrollUp();
			HotSpot = TRUE;
			break;

		case VK_DOWN:
			OnScrollDown();
			HotSpot = TRUE;
			break;

		default:
			CCeDialog::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

BOOL CArticleDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	LOG0(5, "CArticleDlg::OnNotify()");

	NMHDR *pnmh = (LPNMHDR) lParam;
	if (pnmh != NULL) {
		NM_HTMLVIEW *pnmHTML = (NM_HTMLVIEW *) lParam;

		if (pnmh->code == NM_HOTSPOT) {
			HotSpot = TRUE;
			OpenOnlineMessage(pnmHTML->szTarget, m_pFeedItem->SiteItem);

			*pResult = 1;
			return 1;
		}
		else if (pnmh->code == NM_CONTEXTMENU) {
			OnContextMenu((NM_HTMLCONTEXT *) lParam);
			*pResult = 1;
			return 1;
		}
		else if (pnmh->code == NM_INLINE_IMAGE) {
			if (IsHTMLCached(pnmHTML->szTarget, FALSE)) {
				// rewrite the source of the image to the cache
				m_strImageLoc = MakeOfflineUrl(GetCacheFile(FILE_TYPE_IMAGE, Config.CacheLocation, pnmHTML->szTarget));
				pnmHTML->szTarget = m_strImageLoc;

				*pResult = 0;	// use default handling of an image
				return 0;
			}
			else {
				// not cached -> show empty box
				CSiteItem *si= m_pFeedItem->SiteItem;

				BOOL cacheImgs;
				if (si->Info->UseGlobalCacheOptions)
					cacheImgs = Config.CacheImages;
				else
					cacheImgs = si->Info->CacheItemImages;

				if (Config.WorkOffline || !cacheImgs) {
					::SendMessage(m_ctlHTML.GetHwnd(), DTM_IMAGEFAIL, 0, (LPARAM) (INLINEIMAGEINFO*) pnmHTML->dwCookie);
					*pResult = 1;
					return 1;
				}
				else {
					*pResult = 0;	// use default handling of an image
					return 0;
				}
			}
		}
	}

	return CCeDialog::OnNotify(wParam, lParam, pResult);
}

void CArticleDlg::AppendBookmarkMenu(CMenu *menu) {
	// bookmarking
	if (Config.SocialBookmarkSites.GetSize() > 1) {
		// create popup menu
		HMENU hSubMenu = CreatePopupMenu();

		for (int i = 0; i < Config.SocialBookmarkSites.GetSize(); i++) {
			CSocialBookmarkSite *sbs = Config.SocialBookmarkSites[i];
			::AppendMenu(hSubMenu, MF_STRING | MF_ENABLED, ID_SOCIAL_BOOKMARK_BASE + i, sbs->Name);
		}

		CString sText;
		sText.Format(IDS_BOOKMARK_AT, _T(""));
		menu->AppendMenu(MF_POPUP, (UINT) hSubMenu, sText);
	}
	else {
		CSocialBookmarkSite *sbs = Config.SocialBookmarkSites[0];

		CString sText;
		sText.Format(IDS_BOOKMARK_AT, sbs->Name);
		menu->AppendMenu(MF_STRING, ID_SOCIAL_BOOKMARK_BASE, sText);
	}
}

void CArticleDlg::OnContextMenu(NM_HTMLCONTEXT *pnmhc) {
	LOG0(3, "CArticleDlg::OnContextMenu()");

	CMenu popup;
	popup.CreatePopupMenu();

	if (pnmhc->uTypeFlags & HTMLCONTEXT_LINK) {
		m_strContextMnuUrl = pnmhc->szLinkHREF;
		m_strContextMenuLinkName = pnmhc->szLinkName;

		AppendMenuFromResource(&popup, IDR_LINK_CTX);
		popup.InsertMenu(1, MF_SEPARATOR | MF_BYPOSITION);
		AppendBookmarkMenu(&popup);
		AppendMenuFromResource(&popup, IDR_SEND_BY_EMAIL);
		AppendMenuFromResource(&popup, IDR_COPY_LINK_URL);
	}
	else if (pnmhc->uTypeFlags & (HTMLCONTEXT_IMAGE | HTMLCONTEXT_IMAGENOTLOADED)) {
		m_strContextMnuUrl = pnmhc->szLinkHREF;
		m_strContextMenuLinkName = pnmhc->szLinkName;

		AppendMenuFromResource(&popup, IDR_ARTICLE_IMAGE);
		AppendMenuFromResource(&popup, IDR_SEND_BY_EMAIL);
	}
	else {
		m_strContextMnuUrl.Empty();
		m_strContextMenuLinkName.Empty();

		AppendMenuFromResource(&popup, IDR_OPEN);
		if (m_pFeedItem->HasEnclosure()) {
			AppendMenuFromResource(&popup, IDR_ENCLOSURES);
		}
		popup.AppendMenu(MF_SEPARATOR);
		AppendBookmarkMenu(&popup);
		AppendMenuFromResource(&popup, IDR_SEND_BY_EMAIL);
		AppendMenuFromResource(&popup, IDR_COPY_URL);
	}

	popup.AppendMenu(MF_SEPARATOR);
	AppendMenuFromResource(&popup, IDR_ITEM_FLAG);
	AppendMenuFromResource(&popup, IDR_COPY);
	AppendMenuFromResource(&popup, IDR_FULLSCREEN);

	//
	CPoint point = pnmhc->pt;
	popup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CArticleDlg::ResizeControls() {
	LOG0(3, "CArticleDlg::ResizeControls()");

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcBanner;
	m_ctlBanner.GetClientRect(&rcBanner);

	m_ctlBanner.SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), SCALEY(21), SWP_NOZORDER);

	rcClient.top += rcBanner.Height();

	// info bar
	CRect rcInfo;
	if (::IsWindow(m_ctlInfoBar.GetSafeHwnd())) {
		m_ctlInfoBar.GetClientRect(rcInfo);
		m_ctlInfoBar.SetWindowPos(&wndTop, rcClient.left, rcClient.bottom - SCALEX(20),
			rcClient.Width(), SCALEX(20), 0);
		m_ctlInfoBar.Invalidate();
	}

	if (m_pFeedItem->HasEnclosure()) {
		CRect rcEnc;
		m_ctlEnclosureBar.GetClientRect(rcEnc);

		m_ctlEnclosureBar.SetWindowPos(NULL, rcClient.left, rcClient.bottom - SCALEX(20),
			rcClient.Width(), SCALEX(20), SWP_NOZORDER);
		m_ctlEnclosureBar.Invalidate();

		rcClient.bottom -= rcEnc.Height();
	}

	// reposition the html control
	::SetWindowPos(m_ctlHTML.GetHwnd(), NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
}

LRESULT CArticleDlg::OnHotKey(WPARAM wParam, LPARAM lParam) {
	LOG0(3, "CArticleDlg::OnHotKey()");

	return 0;
}

void CArticleDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) {
	LOG0(3, "CArticleDlg::OnInitMenuPopup()");

	if (bSysMenu)
		return; // don't support system menu

	ASSERT(pMenu != NULL); // check the enabled state of various menu items
	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	/*
	// determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu; // parent == child for tracking popup
	else if ((hParentMenu = ::WCE_FCTN(GetMenu)(m_hWnd)) != NULL) {
		CWnd *pParent = GetTopLevelParent(); // child windows don't have menus -- need to go to the top!

		if (pParent != NULL &&
			(hParentMenu = ::WCE_FCTN(GetMenu)(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::WCE_FCTN(GetMenuItemCount)(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu) {
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}
	*/
		// determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	// JAB: Changed from HWND to CMenu because you're not supposed to use WCE_FCTN
	CMenu *pParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu; // parent == child for tracking popup
	else if ((pParentMenu = GetMenu()) != NULL) {
		CWnd *pParent = GetTopLevelParent(); // child windows don't have menus -- need to go to the top!

		if (pParent != NULL &&
			((pParentMenu = pParent->GetMenu()) != NULL))
		{
			int nIndexMax = pParentMenu->GetMenuItemCount();
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				if (pParentMenu->GetSubMenu(nIndex) == pMenu) {
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = pParentMenu;
					break;
				}
			}
		}
	}


	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (pMenu->GetSubMenu(state.m_nIndex) != NULL) {
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue; // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE); // popups are never auto disabled
		}
		else {
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, TRUE);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax) {
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CArticleDlg::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized) {
	if (InFullScreen)
		ToFullScreenMode();
	else
		ToNormalMode();
}

// commands

void CArticleDlg::OnItemNext() {
	LOG0(1, "CArticleDlg::OnItemNext()");

	if (View == NULL) return;

	if (View->SiteItem != NULL && View->SiteItem->Type == CSiteItem::VFolder) {
		CWaitCursor wait;

		int oldIdx = View->GetSelectedItem();
		int idx = View->GetSelectedItem();

		BOOL found = FALSE;
		while (!found) {
			if (idx < View->GetItemCount() - 1)
				idx++;
			else if (Config.WrapAround)
				idx = 0;
			else
				idx = oldIdx;

			// we are back on the original site
			if (idx == oldIdx) {
				NoNewMessage();
				break;
			}

			// check
			if (View->SiteItem->FlagMask == MESSAGE_READ_STATE) {
				CFeedItem *fi = View->GetItem(idx);
				if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread()))
					found = TRUE;
			}
			else
				found = TRUE;
		}

		if (found) {
			View->OpenItem(idx);
			View->EnsureVisible(idx);

			ShowFeedItem();
			m_ctlBanner.Invalidate();
		}
	}
	else {
		CWaitCursor wait;

		int oldIdx = View->GetSelectedItem();
		int oldSite = Config.ActSiteIdx;

		int site = Config.ActSiteIdx;
		int idx = View->GetSelectedItem();

		BOOL found = FALSE;
		while (!found) {
			if (idx < View->GetItemCount() - 1) {
				idx++;
				if (site == oldSite) {
					// we are back on the original item
					if (idx == oldIdx) {
						NoNewMessage();
						break;
					}
				}
			}
			else {
				int t = site;
				site = View->MoveToNextChannel();
				if (t == site) {
					NoNewMessage();
					break;
				}
				else {
					idx = 0;
				}
			}

			// check
			if (View->GetItemCount() > 0) {
				if (Config.MoveToUnread || Config.HideReadItems) {
					CFeedItem *fi = View->GetItem(idx);
					if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread()))
						found = TRUE;
				}
				else
					found = TRUE;
			}
		}

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		if (found) {
			View->OpenItem(idx);
			View->EnsureVisible(idx);

			ShowFeedItem();
			m_ctlBanner.Invalidate();

			if (oldSite != site)
				frame->AddSiteToSave(oldSite);
			// TODO: preload the site with unread items if reading only unread
			frame->PreloadSite(site);
		}
		else {
			// get back to original site and item
			frame->SelectSite(oldSite);
			View->EnsureVisible(oldIdx);
		}
	}
}

void CArticleDlg::OnItemPrev() {
	LOG0(1, "CArticleDlg::OnItemPrev()");

	if (View == NULL) return;

	if (View->SiteItem != NULL && View->SiteItem->Type == CSiteItem::VFolder) {
		CWaitCursor wait;

		int oldIdx = View->GetSelectedItem();
		int idx = View->GetSelectedItem();

		BOOL found = FALSE;
		while (!found) {
			if (idx > 0)
				idx--;
			else if (Config.WrapAround)
				idx = View->GetItemCount() - 1;
			else
				idx = oldIdx;

			// we are back on the original site
			if (idx == oldIdx) {
				NoNewMessage();
				break;
			}

			// check
			if (View->SiteItem->FlagMask == MESSAGE_READ_STATE) {
				CFeedItem *fi = View->GetItem(idx);
				if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread()))
					found = TRUE;
			}
			else
				found = TRUE;
		}

		if (found) {
			View->OpenItem(idx);
			View->EnsureVisible(idx);

			ShowFeedItem();
			m_ctlBanner.Invalidate();
		}
	}
	else {
		CWaitCursor wait;

		int oldIdx = View->GetSelectedItem();
		int oldSite = Config.ActSiteIdx;

		int site = Config.ActSiteIdx;
		int idx = View->GetSelectedItem();

		BOOL found = FALSE;
		while (!found) {
			if (idx > 0) {
				idx--;
				if (site == oldSite) {
					// we are back on the original site
					if (idx == oldIdx) {
						NoNewMessage();
						break;
					}
				}
			}
			else {
				int t = site;
				site = View->MoveToPrevChannel();
				if (t == site) {
					NoNewMessage();
					break;
				}
				else {
					idx = View->GetItemCount() - 1;
				}
			}

			// check
			if (View->GetItemCount() > 0) {
				if (Config.MoveToUnread || Config.HideReadItems) {
					CFeedItem *fi = View->GetItem(idx);
					if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread()))
						found = TRUE;
				}
				else
					found = TRUE;
			}
		}

		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		if (found) {
			View->OpenItem(idx);
			View->EnsureVisible(idx);

			ShowFeedItem();
			m_ctlBanner.Invalidate();

			if (oldSite != site)
				frame->AddSiteToSave(oldSite);
			// TODO: preload the site with unread items if reading only unread
			frame->PreloadSite(site);
		}
		else {
			// get back to original site and item
			frame->SelectSite(oldSite);
			View->EnsureVisible(oldIdx);
		}
	}
}

void CArticleDlg::OnItemFlag() {
	LOG0(1, "CArticleDlg::OnItemFlag()");

	if (View != NULL) {
		int selItem = View->GetSelectedItem();
		if (m_pFeedItem->IsFlagged()) {
			View->UnflagItem(selItem);
			m_pFeedItem->SetFlags(0, MESSAGE_FLAG);
			m_ctlBanner.SetFlagged(-1);
		}
		else {
			View->FlagItem(selItem);
			m_pFeedItem->SetFlags(MESSAGE_FLAG, MESSAGE_FLAG);
			m_ctlBanner.SetFlagged(FLAG_ICON);
		}
		m_ctlBanner.Invalidate();
	}
}

void CArticleDlg::OnUpdateItemFlag(CCmdUI *pCmdUI) {
	LOG0(5, "CArticleDlg::OnUpdateItemFlag()");

	pCmdUI->SetCheck(m_pFeedItem->IsFlagged());
}

void CArticleDlg::OnRefresh() {
	LOG0(3, "CArticleDlg::OnRefresh()");

	ShowFeedItem();
}

void CArticleDlg::OnItemOpen() {
	LOG0(1, "CArticleDlg::OnItemOpen()");

	ToNormalMode();
	if (m_strContextMnuUrl.IsEmpty())
		OpenOnlineMessage(m_pFeedItem->Link, m_pFeedItem->SiteItem);
	else
		OpenOnlineMessage(m_strContextMnuUrl, m_pFeedItem->SiteItem);
}


void CArticleDlg::OnEnclosureOpen() {
	LOG0(1, "CArticleDlg::OnEnclosureOpen()");

	if (m_pFeedItem != NULL && m_pFeedItem->HasEnclosure()) {
		CEnclosureItem *ei = m_pFeedItem->Enclosures.GetHead();
		ToNormalMode();
		OpenEnclosure(ei);
	}
}

void CArticleDlg::OnUpdateEnclosureOpen(CCmdUI *pCmdUI) {
	LOG0(5, "CArticleDlg::OnUpdateEnclosureOpen()");

	pCmdUI->Enable(m_pFeedItem->HasEnclosure());
}


void CArticleDlg::OnEnclosureGet() {
	LOG0(1, "CArticleDlg::OnEnclosureOpen()");

	CArray<CFeedItem *, CFeedItem *> items;
	items.Add(m_pFeedItem);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->m_wndUpdateBar.EnqueueEnclosures(items);
	frame->m_wndUpdateBar.Redraw();
	frame->m_wndUpdateBar.Start();
}

void CArticleDlg::OnUpdateEnclosureGet(CCmdUI *pCmdUI) {
	if (m_pFeedItem != NULL && m_pFeedItem->HasEnclosure()) {
		CEnclosureItem *ei = m_pFeedItem->Enclosures.GetHead();
		pCmdUI->Enable(!IsEnclosureCached(ei->URL));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CArticleDlg::OnEnclosureDelete() {
	LOG0(1, "CArticleDlg::OnEnclosureOpen()");

	CArray<CFeedItem *, CFeedItem *> items;
	items.Add(m_pFeedItem);
	ClearEnclosures(items);

	CEnclosureItem *ei = m_pFeedItem->Enclosures.GetHead();
	SetupEnclosureBar(ei);
	m_ctlEnclosureBar.Invalidate();
}

void CArticleDlg::OnUpdateEnclosureDelete(CCmdUI *pCmdUI) {
	if (m_pFeedItem->HasEnclosure()) {
		CEnclosureItem *ei = m_pFeedItem->Enclosures.GetHead();
		pCmdUI->Enable(IsEnclosureCached(ei->URL));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CArticleDlg::OnCopyUrl() {
	LOG0(1, "CArticleDlg::OnCopyUrl()");

	CString link;
	if (m_strContextMnuUrl.IsEmpty() && m_pFeedItem != NULL)
		link = m_pFeedItem->Link;
	else
		link = m_strContextMnuUrl;

	CopyTextToClipboard(GetSafeHwnd(), link);
}

void CArticleDlg::OnCopy() {
	LOG0(1, "CArticleDlg::OnCopy()");

	LPSTREAM stream = 0;		// give us the output stream here
	DWORD rsd = 0;				// required, can be checked with SUCCEEDED?...
	m_ctlHTML.CopySelectionToNewIStream(&rsd, &stream);
	if (stream) {
		// got it
		STATSTG stat = { 0 };
		stream->Stat(&stat, 0); // probably check for the S_OK code...

		if (LPBYTE buf = (LPBYTE) LocalAlloc(LHND, (UINT) stat.cbSize.QuadPart + 2)) {
			ULONG ulNumChars;
			// read whole
			if (SUCCEEDED(stream->Read(buf, (ULONG) stat.cbSize.QuadPart, &ulNumChars)) &&
				ulNumChars == stat.cbSize.QuadPart)
			{
				CString strSelectedText((LPCWSTR) buf); // our text here!
				CopyTextToClipboard(m_ctlHTML.GetHwnd(), strSelectedText);
			}

			LocalFree(buf);
		}
		stream->Release();
	}
}

void CArticleDlg::OnUpdateCopy(CCmdUI *pCmdUI) {
	BOOL isTextSelected = (BOOL) m_ctlHTML.IsSelection();
	pCmdUI->Enable(isTextSelected);
}

void CArticleDlg::OnLinkOpen() {
	LOG0(1, "CArticleDlg::OnLinkOpen()");

	ToNormalMode();
	OpenOnlineMessage(m_strContextMnuUrl, m_pFeedItem->SiteItem);
}

void CArticleDlg::OnLinkDownload() {
	LOG0(1, "CArticleDlg::OnLinkDownload()");

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->m_wndUpdateBar.EnqueueItem(m_strContextMnuUrl, FILE_TYPE_HTML);
	frame->m_wndUpdateBar.Redraw();
	frame->m_wndUpdateBar.Start();
}

void CArticleDlg::OnBookmarkLink(UINT nID) {
	ToNormalMode();

	// link to bookmark
	CString link;
	if (m_strContextMnuUrl.IsEmpty() && m_pFeedItem != NULL)
		link = m_pFeedItem->Link;
	else
		link = m_strContextMnuUrl;

	int idx = nID - ID_SOCIAL_BOOKMARK_BASE;
	CSocialBookmarkSite *sbs = Config.SocialBookmarkSites[idx];
	CString url = sbs->Url;
	if (url.Replace(_T("[%URL%]"), UrlEncode(link)) != 1) {
		AfxMessageBox(IDS_BAD_URL_FMT_SBS);
		return;
	}

	// if the [%TITLE%] is not present, this will do nothing, which is ok
	if (m_strContextMenuLinkName.GetLength() > 0)
		url.Replace(_T("[%TITLE%]"), UrlEncode(m_pFeedItem->Title + _T(" - ") + m_strContextMenuLinkName));
	else
		url.Replace(_T("[%TITLE%]"), UrlEncode(m_pFeedItem->Title));

	// open browser
	OpenUrlExt(url);

	m_strContextMenuLinkName.Empty();
}

void CArticleDlg::OnViewImage() {
	CString fileName = GetCacheFile(FILE_TYPE_IMAGE, Config.CacheLocation, m_strContextMnuUrl);
	if (FileExists(fileName)) {
		// file is cached -> open it with a local program
		ShellOpenFile(fileName);
	}
	else {
		// file not cached -> use web browser to open it
		OpenUrlExt(m_strContextMnuUrl);
	}
}

void CArticleDlg::OnCopyImageLocation() {
	CopyTextToClipboard(GetSafeHwnd(), m_strContextMnuUrl);
}

void CArticleDlg::OnSendByEmail() {
	ToNormalMode();

	CString link;
	if (m_strContextMnuUrl.IsEmpty() && m_pFeedItem != NULL)
		link = m_pFeedItem->Link;
	else
		link = m_strContextMnuUrl;

	SendByEmail(link);
}

void CArticleDlg::ToFullScreenMode() {
	if (!InFullScreen) {
		HWND hWnd = GetSafeHwnd();

		SHSipPreference(hWnd, SIP_FORCEDOWN);				// hide SIP
		SetForegroundWindow();
		SHFullScreen(hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON);

		HDC hDC = ::GetDC(hWnd);
		::MoveWindow(hWnd, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), TRUE);
		::ReleaseDC(hWnd, hDC);

		InFullScreen = TRUE;
		ResizeControls();
	}
}

void CArticleDlg::ToNormalMode() {
	if (InFullScreen) {
		HWND hWnd = GetSafeHwnd();

		SHFullScreen(hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);

#define MENU_HEIGHT 	26
		RECT rc;
		::GetWindowRect(hWnd, &rc);
		::MoveWindow(hWnd, rc.left, rc.top + SCALEY(MENU_HEIGHT), rc.right, rc.bottom - (2 * SCALEY(MENU_HEIGHT)), TRUE);

		InFullScreen = FALSE;

		ResizeControls();
	}
}

void CArticleDlg::OnFullscreen() {
	if (InFullScreen)
		ToNormalMode();
	else
		ToFullScreenMode();
}

