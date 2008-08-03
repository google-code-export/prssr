/**
 *  XMLFile.h
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

#if !defined(AFX_XMLFILE_H__42AB7C75_1CE0_475A_A00F_5906D89BE100__INCLUDED_)
#define AFX_XMLFILE_H__42AB7C75_1CE0_475A_A00F_5906D89BE100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../libexpat/expat.h"
#include "../../share/file.h"

///////////////////////////////////////////////////////////////////////////////

class CXmlAttr {
public:
	CXmlAttr();
	CXmlAttr(const CString &name, const CString &value);
	CXmlAttr(const CString &name, int value);
	virtual ~CXmlAttr();

	CString GetName() { return Name; }
	CString GetValue() { return Value; }

protected:
	CString Name;
	CString Value;
};

///////////////////////////////////////////////////////////////////////////////

class CXmlNode {
public:
	enum EType {
		Data,
		Tag
	};

	CXmlNode(EType type, CXmlNode *parent);
	CXmlNode(EType type, CXmlNode *parent, const CString &name, CList<CXmlAttr*, CXmlAttr*> &attrList);
	virtual ~CXmlNode();

#ifdef PRSSR_APP
//	void SetValue(const CString &value) { Value = value; }
#endif

	EType GetType() { return Type; }
	CString GetName();
	CString GetValue();
	CString GetXML();

	int GetChildCount() { return Childs.GetCount(); }
	POSITION GetFirstChildPos() { return Childs.GetHeadPosition(); }
	CXmlNode *GetNextChild(POSITION &pos) { return Childs.GetNext(pos); }

	CXmlNode *GetParentNode() { return Parent; }
	BOOL AddChild(CXmlNode *child);

	POSITION GetFirstAttrPos() { return Attrs.GetHeadPosition(); }
	CXmlAttr *GetNextAttr(POSITION &pos) { return Attrs.GetNext(pos); }

protected:
	CXmlNode *Parent;						// parent node
	CList<CXmlNode*, CXmlNode*> Childs;		// child nodes

	EType Type;								// type (data/tag)
	CString Text;							// name of the tag (if type == Tag) or else the text data of the tag
	CList<CXmlAttr *, CXmlAttr *> Attrs;	// atributes of the node

	friend class CXmlFile;
};

///////////////////////////////////////////////////////////////////////////////

class CXmlFile {
public:
	CXmlFile();
	virtual ~CXmlFile();

	virtual BOOL LoadFromFile(LPCTSTR fileName);
#if defined PRSSR_APP
	virtual BOOL LoadFromMemory(char buffer[], int len);
	virtual BOOL Save(LPCTSTR fileName);

	CXmlNode *GetRootNode() { return RootNode; }
#endif

protected:
	CXmlNode *RootNode;
	char *Encoding;

	CXmlNode *TempNode;		// used during the parsing
	CString TempValue;		// user during parsing for the value of a node

#if defined PRSSR_APP
	CString FormatAttributeValue(const CString &value);
	virtual BOOL SaveNode(CBufferedFile &file, CXmlNode *node);
#endif

	virtual void OnStartElement(const char *name, const char **atts);
	virtual void OnEndElement(const char *name);
	virtual void OnCharacterData(const char *name, int len);
	virtual void OnDeclaration(const XML_Char *version, const XML_Char *encoding, int standalone);
//	virtual void OnDefault(const XML_Char *s, int len);

	BOOL DetermineEncoding(char buffer[], int len);

	//
	friend void XMLCALL startElement(void *userData, const char *name, const char **atts);
	friend void XMLCALL endElement(void *userData, const char *name);
	friend void XMLCALL charDataHandler(void *userData, const XML_Char *s, int len);
	friend int XMLCALL unknownEncoding(void *userData, const XML_Char *name, XML_Encoding *info);
	friend void XMLCALL declHandler(void *userData, const XML_Char *version, const XML_Char *encoding, int standalone);
//	friend void defaultHandler(void *userData, const XML_Char *s, int len);
};

#endif // !defined(AFX_XMLFILE_H__42AB7C75_1CE0_475A_A00F_5906D89BE100__INCLUDED_)
