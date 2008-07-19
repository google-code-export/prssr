// TodayPluginWnd.cpp : implementation file
//

#include "stdafx.h"
#include "prssrtoday.h"
#include "TodayPluginWnd.h"

#include "todaycmn.h"
#include "Config.h"
#include "../prssr/ctrls/CePropertySheet.h"
#include "OptChannelsPg.h"
#include "OptAppearancePg.h"
#include "OptModePg.h"

#include "../prssr/xml/FeedFile.h"
#include "../share/helpers.h"

#include "../share/defs.h"
#include "../share/reg.h"
#include "../share/wnd.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../prssr/debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FILE_CHANGE_NOT_EXISTS              0
#define FILE_CHANGE_NO_CHANGE				1
#define FILE_CHANGE_UDPATED					2

#define CONFIG_DEFAULT_FONT_SIZE			9
#define LINE_SPACING						2

int CyclingSpeed[] = { 1000, 2500, 5000, 7500, 10000 };


UINT				WM_SH_UIMETRIC_CHANGE;
UINT				ReadConfigMessage;
UINT				CheckFeedsMessage;
UINT				UpdateFeedFlagsMessage;
UINT				ReloadSubscriptionsMessage;

CTodayPluginWnd		TodayPluginWnd;

BOOL APIENTRY CustomItemOptionsDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);

DWORD WINAPI ThreadStubProc(LPVOID lpPar) {
	CTodayPluginWnd *plugin = (CTodayPluginWnd *) lpPar;
	return plugin->Thread();
}

/////////////////////////////////////////////////////////////////////////////
// CTodayPluginWnd

CTodayPluginWnd::CTodayPluginWnd() {
	ItemIdx = 0;
	SiteIdx = -1;

	HThread = NULL;

	CycleTimer = 1;
	TapAndHoldTimer = 2;

	FileCheckIdx = 0;

	InitializeCriticalSection(&CSSiteList);
	HTerminate = CreateEvent(NULL, FALSE, FALSE, NULL);
	HReloadSubscriptions = CreateEvent(NULL, FALSE, FALSE, NULL);
	HCheckFeeds = CreateEvent(NULL, FALSE, FALSE, NULL);
	HUpdateFeedFlags = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CTodayPluginWnd::~CTodayPluginWnd() {
	SiteList.Destroy();

	DeleteCriticalSection(&CSSiteList);
	CloseHandle(HTerminate);
	CloseHandle(HReloadSubscriptions);
	CloseHandle(HCheckFeeds);
	CloseHandle(HUpdateFeedFlags);
}


BEGIN_MESSAGE_MAP(CTodayPluginWnd, CWnd)
	//{{AFX_MSG_MAP(CTodayPluginWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_TODAY_BRIEF, OnTodayBrief)
	ON_COMMAND(ID_TODAY_CYCLING, OnTodayCycling)
	ON_COMMAND(ID_TODAY_UPDATEALL, OnTodayUpdateall)
	ON_COMMAND(ID_TODAY_SETTINGS, OnTodaySettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTodayPluginWnd message handlers

BOOL CTodayPluginWnd::Register() {
	Unregister();

	WNDCLASS wc; 
	memset(&wc, 0, sizeof(wc));
	
	wc.style		 = 0;				   
	wc.lpfnWndProc	 = (WNDPROC) ::DefWindowProc;
	wc.hInstance	 = AfxGetInstanceHandle();
	wc.hIcon		 = 0;
	wc.hCursor		 = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = TODAY_CLASS_NAME;
	
	// register our window
	if (!AfxRegisterClass(&wc)) {
		TRACE(_T("Class Registration Failed\n"));
		return FALSE;
	}


	WM_SH_UIMETRIC_CHANGE = RegisterWindowMessage(SH_UIMETRIC_CHANGE);
	ReadConfigMessage = RegisterWindowMessage(READ_CONFIG_MESSAGE);
	ReloadSubscriptionsMessage = RegisterWindowMessage(RELOAD_SUBSCRIPTIONS_MESSAGE);
	CheckFeedsMessage = RegisterWindowMessage(CHECK_FEEDS_MESSAGE);
	UpdateFeedFlagsMessage = RegisterWindowMessage(UPDATE_FEED_FLAGS_MESSAGE);

	return TRUE;
}

BOOL CTodayPluginWnd::Unregister() {
	UnregisterClass(TODAY_CLASS_NAME, AfxGetInstanceHandle());
	return TRUE;
}


void CTodayPluginWnd::LoadFonts() {
	LOG0(5, "CTodayPluginWnd::LoadFonts()");

	m_fntNormal.DeleteObject();
	m_fntBold.DeleteObject();

	LONG dwFontSize;
	if (Config.FontSize == -1) {
		dwFontSize = SCALEY(CONFIG_DEFAULT_FONT_SIZE);
		CDC *pDC = GetDC();
		SHGetUIMetrics(SHUIM_FONTSIZE_PIXEL, &dwFontSize, sizeof(dwFontSize), NULL);
		ReleaseDC(pDC);
	}
	else {
		dwFontSize = SCALEY(Config.FontSize);
	}

	HGDIOBJ hObj = ::GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	::GetObject(hObj, sizeof(LOGFONT), (LPVOID) &lf);
	lf.lfHeight = -dwFontSize;
	m_fntNormal.CreateFontIndirect(&lf);

	RowHeight = (int) (1.2 * dwFontSize);

	lf.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lf);
}

BOOL CTodayPluginWnd::Create(HWND hwndParent) {
	LOG1(5, "CTodayPluginWnd::Create(%x)", hwndParent);

	m_hWndParent = hwndParent;
	BOOL ret = CreateEx(0, TODAY_CLASS_NAME, TODAY_WINDOW_NAME, WS_VISIBLE | WS_CHILD,
		0, 0, GetSystemMetrics(SM_CXSCREEN), 0, hwndParent, (HMENU) 0x1234);

	// load the font
	LoadFonts();

	HIcon = (HICON) ::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISPLAYICON), IMAGE_ICON, SCALEX(16), SCALEX(16), 0);

	EnterCriticalSection(&CSSiteList);
