/**
 *  HTMLFile.h
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
