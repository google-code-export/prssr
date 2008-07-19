// Errors.cpp: implementation of the CErrors class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Errors.h"

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

