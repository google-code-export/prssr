/**
 *  date.cpp
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
#include "date.h"

// structure to hold date
struct date {
	int month;
	int day;
	int year;
};

// part of # of days calc.
long int funct1 (int y, int m) {
	long int result;
	if ( m <= 2 )
		y -= 1;
	result = y;
	return (result);
}

long int funct2 (int m) {
	long int result;
	if ( m <= 2 )
		result = m + 13;
	else
		result = m + 1;
	return(result);
}

/* Function to calculate the number of days in dates */
long int day_count (int m, int d, int y) {
	long int number;
	number = 1461 * funct1(y, m) / 4 + 153 * funct2(m) / 5 + d;
	return (number);
}

long int MakeLinearDate(SYSTEMTIME &st) {
	return day_count(st.wMonth, st.wDay, st.wYear);
}

int SystemTimeToLong(SYSTEMTIME *today) {
	return (today->wHour * 60) + today->wMinute;
}

SYSTEMTIME TimeToTimeZone(SYSTEMTIME *time) {
	SYSTEMTIME st = *time;
	FILETIME ft, lFt;
	SystemTimeToFileTime(&st, &ft);
	FileTimeToLocalFileTime(&ft, &lFt);
	FileTimeToSystemTime(&lFt, &st);

	return st;
}

SYSTEMTIME TimeToGMT(SYSTEMTIME *time, int tzhour, int tzmin) {
	CTime tm(*time);

	if (tzhour < 0)
		tm -= CTimeSpan(0, abs(tzhour), abs(tzmin), 0);
	else
		tm += CTimeSpan(0, abs(tzhour), abs(tzmin), 0);

	SYSTEMTIME st;
	if (tm.GetAsSystemTime(st))
		return st;
	else
		return *time;
}

void SystemTimeToLinearDate(SYSTEMTIME *st, FILETIME *ft) {
	DWORD hi = 0;
	hi = (st->wYear - 2000);
	hi = (hi << 4) | (st->wMonth - 1);
	hi = (hi << 5) | (st->wDay - 1);
	hi = (hi << 5) | (st->wHour);
	hi = (hi << 6) | (st->wMinute);
	hi = (hi << 6) | (st->wSecond);

	ft->dwHighDateTime = hi;
	ft->dwLowDateTime = st->wMilliseconds;
}