//	LoadSiteList(SiteList);
	if (SiteList.GetCount() > 0)
		SiteIdx = 0;
	else
		SiteIdx = -1;
	LeaveCriticalSection(&CSSiteList);

	SetTimer(CycleTimer, CyclingSpeed[Config.CyclingSpeedIdx], NULL);

	return TRUE;
}

void CTodayPluginWnd::OnDestroy() {
	LOG0(5, "CTodayPluginWnd::OnDestroy()");

	SetEvent(HTerminate);
	if (WaitForSingleObject(HThread, 4000) == WAIT_OBJECT_0) {
	}

	CWnd::OnDestroy();

	KillTimer(CycleTimer);
	KillTimer(TapAndHoldTimer);

	DestroyIcon(HIcon); HIcon = NULL;
	m_fntNormal.DeleteObject();
	m_fntBold.DeleteObject();
}

BOOL CTodayPluginWnd::OnEraseBkgnd(CDC* pDC) {
	LOG1(5, "CTodayPluginWnd::OnEraseBkgnd(%x)", pDC);

	TODAYDRAWWATERMARKINFO dwi;
	dwi.hwnd = GetSafeHwnd();
	dwi.hdc = pDC->GetSafeHdc();
	GetClientRect(&dwi.rc);
	::SendMessage(m_hWndParent, TODAYM_DRAWWATERMARK, 0, (LPARAM) &dwi);

	return TRUE;	
}


int CTodayPluginWnd::DrawSiteTitle(CDC &dc, CSiteItem *si, CRect *rc, UINT uFormat) {
	LOG2(5, "CTodayPluginWnd::DrawSiteTitle(, %p, , %d)", si, uFormat);

	int nNewCount = si->GetUnreadCount();
	CString strTitle = si->Name;
	ReplaceHTMLEntities(strTitle);

	CString buffer;
	if (nNewCount > 0)
		buffer.Format(_T("%s (%d)"), strTitle, nNewCount);
	else
		buffer.Format(_T("%s"), strTitle);

	int nWidth = rc->Width();

	CRect rcTemp = *rc;
	dc.DrawText(buffer, &rcTemp, uFormat | DT_CALCRECT);
	if (rcTemp.Width() > nWidth) {
		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.
		for (int i = strTitle.GetLength(); i >= 0; i--) {
			CString strName = strTitle.Left(i);

			if (nNewCount > 0)
				buffer.Format(_T("%s... (%d)"), strName, nNewCount);
			else
				buffer.Format(_T("%s..."), strName);

			rcTemp = *rc;
			dc.DrawText(buffer, &rcTemp, uFormat | DT_CALCRECT);
			if (rcTemp.Width() < nWidth) {
				// Gotcha!
				break;
			}
		}
		return dc.DrawText(buffer, rc, uFormat);
	}
	return dc.DrawText(buffer, rc, uFormat);
}

void CTodayPluginWnd::DrawMessage(CDC &dc, CRect &rect, UINT nID) {
	// no news
	CString strNoNews;
	strNoNews.LoadString(nID);

	dc.SelectObject(m_fntNormal);
	dc.DrawText(strNoNews, &rect, DT_LEFT | DT_TOP);
}

