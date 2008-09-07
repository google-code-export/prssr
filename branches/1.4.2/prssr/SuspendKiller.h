/**
 *  SuspendKiller.h
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

#if !defined(_SUSPENDKILLER_H_)
#define _SUSPENDKILLER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSuspendKiller {
public:
	CSuspendKiller();
	virtual ~CSuspendKiller();

	void Start();
	void Stop();

protected:
	HANDLE Terminate;			// terminating event
	HANDLE Thread;				// handle of thread

	void ThreadProc();

	friend DWORD WINAPI SuspendKillerThreadProc(LPVOID lpParameter);
};

#endif // !defined(_SUSPENDKILLER_H_)
