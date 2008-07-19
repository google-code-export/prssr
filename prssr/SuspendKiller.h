// SuspendKiller.h: interface for the CSuspendKiller class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SUSPENDKILLER_H_)
#define _SUSPENDKILLER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSuspendKiller {
public:
	CSuspendKiller();
	virtual ~CSuspendKiller();

	void Start();
	void Stop();

protected:
	HANDLE Terminate;			// terminating event
	HANDLE Thread;				// handle of thread

	void ThreadProc();

	friend DWORD WINAPI SuspendKillerThreadProc(LPVOID lpParameter);
};

#endif // !defined(_SUSPENDKILLER_H_)
