// notif.cpp - notifications
//

#include <afx.h>
#include "notif.h"
#include <notify.h>
#include "defs.h"


// Cradling Event

static HANDLE GetCradlingEvent() {
	HANDLE handle = NULL;

	DWORD nNotifs;
	if (!CeGetUserNotificationHandles(NULL, 0, &nNotifs))
		return NULL;

	HANDLE *hNotifHandlers = new HANDLE [nNotifs];
	if (hNotifHandlers == NULL)
		return NULL;

	DWORD nNumHandlers;
	if (CeGetUserNotificationHandles(hNotifHandlers, nNotifs, &nNumHandlers)) {
		PBYTE pBuff = (PBYTE) LocalAlloc(LPTR, 8192);
		if (pBuff) {
			for (DWORD i = 0; i < nNumHandlers; i++) {
				DWORD dwSize = 0;
				if (CeGetUserNotification(hNotifHandlers[i], 8192, &dwSize, pBuff)) {
					PCE_NOTIFICATION_INFO_HEADER pnih = (PCE_NOTIFICATION_INFO_HEADER) pBuff;
					PCE_NOTIFICATION_TRIGGER pNotifTrigger = pnih->pcent;
					// Notice some events with NULL lpszApplication might be inserted!
					if (pNotifTrigger && pNotifTrigger->lpszApplication && 
//						!_tcsicmp(pNotifTrigger->lpszApplication, L"\\windows\\"PRSSRNOT_BIN) &&
						!_tcsicmp(pNotifTrigger->lpszApplication, PRSSRNOT_BIN) &&
//						!_tcsicmp(pNotifTrigger->lpszApplication, CString(Config.InstallDir + _T("\\") + PRSSREADER_BIN)) &&
						pNotifTrigger->dwEvent == NOTIFICATION_EVENT_RS232_DETECTED)
					{
						handle = pnih->hNotification;
						break;
					}
				}
			}
			LocalFree(pBuff);
		}
	}

	delete [] hNotifHandlers;

	return handle;
}

static HANDLE SetCradlingEvent() {
	// set a CE_NOTIFICATION_TRIGGER
	CE_NOTIFICATION_TRIGGER notifTrigger;
	memset(&notifTrigger, 0, sizeof(CE_NOTIFICATION_TRIGGER));
	notifTrigger.dwSize = sizeof(CE_NOTIFICATION_TRIGGER);
	notifTrigger.dwType = CNT_EVENT;
	notifTrigger.dwEvent = NOTIFICATION_EVENT_RS232_DETECTED;
	notifTrigger.lpszApplication = (LPTSTR) PRSSRNOT_BIN;
//	notifTrigger.lpszApplication = (LPTSTR) L"\\windows\\"PRSSRNOT_BIN;
//	notifTrigger.lpszApplication = (LPTSTR) (LPCTSTR) CString(Config.InstallDir + _T("\\") + PRSSREADER_BIN);
	notifTrigger.lpszArguments = L"AppRunAtRs232Detected";

	return CeSetUserNotificationEx(0, &notifTrigger, NULL);
}

//

void SetupCradlingEvent(BOOL bEnable) {
	HANDLE hCradling = GetCradlingEvent();
	if (hCradling != NULL) {
		CeClearUserNotification(hCradling);
		CloseHandle(hCradling);
	}

	if (bEnable) {
		// add cradling event
		CloseHandle(SetCradlingEvent());
	}
}

///////////////////////////////////////////////////////////////////////////////