void CTodayPluginWnd::DrawBrief(CDC &dc, CRect &rcLabel) {
	LOG0(5, "CTodayPluginWnd::DrawBrief()");

	// only brief view
	if (NewItems == 0 && UnreadItems == 0) {
		DrawMessage(dc, rcLabel, IDS_NO_NEWS);
	}
	else {
		CString strText;
		if (NewItems > 0) {
			if (UnreadItems > 0)
				strText.Format(IDS_NEW_UNREAD_ITEMS, NewItems, UnreadItems);
			else
				strText.Format(IDS_NEW_ITEMS, NewItems);
		}
		else
			strText.Format(IDS_UNREAD_ITEMS, UnreadItems);
		dc.SelectObject(m_fntBold);
		dc.DrawText(strText, &rcLabel, DT_LEFT | DT_TOP);
	}
}

void CTodayPluginWnd::DrawCycling(CDC &dc, CRect &rcLabel) {
	dc.SelectObject(m_fntBold);
	CSiteItem *si = SiteList.GetAt(SiteIdx);

	switch (Config.CyclingSubMode) {
		case CYCLING_SUBMODE_SITES:
			DrawSiteTitle(dc, si, &rcLabel, DT_LEFT | DT_TOP);
			break;

		case CYCLING_SUBMODE_FEED_ITEMS:
			// show site name
			if (Config.ShowSiteName) {
				DrawSiteTitle(dc, si, &rcLabel, DT_LEFT | DT_TOP);

				CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
				dc.SelectObject(pen);
				POINT ln[2] = {
					{ rcLabel.left, rcLabel.top + RowHeight + SCALEY(2) },
					{ rcLabel.right, rcLabel.top + RowHeight + SCALEY(2) }
				};
				dc.Polyline(ln, 2);

				rcLabel.top += RowHeight + SCALEY(2) + SCALEY(2);
				rcLabel.bottom += RowHeight + SCALEY(2) + SCALEY(2);
			}
			dc.SelectObject(m_fntNormal);

			// show feed item
			if (si->Feed != NULL &&
				ItemIdx >= 0 && ItemIdx < si->Feed->GetItemCount())
			{
				CFeedItem *fi = si->Feed->GetItem(ItemIdx);

				dc.SelectObject(m_fntNormal);

				CString sFeedTitle = fi->Title;
				ReplaceHTMLEntities(sFeedTitle);
				DrawTextEndEllipsis(dc, sFeedTitle, rcLabel, DT_LEFT | DT_TOP | DT_NOPREFIX);
			
				if (Config.ShowDateTime) {
					rcLabel.top += RowHeight;
					rcLabel.bottom += RowHeight;

					CString strDateTime;
					SYSTEMTIME st = { 0 };
					if (fi->PubDate.wYear != 0) {
//						if (Config.GMTTimes) 
//							st = fi->PubDate;		// leave at GMT
//						else			
							st = TimeToTimeZone(&fi->PubDate);		// convert to local time zone
					}
					if (!FormatDateTime(strDateTime, st, Config.ShowRelativeDates))
						strDateTime.Empty();
					dc.DrawText(strDateTime, &rcLabel, DT_LEFT | DT_TOP);
				}
			}
			else {
				DrawMessage(dc, rcLabel, IDS_NO_NEWS);
			}
			break;

		default:
			DrawMessage(dc, rcLabel, IDS_NO_NEWS);
			break;
	}
}

void CTodayPluginWnd::OnPaint() {
	LOG0(5, "CTodayPluginWnd::OnPaint()");

	CPaintDC dc(this); // device context for painting

	int saveDC = dc.SaveDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	// Get appropriate text color from shell
	COLORREF crText;

	if (Selected) {
		COLORREF clrBkgnd = GetParent()->SendMessage(TODAYM_GETCOLOR, (WPARAM) TODAYCOLOR_HIGHLIGHT, NULL);
		CBrush brush(clrBkgnd);
		dc.FillRect(&rcClient, &brush);

		crText = GetParent()->SendMessage(TODAYM_GETCOLOR, (WPARAM) TODAYCOLOR_HIGHLIGHTEDTEXT, NULL);
	}
	else {
		crText = GetParent()->SendMessage(TODAYM_GETCOLOR, (WPARAM) TODAYCOLOR_TEXT, NULL);
	}

	// Important if you want the watermark behind the text      
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(crText);

	CRect rcLabel = rcClient;
	rcLabel.top += SCALEY(Config.VOffset);
	rcLabel.left += SCALEX(4);
	rcLabel.right -= SCALEX(4);

	if (Loading) {
		if (Config.DisplayIcon) {
			DrawIconEx(dc.GetSafeHdc(), SCALEX(2), SCALEY(2), HIcon, SCALEX(16), SCALEY(16), 0, NULL, DI_NORMAL);
			rcLabel.left += SCALEX(24);
		}

		DrawMessage(dc, rcLabel, IDS_LOADING);
	}
	else {
		BOOL bNewsAvailable = NewsAvailable();
		if (bNewsAvailable || !Config.HidePlugin) {
			if (Config.DisplayIcon) {
				DrawIconEx(dc.GetSafeHdc(), SCALEX(2), SCALEY(2), HIcon, SCALEX(16), SCALEY(16), 0, NULL, DI_NORMAL);
				rcLabel.left += SCALEX(24);
			}

			if (Config.Mode == MODE_BRIEF)
				DrawBrief(dc, rcLabel);
			else if (Config.Mode == MODE_CYCLING) {
				if (SiteIdx != -1 && bNewsAvailable)
					DrawCycling(dc, rcLabel);
				else
					DrawMessage(dc, rcLabel, IDS_NO_NEWS);
			}
		}
	}

	ValidateRect(rcClient);

	dc.RestoreDC(saveDC);	
	// Do not call CWnd::OnPaint() for painting messages
}

