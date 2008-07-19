// setup.h : main header file for the SETUP DLL
//

#if !defined(AFX_SETUP_H__86A4F6A1_EEF8_468C_B540_C43A2B181326__INCLUDED_)
#define AFX_SETUP_H__86A4F6A1_EEF8_468C_B540_C43A2B181326__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSetupApp
// See setup.cpp for the implementation of this class
//

class CSetupApp : public CWinApp
{
public:
	CSetupApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSetupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUP_H__86A4F6A1_EEF8_468C_B540_C43A2B181326__INCLUDED_)
