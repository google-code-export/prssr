// FeedFile.h: interface for the CFeedFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEEDFILE_H__557E1736_30F4_4A2B_9317_B4FF4A57DC74__INCLUDED_)
#define AFX_FEEDFILE_H__557E1736_30F4_4A2B_9317_B4FF4A57DC74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XMLFile.h"
#include "..\Feed.h"
#include "..\Site.h"

//class CFeedItem;

class CFeedFile : public CXmlFile {
public:
	BOOL IsValid();
	CFeedFile();
	virtual ~CFeedFile();

	BOOL Parse(CFeed *feed, CSiteItem *si);

protected:
	enum EType {
		RSS,
		RDF,
		Atom,
		Unknown
	};

//	CString GetXML(MSXML::IXMLDOMNode *parent);
//	CString GetNodeAttrs(MSXML::IXMLDOMNode *parent);
//	CString GetXML(CXmlNode *parent);

private:
	EType GetType();
	CString GetVersion();

	BOOL RSSFillInfo(CXmlNode *parent, CFeed *feed);
	BOOL RSSFillItem(CXmlNode *xmlItem, CFeedItem *item);

	BOOL RSSFill(CFeed *feed, CSiteItem *si);
	BOOL RDFFill(CFeed *feed, CSiteItem *si);
	BOOL AtomFill(CFeed *feed, CSiteItem *si);

#if defined PRSSR_APP
	BOOL RSSFillEnclosure(CXmlNode *xmlElem, CEnclosureItem *enclosure);
	BOOL AtomFillPerson(CXmlNode *xmlItem, CString &name);
#endif

	BOOL AtomFillInfo(CXmlNode *xmlElem, CFeed *feed);
	BOOL AtomFillItem(CXmlNode *xmlItem, CFeedItem *item);
//	BOOL AtomFillList(MSXML::IXMLDOMElement *xmlElem, CFeed *feed);
};

#endif // !defined(AFX_FEEDFILE_H__557E1736_30F4_4A2B_9317_B4FF4A57DC74__INCLUDED_)
