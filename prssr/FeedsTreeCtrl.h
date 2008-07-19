#if !defined(_FEEDSTREECTRL_H_)
#define _FEEDSTREECTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FeedsTreeCtrl.h : header file
//

#include "ctrls/CeTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CFeedsTreeCtrl class

class CFeedsTreeCtrl : public CCeTreeCtrl {
public:
	CFeedsTreeCtrl(UINT nID = IDS_NO_ITEMS_TO_DISPLAY);

	int m_nGroupIconIdx;

	void SetGroupCheck(HTREEITEM hItem, BOOL check);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFeedsTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_IMPORTFEEDSPG_H_)
