// HTMLFile.h: interface for the CHtmlFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_HTMLFILE_H__INCLUDED_)
#define _HTMLFILE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../libsgml/libsgml.h"
#include "../../libexpat/expat.h"
#include "../../share/file.h"


///////////////////////////////////////////////////////////////////////////////

class CHtmlFile {
public:
	CHtmlFile();
	virtual ~CHtmlFile();

	void SetFlags(unsigned long flags);

	virtual BOOL LoadFromFile(LPCTSTR fileName);
#if defined PRSSR_APP
	virtual BOOL LoadFromMemory(char buffer[], int len);
	virtual BOOL LoadFromMemory(const CString &html);
	virtual BOOL Save(LPCTSTR fileName);
	virtual CString ToString();

	DOM_NODE *GetDocumentNode() { return sgmlExtensionHtmlGetDocument(&Parser); }

	void DetectEncoding(const CString &encoding);
	void Recode();
#endif

protected:
	SGML_PARSER Parser;
	CString Charset;
	XML_Encoding EncInfo;

#if defined PRSSR_APP
	BOOL DetectEncoding(DOM_NODE *node);
	void Recode(DOM_NODE *node);
#endif
};


CString recode(const char *str, XML_Encoding *enc_info);
bool setEncoding(const TCHAR *encoding, XML_Encoding *info);

#endif // !defined(_HTMLFILE_H__INCLUDED_)