static HANDLE GetUpdateNotification(LPCTSTR args) {
	HANDLE handle = NULL;

	DWORD nNotifs;
	if (!CeGetUserNotificationHandles(NULL, 0, &nNotifs))
		return NULL;

	HANDLE *hNotifHandlers = new HANDLE [nNotifs];
	if (hNotifHandlers == NULL)
		return NULL;

	DWORD nNumHandlers;
	if (CeGetUserNotificationHandles(hNotifHandlers, nNotifs, &nNumHandlers)) {
		PBYTE pBuff = (PBYTE) LocalAlloc(LPTR, 8192);
		if (pBuff) {
			for (DWORD i = 0; i < nNumHandlers; i++) {
				DWORD dwSize = 0;
				if (CeGetUserNotification(hNotifHandlers[i], 8192, &dwSize, pBuff)) {
					PCE_NOTIFICATION_INFO_HEADER pnih = (PCE_NOTIFICATION_INFO_HEADER) pBuff;
					PCE_NOTIFICATION_TRIGGER pNotifTrigger = pnih->pcent;
					// Notice some events with NULL lpszApplication might be inserted!
					if (pNotifTrigger && pNotifTrigger->lpszApplication && 
//						!_tcsicmp(pNotifTrigger->lpszApplication, L"\\windows\\"PRSSRNOT_BIN) &&
						!_tcsicmp(pNotifTrigger->lpszApplication, PRSSRNOT_BIN) &&
//						!_tcsicmp(pNotifTrigger->lpszApplication, CString(Config.InstallDir + _T("\\") + PRSSREADER_BIN)) &&
						pNotifTrigger->dwType == CNT_TIME && 
						!_tcsicmp(pNotifTrigger->lpszArguments, args))
					{
						handle = pnih->hNotification;
						break;
					}
				}
			}
			LocalFree(pBuff);
		}
	}

	delete [] hNotifHandlers;

	return handle;
}

static HANDLE SetUpdateNotification(SYSTEMTIME *st, LPCTSTR args) {
	// set a CE_NOTIFICATION_TRIGGER
	CE_NOTIFICATION_TRIGGER notifTrigger;
	memset(&notifTrigger, 0, sizeof(CE_NOTIFICATION_TRIGGER));
	notifTrigger.dwSize = sizeof(CE_NOTIFICATION_TRIGGER);
	notifTrigger.dwType = CNT_TIME;
	notifTrigger.stStartTime = *st;
//	notifTrigger.lpszApplication = (LPTSTR) L"\\windows\\"PRSSRNOT_BIN;
	notifTrigger.lpszApplication = (LPTSTR) PRSSRNOT_BIN;
//	notifTrigger.lpszApplication = (LPTSTR) (LPCTSTR) CString(Config.InstallDir + _T("\\") + PRSSREADER_BIN);
	notifTrigger.lpszArguments = (LPTSTR) args;

	return CeSetUserNotificationEx(0, &notifTrigger, NULL);
}


//

void AddSeconds(SYSTEMTIME &st, DWORD seconds) {
	__int64 ft;
	SystemTimeToFileTime(&st, (FILETIME *) &ft);

	ft += (__int64) seconds * 10000000;

	FileTimeToSystemTime((FILETIME *) &ft, &st);
}

/*
void SetupPeriodicNotification(BOOL bEnable, int updateInterval) {
	HANDLE hNotif = GetUpdateNotification(L"/periodic");

	if (hNotif != NULL) {
		CeClearUserNotification(hNotif);
		CloseHandle(hNotif);
	}

	if (bEnable) {
		// add notification
		SYSTEMTIME st;
		GetLocalTime(&st);
		AddSeconds(st, 60 * updateInterval);

		CloseHandle(SetUpdateNotification(&st, L"/periodic"));
	}
}

void SetupDailyNotification(BOOL bEnable, SYSTEMTIME &updateTime) {
	HANDLE hNotif = GetUpdateNotification(L"/daily");

	// delete notification
	if (hNotif != NULL) {
		CeClearUserNotification(hNotif);
		CloseHandle(hNotif);
	}

	if (bEnable) {
		// add notification
		SYSTEMTIME st;
		GetLocalTime(&st);						// get date
		st.wHour = updateTime.wHour;
		st.wMinute = updateTime.wMinute;
		st.wSecond = updateTime.wSecond;

		AddSeconds(st, 60 * 60 * 24);			// 1 day

		CloseHandle(SetUpdateNotification(&st, L"/daily"));
	}
}
*/

void SetupUpdateEvent(LPCTSTR arg, BOOL bEnable, SYSTEMTIME *updateTime) {
	HANDLE hNotif = GetUpdateNotification(arg);

	if (hNotif != NULL) {
		CeClearUserNotification(hNotif);
		CloseHandle(hNotif);
	}

	if (bEnable) {
		// add notification
		CloseHandle(SetUpdateNotification(updateTime, arg));
	}
}
