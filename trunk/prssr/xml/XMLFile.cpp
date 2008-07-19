// XMLFile.cpp: implementation of the CXmlFile class.
//
//////////////////////////////////////////////////////////////////////

#ifdef PRSSR_APP
	#include "..\stdafx.h"
	#include "..\prssr.h"
#endif

#ifdef PRSSR_TODAY
	#include "..\prssrtoday\stdafx.h"
#endif

#include "XmlFile.h"
#include "../../prssrenc/codepages.h"
#include "../../share/helpers.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XML default encoding
static char *DefaultEncoding = "utf-8";

static void XMLCALL
startElement(void *userData, const char *name, const char **atts) {
	CXmlFile *xml = (CXmlFile *) userData;
	xml->OnStartElement(name, atts);
}

static void XMLCALL
endElement(void *userData, const char *name) {
	CXmlFile *xml = (CXmlFile *) userData;
	xml->OnEndElement(name);
}

static void XMLCALL 
charDataHandler(void *userData, const XML_Char *s, int len) {
	CXmlFile *xml = (CXmlFile *) userData;
	xml->OnCharacterData(s, len);
}

//
// Codepage convert
//

static int XMLCALL
utf8EncodingConvert(void *data, const char *p) {
//	LOG0(1, "utf8EncodingConvert");

	unsigned short c = '?';

	BYTE *a = (BYTE *) p;
//	LOG1(1, "a = %x", a[0]);

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


static int XMLCALL
unknownEncodingConvert(void *data, const char *p) {
//	LOG0(1, "unknownEncodingConvert");

	unsigned short c;
	if (MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED|MB_ERR_INVALID_CHARS, p, 1, &c, 1) == 1)
		return c;
	return '?';
}

/*
static int XMLCALL
eucEncodingConvert(void *data, const char *p) {
	unsigned short c;

	BYTE *a = (BYTE *) p;
	if (a[0] < 0x80) {
		// single byte
		c = a[0];
	}
	else {
		// two bytes
		c = ((a[0] - 0xA0) << 8) | (a[1] - 0xA0);
	}

	return c;
}
*/

static int XMLCALL
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

static int XMLCALL
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

static int XMLCALL
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


