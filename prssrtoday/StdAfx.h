// StdAfx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E095F12D_6D94_4D5C_B7CE_8CE85C0F2CDC__INCLUDED_)
#define AFX_STDAFX_H__E095F12D_6D94_4D5C_B7CE_8CE85C0F2CDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// JAB 2008-11-06 - This solves "unresolved vftable" linker errors
#if _WIN32_WCE < 0x500 && _MSC_VER > 1220
// only needed for WM2003 builds under VS2005
#pragma comment(lib, "ccrtrtti.lib")
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
// OLE support available only for dynamic builds in MFC for Windows CE
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT

#include <afxtempl.h>
#include <afxmt.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <aygshell.h>
#include "todaycmn.h"

//#include "../share/shguim.h"
#include "../share/UIHelper.h"

#include "resource.h"

#include <winsock.h>
#include "../prssr/debug/Logger.h"

#define countof(a) (sizeof(a) / sizeof(a[0]))

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E095F12D_6D94_4D5C_B7CE_8CE85C0F2CDC__INCLUDED_)
