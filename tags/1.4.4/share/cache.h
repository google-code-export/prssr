/**
 *  cache.h
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

#ifndef SHARE_CACHE_H_
#define SHARE_CACHE_H_

// cache

class CCache {
public:
	CCache();
	virtual ~CCache();

	BOOL InCache(const CString &item);
	BOOL InCache(const CString &item, void *&ptr);
	BOOL AddItem(const CString &item, void *ptr = NULL);
	void Empty();

protected:
	CMapStringToPtr Items;
	CRITICAL_SECTION CS;
};

#endif
