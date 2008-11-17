/**
 *  ArticleView.cpp
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
#include "ArticleView.h"

//#include "ctrls/CeDialog.h"
//#include "../share/helpers.h"
//#include "FeedView.h"
#include "Config.h"
#include "Feed.h"
#include "MainFrm.h"
#include "Appearance.h"
//#include "www/LocalHtmlFile.h"
//#include "www/url.h"

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

/////////////////////////////////////////////////////////////////////////////
// CArticleView dialog

CArticleView::CArticleView() {
	//{{AFX_DATA_INIT(CArticleView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	HotSpot = FALSE;

	m_pArticle = NULL;

	InFullScreen = FALSE;
}

CArticleView::~CArticleView() {
	delete m_pArticle;
}

BEGIN_MESSAGE_MAP(CArticleView, CHTMLCtrl)
	//{{AFX_MSG_MAP(CArticleView)
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
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN, OnUpdateFullscreen)

	ON_COMMAND(ID_VIEW_IMAGE, OnViewImage)
	ON_COMMAND(ID_COPY_IMAGE_LOCATION, OnCopyImageLocation)
END_MESSAGE_MAP()

void CArticleView::SetFeedItem(CFeedItem *fi) {
	delete m_pArticle;
	m_pArticle = new CFeedItem(*fi);
}

void CArticleView::ShowArticle() {
	LOG0(1, "CArticleView::ShowFeedItem()");

//	// reposition controls before displaying the article (enclosure bar may get hidden or visible)
//	ResizeControls();

	CString sText, sTemp;

	::SendMessage(GetSafeHwnd(), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) _T(""));
	Clear();
	EnableContextMenu(TRUE);

	EnableClearType(Appearance.ClearType);
	EnableScripting(FALSE);
	ZoomLevel(1);

	AddText(L"<html>");
	AddText(L"<head>");
	CString strCSS;
	strCSS.Format(
		L"<style type=\"text/css\"><!-- "
		L"body { font-family: \"%s\"; font-size: %dpt; } "
		L"--></style>",
		Appearance.ArticleFontCfg.FontFace, Appearance.ArticleFontCfg.Size
	);
	AddText(strCSS);

	AddText(L"</head>");

	sText.Format(_T("<body bgcolor=#%x text=#%x>"),
		MakeRGBVal(Appearance.ClrArticleBg), MakeRGBVal(Appearance.ClrArticleFg));
	AddText(sText);

	sText.Format(_T("<font face=\"%s\">"), Appearance.ArticleFontCfg.FontFace);
	AddText(sText);

	// FIXME: critical section
	if (m_pArticle != NULL) {
		AddText(L"<p><strong>");
		AddText(m_pArticle->Title);
		AddText(L"</strong></p>");

		// translate
		CString description ;
		description.Format(_T("<div>%s</div>"), m_pArticle->Description);		// workaround for libsgml
		AddText(L"<p>");
		AddText(description);
		AddText(L"</p>");

		// author
		sText.Empty();
		if (!m_pArticle->Author.IsEmpty())
			sText += m_pArticle->Author + _T(" | ");
		// date/time
		CString sDateTime;
		SYSTEMTIME st = TimeToTimeZone(&m_pArticle->PubDate);		// convert to local time zone
		FormatDateTime(sDateTime, st, Config.ShowRelativeDates);
		sText += sDateTime;

		// keywords
		if (m_pArticle->HasKeywordMatch()) {
			AddText(L"<p>Keywords: ");
			for (int i = 0; i < m_pArticle->KeywordPos.GetSize(); i++) {
				if (i > 0) AddText(L", ");
				AddText(Config.Keywords.GetAt(i));
			}
			AddText(L"</p>");
		}

		AddText(L"<p><font color=\"#aaa\">");
		AddText(sText);
		AddText(L"</font></p>");

		// horz divider
		AddText(L"<hr/>");

		sTemp.LoadString(IDS_LINK_TO_ARTICLE);
		if (IsHTMLCached(m_pArticle->Link, TRUE))
			sText.Format(_T("<a href=\"%s\"><strong>%s</strong></a>"), m_pArticle->Link, sTemp);
		else
			sText.Format(_T("<a href=\"%s\">%s</a>"), m_pArticle->Link, sTemp);

		AddText(L"<p>");
		AddText(sText);
		AddText(L"</p>");

		AddText(L"</font></p>");
	}
	else {
		sTemp.Format(IDS_NOTHING_TO_DISPLAY);
		AddText(L"<p>");
		AddText(sTemp);
		AddText(L"</p>");
	}
	AddText(L"</font>");

	AddText(L"</body>");
	AddText(L"</html>");

	EndOfSource();
}

void CArticleView::CreateMenu(HWND hwndCmdBar) {
	// create menu for the right Softkey
	TBBUTTON tb;
	::SendMessage(hwndCmdBar, TB_GETBUTTON, 1, (LPARAM) &tb);

	CMenu mnu;
	mnu.Attach((HMENU) tb.dwData);
	while (mnu.GetMenuItemCount() > 0)
		mnu.DeleteMenu(0, MF_BYPOSITION);

	AppendMenuFromResource(&mnu, IDR_OPEN);
	if (m_pArticle != NULL && m_pArticle->HasEnclosure()) {
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
//	AppendMenuFromResource(&mnu, IDR_REFRESH);
	AppendMenuFromResource(&mnu, IDR_EXIT);

	mnu.Detach();
}

void CArticleView::OnScrollUp() {
	LOG0(1, "CArticleView::OnScrollUp()");

	SCROLLINFO si;
	HWND hSB = ::GetWindow(GetSafeHwnd(), GW_CHILD);
	if (hSB == NULL)
		hSB = GetSafeHwnd();

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

void CArticleView::OnScrollDown() {
	LOG0(1, "CArticleView::OnScrollDown()");

	SCROLLINFO si;
	HWND hSB = ::GetWindow(GetSafeHwnd(), GW_CHILD);
	if (hSB == NULL)
		hSB = GetSafeHwnd();

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

BOOL CArticleView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	LOG0(5, "CArticleView::OnNotify()");

	NMHDR *pnmh = (LPNMHDR) lParam;
	if (pnmh != NULL) {
		NM_HTMLVIEW *pnmHTML = (NM_HTMLVIEW *) lParam;

		if (pnmh->code == NM_HOTSPOT) {
			HotSpot = TRUE;
			OpenOnlineMessage(pnmHTML->szTarget, m_pArticle->SiteItem);

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
				CSiteItem *si= m_pArticle->SiteItem;

				BOOL cacheImgs;
				if (si->Info->UseGlobalCacheOptions)
					cacheImgs = Config.CacheImages;
				else
					cacheImgs = si->Info->CacheItemImages;

				if (Config.WorkOffline || !cacheImgs) {
					::SendMessage(GetSafeHwnd(), DTM_IMAGEFAIL, 0, (LPARAM) (INLINEIMAGEINFO*) pnmHTML->dwCookie);
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

	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CArticleView::AppendBookmarkMenu(CMenu *menu) {
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

void CArticleView::ContextMenu(CPoint point) {
	LOG0(3, "CArticleView::ContextMenu()");

	CMenu popup;
	popup.CreatePopupMenu();

	m_strContextMnuUrl.Empty();
	m_strContextMenuLinkName.Empty();

	AppendMenuFromResource(&popup, IDR_OPEN);
	if (m_pArticle->HasEnclosure()) {
		AppendMenuFromResource(&popup, IDR_ENCLOSURES);
	}
	popup.AppendMenu(MF_SEPARATOR);
	AppendBookmarkMenu(&popup);
	AppendMenuFromResource(&popup, IDR_SEND_BY_EMAIL);
	AppendMenuFromResource(&popup, IDR_COPY_URL);

	popup.AppendMenu(MF_SEPARATOR);
	AppendMenuFromResource(&popup, IDR_ITEM_FLAG);
	AppendMenuFromResource(&popup, IDR_COPY);
	AppendMenuFromResource(&popup, IDR_FULLSCREEN);

	popup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, GetParent());
}

void CArticleView::OnContextMenu(NM_HTMLCONTEXT *pnmhc) {
	LOG0(3, "CArticleView::OnContextMenu()");

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
		if (m_pArticle->HasEnclosure()) {
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
	popup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, GetParent());
}

/*
void CArticleView::ResizeControls() {
	LOG0(3, "CArticleView::ResizeControls()");

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

	if (m_pArticle->HasEnclosure()) {
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
*/

