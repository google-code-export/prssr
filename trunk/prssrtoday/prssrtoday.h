// prssrtoday.h : main header file for the PRSSRTODAY DLL
//

#if !defined(AFX_PRSSRTODAY_H__74B4EFC5_446D_496C_A33A_33A5B1D79A82__INCLUDED_)
#define AFX_PRSSRTODAY_H__74B4EFC5_446D_496C_A33A_33A5B1D79A82__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#ifdef MYDEBUG
#include "../prssr/debug/crtdbg.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrssrtodayApp
// See prssrtoday.cpp for the implementation of this class
//

class CPrssrtodayApp : public CWinApp
{
public:
	CPrssrtodayApp();
	virtual ~CPrssrtodayApp();

#ifdef MYDEBUG
	garbageCollector gb;
#endif

	HINSTANCE GetInstanceHandle() { return AfxGetInstanceHandle(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrssrtodayApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPrssrtodayApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CPrssrtodayApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRSSRTODAY_H__74B4EFC5_446D_496C_A33A_33A5B1D79A82__INCLUDED_)
