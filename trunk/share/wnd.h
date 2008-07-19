//
// Miscelanous helping functions related to windows
//

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