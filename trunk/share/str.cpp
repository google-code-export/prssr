/**
 *  str.cpp
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
#include "str.h"

char *WCharToChar(const CString &str, UINT codePage/* = CP_ACP*/) {
	int len = WideCharToMultiByte(codePage, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	char *conv = new char [len + 1];
	WideCharToMultiByte(codePage, 0, str, str.GetLength(), conv, len, NULL, NULL);
	conv[len] = '\0';
	return conv;
}

/*CString Utf8ToWChar(const char *str) {
	int len = strlen(str);
	int wLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	TCHAR *wStr = new TCHAR [wLen];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wStr, wLen);
	wStr[wLen - 1] = '\0';
	CString s;
	s.Format(_T("%s"), wStr);
	delete [] wStr;
	return s;
}
*/

CString CharToWChar(const char *str, UINT codePage) {
	int wLen = MultiByteToWideChar(codePage, 0, str, -1, NULL, 0);
	CString s;
	LPTSTR wStr = s.GetBufferSetLength(wLen - 1);
	MultiByteToWideChar(codePage, 0, str, -1, wStr, wLen - 1);
	return s;
}

CString CharToWChar(const char *str) {
	int len = strlen(str);
	CString s;
	LPTSTR sStr = s.GetBufferSetLength(len);
	for (int i = 0; i < len; i++)
		s.SetAt(i, (TCHAR) str[i]);

	return s;
}

char *DuplicateString(const char *s) {
	int len = strlen(s);
	char *dup = new char [len + 1];
	strncpy(dup, s, len);
	dup[len] = '\0';
	return dup;
}
