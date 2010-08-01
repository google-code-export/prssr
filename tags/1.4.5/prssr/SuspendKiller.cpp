/**
 *  SuspendKiller.cpp
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

#include "StdAfx.h"
#include "SuspendKiller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DWORD WINAPI SuspendKillerThreadProc(LPVOID lpParameter) {
	CSuspendKiller *sk = (CSuspendKiller *) lpParameter;
	sk->ThreadProc();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSuspendKiller::CSuspendKiller() {
	Terminate = CreateEvent(NULL, FALSE, FALSE, NULL);
	Thread = NULL;
	Start();
}

CSuspendKiller::~CSuspendKiller() {
	Stop();
}

void CSuspendKiller::Start() {
	DWORD threadID;
	Thread = CreateThread(NULL, 0, SuspendKillerThreadProc, this, 0, &threadID);
}

void CSuspendKiller::Stop() {
	if (Thread != NULL) {
		DWORD exitCode;
		GetExitCodeThread(Thread, &exitCode);
		if (exitCode == STILL_ACTIVE) {
			SetEvent(Terminate);
			WaitForSingleObject(Thread, INFINITE);
			CloseHandle(Thread);
			Thread = NULL;
		}
	}
}

void CSuspendKiller::ThreadProc() {
	BOOL running = TRUE;
	while (running) {	
		SystemIdleTimerReset();
		keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);

		DWORD res = WaitForSingleObject(Terminate, 20000);	// every 20 secs
		if (res == WAIT_OBJECT_0)
			running = FALSE;
	}
}
