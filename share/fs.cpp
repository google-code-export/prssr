/**
 *  fs.cpp
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
#include "fs.h"

//
// Creates path for file 'fileNamePath'
//
BOOL CreatePath(const CString &fileNamePath) {
	// remove file name
	int slashPos = fileNamePath.ReverseFind('\\');
	CString strPath = fileNamePath.Left(slashPos + 1);

	// create path
	int start = 0;
	int pos = strPath.Find('\\', start);
	while (pos != -1) {
		if (start != -1) {
			CString strDirName = strPath.Left(pos);
			CreateDirectory(strDirName, NULL);
		}

		start = pos + 1;
		pos = strPath.Find('\\', start);
	}

	return TRUE;
}

BOOL FileExists(const CString &strFileName) {
	HANDLE file = CreateFile(strFileName, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	else {
		CloseHandle(file);
		return TRUE;
	}
}

BOOL DirectoryExists(const CString &strDirName) {
	DWORD attrs = GetFileAttributes(strDirName);
	if (attrs == 0xFFFFFFFF)
		return FALSE;
	else if ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return TRUE;
	else
		return FALSE;
}

BOOL GetFileTime(LPCTSTR fileName, FILETIME *ft) {
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		BOOL ret = GetFileTime(hFile, NULL, NULL, ft);
		CloseHandle(hFile);

		return ret;
	}
	else
		return FALSE;
}

BOOL IsFileChanged(LPCTSTR fileName, FILETIME *ft) {
	if (ft == NULL)
		return FALSE;

	BOOL change = FALSE;

	FILETIME lastWrite;
	if (GetFileTime(fileName, &lastWrite)) {
		if (CompareFileTime(ft, &lastWrite) != 0) {
			*ft = lastWrite;
			change = TRUE;
		}
	}
	else {
		if (ft->dwLowDateTime == 0 && ft->dwHighDateTime == 0)
			change = FALSE;
		else {
			change = TRUE;
			memset(ft, 0, sizeof(FILETIME));
		}
	}

	return change;
}

//
// Removes empty directories traversing from 'rootPath' deep in the structure
// 'rootPath' is not deleted
//
void RemoveDirectories(const CString &rootPath) {
	CString path;
	path.Format(_T("%s\\*.*"), rootPath);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString fileNamePath;
				fileNamePath.Format(_T("%s\\%s"), rootPath, CString(fd.cFileName));
				// remove the content of directory
				RemoveDirectories(fileNamePath);
				// remove the directory itself
				RemoveDirectory(fileNamePath);
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}

void ScanDirForFiles(const CString &dir, const CString &mask, CList<CFileData *, CFileData *> &list) {
	CString path;
	path.Format(_T("%s\\*.*"), dir);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			CString fileNamePath;
			fileNamePath.Format(_T("%s\\%s"), dir, CString(fd.cFileName));

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				ScanDirForFiles(fileNamePath, mask, list);
			}
			else {
				list.AddTail(new CFileData(fileNamePath, fd.nFileSizeLow));
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}

BOOL RemoveLastPathPart(CString &strPath) {
	int slashPos = strPath.ReverseFind('\\');
	if (slashPos != -1) {
		strPath = strPath.Left(slashPos);
		return TRUE;
	}
	else
		return FALSE;
}
