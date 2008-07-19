// LocalHtmlFile.h: interface for the CHtmlFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_LOCAL_HTMLFILE_H__INCLUDED_)
#define _LOCAL_HTMLFILE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HTMLFile.h"

/////////////////////////////////////////////////////////////////////////////

class CLocalHtmlFile : public CHtmlFile {
public:
	CLocalHtmlFile();
	virtual ~CLocalHtmlFile();

	void Filter();
	void TranslateForOffline();
	void ExtractImages(CStringList &list);

protected:
	BOOL Trans;
	CString Server, ActivePath;

	// internal use
	void Filter(DOM_NODE *node);
	void TranslateForOffline(DOM_NODE *node);
	void ExtractImages(DOM_NODE *node, CStringList &list);
};

#endif // !defined(_DISCOVER_FEED_HTMLFILE_H__INCLUDED_)
