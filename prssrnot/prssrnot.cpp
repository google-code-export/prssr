/**
 *  prssrnot.cpp : Defines the entry point for the application.
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


#include "stdafx.h"
#include "../share/defs.h"
#include "../share/wnd.h"
#include "../share/reg.h"
#include "../share/notif.h"
#include "../prssr/net/connection.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_APP);
	CRegistry regConf(HKEY_CURRENT_USER, REG_KEY_CONFIGURATION);

	CString strCmdLine(lpCmdLine);

	if (strCmdLine.Find(_T("/daily")) != -1) {
		SYSTEMTIME tm, updateAtTime;
		GetLocalTime(&updateAtTime);
		if (regConf.Read(szUpdateAtTime, &tm, sizeof(tm))) {
			updateAtTime.wHour = tm.wHour;
			updateAtTime.wMinute = tm.wMinute;
			updateAtTime.wSecond = tm.wSecond;
		}

		// reschedule event
		AddSeconds(updateAtTime, 60 * 60 * 24);			// 1 day

		SetupUpdateEvent(_T("/daily"), TRUE, &updateAtTime);
	}

	if (strCmdLine.Find(_T("/periodic")) != -1) {
		DWORD updateInterval = regConf.Read(szUpdateInterval, 0);

		// reschedule event
		if (updateInterval > 0) {
			SYSTEMTIME st;
			GetLocalTime(&st);
			AddSeconds(st, 60 * updateInterval);

			SetupUpdateEvent(_T("/periodic"), TRUE, &st);
		}
	}

	if (strCmdLine.Find(_T("AppRunAtRs232Detected")) != -1) {
		// wait for the connection
		int retry = 10;
		while (retry > 0 && Connection.IsAvailable() != S_OK) {
			Sleep(2000);
			retry--;
		}
	}

	//
	HWND hWnd = FindPRSSRMainWindow();
	if (hWnd == NULL) {
		// pRSSreader is not running -> start it
		CString sInstallDir = reg.Read(szInstallDir, _T(""));

		BOOL bMinimized = regConf.Read(szBackgroundUpdate, FALSE);

		CString sPars = _T("/updateall");
		if (bMinimized)
			sPars += CString(" /minimized");

		CString sPrssrBin;
		sPrssrBin.Format(_T("%s\\%s"), sInstallDir, PRSSREADER_BIN);
		CreateProcess(sPrssrBin, sPars, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		return 0;
	}
	else {
		// pRSSreader is running -> send update all message
		::PostMessage(hWnd, UWM_UPDATE_ALL, 0, 0);
	}

	return 0;
}