void CTodayPluginWnd::OnAction() {
	LOG0(1, "CTodayPluginWnd::OnAction()");

	switch (Config.Mode) {
		case MODE_BRIEF:
			OpenReader();
			break;

		case MODE_CYCLING:
			if (SiteIdx != -1) {
				if (Config.CyclingSubMode == CYCLING_SUBMODE_FEED_ITEMS)
					OpenReaderFeedItem(SiteIdx, ItemIdx);
				else
					OpenReaderSite(SiteIdx);
			}
			else
				OpenReader();
			break;
	}
}

BOOL CTodayPluginWnd::OnQueryRefreshCache(TODAYLISTITEM *ptli) {
	LOG1(5, "CTodayPluginWnd::OnQueryRefreshCache(%p)", ptli);

	if (ptli == NULL)
		return FALSE;

	Config.LoadSharedConfig();

	if (HThread == NULL) {
		HThread = CreateThread(NULL, 0, ThreadStubProc, this, 0, NULL);
	}

	BOOL fRet = FALSE;

	// calc height
	RECT rcClient;
	GetClientRect(&rcClient);

	UINT nMinHeight = (UINT) SCALEY(20);

	UINT nHeight = nMinHeight;
	BOOL bNewsAvailable = NewsAvailable();
	switch (Config.Mode) {
		case MODE_BRIEF:
			nHeight = (UINT) SCALEY(2 * Config.VOffset) + RowHeight;
			break;

		case MODE_CYCLING:
			nHeight = (UINT) SCALEY(2 * Config.VOffset) + RowHeight;
			if (bNewsAvailable) {
				if (Config.CyclingSubMode == CYCLING_SUBMODE_FEED_ITEMS) {
					if (Config.ShowSiteName)
						nHeight += RowHeight + SCALEY(2) + SCALEY(2) + SCALEY(1);

					if (Config.ShowDateTime)
						nHeight += RowHeight;
				}
			}
			break;
	}

	// ensure minimal height
	if (nHeight < nMinHeight)
		nHeight = nMinHeight;

	// don't show the plugin if there are no news items
	if (!bNewsAvailable && Config.HidePlugin)
		nHeight = -1;

	if (nHeight != ptli->cyp) {
		ptli->cyp = nHeight;
		fRet = TRUE;
	}

	return fRet;
}

void CTodayPluginWnd::OnClearCache(TODAYLISTITEM *ptli) {
	LOG1(5, "CTodayPluginWnd::OnClearCache(%p)", ptli);

	InvalidateRect(NULL, TRUE);
}

LRESULT CTodayPluginWnd::OnUIMetricChange(WPARAM wParam, LPARAM lParam) {
	LOG0(5, "CTodayPluginWnd::OnUIMetricChange()");

	LoadFonts();
	InvalidateRect(NULL, TRUE);
	return 0;
}

void CTodayPluginWnd::OnReadConfig() {
	LOG0(5, "CTodayPluginWnd::OnReadConfig()");

	Config.LoadSharedConfig();
}

void CTodayPluginWnd::OnLButtonDown(UINT nFlags, CPoint point) {
	LOG0(5, "CTodayPluginWnd::OnLButtonDown()");

	SetCapture();
	// only on VGA displays
	CDC *pDC = GetDC();
	if (pDC->GetDeviceCaps(LOGPIXELSX) > 130)
		Selected = TRUE;
	ReleaseDC(pDC);
	GetParent()->PostMessage(TODAYM_TOOKSELECTION, (WPARAM) GetSafeHwnd(), 0);

	LastCursorPos = point;

	KillTimer(TapAndHoldTimer);	// for sure
	SetTimer(TapAndHoldTimer, 100, NULL);

//	CWnd::OnLButtonDown(nFlags, point);
}

void CTodayPluginWnd::OnLButtonUp(UINT nFlags, CPoint point) {
	LOG0(5, "CTodayPluginWnd::OnLButtonUp()");

	Selected = FALSE;
	ReleaseCapture();
	KillTimer(TapAndHoldTimer);

	CRect rc;
	GetClientRect(&rc);
	if (rc.PtInRect(point)) {
		// if user taps icon -> open config
		if (Config.DisplayIcon && (point.x >= 0 && point.x <= SCALEX(21) && point.y >= 0 && point.y <= SCALEY(21))) {
			OpenConfig();
		}
		else
			OnAction();
	}

	Invalidate();
}


