// StdAfx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BA7C274F_4725_44BE_931A_70ED01652B79__INCLUDED_)
#define AFX_STDAFX_H__BA7C274F_4725_44BE_931A_70ED01652B79__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (_WIN32_WCE <= 200)
#error : This project does not support MFCCE 2.00 or earlier, because it requires CControlBar, available only in MFCCE 2.01 or later
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>

//#include <wininet.h>
#include <htmlctrl.h>
#include <winuser.h>

//#include <afxinet.h>
#include <afxmt.h>
#include <afxtempl.h>


#include "../share/shguim.h"
#include "../share/UIHelper.h"

#if defined(_WIN32_WCE) && (_WIN32_WCE >= 211) && (_AFXDLL)
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <winsock.h>
#include "debug/Logger.h"

#define SAFERELEASE(p) {if (p) {(p)->Release(); p = NULL;}}

#define countof(a) (sizeof(a) / sizeof(a[0]))

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BA7C274F_4725_44BE_931A_70ED01652B79__INCLUDED_)
