// Errors.h: interface for the CErrors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORS_H__42E1DFE6_27E9_4D48_A6EB_01463E733CCD__INCLUDED_)
#define AFX_ERRORS_H__42E1DFE6_27E9_4D48_A6EB_01463E733CCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../share/helpers.h"

/////////////////////////////////////////////////////////////////////////////////////////

struct CErrorItem {
	enum {
		Site,
		File,
		Other
	} Type;
	SYSTEMTIME Time;
//	CString SiteName;
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

//	CErrorItem(SYSTEMTIME *time, const CString &siteName, const CString &errorMsg) {
/*	CErrorItem(SYSTEMTIME *time, const CString &errorMsg) {
		Time = *time;
//		SiteName = siteName;
		ErrorMsg = errorMsg;
	}
*/

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

extern CErrors Errors;

#endif // !defined(AFX_ERRORS_H__42E1DFE6_27E9_4D48_A6EB_01463E733CCD__INCLUDED_)