LRESULT CTodayPluginWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_SH_UIMETRIC_CHANGE) {
		OnUIMetricChange(wParam, lParam);
		return 0;
	}
	else if (message == ReadConfigMessage) {
		OnReadConfig();
		return 0;
	}
	else if (message == ReloadSubscriptionsMessage) {
		if (wParam == -1)
			SetEvent(HReloadSubscriptions);
		else {
			// reload particular site
			int idx = (int) wParam;
			if (0 <= idx && idx < SiteList.GetCount()) {
				EnterCriticalSection(&CSSiteList);
				
				CSiteItem *si = SiteList.GetAt(idx);
				LoadSiteItem(si, idx + 1);
				LoadSiteItemUnreadCount(si, idx + 1);

				if (si->Info->TodayShow)
					ReloadFeed(si);
				else
					UnloadFeed(si);

				UpdateUnreadNewCounts();

				LeaveCriticalSection(&CSSiteList);
			}
		}
		return 0;
	}
	else if (message == CheckFeedsMessage) {
		SetEvent(HCheckFeeds);
		return 0;
	}
	else if (message == UpdateFeedFlagsMessage) {
		int idx = wParam;
		EnterCriticalSection(&CSSiteList);
		if (0 <= idx && idx < SiteList.GetCount())
			SiteList.GetAt(idx)->SetModified();
		LeaveCriticalSection(&CSSiteList);

		SetEvent(HUpdateFeedFlags);
		return 0;
	}
	else {
		switch (message) {
			case WM_TODAYCUSTOM_QUERYREFRESHCACHE:
				return OnQueryRefreshCache((TODAYLISTITEM *) wParam);

			case WM_TODAYCUSTOM_CLEARCACHE:
				OnClearCache((TODAYLISTITEM *) wParam);
				return 0;

			case WM_TODAYCUSTOM_RECEIVEDSELECTION:
				if (!NewsAvailable() && Config.HidePlugin)
					// if zero height, pass the selection along to the next item
					return FALSE;
				else {
					Selected = TRUE;
					InvalidateRect(NULL, FALSE);
					return TRUE;
				}

			case WM_TODAYCUSTOM_LOSTSELECTION:
				Selected = FALSE;
				InvalidateRect(NULL, FALSE);
				break;

			case WM_TODAYCUSTOM_USERNAVIGATION:
				return FALSE;

			case WM_TODAYCUSTOM_ACTION:
				OnAction();
				break;
		}

		return CWnd::WindowProc(message, wParam, lParam);
	}
}

extern CPrssrtodayApp theApp;

void CTodayPluginWnd::ContextMenu(CPoint pt) {
	LOG0(5, "CTodayPluginWnd::ContextMenu()");

	// Note: Loading from resource did not work for me :(
	CString s;
	CMenu popup;
	popup.CreatePopupMenu();
	s.LoadString(IDS_UPDATE_ALL); popup.AppendMenu(MF_ENABLED | MF_STRING, ID_TODAY_UPDATEALL, s);
	popup.AppendMenu(MF_SEPARATOR);
	s.LoadString(IDS_MODE_BRIEF); popup.AppendMenu(MF_ENABLED | MF_STRING, ID_TODAY_BRIEF, s);
	s.LoadString(IDS_MODE_CYCLING); popup.AppendMenu(MF_ENABLED | MF_STRING, ID_TODAY_CYCLING, s);
	popup.AppendMenu(MF_SEPARATOR);
	s.LoadString(IDS_SETTINGS); popup.AppendMenu(MF_ENABLED | MF_STRING, ID_TODAY_SETTINGS, s);

	UINT idCheck;
	switch (Config.Mode) {
		case MODE_BRIEF: idCheck = ID_TODAY_BRIEF; break;
		case MODE_CYCLING: idCheck = ID_TODAY_CYCLING; break;
	}
	popup.CheckMenuRadioItem(ID_TODAY_BRIEF, ID_TODAY_TREE, idCheck, MF_BYCOMMAND);

	ClientToScreen(&pt);
	popup.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
}

