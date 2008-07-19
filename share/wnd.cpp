//
// Miscelanous helping functions
//

#include <windows.h>
#include "wnd.h"
#include <afxwin.h>
#include "../prssr/debug/Logger.h"

HWND FindPRSSRMainWindow() {
	return ::FindWindow(PRSSR_CLASS_NAME_FIND, PRSSR_WINDOW_NAME);
}

HWND FindPRssRTodayPluginWindow(HWND hWnd) {
	if (hWnd == NULL)
		return NULL;

	TCHAR className[64];	// should be long enough to store CLASS_NAME
	GetClassName(hWnd, className, 64);

	if (wcscmp(className, TODAY_CLASS_NAME_FIND) == 0)
		return hWnd;

	// iterate siblings
	HWND hChild = GetWindow(hWnd, GW_CHILD);
	while (hChild != NULL) {
		HWND hToday = FindPRssRTodayPluginWindow(hChild);
		if (hToday != NULL)
			return hToday;

		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}

	return NULL;
}

void NotifyTodayPlugin(UINT msg, WPARAM wParam/* = 0*/, LPARAM lParam/* = 0*/) {
	HWND hWnd = FindWindow(_T("DesktopExplorerWindow"), NULL);
	HWND hPRSSRTodayWnd = FindPRssRTodayPluginWindow(hWnd);
	if (hPRSSRTodayWnd != NULL) {
		LOG4(3, "Posting msg to Today Plugin (%x) = %d, WPARAM = %d, LPARAM = %d", hWnd, msg, wParam, lParam);
		::PostMessage(hPRSSRTodayWnd, msg, wParam, lParam);
	}
}