LRESULT CArticleView::OnHotKey(WPARAM wParam, LPARAM lParam) {
	LOG0(3, "CArticleView::OnHotKey()");

	return 0;
}

void CArticleView::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) {
	LOG0(3, "CArticleView::OnInitMenuPopup()");

	if (bSysMenu)
		return; // don't support system menu

	ASSERT(pMenu != NULL); // check the enabled state of various menu items
	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

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

void CArticleView::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized) {
	if (InFullScreen)
		ToFullScreenMode();
	else
		ToNormalMode();
}

// commands

void CArticleView::OnItemNext() {
	LOG0(1, "CArticleView::OnItemNext()");

	if (View == NULL) return;

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
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
				frame->NoNewMessage();
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

			frame->m_wndBanner.Invalidate();
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
						frame->NoNewMessage();
						break;
					}
				}
			}
			else {
				int t = site;
				site = View->MoveToNextChannel();
				if (t == site) {
					frame->NoNewMessage();
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

		if (found) {
			View->OpenItem(idx);
			View->EnsureVisible(idx);

			frame->m_wndBanner.Invalidate();

			if (oldSite != site)
				frame->AddSiteToSave(oldSite);
			// TODO: preload the site with unread items if reading only unread
			frame->PreloadSite(site);
		}
		else {
			// get back to original site and item
			frame->SelectSite(oldSite);
			if (View->GetItemCount() > 0) {
				View->SetSelectedItem(oldIdx);
				View->EnsureVisible(oldIdx);
			}

			frame->NoNewMessage();
		}
	}
}

