/**
 *  shnotif.cpp
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

#include "../StdAfx.h"
#include "../prssr.h"
#include "shnotif.h"

//
// notification
//

// from AYGSHELL.H from PPC 2003 SDK

// Force the display to turn on for notification.
#define SHNF_DISPLAYON       0x00000010
// Force the notification to be silent and not vibrate, regardless of Settings
#define SHNF_SILENT          0x00000020

// pRSSreader notification ID
#define PRSSRSRV_NOTIFICATION_ID				2

// {5056DB7C-77D7-4DEE-A7B5-8B9578370A8F}
static const GUID CLSID_pRSSreaderNotification = 
	{ 0x5056db7c, 0x77d7, 0x4dee, { 0xa7, 0xb5, 0x8b, 0x95, 0x78, 0x37, 0xa, 0x8f } };

static SHNOTIFICATIONDATAEX SND = { 0 };

void prssrNotification(int newItems) {
	LOG1(5, "prssrNotification(%d)", newItems);

	CString sTitle;
	sTitle.LoadString(IDS_PRSSR_NOTIFICATION);

	CString sText, sHide, sOpen;
	sText.Format(IDS_NEW_ITEMS, newItems);
	sHide.LoadString(IDS_HIDE);
	sOpen.LoadString(IDS_OPEN);

	TCHAR buf[512];

	memset(&SND, 0, sizeof(SND));
	if (OSVI.dwMajorVersion >= 5) {
		swprintf(buf,
			L"<html><body>"
			L"<p>%s</p>"
			L"</body></html>", sText);

		SND.cbStruct = sizeof(SHNOTIFICATIONDATAEX);

		SND.rgskn[0].pszTitle = sOpen;
		SND.rgskn[0].skc.wpCmd = ID_OPEN;
		SND.rgskn[0].skc.grfFlags = NOTIF_SOFTKEY_FLAGS_DISMISS;
		SND.rgskn[1].pszTitle = sHide;
		SND.rgskn[1].skc.wpCmd = ID_HIDE;
		SND.rgskn[1].skc.grfFlags = NOTIF_SOFTKEY_FLAGS_DISMISS;
	}
	else {
		swprintf(buf,
			L"<html><body>"
			L"<p>%s</p>"
			L"<hr size=1/>"
			L"<table width=100%%>"
			L"<tr>"
			L"<td><a href=\"cmd:%d\">%s</a></td>"
			L"<td align=right><input type=button name='cmd:%d' value='%s'></td>"
			L"</tr>"
			L"</table>"
			L"</form></p>"
			L"</body></html>", sText, 
			ID_OPEN, sOpen,
			ID_HIDE, sHide);

		SND.cbStruct = sizeof(SHNOTIFICATIONDATA);
	}

	SND.grfFlags = SHNF_STRAIGHTTOTRAY | SHNF_SILENT;
//	SND.grfFlags = SHNF_STRAIGHTTOTRAY;
//	SND.grfFlags = SHNF_SILENT;
	SND.dwID = PRSSRSRV_NOTIFICATION_ID;
	SND.npPriority = SHNP_INFORM;
	SND.csDuration = (DWORD) 0xFFFFFFFF;
	SND.hicon = (HICON) LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOTIFY), IMAGE_ICON, SCALEX(16), SCALEY(16), 0);
	SND.hwndSink = AfxGetMainWnd()->GetSafeHwnd();
	SND.pszHTML = buf;
	SND.pszTitle = sTitle;
	SND.lParam = 0;
	SND.clsid = CLSID_pRSSreaderNotification;

	SHNotificationAdd((SHNOTIFICATIONDATA *) &SND);
}

void prssrNotificationRemove() {
	// if we have cached new items, show notification
	SHNotificationRemove(&CLSID_pRSSreaderNotification, PRSSRSRV_NOTIFICATION_ID);
}

