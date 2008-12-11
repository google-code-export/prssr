/**
 *  Errors.h
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

#if !defined(AFX_ERRORS_H__42E1DFE6_27E9_4D48_A6EB_01463E733CCD__INCLUDED_)
#define AFX_ERRORS_H__42E1DFE6_27E9_4D48_A6EB_01463E733CCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc.h"

/////////////////////////////////////////////////////////////////////////////////////////

struct CErrorItem {
	enum {
		Site,
		File,
		System,
		Other
	} Type;
	SYSTEMTIME Time;
	CString Message;

	union {
		struct {			// update site
			BOOL UpdateOnly;
		};
		struct {			// file
			EFileType FileType;
		};
	};
	int SiteIdx;
	CString Url;			// url of the file to download

	CErrorItem(const CString &errorMsg, SYSTEMTIME *time = NULL) {
		if (time == NULL)
			GetLocalTime(&Time);
		else
			Time = *time;

		Message = errorMsg;
		Type = Other;
	}

	CErrorItem(UINT nID, SYSTEMTIME *time = NULL) {
		if (time == NULL)
			GetLocalTime(&Time);
		else
			Time = *time;

		Message.LoadString(nID);
		Type = Other;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class CErrors {
public:
	CErrors();
	virtual ~CErrors();

	void Add(CErrorItem *item);
	void Delete(CErrorItem *item);
	void Cleanup();

	int GetCount() { return Items.GetCount(); }
	POSITION GetFirstPos() { return Items.GetHeadPosition(); }
	CErrorItem *GetNext(POSITION &pos) { return Items.GetNext(pos); }

	void Lock() { EnterCriticalSection(&CSItems); }
	void Unlock() { LeaveCriticalSection(&CSItems); }

protected:
	CList<CErrorItem *, CErrorItem *> Items;
	CRITICAL_SECTION CSItems;
};

/////////////////////////////////////////////////////////////////////////////////////////

BOOL Error(LPCTSTR str);
BOOL Error(const CString &str, ...);
BOOL Error(UINT nID, ...);

CString FormatSysError(DWORD errCode);
BOOL SysError(int resID, DWORD err, BOOL quiet, ...);

BOOL PrssrMessageBox(UINT nCaption, UINT nText, DWORD style, UINT btnID = IDS_DONE);

/////////////////////////////////////////////////////////////////////////////////////////

extern CErrors Errors;

#endif // !defined(AFX_ERRORS_H__42E1DFE6_27E9_4D48_A6EB_01463E733CCD__INCLUDED_)