void CArticleView::OnItemPrev() {
	LOG0(1, "CArticleView::OnItemPrev()");

	if (View == NULL) return;

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
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
				frame->NoNewMessage();
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

			frame->m_wndBanner.Invalidate();
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
						frame->NoNewMessage();
						break;
					}
				}
			}
			else {
				int t = site;
				site = View->MoveToPrevChannel();
				if (t == site) {
					frame->NoNewMessage();
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

		if (found) {
			View->OpenItem(idx);
			View->EnsureVisible(idx);

			frame->m_wndBanner.Invalidate();

			if (oldSite != site)
				frame->AddSiteToSave(oldSite);
			// TODO: preload the site with unread items if reading only unread
			frame->PreloadSite(site);
		}
		else {
			// get back to original site and item
			frame->SelectSite(oldSite);
			if (View->GetItemCount() <= 0) {
				View->SetSelectedItem(oldIdx);
				View->EnsureVisible(oldIdx);
			}
			frame->NoNewMessage();
		}
	}
}

void CArticleView::OnItemFlag() {
	LOG0(1, "CArticleView::OnItemFlag()");

	if (View != NULL) {
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();

		int selItem = View->GetSelectedItem();
		if (m_pArticle->IsFlagged()) {
			View->UnflagItem(selItem);
			m_pArticle->SetFlags(0, MESSAGE_FLAG);
			frame->m_wndBanner.SetFlagged(-1);
		}
		else {
			View->FlagItem(selItem);
			m_pArticle->SetFlags(MESSAGE_FLAG, MESSAGE_FLAG);
			frame->m_wndBanner.SetFlagged(FLAG_ICON);
		}
		frame->m_wndBanner.Invalidate();
	}
}

void CArticleView::OnUpdateItemFlag(CCmdUI *pCmdUI) {
	LOG0(5, "CArticleView::OnUpdateItemFlag()");

	pCmdUI->SetCheck(m_pArticle->IsFlagged());
}

void CArticleView::OnRefresh() {
	LOG0(3, "CArticleView::OnRefresh()");

	ShowArticle();
}

void CArticleView::OnItemOpen() {
	LOG0(1, "CArticleView::OnItemOpen()");

	ToNormalMode();
	if (m_strContextMnuUrl.IsEmpty())
		OpenOnlineMessage(m_pArticle->Link, m_pArticle->SiteItem);
	else
		OpenOnlineMessage(m_strContextMnuUrl, m_pArticle->SiteItem);
}


void CArticleView::OnEnclosureOpen() {
	LOG0(1, "CArticleView::OnEnclosureOpen()");

	if (m_pArticle != NULL && m_pArticle->HasEnclosure()) {
		CEnclosureItem *ei = m_pArticle->Enclosures.GetHead();
		ToNormalMode();
		OpenEnclosure(ei);
	}
}

void CArticleView::OnUpdateEnclosureOpen(CCmdUI *pCmdUI) {
	LOG0(5, "CArticleView::OnUpdateEnclosureOpen()");

	pCmdUI->Enable(m_pArticle->HasEnclosure());
}


void CArticleView::OnEnclosureGet() {
	LOG0(1, "CArticleView::OnEnclosureOpen()");

	CArray<CFeedItem *, CFeedItem *> items;
	items.Add(m_pArticle);

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->m_wndUpdateBar.EnqueueEnclosures(items);
	frame->m_wndUpdateBar.Redraw();
	frame->m_wndUpdateBar.Start();
}

