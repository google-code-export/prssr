/**
 *  cache.cpp
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
#include "cache.h"

CCache::CCache() : Items(100) {
	InitializeCriticalSection(&CS);
}

CCache::~CCache() {
	DeleteCriticalSection(&CS);
}

BOOL CCache::InCache(const CString &item) {
	void *ptr;
	EnterCriticalSection(&CS);
	BOOL res = Items.Lookup(item, ptr);
	LeaveCriticalSection(&CS);

	return res;
}

BOOL CCache::InCache(const CString &item, void *&ptr) {
	EnterCriticalSection(&CS);
	BOOL res = Items.Lookup(item, ptr);
	LeaveCriticalSection(&CS);

	return res;
}

BOOL CCache::AddItem(const CString &item, void *ptr/* = NULL*/) {
	if (!InCache(item)) {
		EnterCriticalSection(&CS);
		Items.SetAt(item, ptr);
		LeaveCriticalSection(&CS);
		return TRUE;
	}
	else
		return FALSE;
}

void CCache::Empty() {
	EnterCriticalSection(&CS);
	Items.RemoveAll();
	LeaveCriticalSection(&CS);
}
