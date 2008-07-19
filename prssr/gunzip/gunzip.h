// gunzip.h: interface for gunzip.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GUNZIP_H_)
#define _GUNZIP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL GunzipFile(LPCTSTR inFile, LPCTSTR outFile);
BOOL InflateFile(LPCTSTR inFile, LPCTSTR outFile);

#endif // !defined(_GUNZIP_H_)
