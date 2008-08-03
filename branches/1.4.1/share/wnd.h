/**
 *  wnd.h : Miscelaneous helping functions related to windows 
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

#ifndef _WND_H_
#define _WND_H_

#define PRSSR_CLASS_NAME					L"pRSSreaderClass"
#define PRSSR_CLASS_NAME_FIND				L"WCE_"PRSSR_CLASS_NAME
#define PRSSR_WINDOW_NAME					L"pRSSreader"

// today class
#define TODAY_CLASS_NAME					L"pRSSreaderTodayClass"
#define TODAY_CLASS_NAME_FIND				L"WCE_"TODAY_CLASS_NAME
//#define TODAY_CLASS_NAME_FIND				TODAY_CLASS_NAME
#define TODAY_WINDOW_NAME					L"pRSSreaderTodayWindow"

/*
// service
#define SERVICE_CLASS_NAME					L"pRSSrSrvClass"
#define SERVICE_CLASS_NAME_FIND				L"WCE_"SERVICE_CLASS_NAME
#define SERVICE_WINDOW_NAME					L""
*/

HWND FindPRSSRMainWindow();
HWND FindPRssRTodayPluginWindow(HWND hWnd);
void NotifyTodayPlugin(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0);

#endif