static int XMLCALL
unknownEncoding(void *userData, const XML_Char *name, XML_Encoding *info) {
	LOG1(7, "unknownEncoding(, '%s', )", name);

	CXmlFile *xml = (CXmlFile *) userData;

	char *encoding = xml->Encoding;

	if (strcmp(encoding, "big5") == 0) {
		int i;
		// first 0x80 bytes are single byte
		for (i = 0; i < 0x80; i++) info->map[i] = i;
		// the rest is 2-byte
		for (i = 0x80; i <= 0xFF; i++) info->map[i] = -2;
		info->convert = big5EncodingConvert;
	}
	else if (strncmp(encoding, "euc-jp", 6) == 0) {
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
	else if (strncmp(encoding, "euc-kr", 6) == 0 ||
		strncmp(encoding, "ks_c_5601-1987", 14) == 0)
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
	else if (strcmp(encoding, "utf-8") == 0) {
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
		if (strcmp(encoding, "iso-8859-2") == 0) map = cpISO_8859_2;
		else if (strcmp(encoding, "iso-8859-3") == 0) map = cpISO_8859_3;
		else if (strcmp(encoding, "iso-8859-4") == 0) map = cpISO_8859_4;
		else if (strcmp(encoding, "iso-8859-5") == 0) map = cpISO_8859_5;
		else if (strcmp(encoding, "iso-8859-6") == 0) map = cpISO_8859_6;
		else if (strcmp(encoding, "iso-8859-7") == 0) map = cpISO_8859_7;
		else if (strcmp(encoding, "iso-8859-8") == 0) map = cpISO_8859_8;
		else if (strcmp(encoding, "iso-8859-9") == 0) map = cpISO_8859_9;
		else if (strcmp(encoding, "iso-8859-10") == 0) map = cpISO_8859_10;
		else if (strcmp(encoding, "iso-8859-11") == 0) map = cpISO_8859_11;
		else if (strcmp(encoding, "iso-8859-13") == 0) map = cpISO_8859_13;
		else if (strcmp(encoding, "iso-8859-14") == 0) map = cpISO_8859_14;
		else if (strcmp(encoding, "iso-8859-15") == 0) map = cpISO_8859_15;
		else if (strcmp(encoding, "iso-8859-16") == 0) map = cpISO_8859_16;
		else if (strcmp(encoding, "windows-1250") == 0) map = cpCP1250;
		else if (strcmp(encoding, "windows-1251") == 0) map = cpCP1251;
		else if (strcmp(encoding, "windows-1252") == 0) map = cpCP1252;
		else if (strcmp(encoding, "windows-1253") == 0) map = cpCP1253;
		else if (strcmp(encoding, "windows-1254") == 0) map = cpCP1254;
		else if (strcmp(encoding, "windows-1255") == 0) map = cpCP1255;
		else if (strcmp(encoding, "windows-1256") == 0) map = cpCP1256;
		else if (strcmp(encoding, "windows-1257") == 0) map = cpCP1257;
		else if (strcmp(encoding, "windows-1258") == 0) map = cpCP1258;
		else if (strcmp(encoding, "koi8-r") == 0) map = cpKOI8_R;
		else map = cpCP1252;

		for (int i = 0; i < 256; i++)
			info->map[i] = map[i];
		info->convert = unknownEncodingConvert;
	}

//	LOG1(1, "-- %s", encoding);

	return XML_STATUS_OK;
}

static void XMLCALL
declHandler(void *userData, const XML_Char *version, const XML_Char *encoding, int standalone) {
	CXmlFile *xml = (CXmlFile *) userData;
	xml->OnDeclaration(version, encoding, standalone);
}

/*static void XMLCALL 
defaultHandler(void *userData, const XML_Char *s, int len) {
	CXmlFile *xml = (CXmlFile *) userData;
	xml->OnDefault(s, len);
}
*/

//////////////////////////////////////////////////////////////////////


CXmlAttr::CXmlAttr() {
}

CXmlAttr::CXmlAttr(const CString &name, const CString &value) {
	Name = name;
	Value = value;
}

CXmlAttr::CXmlAttr(const CString &name, int value) {
	Name = name;
	Value.Format(_T("%d"), value);
}

CXmlAttr::~CXmlAttr() {
}

//////////////////////////////////////////////////////////////////////

CXmlNode::CXmlNode(EType type, CXmlNode *parent) {
	Type = type;
	Parent = parent;
}

CXmlNode::CXmlNode(EType type, CXmlNode *parent, const CString &name, CList<CXmlAttr*, CXmlAttr*> &attrList) {
	Type = type;
	Parent = parent;
	Text = name;
	POSITION pos = attrList.GetHeadPosition();
	while (pos != NULL) {
		CXmlAttr *a = attrList.GetNext(pos);
		Attrs.AddTail(a);
	}
}

CXmlNode::~CXmlNode() {
	// delete attributes
	while (!Attrs.IsEmpty())
		delete Attrs.RemoveHead();
	// delete childs
	while (!Childs.IsEmpty())
		delete Childs.RemoveHead();
}

BOOL CXmlNode::AddChild(CXmlNode *child) {
	Childs.AddTail(child);
	return TRUE;
}

CString CXmlNode::GetName() { 
	if (Type == Tag)
		return Text;
	else
		return _T("");
}

CString CXmlNode::GetValue() {
	if (!Childs.IsEmpty()) {
		CXmlNode *node = Childs.GetHead();
		if (node->Type == Data) {
			CString text = node->Text;
			text.TrimLeft();
			text.TrimRight();
			return text;
		}
		else
			return _T("");
	}
	else
		return _T("");
}

CString CXmlNode::GetXML() {
	CString strXML;
	POSITION pos = GetFirstChildPos();
	while (pos != NULL) {
		CXmlNode *child = GetNextChild(pos);

		CString sName, sAttrs;
		CString sTag;
		POSITION posAttr;
		switch (child->GetType()) {
			case CXmlNode::Tag:
				sAttrs.Empty();
				posAttr = child->Attrs.GetHeadPosition();
				while (posAttr != NULL) {
					CXmlAttr *attr = child->Attrs.GetNext(posAttr);
					CString strPair;
					strPair.Format(_T(" %s=\"%s\""), attr->GetName(), attr->GetValue());
					sAttrs += strPair;
				}
				
				sName = child->GetName();
				if (child->GetChildCount() > 0) {
					sTag.Format(_T("<%s%s>%s</%s>"), sName, sAttrs, child->GetXML(), sName);
				}
				else {
					sTag.Format(_T("<%s%s/>"), sName, sAttrs);
				}
				strXML += sTag;
				break;
		
			case CXmlNode::Data:
				strXML += child->Text;
				break;
		}
	}	

	strXML.TrimLeft();
	strXML.TrimRight();
	return strXML;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlFile::CXmlFile() {
	RootNode = NULL;
	Encoding = NULL;

	TempNode = NULL;
}

CXmlFile::~CXmlFile() {
	delete RootNode;
	if (Encoding != NULL) delete [] Encoding;
}

void CXmlFile::OnStartElement(const char *name, const char **attr) {
//	LOG1(1, "OnStart('%s')", name);

	CString sName = CharToWChar(name);
	CList<CXmlAttr*, CXmlAttr*> arAttrs;
	// create array of attributes
	for (int i = 0; attr[i]; i += 2)
		arAttrs.AddTail(new CXmlAttr(CharToWChar(attr[i]), CharToWChar(attr[i + 1], CP_UTF8)));
	CXmlNode *newNode = new CXmlNode(CXmlNode::Tag, TempNode, sName, arAttrs);

	if (RootNode == NULL)
		RootNode = newNode;
	else {
//	if (TempNode != NULL) {
		CString temp = TempValue;
		temp.TrimLeft();
		temp.TrimRight();
		if (!temp.IsEmpty()) {
			CList<CXmlAttr*, CXmlAttr*> arNoAttrs;
			TempNode->AddChild(new CXmlNode(CXmlNode::Data, TempNode, TempValue, arNoAttrs));
		}
		TempNode->AddChild(newNode);
	}

	TempNode = newNode;
	TempValue.Empty();
}

void CXmlFile::OnEndElement(const char *name) {
//	LOG1(1, "OnEnd('%s')", name);

	CList<CXmlAttr*, CXmlAttr*> arNoAttrs;

	if (TempNode != NULL) {
		CXmlNode *node = new CXmlNode(CXmlNode::Data, TempNode, TempValue, arNoAttrs);
		TempNode->AddChild(node);
		TempNode = TempNode->GetParentNode();
	}
	TempValue.Empty();
}

void CXmlFile::OnCharacterData(const char *name, int len) {
//	LOG1(1, "CXmlFile::OnCharacterData(, %d)", len);

	int wLen = MultiByteToWideChar(CP_UTF8, 0, name, len, NULL, 0);
	CString s;
	LPTSTR wStr = s.GetBufferSetLength(wLen);
	MultiByteToWideChar(CP_UTF8, 0, name, len, wStr, wLen);
	TempValue += s;
}

/*void CXmlFile::OnDefault(const XML_Char *s, int len) {
	LOG1(1, "CXmlFile::OnDefault(, %d)", len);

	char *n = new char [len + 1];
	strncpy(n, s, len);
	n[len] = '\0';
	LOG1(1, "D: '%s'", n);
	delete [] n;
}
*/

void CXmlFile::OnDeclaration(const XML_Char *version, const XML_Char *encoding, int standalone) {
	LOG3(7, "CXmlFile::OnDeclaration('%s', '%s', %d)", version, encoding, standalone);

	if (Encoding != NULL) delete [] Encoding;
	if (encoding != NULL) {
		Encoding = DuplicateString(encoding);
		_strlwr(Encoding);

		// crippling pRSSreader: some sites reports iso-8859-1 but they use cp1252 (it is common, to
		// treat iso-8859-1 encoded files as cp-1252)
		if (strcmp(Encoding, "iso-8859-1") == 0) {
			delete [] Encoding;
			Encoding = DuplicateString("windows-1252");
		}
	}
	else {
		Encoding = DuplicateString(DefaultEncoding);
		_strlwr(Encoding);
	}
}


#define BUFSIZ					16384

// tries to determine XML document encoding from buffer
BOOL CXmlFile::DetermineEncoding(char buffer[], int len) {
	LOG0(7, "CXmlFile::DetermineEncoding(, )");

	XML_Parser parser;

	parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, this);
	XML_SetXmlDeclHandler(parser, declHandler);
	XML_SetElementHandler(parser, NULL, NULL);
	XML_SetCharacterDataHandler(parser, NULL);
	XML_SetUnknownEncodingHandler(parser, NULL, 0);

	// set default encoding
	if (Encoding != NULL) delete [] Encoding;
	Encoding = DuplicateString(DefaultEncoding);

	BOOL done = FALSE;
	XML_Parse(parser, buffer, len, done);

	XML_ParserFree(parser);

	delete RootNode;
	RootNode = TempNode = NULL;

	return TRUE;
}

BOOL CXmlFile::LoadFromFile(LPCTSTR fileName) {
	LOG1(5, "CXmlFile::LoadFromFile('%S')", fileName);

	char buf[BUFSIZ];
	BOOL done;
	int depth = 0;

	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	delete RootNode;
	RootNode = TempNode = NULL;

	// first get the encoding
	// load first 8K (should be enough to determine the encoding)
	DWORD read = 0;
	if (ReadFile(hFile, buf, BUFSIZ, &read, NULL)) {
		if (!DetermineEncoding(buf, BUFSIZ))
			return FALSE;
	}
	else
		return FALSE;

	BOOL ret = TRUE;

	XML_Parser parser = XML_ParserCreate("prssr");
	XML_SetUserData(parser, this);
	XML_SetXmlDeclHandler(parser, NULL);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, charDataHandler);
	XML_SetUnknownEncodingHandler(parser, unknownEncoding, this);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	// main load loop with the parsing
	do {
		DWORD read;
		done = FALSE;
		if (ReadFile(hFile, buf, BUFSIZ, &read, NULL)) {
			done = read < BUFSIZ;
			if (XML_Parse(parser, buf, read, done) == XML_STATUS_ERROR) {
				LOG2(7, "Error: %s at line %d", 
					XML_ErrorString(XML_GetErrorCode(parser)),
					XML_GetCurrentLineNumber(parser));
				ret = FALSE;

				delete RootNode;
				RootNode = TempNode = NULL;
				break;
			}
		}
	} while (!done);

	CloseHandle(hFile);

//	RootNode = TempNode;
	TempNode = NULL;

	XML_ParserFree(parser);

	return ret;
}