void CTodayPluginWnd::OnTimer(UINT nIDEvent) {
	LOG0(5, "CTodayPluginWnd::OnTimer()");

	if (nIDEvent == (UINT) CycleTimer) {
		if (Config.Mode == MODE_CYCLING)
			Cycle();
	}
	else if (nIDEvent == TapAndHoldTimer) {
		ReleaseCapture();

		KillTimer(TapAndHoldTimer);
		Selected = FALSE;

		// tap and hold
		SHRGINFO  shrg;
		shrg.cbSize = sizeof(shrg);
		shrg.hwndClient = GetSafeHwnd();
		shrg.ptDown.x = LastCursorPos.x;
		shrg.ptDown.y = LastCursorPos.y;
		shrg.dwFlags = SHRG_NOTIFYPARENT | SHRG_RETURNCMD;

		if (::SHRecognizeGesture(&shrg) == GN_CONTEXTMENU) {
			ContextMenu(LastCursorPos);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CTodayPluginWnd::OnTodayBrief() {
	LOG0(5, "CTodayPluginWnd::OnTodayBrief()");

	Config.Mode = MODE_BRIEF;
	Config.Save();

	Invalidate();
}

void CTodayPluginWnd::OnTodayCycling() {
	LOG0(5, "CTodayPluginWnd::OnTodayCycling()");

	Config.Mode = MODE_CYCLING;
	Config.Save();

	Invalidate();
}

void CTodayPluginWnd::OnTodayUpdateall() {
	LOG0(5, "CTodayPluginWnd::OnTodayUpdateall()");

	UpdateAll();
}

void CTodayPluginWnd::OnTodaySettings() {
	LOG0(5, "CTodayPluginWnd::OnTodaySettings()");

	OpenConfig();
}


void CTodayPluginWnd::OnSize(UINT nType, int cx, int cy) {
	LOG0(5, "CTodayPluginWnd::OnSize()");

	CWnd::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////

void CTodayPluginWnd::UpdateAll() {
	LOG0(1, "CTodayPluginWnd::UpdateAll()");

	HWND hPRSSRWnd = FindPRSSRMainWindow();
	if (hPRSSRWnd != NULL) {
		// pRSSreader is running -> send a message
		::SendMessage(hPRSSRWnd, UWM_UPDATE_ALL, 0, 0);
	}
	else {
		// pRSSreader is NOT running -> start it
		PROCESS_INFORMATION pi;
		CString fileName;
		fileName.Format(_T("%s\\%s"), Config.InstallDir, PRSSREADER_BIN);
		::CreateProcess(fileName, _T("/updateall"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
	}
}

void CTodayPluginWnd::Cycle() {
	LOG0(3, "CTodayPluginWnd::Cycle()");

	if (NewsAvailable() && SiteIdx != -1) {
		int oldSiteIdx = SiteIdx;
		int oldItemIdx = ItemIdx;
		BOOL bMoved = FALSE;

		do {
			if (SiteList.GetCount() <= 0) {
				SiteIdx = -1;
				break;
			}

			if (Config.CyclingSubMode == CYCLING_SUBMODE_FEED_ITEMS) {
				CSiteItem *si = SiteList.GetAt(SiteIdx);

				if (si->Status == CSiteItem::Ok && si->Info->TodayShow) {
					if (si->Feed != NULL) {
						ItemIdx++;
						if (ItemIdx >= si->Feed->GetItemCount()) {
							SiteIdx = (SiteIdx + 1) % SiteList.GetCount();
							ItemIdx = -1;
						}
						else if (ItemIdx < 0) {
							ItemIdx = -1;
						}
						else {
							if (Config.ShowOnlyNew) {
								CFeedItem *fi = si->Feed->GetItem(ItemIdx);
								if (!fi->IsDeleted() && (fi->IsNew() || fi->IsUnread())) {
									bMoved = TRUE;
								}
							}
							else {
								// show all items
								bMoved = TRUE;
							}
						}
					}
					else {
						SiteIdx = (SiteIdx + 1) % SiteList.GetCount();
					}
				}
				else {
					SiteIdx = (SiteIdx + 1) % SiteList.GetCount();
				}

				if (SiteIdx == oldSiteIdx && ItemIdx == oldItemIdx)
					break;
			}
			else {
				SiteIdx = (SiteIdx + 1) % SiteList.GetCount();
				ItemIdx = 0;

				CSiteItem *si = SiteList.GetAt(SiteIdx);
				if (si->Status == CSiteItem::Ok && si->Info->TodayShow) {
					if (Config.ShowOnlyNew) {
						if (si->GetUnreadCount() > 0)
							bMoved = TRUE;
					}
					else
						bMoved = TRUE;
				}

				if (SiteIdx == oldSiteIdx)
					break;
			}
		} while (!bMoved);

		// has the site changed?
		if (oldSiteIdx != SiteIdx) {
			InvalidateSiteTitle();
			InvalidateFeedItem();
		}
		// has the feed item changed?
		if (oldItemIdx != ItemIdx)
			InvalidateFeedItem();

		// to reflect changes in config
		KillTimer(CycleTimer);
		SetTimer(CycleTimer, CyclingSpeed[Config.CyclingSpeedIdx], NULL);
	}
}

void CTodayPluginWnd::OpenConfig() {
	LOG0(3, "CTodayPluginWnd::OpenConfig()");

	COptChannelsPg pgChannels;
	COptAppearancePg pgAppearance;
	COptModePg pgMode;

	CCePropertySheet sheet(IDS_OPTION_TITLE);
	sheet.AddPage(&pgChannels);
	sheet.AddPage(&pgMode);
	sheet.AddPage(&pgAppearance);

	if (sheet.DoModal()) {
		Config.Save();

		LoadFonts();
		SetEvent(HCheckFeeds);
	}
}

void CTodayPluginWnd::UpdateUnreadNewCounts() {
	LOG0(3, "CTodayPluginWnd::UpdateUnreadNewCounts()");

	int newItems = NewItems;
	int unreadItems = UnreadItems;

	NewItems = 0;
	UnreadItems = 0;

	// load flags for registry
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *si = SiteList.GetAt(i);

		if (si->Info->TodayShow && si->Feed != NULL) {
			NewItems += si->Feed->GetNewCount();
			UnreadItems += si->Feed->GetUnreadCount();
		}
	}

	if (unreadItems != UnreadItems || newItems != NewItems)
		Invalidate();
}

void CTodayPluginWnd::InvalidateSiteTitle() {
	LOG0(5, "CTodayPluginWnd::InvalidateSiteTitle()");

	CRect rcClient;
	GetClientRect(&rcClient);

	rcClient.top += SCALEY(Config.VOffset);
	rcClient.left += SCALEX(4);
	if (Config.DisplayIcon)
		rcClient.left += SCALEX(24);
	InvalidateRect(rcClient);
	UpdateWindow();
}

void CTodayPluginWnd::InvalidateFeedItem() {
	LOG0(5, "CTodayPluginWnd::InvalidateFeedItem()");

	CRect rcClient;
	GetClientRect(&rcClient);

	rcClient.top += SCALEY(Config.VOffset);
	if (Config.ShowSiteName)
		rcClient.top += RowHeight + SCALEY(2) + SCALEY(2);
	rcClient.left += SCALEX(4);
	if (Config.DisplayIcon)
		rcClient.left += SCALEX(24);
	InvalidateRect(rcClient);
	UpdateWindow();
}


//
// returns TRUE if news are here
//
BOOL CTodayPluginWnd::NewsAvailable() {
	LOG0(5, "CTodayPluginWnd::NewsAvailable()");

	EnterCriticalSection(&CSSiteList);
	int itemCount = 0;
	if (Config.ShowOnlyNew) {
		for (int i = 0; i < SiteList.GetCount(); i++) {
			CSiteItem *si = SiteList.GetAt(i);
			if (si->Status == CSiteItem::Ok && si->Info->TodayShow && si->Feed != NULL)
				itemCount += si->Feed->GetNewCount() + si->Feed->GetUnreadCount();

			if (itemCount > 0)
				break;
		}
	}
	else {
		for (int i = 0; i < SiteList.GetCount(); i++) {
			CSiteItem *si = SiteList.GetAt(i);
			if (si->Status == CSiteItem::Ok && si->Info->TodayShow && si->Feed != NULL)
				itemCount += si->Feed->GetItemCount();
			
			if (itemCount > 0)
				break;
		}
	}
	LeaveCriticalSection(&CSSiteList);

	return itemCount > 0;
}

CString CTodayPluginWnd::GetPrssFilePath() {
	LOG0(5, "CTodayPluginWnd::GetPrssFilePath()");
	CString fileName;
	fileName.Format(_T("%s\\%s"), Config.InstallDir, PRSSREADER_BIN);
//	LOG1(1, "file = '%S'", fileName);

	return fileName;
}

void CTodayPluginWnd::OpenReader() {
	LOG0(1, "CTodayPluginWnd::OpenReader()");
	
	PROCESS_INFORMATION pi;
	::CreateProcess(GetPrssFilePath(), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
}

void CTodayPluginWnd::OpenReaderSite(int siteIdx) {
	LOG1(1, "CTodayPluginWnd::OpenReaderSite(%d)", siteIdx);

	PROCESS_INFORMATION pi;
	CString pars;

	HWND hPRSSRMain = FindPRSSRMainWindow();
	if (hPRSSRMain != NULL) {
		::PostMessage(hPRSSRMain, UWM_OPEN_SITE, siteIdx, 0);
	}
	else {
		pars.Format(_T("/opensite %d"), siteIdx);
	}
	::CreateProcess(GetPrssFilePath(), pars, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
}

void CTodayPluginWnd::OpenReaderFeedItem(int siteIdx, int feedIdx) {
	LOG2(1, "CTodayPluginWnd::OpenReaderFeedItem(%d, %d)", siteIdx, feedIdx);

	PROCESS_INFORMATION pi;
	CString pars;

	HWND hPRSSRMain = FindPRSSRMainWindow();
	if (hPRSSRMain != NULL) {
		::PostMessage(hPRSSRMain, UWM_OPEN_FEEDITEM, siteIdx, (LPARAM) feedIdx);
	}
	else {
		pars.Format(_T("/openitem %d %d"), siteIdx, feedIdx);
	}
	::CreateProcess(GetPrssFilePath(), pars, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
}

void CTodayPluginWnd::UnloadFeed(CSiteItem *si) {
	LOG0(3, "CTodayPluginWnd::UnloadFeed()");

	delete si->Feed;
	si->Feed = NULL;
	si->Status = CSiteItem::Empty;
	memset(&(si->LastUpdate), 0, sizeof(FILETIME));
	si->UpdateCachedCounts();
}

void CTodayPluginWnd::ReloadFeed(CSiteItem *si) {
	LOG0(3, "CTodayPluginWnd::UnloadFeed()");
	// reload
	si->Status = CSiteItem::Empty;
	si->EnsureSiteLoaded();
}

void CTodayPluginWnd::CheckFeedFile(CSiteItem *si) {
	LOG0(3, "CTodayPluginWnd::CheckFeedFile()");

	if (si != NULL && si->Info != NULL) {
		if (si->Info->TodayShow) {
			CString sXmlFileName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, si->Info->FileName);
			if (IsFileChanged(sXmlFileName, &(si->LastUpdate))) {
				LOG1(3, "Reloaded: '%S'", si->Name);
				// feed file was changed -> reload it
				ReloadFeed(si);

				if (!GetFileTime(sXmlFileName, &(si->LastUpdate)))
					memset(&(si->LastUpdate), 0, sizeof(FILETIME));
			}
		}
		else {
			// unload the feed
			if (si->Status == CSiteItem::Ok) {
				LOG1(3, "Unloading: '%S'", si->Name);
				UnloadFeed(si);
			}
		}
	}
}

void CTodayPluginWnd::UpdateFeedFlags(CSiteItem *si) {
	LOG0(3, "CTodayPluginWnd::UpdateFeedFlags()");

	if (si != NULL && si->Info != NULL) {
		if (si->Info->TodayShow && si->IsModified()) {
			LOG1(3, "Updating flags: '%S'", si->Name);
			// reload feed file
			ReloadFeed(si);
		}
	}
}

DWORD CTodayPluginWnd::Thread() {
	LOG0(1, "CTodayPluginWnd::Thread() - start");

	// wait until cache location is accessible
	// (if cache is located on sd-card, is it not available right after the reboot)
	while (!DirectoryExists(Config.CacheLocation))
		Sleep(1000);

	// load all feeds
	Loading = TRUE;
	SetEvent(HCheckFeeds);

	// periodically check each feed file and if it was changed, reload it
	HANDLE handles[] = { HTerminate, HCheckFeeds, HReloadSubscriptions, HUpdateFeedFlags };
	while (TRUE) {
		DWORD dwResult = WaitForMultipleObjects(countof(handles), handles, FALSE, INFINITE);
		if (dwResult == WAIT_OBJECT_0) {
			// terminate
			break;
		}
		else if (dwResult == WAIT_OBJECT_0 + 1) {
			// check if the feed was changed
			EnterCriticalSection(&CSSiteList);
			for (int i = 0; i < SiteList.GetCount(); i++) {
				CSiteItem *si = SiteList.GetAt(i);
				CheckFeedFile(si);

				// break loading if terminated is signaled
				if (WaitForSingleObject(HTerminate, 0) == WAIT_OBJECT_0)
					break;
			}
			UpdateUnreadNewCounts();
			LeaveCriticalSection(&CSSiteList);
			Loading = FALSE;
		}
		else if (dwResult == WAIT_OBJECT_0 + 2) {
			// reload subscriptions
			CSiteList tmpSiteList;
			LoadSiteList(tmpSiteList);

			CSiteItem *root = tmpSiteList.GetRoot();
			EnterCriticalSection(&CSSiteList);
			SiteList.Destroy();
			SiteList.CreateFrom(root);
			SiteList.SetRoot(root);

			if (SiteList.GetCount() > 0)
				SiteIdx = 0;
			else
				SiteIdx = -1;
			LeaveCriticalSection(&CSSiteList);

			SetEvent(HCheckFeeds);
		}
		else if (dwResult == WAIT_OBJECT_0 + 3) {
			// update feed flags
			EnterCriticalSection(&CSSiteList);
			for (int i = 0; i < SiteList.GetCount(); i++) {
				CSiteItem *si = SiteList.GetAt(i);
				UpdateFeedFlags(si);

				// break loading if terminated is signaled
				if (WaitForSingleObject(HTerminate, 0) == WAIT_OBJECT_0)
					break;
			}
			UpdateUnreadNewCounts();
			LeaveCriticalSection(&CSSiteList);
		}
	}

	CloseHandle(HThread);
	HThread = NULL;

	LOG0(1, "CTodayPluginWnd::Thread() - end");

	return 0;
}
