/**
 *  wnd.cpp : Miscelaneous helping functions
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

