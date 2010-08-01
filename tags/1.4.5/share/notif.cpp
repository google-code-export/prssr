/**
 *  notif.cpp : notifications
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

#include <afx.h>
#include "notif.h"
#include <notify.h>
#include "defs.h"

// NOTES: I wonder if one has to call CloseHandle() on the nofitication handles.
// In examples from SDK, all HANDLEs are local and no CloseHandle() calls.


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
						!_tcsicmp(pNotifTrigger->lpszApplication, PRSSRNOT_BIN) &&
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
	CE_NOTIFICATION_TRIGGER trigger;
	CE_USER_NOTIFICATION notification;

	memset(&trigger, 0, sizeof(CE_NOTIFICATION_TRIGGER));
	memset(&notification, 0, sizeof(CE_USER_NOTIFICATION));

	trigger.dwSize = sizeof(CE_NOTIFICATION_TRIGGER);
	trigger.dwType = CNT_EVENT;
	trigger.dwEvent = NOTIFICATION_EVENT_RS232_DETECTED;
	trigger.lpszApplication = (LPTSTR) PRSSRNOT_BIN;
	trigger.lpszArguments = L"AppRunAtRs232Detected";

	return CeSetUserNotificationEx(0, &trigger, NULL);
}

//

void SetupCradlingEvent(BOOL bEnable) {
	HANDLE hCradling = GetCradlingEvent();
	if (hCradling != NULL) {
		CeClearUserNotification(hCradling);
	}

	if (bEnable)
		SetCradlingEvent();
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
						!_tcsicmp(pNotifTrigger->lpszApplication, PRSSRNOT_BIN) &&
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
	CE_NOTIFICATION_TRIGGER trigger;
	CE_USER_NOTIFICATION notification;

	memset(&trigger, 0, sizeof(CE_NOTIFICATION_TRIGGER));
	memset(&notification, 0, sizeof(CE_USER_NOTIFICATION));

	trigger.dwSize = sizeof(CE_NOTIFICATION_TRIGGER);
	trigger.dwType = CNT_TIME;
	trigger.lpszApplication = (LPTSTR) PRSSRNOT_BIN;
	trigger.lpszArguments = (LPTSTR) args;
	trigger.stStartTime = *st;
	trigger.stEndTime = *st;

	return CeSetUserNotificationEx(0, &trigger, &notification);
}


//

void AddSeconds(SYSTEMTIME &st, DWORD seconds) {
	__int64 ft;
	SystemTimeToFileTime(&st, (FILETIME *) &ft);

	ft += (__int64) seconds * 10000000;

	FileTimeToSystemTime((FILETIME *) &ft, &st);
}

void SetupUpdateEvent(LPCTSTR arg, BOOL bEnable, SYSTEMTIME *updateTime) {
	HANDLE hNotif = GetUpdateNotification(arg);

	if (hNotif != NULL) {
		CeClearUserNotification(hNotif);
	}

	if (bEnable)
		SetUpdateNotification(updateTime, arg);
}
