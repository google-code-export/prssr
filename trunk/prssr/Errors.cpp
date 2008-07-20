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

