//
// Supported Codepages
//

#ifndef _CODEPAGES_H_
#define _CODEPAGES_H_

#ifdef PRSSRENC_EXPORTS
	#define DLLEXT						__declspec(dllexport)
#else
	#define DLLEXT						__declspec(dllimport)
#endif

DLLEXT extern int cpISO_8859_2[256];
DLLEXT extern int cpISO_8859_3[256];
DLLEXT extern int cpISO_8859_4[256];
DLLEXT extern int cpISO_8859_5[256];
DLLEXT extern int cpISO_8859_6[256];
DLLEXT extern int cpISO_8859_7[256];
DLLEXT extern int cpISO_8859_8[256];
DLLEXT extern int cpISO_8859_9[256];
DLLEXT extern int cpISO_8859_10[256];
DLLEXT extern int cpISO_8859_11[256];
DLLEXT extern int cpISO_8859_13[256];
DLLEXT extern int cpISO_8859_14[256];
DLLEXT extern int cpISO_8859_15[256];
DLLEXT extern int cpISO_8859_16[256];

DLLEXT extern int cpCP1250[256];
DLLEXT extern int cpCP1251[256];
DLLEXT extern int cpCP1252[256];
DLLEXT extern int cpCP1253[256];
DLLEXT extern int cpCP1254[256];
DLLEXT extern int cpCP1255[256];
DLLEXT extern int cpCP1256[256];
DLLEXT extern int cpCP1257[256];
DLLEXT extern int cpCP1258[256];
DLLEXT extern int cpKOI8_R[256];

DLLEXT extern int cpBig5_1[];
DLLEXT extern int cpBig5_2[];

DLLEXT extern int cpEUC_JP[];

DLLEXT extern int cpEUC_KR1[];
DLLEXT extern int cpEUC_KR2[];
DLLEXT extern int cpEUC_KR3[];

#endif // _CODEPAGES_H_