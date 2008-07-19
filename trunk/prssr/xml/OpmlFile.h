// OPMLFile.h: interface for the COpmlFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPMLFILE_H__95F58019_D218_445D_BD53_4BA646B8FB8A__INCLUDED_)
#define AFX_OPMLFILE_H__95F58019_D218_445D_BD53_4BA646B8FB8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XMLFile.h"

class CSiteList;
class CFeedInfo;
class CSiteItem;

class COpmlFile : public CXmlFile {
public:
	COpmlFile();
	virtual ~COpmlFile();

	BOOL Parse(CSiteItem *item, CStringArray &keywords);

#ifdef PRSSR_APP
	BOOL Export(LPCTSTR fileName, CSiteList *siteList);
	virtual BOOL Save(LPCTSTR fileName, CSiteList *siteList);
#endif

protected:
	BOOL ParseOutline(CXmlNode *parent, CSiteItem *item);
	BOOL ParseHead(CXmlNode *parent, CStringArray &keywords);

#ifdef PRSSR_APP
	BOOL SaveGroup(CXmlNode *parent, CSiteItem *item);
	BOOL SaveSite(CXmlNode *parent, CSiteItem *item);
	BOOL SaveHead(CXmlNode *parent, CSiteList *siteList);
#endif
};

#endif // !defined(AFX_OPMLFILE_H__95F58019_D218_445D_BD53_4BA646B8FB8A__INCLUDED_)
