// HTMLFile.cpp: implementation of the CHtmlFile class.
//
//////////////////////////////////////////////////////////////////////

#ifdef PRSSR_APP
	#include "..\stdafx.h"
	#include "..\prssr.h"
#endif

#ifdef PRSSR_TODAY
	#include "../../prssrtoday/stdafx.h"
#endif

#include "HTMLFile.h"
#include "../../prssrenc/codepages.h"
#include "../../share/helpers.h"

/*#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmlFile::CHtmlFile() {
	sgmlParserInitialize(&Parser, SGML_EXTENSION_TYPE_HTML, NULL, NULL);
	unsigned long flags = SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT;
	sgmlParserExtensionSetParam(&Parser, SGML_EXTENSION_HTML_PARAM_FLAG, (void *) flags);
}

CHtmlFile::~CHtmlFile() {
	sgmlParserDestroy(&Parser, 0);
}

void CHtmlFile::SetFlags(unsigned long flags) {
	sgmlParserExtensionSetParam(&Parser, SGML_EXTENSION_HTML_PARAM_FLAG, (void *) flags);
}

#define BUFSIZ					16384

BOOL CHtmlFile::LoadFromFile(LPCTSTR fileName) {
	LOG1(1, "CHtmlFile::LoadFromFile('%S')", fileName);

	char buf[BUFSIZ];
	BOOL done;
	int depth = 0;

	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL ret = TRUE;

	if (Parser.handlers.preparse)
		Parser.handlers.preparse(&Parser, Parser.internal.userContext);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	// main load loop with the parsing
	do {
		DWORD read;
		done = FALSE;
		if (ReadFile(hFile, buf, BUFSIZ, &read, NULL)) {
			done = read < BUFSIZ;
			if (!_sgmlParseChunk(&Parser, buf, read)) {
				ret = FALSE;
				break;
			}
		}
		else
			done = TRUE;
	} while (!done);

	if (Parser.handlers.postparse)
		Parser.handlers.postparse(&Parser, Parser.internal.userContext);

	CloseHandle(hFile);

	if (Parser.internal.lastAttributeName)
		free(Parser.internal.lastAttributeName);
	if (Parser.internal.lastElementName)
		free(Parser.internal.lastElementName);
	if (Parser.internal.currentBuffer)
		free(Parser.internal.currentBuffer);

	return ret;
}

#if defined PRSSR_APP

BOOL CHtmlFile::LoadFromMemory(char buffer[], int len) {
	LOG0(7, "CHtmlFile::LoadFromMemory()");

	BOOL ret = TRUE;
	if (sgmlParserParseString(&Parser, buffer, len) == 0) {
	}
	else
		ret = FALSE;

	return ret;
}

BOOL CHtmlFile::LoadFromMemory(const CString &html) {
	LOG0(3, "CHtmlFile::LoadFromMemory()");

	char *s = WCharToChar(html, CP_UTF8);
	BOOL ret = LoadFromMemory(s, strlen(s));
	delete [] s;
	return ret;
}

BOOL CHtmlFile::Save(LPCTSTR fileName) {
	LOG1(3, "CHtmlFile::Save(%S)", fileName);

	FILE *fout = _wfopen(fileName, L"w");
	if (fout != NULL) {
		domNodeSerializeToFd(sgmlExtensionHtmlGetDocument(&Parser), fout);
		fclose(fout);
		return TRUE;
	}
	else
		return FALSE;
}

CString CHtmlFile::ToString() {
	LOG0(3, "CHtmlFile::ToString()");

	char *string;
	unsigned long stringLength = domNodeSerializeToString(GetDocumentNode(), &string);
	if (string != NULL) {
		CString s = CharToWChar(string, CP_UTF8);
		free(string);
		return s;
	}
	else
		return _T("");
}

BOOL CHtmlFile::DetectEncoding(DOM_NODE *node) {
	LOG1(1, "CHtmlFile::DetectEncoding(%p)", node);

	BOOL res = FALSE;

	const char *tag = domNodeGetName(node);
	if (tag != NULL && node != NULL) {
		if (_stricmp(tag, "meta") == 0) {
			const char *content = domElementGetAttribute(node, "content");
			const char *httpEquiv = domElementGetAttribute(node, "http-equiv");
			LOG2(5, "httpEquiv = '%s' content='%s'", httpEquiv, content);

			if (content != NULL) {
//			if (httpEquiv != NULL && _stricmp(httpEquiv, "content-type") == 0) {
				LOG1(5, "content-type: %s", content);

				char *ctnt = _strdup(content);
				_strlwr(ctnt);

				char *charset = strstr(ctnt, "charset=");
				if (charset != NULL) {
					char *semicolon = strchr(charset, ';');
					if (semicolon != NULL)
						*semicolon = '\0';

					Charset.Format(_T("%S"), charset + 8);
					res = TRUE;
				}

				free(ctnt);
			}
		}
		else if (_stricmp(tag, "body") == 0) {
			// <body> tag reached, end iterating
			return TRUE;
		}
	}

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (!res && child != NULL) {
		res = DetectEncoding(child);
		child = domNodeGetNextSibling(child);
	}

	return res;
}

void CHtmlFile::DetectEncoding(const CString &encoding) {
	LOG1(3, "CHtmlFile::DetectEncoding('%S')", encoding);

	Charset.Empty();
	DetectEncoding(GetDocumentNode());

	if (Charset.IsEmpty())	// not found, set encoding according to the parameter
		Charset = encoding;
}

//
// Codepage convert
//
// based on libexpat recoding engine
//

static int
utf8EncodingConvert(void *data, const char *p) {
	unsigned short c = '?';

	BYTE *a = (BYTE *) p;

	if (a[0] < 0xc0)
		c = a[0];
	else if (a[0] < 0xe0)
		MultiByteToWideChar(CP_UTF8, 0, p, 2, &c, 1);
	else if (a[0] < 0xf0)
		MultiByteToWideChar(CP_UTF8, 0, p, 3, &c, 1);
	else if (a[0] <= 0xf4)
		MultiByteToWideChar(CP_UTF8, 0, p, 4, &c, 1);
	else
		c = a[0];

	return c;
}


static int
unknownEncodingConvert(void *data, const char *p) {
	int c = *p;
	return c;
}

static int
big5EncodingConvert(void *data, const char *p) {
	int c;

	BYTE *cs = (BYTE *) p;
	if (cs[0] < 0x80) {
		// single byte
		c = cs[0];
	}
	else {
		// two bytes
		int idx1 = cs[0] - 0xA1;
		if (cs[1] >= 0x40 && cs[1] <= 0x7E) {
			int idx2 = cs[1] - 0x40;
			c = cpBig5_1[(idx1 * 0x3F) + idx2];
		}
		else if (cs[1] >= 0xA1 && cs[1] <= 0xFE) {
			int idx2 = cs[1] - 0xA1;
			c = cpBig5_2[(idx1 * 0x5E) + idx2];
		}
		else
			c = '?';
	}

	return c;
}

static int
eucJpEncodingConvert(void *data, const char *p) {
	int c;

	BYTE *cs = (BYTE *) p;
	if (cs[0] < 0xA1 || cs[0] == 0xFF) {
		// single byte
		c = cs[0];
	}
	else if (cs[0] == 0x8E) {
		c = 0xFF61 + (cs[1] - 0xA1);
	}
	else {
		// two bytes
		if (cs[0] >= 0xA1 && cs[0] <= 0xFE &&
			cs[1] >= 0xA1 && cs[1] <= 0xFE)
		{
			int idx1 = cs[0] - 0xA1;
			int idx2 = cs[1] - 0xA1;
			c = cpEUC_JP[(idx1 * 0x5E) + idx2];
		}
		else {
			c = '?';
		}
	}

	return c;
}

static int
eucKrEncodingConvert(void *data, const char *p) {
	int c = -1;

	BYTE *cs = (BYTE *) p;
	if (cs[0] <= 0x7E || cs[0] == 0xFF) {
		// single byte
		c = cs[0];
	}
	else if (cs[0] >= 0x81 && cs[0] <= 0xFE) {
		// two bytes
		int idx1 = cs[0] - 0x81;
		if (cs[1] >= 0x41 && cs[1] <= 0x5A) {
			int idx2 = cs[1] - 0x41;
			c = cpEUC_KR1[(idx1 * 0x7E) + idx2];
		}
		else if (cs[1] >= 0x61 && cs[1] <= 0x7A) {
			int idx2 = cs[1] - 0x61;
			c = cpEUC_KR2[(idx1 * 0x7E) + idx2];
		}
		else if (cs[1] >= 0x81 && cs[1] <= 0xFE) {
			int idx2 = cs[1] - 0x81;
			c = cpEUC_KR3[(idx1 * 0x7E) + idx2];
		}
	}

	return c;
}


bool setEncoding(const TCHAR *encoding, XML_Encoding *info) {
	LOG1(7, "setEncoding(, '%S', )", encoding);

	if (wcscmp(encoding, _T("big5")) == 0) {
		int i;
		// first 0x80 bytes are single byte
		for (i = 0; i < 0x80; i++) info->map[i] = i;
		// the rest is 2-byte
		for (i = 0x80; i <= 0xFF; i++) info->map[i] = -2;
		info->convert = big5EncodingConvert;
	}
	else if (wcsncmp(encoding, _T("euc-jp"), 6) == 0) {
		int i;
		// first 0x80 bytes are single byte
		for (i = 0; i <= 0x8D; i++) info->map[i] = i;
		info->map[0x8E] = -2;
		info->map[0x8F] = -3;
		for (i = 0x90; i <= 0x9F; i++) info->map[i] = i;
		info->map[0xA0] = -1;
		// the rest is 2-byte
		for (i = 0xA1; i <= 0xFE; i++) info->map[i] = -2;
		info->map[0xFF] = -1;
		info->convert = eucJpEncodingConvert;
	}
	else if (wcsncmp(encoding, _T("euc-kr"), 6) == 0 ||
		wcsncmp(encoding, _T("ks_c_5601-1987"), 14) == 0)
	{
		int i;
		// first 0x80 bytes are single byte
		for (i = 0; i <= 0x7E; i++) info->map[i] = i;
		info->map[0x7F] = -1;
		info->map[0x80] = -1;
		for (i = 0x81; i <= 0xFE; i++) info->map[i] = -2;
		info->map[0xFF] = -1;
		info->convert = eucKrEncodingConvert;
	}
	else if (wcscmp(encoding, _T("utf-8")) == 0) {
		int i;
		for (i = 0; i < 0xc0; i++)
			info->map[i] = i;
		for (i = 0xc0; i < 0xe0; i++)
			info->map[i] = -2;
		for (i = 0xe0; i < 0xf0; i++)
			info->map[i] = -3;
		for (i = 0xf0; i <= 0xf4; i++)
			info->map[i] = -4;
		for (i = 0xf5; i <= 0xff; i++)
			info->map[i] = i;
		info->convert = utf8EncodingConvert;
	}
	else {
		int *map;
		if (wcscmp(encoding, _T("iso-8859-2")) == 0) map = cpISO_8859_2;
		else if (wcscmp(encoding, _T("iso-8859-3")) == 0) map = cpISO_8859_3;
		else if (wcscmp(encoding, _T("iso-8859-4")) == 0) map = cpISO_8859_4;
		else if (wcscmp(encoding, _T("iso-8859-5")) == 0) map = cpISO_8859_5;
		else if (wcscmp(encoding, _T("iso-8859-6")) == 0) map = cpISO_8859_6;
		else if (wcscmp(encoding, _T("iso-8859-7")) == 0) map = cpISO_8859_7;
		else if (wcscmp(encoding, _T("iso-8859-8")) == 0) map = cpISO_8859_8;
		else if (wcscmp(encoding, _T("iso-8859-9")) == 0) map = cpISO_8859_9;
		else if (wcscmp(encoding, _T("iso-8859-10")) == 0) map = cpISO_8859_10;
		else if (wcscmp(encoding, _T("iso-8859-11")) == 0) map = cpISO_8859_11;
		else if (wcscmp(encoding, _T("iso-8859-13")) == 0) map = cpISO_8859_13;
		else if (wcscmp(encoding, _T("iso-8859-14")) == 0) map = cpISO_8859_14;
		else if (wcscmp(encoding, _T("iso-8859-15")) == 0) map = cpISO_8859_15;
		else if (wcscmp(encoding, _T("iso-8859-16")) == 0) map = cpISO_8859_16;
		else if (wcscmp(encoding, _T("windows-1250")) == 0) map = cpCP1250;
		else if (wcscmp(encoding, _T("windows-1251")) == 0) map = cpCP1251;
		else if (wcscmp(encoding, _T("windows-1252")) == 0) map = cpCP1252;
		else if (wcscmp(encoding, _T("windows-1253")) == 0) map = cpCP1253;
		else if (wcscmp(encoding, _T("windows-1254")) == 0) map = cpCP1254;
		else if (wcscmp(encoding, _T("windows-1255")) == 0) map = cpCP1255;
		else if (wcscmp(encoding, _T("windows-1256")) == 0) map = cpCP1256;
		else if (wcscmp(encoding, _T("windows-1257")) == 0) map = cpCP1257;
		else if (wcscmp(encoding, _T("windows-1258")) == 0) map = cpCP1258;
		else if (wcscmp(encoding, _T("koi8-r")) == 0) map = cpKOI8_R;
		else map = cpCP1252; // fallback encoding

		for (int i = 0; i < 256; i++)
			info->map[i] = map[i];
		info->convert = unknownEncodingConvert;
	}

	return true;
}

CString recode(const char *str, XML_Encoding *enc_info) {
	LOG2(1, "recode('%s', %p)", str, enc_info);

	CString out;

	if (enc_info == NULL) {
		out.Format(_T("%S"), str);
	}
	else {
		for (const char *s = str; *s != '\0';) {
			int c = enc_info->map[(BYTE) *s];
			if (c < 0) {
				if (enc_info->convert != NULL) {
					int len = -c;
					c = enc_info->convert(NULL, s);
					s += len;
				}
				else
					s++;
			}
			else
				s++;
			out += TCHAR(c);
		}
	}

	return out;
}


void CHtmlFile::Recode(DOM_NODE *node) {
	LOG1(5, "CHtmlFile::Recode(%p)", node);

	//
	if (node != NULL && node->type == DOM_NODE_TYPE_TEXT) {
		LOG1(1, "or: %s", node->value);
		CString str = recode(node->value, &EncInfo);
		LOG1(1, "wd: %S", str);
		if (node->value)
			free(node->value);
		node->value = WCharToChar(str, CP_UTF8);
		LOG1(1, "re: %s", node->value);
	}

	DOM_NODE *child = domNodeGetFirstChild(node);
	while (child != NULL) {
		Recode(child);
		child = domNodeGetNextSibling(child);
	}
}

void CHtmlFile::Recode() {
	LOG0(1, "CHtmlFile::Recode()");

	setEncoding(Charset, &EncInfo);
	Recode(GetDocumentNode());
}

#endif