void CArticleView::OnUpdateEnclosureGet(CCmdUI *pCmdUI) {
	if (m_pArticle != NULL && m_pArticle->HasEnclosure()) {
		CEnclosureItem *ei = m_pArticle->Enclosures.GetHead();
		pCmdUI->Enable(!IsEnclosureCached(ei->URL));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CArticleView::OnEnclosureDelete() {
	LOG0(1, "CArticleView::OnEnclosureOpen()");

	CArray<CFeedItem *, CFeedItem *> items;
	items.Add(m_pArticle);
	ClearEnclosures(items);

//	CEnclosureItem *ei = m_pArticle->Enclosures.GetHead();
//	SetupEnclosureBar(ei);
//	m_ctlEnclosureBar.Invalidate();
	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->SetupEnclosureBar(m_pArticle);
}

void CArticleView::OnUpdateEnclosureDelete(CCmdUI *pCmdUI) {
	if (m_pArticle->HasEnclosure()) {
		CEnclosureItem *ei = m_pArticle->Enclosures.GetHead();
		pCmdUI->Enable(IsEnclosureCached(ei->URL));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CArticleView::OnCopyUrl() {
	LOG0(1, "CArticleView::OnCopyUrl()");

	CString link;
	if (m_strContextMnuUrl.IsEmpty() && m_pArticle != NULL)
		link = m_pArticle->Link;
	else
		link = m_strContextMnuUrl;

	CopyTextToClipboard(GetSafeHwnd(), link);
}

void CArticleView::OnCopy() {
	LOG0(1, "CArticleView::OnCopy()");

	LPSTREAM stream = 0;		// give us the output stream here
	DWORD rsd = 0;				// required, can be checked with SUCCEEDED?...
	CopySelectionToNewIStream(&rsd, &stream);
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
				CopyTextToClipboard(GetSafeHwnd(), strSelectedText);
			}

			LocalFree(buf);
		}
		stream->Release();
	}
}

void CArticleView::OnUpdateCopy(CCmdUI *pCmdUI) {
	BOOL isTextSelected = (BOOL) IsSelection();
	pCmdUI->Enable(isTextSelected);
}

void CArticleView::OnLinkOpen() {
	LOG0(1, "CArticleView::OnLinkOpen()");

	ToNormalMode();
	OpenOnlineMessage(m_strContextMnuUrl, m_pArticle->SiteItem);
}

void CArticleView::OnLinkDownload() {
	LOG0(1, "CArticleView::OnLinkDownload()");

	CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
	frame->m_wndUpdateBar.EnqueueItem(m_strContextMnuUrl, FILE_TYPE_HTML);
	frame->m_wndUpdateBar.Redraw();
	frame->m_wndUpdateBar.Start();
}

void CArticleView::OnBookmarkLink(UINT nID) {
	ToNormalMode();

	// link to bookmark
	CString link;
	if (m_strContextMnuUrl.IsEmpty() && m_pArticle != NULL)
		link = m_pArticle->Link;
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
		url.Replace(_T("[%TITLE%]"), UrlEncode(m_pArticle->Title + _T(" - ") + m_strContextMenuLinkName));
	else
		url.Replace(_T("[%TITLE%]"), UrlEncode(m_pArticle->Title));

	// open browser
	OpenUrlExt(url);

	m_strContextMenuLinkName.Empty();
}

void CArticleView::OnViewImage() {
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

void CArticleView::OnCopyImageLocation() {
	CopyTextToClipboard(GetSafeHwnd(), m_strContextMnuUrl);
}

void CArticleView::OnSendByEmail() {
	ToNormalMode();

	CString link;
	if (m_strContextMnuUrl.IsEmpty() && m_pArticle != NULL)
		link = m_pArticle->Link;
	else
		link = m_strContextMnuUrl;

	SendByEmail(link);
}

void CArticleView::ToFullScreenMode() {
	if (!InFullScreen) {
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		HWND hWnd = frame->GetSafeHwnd();

		SHSipPreference(hWnd, SIP_FORCEDOWN);				// hide SIP
		SetForegroundWindow();
		SHFullScreen(hWnd, SHFS_HIDETASKBAR | SHFS_HIDESTARTICON | SHFS_HIDESIPBUTTON);
		::ShowWindow(frame->m_hwndCmdBar, SW_HIDE);


		HDC hDC = ::GetDC(hWnd);
		::MoveWindow(hWnd, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), TRUE);
		::ReleaseDC(hWnd, hDC);

		InFullScreen = TRUE;
//		ResizeControls();
	}
}

void CArticleView::ToNormalMode() {
	if (InFullScreen) {
		CMainFrame *frame = (CMainFrame *) AfxGetMainWnd();
		HWND hWnd = frame->GetSafeHwnd();

		SHFullScreen(hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSTARTICON | SHFS_SHOWSIPBUTTON);
		::ShowWindow(frame->m_hwndCmdBar, SW_SHOW);

#define MENU_HEIGHT 	26
		RECT rc;
		::GetWindowRect(hWnd, &rc);
		::MoveWindow(hWnd, rc.left, rc.top + SCALEY(MENU_HEIGHT), rc.right, rc.bottom - (2 * SCALEY(MENU_HEIGHT)), TRUE);

		InFullScreen = FALSE;
//		ResizeControls();
	}
}

void CArticleView::OnFullscreen() {
	if (InFullScreen)
		ToNormalMode();
	else
		ToFullScreenMode();
}

void CArticleView::OnUpdateFullscreen(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(InFullScreen);
}