#if defined PRSSR_APP

BOOL CXmlFile::LoadFromMemory(char buffer[], int len) {
	LOG0(7, "CXmlFile::LoadFromMemory()");

	int depth = 0;

	delete RootNode;
	RootNode = TempNode = NULL;

	// first get the encoding
	if (!DetermineEncoding(buffer, min(BUFSIZ, len)))
		return FALSE;
//	LOG1(7, "Encoding ('%S')", Encoding);

	XML_Parser parser = XML_ParserCreate("prssr");
	XML_SetUserData(parser, this);
	XML_SetXmlDeclHandler(parser, NULL);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, charDataHandler);
	XML_SetUnknownEncodingHandler(parser, unknownEncoding, this);

	BOOL ret = TRUE;
	// main load loop with the parsing
	BOOL done = FALSE;
	if (XML_Parse(parser, buffer, len, done) == XML_STATUS_ERROR) {
		LOG2(7, "Error: %s at line %d", 
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser));
		ret = FALSE;

		delete RootNode;
		RootNode = TempNode = NULL;
	}
	else {
//		RootNode = TempNode;
		TempNode = NULL;
	}

	XML_ParserFree(parser);

	return ret;
}


CString CXmlFile::FormatAttributeValue(const CString &value) {
	CString s = value;
	s.Replace(_T("&"), _T("&amp;"));
	s.Replace(_T("<"), _T("&lt;"));
	s.Replace(_T("\""), _T("&quot;"));
	s.Replace(_T(">"), _T("&gt;"));

	s.Replace(_T("\t"), _T("&#9"));
	s.Replace(_T("\n"), _T("&#10"));
	s.Replace(_T("\r"), _T("&#13"));

	return s;
}

