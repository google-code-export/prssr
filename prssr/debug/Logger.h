// Logger.h: interface for the CLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__44820D31_DBC9_436C_A95C_D4B05CCE691A__INCLUDED_)
#define AFX_LOGGER_H__44820D31_DBC9_436C_A95C_D4B05CCE691A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef LOGGING

#define LOG0(lvl, str) if (Logger != NULL) Logger->Log(lvl, str);
#define LOG1(lvl, str, p1) if (Logger != NULL) Logger->Log(lvl, str, p1);
#define LOG2(lvl, str, p1, p2) if (Logger != NULL) Logger->Log(lvl, str, p1, p2);
#define LOG3(lvl, str, p1, p2, p3) if (Logger != NULL) Logger->Log(lvl, str, p1, p2, p3);
#define LOG4(lvl, str, p1, p2, p3, p4) if (Logger != NULL) Logger->Log(lvl, str, p1, p2, p3, p4);
#define LOG5(lvl, str, p1, p2, p3, p4, p5) if (Logger != NULL) Logger->Log(lvl, str, p1, p2, p3, p4, p5);
#define LOG6(lvl, str, p1, p2, p3, p4, p5, p6) if (Logger != NULL) Logger->Log(lvl, str, p1, p2, p3, p4, p5, p6);
#define LOG7(lvl, str, p1, p2, p3, p4, p5, p6, p7) if (Logger != NULL) Logger->Log(lvl, str, p1, p2, p3, p4, p5, p6, p7);

//////////////////////////////////////////////////////////
// class CLogger

class CLogger {
public:
	CLogger(LPCTSTR fileName, int level = 1);
	virtual ~CLogger();

	void Log(int level, char *str, ...);

protected:
	HANDLE File;
	CRITICAL_SECTION CS;
	int Level;
};

extern CLogger *Logger;

#else

#define LOG0(lvl, str)
#define LOG1(lvl, str, p1)
#define LOG2(lvl, str, p1, p2)
#define LOG3(lvl, str, p1, p2, p3)
#define LOG4(lvl, str, p1, p2, p3, p4)
#define LOG5(lvl, str, p1, p2, p3, p4, p5)
#define LOG6(lvl, str, p1, p2, p3, p4, p5, p6)
#define LOG7(lvl, str, p1, p2, p3, p4, p5, p6, p7)

#endif


#endif // !defined(AFX_LOGGER_H__44820D31_DBC9_436C_A95C_D4B05CCE691A__INCLUDED_)
