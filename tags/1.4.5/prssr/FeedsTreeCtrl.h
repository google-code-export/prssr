/**
 *  FeedsTreeCtrl.h
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

#if !defined(_FEEDSTREECTRL_H_)
#define _FEEDSTREECTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
