/*****************************************************************

FILENAME:	crtdbg.h

AUTHOR:		Ciprian Miclaus (Ciprian_Miclaus@yahoo.com)

DESCRIPTION:
Detects memory leaks in eVC++ almost the same way crtdbg does in VC++.
At the end of program execution it will display in the debug window if
there were any memory leaks and how the memory looks so you can identify
where your memory leak occurred. It will display in the debug window a
message saying no memory leaks detected if there are no memory leaks.
Similar to what crtdbg does in VC++.
It will fail to display the line same as crtdbg does.
There are 3 simple steps in order to enable memory leak detection:
1. Define _DEBUG
#define _DEBUG
2. Include "crtdbg.h"
#include "crtdbg.h"
3. Let your first line in the code be:
_CrtSetDbgFlag (ON);

Tips on debugging:
Tip 1:
Altho it doesn't display the line where the memory leak occurred (read
Tip 2), the utility display the address in hexa, and you can add a small
code to the operator new function, just after the first malloc:
	if (retPtr == (void*)0x76DA0)
		dumb instruction; <- place a breakpoint on this one
so you can detect easily which line of your code called the operator new
to allocate memory at the specified address and wasn't freed.
Tip 2:
Here's a trick that allow you to get the correct line and filename where
the memory leak occurred. Define the following line in every file, or define
it in a .h and include it in every file where you want accurate line and
filename:

#define new new(_T(__FILE__), __LINE__)

Happy debugging!

LICENSE: Public domain

COMMENTS:
Please report any bugs to Ciprian_Miclaus@yahoo.com.
You can use and distribute this code freely, but please keep these
few lines.
If you make any improvements, or have any ideas about how this code
could be improved or just you feel you need to comment this code in
any way, please send your comments, idea, imporvements to me to my 
email above.
The code doesn't detect memory leaks generated with C functions:
malloc, calloc, free, but that can be done in the future. Let me know
and I will program it.

********************************************************************/

#include "crtdbg.h"

#ifdef MYDEBUG

#include "Logger.h"

CRITICAL_SECTION gcCS;

_CrtMem*			_CrtMemRoot = 0;
_CrtFileName*		_CrtFileNameRoot = 0;

void* __cdecl operator new(
        size_t s,
        TCHAR *name,
        int line
        )
{
	EnterCriticalSection(&gcCS);
	void*	retPtr = malloc (s);
	if (retPtr) {
		_CrtMem*	_crtMemCell = (struct _CrtMem*)malloc (sizeof(_CrtMem));
		_crtMemCell->_CrtLine		= line;
		_crtMemCell->_CrtMemLen		= s;
		_crtMemCell->_CrtMemAddr	= retPtr;
		_crtMemCell->_CrtNext		= 0;
		
		_CrtFileName*	_tmpCrtFileName;
		for (_tmpCrtFileName = _CrtFileNameRoot; _tmpCrtFileName && wcscmp(name, _tmpCrtFileName->_CrtName); _tmpCrtFileName = _tmpCrtFileName->_CrtNext);
		if (!_tmpCrtFileName) {
			TCHAR *_crtName = (TCHAR *) malloc ((wcslen (name) + 1) * sizeof(TCHAR));
			wcscpy (_crtName, name);
			_CrtFileName*	_crtFileName = (struct _CrtFileName*)malloc (sizeof (_CrtFileName));
			_crtFileName->_CrtName = _crtName;
			_crtFileName->_CrtNext = 0;
			if (!_CrtFileNameRoot)
				_CrtFileNameRoot = _crtFileName;
			else {
				for (_tmpCrtFileName = _CrtFileNameRoot; _tmpCrtFileName->_CrtNext; _tmpCrtFileName = _tmpCrtFileName->_CrtNext);
				_tmpCrtFileName->_CrtNext = _crtFileName;
			}
			_tmpCrtFileName = _crtFileName;
		}

		_crtMemCell->_CrtFileName = _tmpCrtFileName;

		if (!_CrtMemRoot) {
			_CrtMemRoot		= _crtMemCell;
		}
		else {
			_CrtMem*	_tmpMemPtr;
			for (_tmpMemPtr = _CrtMemRoot; _tmpMemPtr->_CrtNext; _tmpMemPtr = _tmpMemPtr->_CrtNext);
			_tmpMemPtr->_CrtNext = _crtMemCell;
		}
	}
	LeaveCriticalSection(&gcCS);

	return retPtr;

}

void __cdecl operator delete(void *pvMem)
{
	EnterCriticalSection(&gcCS);

	if (pvMem) {
		_CrtMem*	_tmpMem;
		if (pvMem == _CrtMemRoot->_CrtMemAddr) {
			_tmpMem			= _CrtMemRoot;
			_CrtMemRoot		= _CrtMemRoot->_CrtNext;
			free (_tmpMem);
		}
		else {
			for (_tmpMem = _CrtMemRoot; _tmpMem->_CrtNext && (_tmpMem->_CrtNext->_CrtMemAddr != pvMem); _tmpMem = _tmpMem->_CrtNext);
			if (_tmpMem->_CrtNext) {
				_CrtMem*	_tmpMem2;
				_tmpMem2	= _tmpMem->_CrtNext;
				_tmpMem->_CrtNext = _tmpMem2->_CrtNext;
				free (_tmpMem2);
			}
			else {
				NKDbgPrintfW (_T("%s(%i) : Warning : deletes memory pointer not allocated with new!\n"), _T(__FILE__), __LINE__);
#ifdef LOGGING
				LOG2(1, "%s(%i) : Warning : deletes memory pointer not allocated with new!", __FILE__, __LINE__);
#endif
			}
		}
		free (pvMem);
	}

	LeaveCriticalSection(&gcCS);
}


garbageCollector::garbageCollector() {
	InitializeCriticalSection(&gcCS);
}

garbageCollector::~garbageCollector ()
{
	if (!_CrtMemRoot) {
		NKDbgPrintfW (_T("No memory leaks detected!\n"));
#ifdef LOGGING
		LOG0(1, "No memory leaks detected!");
#endif
	}
	else {
		_CrtMem*	_tmpMem;
		NKDbgPrintfW (_T("Detected memory leaks!\nDumping objects ->\n"));
#ifdef LOGGING
		LOG0(1, "Detected memory leaks! Dumping objects...");
#endif
		for (_tmpMem = _CrtMemRoot; _tmpMem; _tmpMem = _tmpMem->_CrtNext) {
			NKDbgPrintfW (_T("%s(%i) : normal block at 0x%08X, %i bytes long\n Data <"), _tmpMem->_CrtFileName->_CrtName, _tmpMem->_CrtLine, _tmpMem->_CrtMemAddr, _tmpMem->_CrtMemLen);
#ifdef LOGGING
			LOG4(1, "%S(%i) : normal block at 0x%08X, %i bytes long", _tmpMem->_CrtFileName->_CrtName, _tmpMem->_CrtLine, _tmpMem->_CrtMemAddr, _tmpMem->_CrtMemLen);
#endif
			for (unsigned int i = 0; i < _tmpMem->_CrtMemLen; i++)
				NKDbgPrintfW (_T("%c"), *(((char*)_tmpMem->_CrtMemAddr)+i));
			NKDbgPrintfW (_T(">\n"));
		}
	}
	_CrtFileName*	_tmpName = _CrtFileNameRoot;
	for (;_tmpName;) {
		_CrtFileNameRoot	= _tmpName->_CrtNext;
		free(_tmpName->_CrtName);
		free(_tmpName);
		_tmpName			= _CrtFileNameRoot;
	}

	DeleteCriticalSection(&gcCS);
}

#endif