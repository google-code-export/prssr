/**
 *  date.h
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

#ifndef SHARE_DATE_H_
#define SHARE_DATE_H_

long int MakeLinearDate(SYSTEMTIME &st);
int SystemTimeToLong(SYSTEMTIME *today);
BOOL FormatDateTime(CString &strDateTime, SYSTEMTIME date, BOOL relDates = FALSE);
SYSTEMTIME TimeToTimeZone(SYSTEMTIME *time);
SYSTEMTIME TimeToGMT(SYSTEMTIME *time, int tzhour, int tzmin);
void SystemTimeToLinearDate(SYSTEMTIME *st, FILETIME *ft);

#endif