BOOL CXmlFile::SaveNode(CBufferedFile &file, CXmlNode *node) {
	static int depth = 0;

	CString indent;
	if (depth > 0)
		indent = CString('\t', depth);

	CString sAttrs;
	POSITION posAttr = node->GetFirstAttrPos();
	while (posAttr != NULL) {
		CXmlAttr *attr = node->GetNextAttr(posAttr);
		CString sA;
		sA.Format(_T(" %s=\"%s\""), attr->GetName(), FormatAttributeValue(attr->GetValue()));
		sAttrs += sA;
	}

	BOOL ret = TRUE;

	CString tag;
	if (node->GetValue().GetLength() == 0 && node->GetChildCount() == 0) {
		// just to have nice output
		if (sAttrs.GetLength() > 0)
			sAttrs += ' ';

		ret = ret ? WriteFileString(file, indent, CP_UTF8) : FALSE;
		tag.Format(_T("<%s%s/>\n"), node->GetName(), sAttrs);
		ret = ret ? WriteFileString(file, tag, CP_UTF8) : FALSE;
	}
	else {
		// openning tag
		ret = ret ? WriteFileString(file, indent, CP_UTF8) : FALSE;
		tag.Format(_T("<%s%s>\n"), node->GetName(), sAttrs);
		ret = ret ? WriteFileString(file, tag, CP_UTF8) : FALSE;

		if (ret) {
			// FIXME: saving the value of the tag
			depth++;
			POSITION pos = node->GetFirstChildPos();
			while (pos != NULL) {
				CXmlNode *child = node->GetNextChild(pos);
				if (!SaveNode(file, child)) {
					ret = FALSE;
					break;
				}
			}
			depth--;
		}

		// closing tag
		ret = ret ? WriteFileString(file, indent, CP_UTF8) : FALSE;
		tag.Format(_T("</%s>\n"), node->GetName());
		ret = ret ? WriteFileString(file, tag, CP_UTF8) : FALSE;
	}

	return ret;
}


BOOL CXmlFile::Save(LPCTSTR fileName) {
	CBufferedFile file;
	if (file.Create(fileName, GENERIC_WRITE, 0, CREATE_ALWAYS, 0)) {
		BOOL ret = TRUE;

		// save xml header
		CString hdr;
		hdr.Format(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"));
		ret = ret ? WriteFileString(file, hdr, CP_UTF8) : FALSE;

		// save xml
		ret = ret ? SaveNode(file, RootNode) : FALSE;
	
		file.Close();

		return ret;
	}
	else
		return FALSE;
}

#endif
