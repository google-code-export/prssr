/**
 *  shnotif.h
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

#ifndef _SH_NOTIF_H_
#define _SH_NOTIF_H_

#define NOTIF_SOFTKEY_FLAGS_DISMISS		0x0001
#define NOTIF_SOFTKEY_FLAGS_HIDE		0x0004


typedef struct _SOFTKEYCMD {
	WPARAM wpCmd;
	DWORD grfFlags;
} SOFTKEYCMD;

typedef struct _SOFTKEYMENU {
	HMENU hMenu;
	SOFTKEYCMD *prgskc;
	UINT cskc;
} SOFTKEYMENU;

typedef struct _SOFTKEYNOTIFY {
	LPCTSTR pszTitle;
	SOFTKEYCMD skc;
} SOFTKEYNOTIFY;

#define NOTIF_NUM_SOFTKEYS			2

typedef struct _SHNOTIFICATIONDATAEX {
	DWORD    cbStruct;     // for verification and versioning
	DWORD    dwID;         // identifier for this particular
						  // notification
	SHNP     npPriority;   // priority
	DWORD    csDuration;   // duration of the notification
						  // (usage depends on prior)
	HICON    hicon;        // the icon for the notification
	DWORD    grfFlags;     // flags - see SHNF_ flags below
	CLSID    clsid;        // unique identifier for the
						  // notification class
	HWND     hwndSink;     // window to receive command choices,
						  // dismiss, etc.
	LPCTSTR  pszHTML;      // HTML content for the bubble
	LPCTSTR  pszTitle;     // Optional title for bubble
	LPARAM   lParam;       // User-defined parameter
	// From here, this is WM 5.0 stuff
	union
	{                      // Defines the softkey bar for the
						  // notification
	   SOFTKEYMENU skm;   // Either pass an HMENU in skn
						  // (and set SHNF_HASMENU)
						  // or two softkeys in rgskn.
	   SOFTKEYNOTIFY rgskn[NOTIF_NUM_SOFTKEYS];
	};
	// Text to put on SK2 on the Today screen. If NULL, will
	// default to "Notification"
	LPCTSTR    pszTodaySK;
	// What to execute when SK2 is pressed. If NULL, the toast
	// will be displayed.
	LPCTSTR    pszTodayExec;
} SHNOTIFICATIONDATAEX;

///

void prssrNotification(int newItems);
void prssrNotificationRemove();


#endif // _SH_NOTIF_H_