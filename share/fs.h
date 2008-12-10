/**
 *  fs.h
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

#ifndef _FS_H_
#define _FS_H_

#include <afxtempl.h>

class CFileData {
public:
	CFileData(const CString &name, DWORD size) {
		Name = name;
		Size = size;
	}

	CString Name;
	DWORD Size;
};


BOOL CreatePath(const CString &fileNamePath);
BOOL FileExists(const CString &strFileName);
BOOL DirectoryExists(const CString &strDirName);

BOOL GetFileTime(LPCTSTR fileName, FILETIME *ft);

BOOL IsFileChanged(LPCTSTR fileName, FILETIME *ft);

void RemoveDirectories(const CString &rootPath);
void ScanDirForFiles(const CString &dir, const CString &mask, CList<CFileData *, CFileData *> &list);
BOOL RemoveLastPathPart(CString &strPath);

#endif
