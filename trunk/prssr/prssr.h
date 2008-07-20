/**
 *  prssr.h : main header file for the PRSSR application
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

#if !defined(AFX_PRSSR_H__7D968694_006D_44B7_8AF8_EA20FC7B7AD1__INCLUDED_)
#define AFX_PRSSR_H__7D968694_006D_44B7_8AF8_EA20FC7B7AD1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "../share/resource.h"       // main symbols
#include "../share/defs.h"
#include "../share/wnd.h"

#ifdef MYDEBUG
#include "debug/crtdbg.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrssrApp:
// See prssr.cpp for the implementation of this class
//
//class garbageCollector;

class CPrssrApp : public CWinApp {
public:
	CPrssrApp();
	~CPrssrApp();

	HINSTANCE GetDPISpecificInstanceHandle() { return m_hResDLL; }

#ifdef MYDEBUG
	garbageCollector *gb;
#endif

protected:
	HINSTANCE m_hResDLL;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrssrApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	virtual BOOL IsIdleMessage(MSG* pMsg);

// Implementation

	//{{AFX_MSG(CPrssrApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

extern CPrssrApp theApp;

//extern HANDLE HUpdateFeedFlagsEvent;

extern OSVERSIONINFO OSVI;

//TCHAR *LoadStr(int resID);
BOOL SysError(int resID, DWORD err, BOOL quiet, ...);

//
extern UINT ReadConfigMessage;
extern UINT ReloadSubscriptionsMessage;
extern UINT CheckFeedsMessage;
extern UINT UpdateFeedFlagsMessage;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRSSR_H__7D968694_006D_44B7_8AF8_EA20FC7B7AD1__INCLUDED_)
