/**
 *  Errors.cpp
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
#include "prssr.h"
#include "Errors.h"
#include "MessageDlg.h"

CErrors Errors;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CErrors::CErrors() {
	InitializeCriticalSection(&CSItems);
}

CErrors::~CErrors() {
	Cleanup();

	DeleteCriticalSection(&CSItems);
}

void CErrors::Add(CErrorItem *item) {
	Lock();
	Items.AddTail(item);
	Unlock();
}

void CErrors::Delete(CErrorItem *item) {
	Lock();
	POSITION pos = Items.Find(item);
	if (pos != NULL) {
		delete Items.GetAt(pos);
		Items.RemoveAt(pos);
	}
	Unlock();
}

void CErrors::Cleanup() {
	Lock();
	while (!Items.IsEmpty())
		delete Items.RemoveHead();
	Unlock();
}

// errors ////

BOOL Error(LPCTSTR str) {
	AfxMessageBox(str, MB_OK | MB_ICONERROR);
	return FALSE;
}

BOOL Error(UINT nID, ...) {
	static TCHAR buffer[1024];

	CString strMsg;
	strMsg.LoadString(nID);

	va_list args;
	va_start(args, nID);
	vswprintf(buffer, strMsg, args);

	return Error(buffer);
}


BOOL Error(const CString &str, ...) {
	static TCHAR buffer[1024];

	va_list args;
	va_start(args, str);
	vswprintf(buffer, str, args);

	return Error(buffer);
}

CString FormatSysError(DWORD errCode) {
	TCHAR errMsg[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errMsg, 1024, NULL);

	CString strMessage;
	strMessage.Format(_T("(%d) %s"), errCode, errMsg);

	return strMessage;
}

BOOL SysError(int resID, DWORD err, BOOL quiet, ...) {
	if (!quiet) {
		TCHAR msg[1024];
		msg[0] = 0;
		va_list arglist;
		va_start(arglist, quiet);

		CString strFmt;
		strFmt.LoadString(resID);
		vswprintf(msg, strFmt, arglist);
		va_end(arglist);

		CString strMessage;
		if (err != ERROR_SUCCESS)
			strMessage.Format(_T("%s\n\n%s"), msg, FormatSysError(err));
		else
			strMessage.Format(_T("%s"), msg);
		AfxMessageBox(strMessage);
	}

	return FALSE;
}

int PrssrMessageBox(UINT nCaption, UINT nText, DWORD style, UINT btnID/* = ID_DONE*/) {
	CMessageDlg dlg(nCaption, nText, style, btnID);
	return dlg.DoModal();
}
