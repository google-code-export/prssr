/**
 *  codepages.h : Supported Codepages
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


#ifndef _CODEPAGES_H_
#define _CODEPAGES_H_

#ifdef PRSSRENC_EXPORTS
	#define DLLEXT						__declspec(dllexport)
#else
	#define DLLEXT						__declspec(dllimport)
#endif

#include <windows.h>

DLLEXT extern WORD cpISO_8859_2[256];
DLLEXT extern WORD cpISO_8859_3[256];
DLLEXT extern WORD cpISO_8859_4[256];
DLLEXT extern WORD cpISO_8859_5[256];
DLLEXT extern WORD cpISO_8859_6[256];
DLLEXT extern WORD cpISO_8859_7[256];
DLLEXT extern WORD cpISO_8859_8[256];
DLLEXT extern WORD cpISO_8859_9[256];
DLLEXT extern WORD cpISO_8859_10[256];
DLLEXT extern WORD cpISO_8859_11[256];
DLLEXT extern WORD cpISO_8859_13[256];
DLLEXT extern WORD cpISO_8859_14[256];
DLLEXT extern WORD cpISO_8859_15[256];
DLLEXT extern WORD cpISO_8859_16[256];

DLLEXT extern WORD cpCP1250[256];
DLLEXT extern WORD cpCP1251[256];
DLLEXT extern WORD cpCP1252[256];
DLLEXT extern WORD cpCP1253[256];
DLLEXT extern WORD cpCP1254[256];
DLLEXT extern WORD cpCP1255[256];
DLLEXT extern WORD cpCP1256[256];
DLLEXT extern WORD cpCP1257[256];
DLLEXT extern WORD cpCP1258[256];
DLLEXT extern WORD cpKOI8_R[256];

DLLEXT extern WORD cpBig5_1[];
DLLEXT extern WORD cpBig5_2[];

DLLEXT extern WORD cpEUC_JP[];

DLLEXT extern WORD cpEUC_KR1[];
DLLEXT extern WORD cpEUC_KR2[];
DLLEXT extern WORD cpEUC_KR3[];

DLLEXT extern WORD cpGBK[];

#endif // _CODEPAGES_H_