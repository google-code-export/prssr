/**
 *  SummaryView.h
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

#if !defined(_SUMMARYVIEW_H_)
#define _SUMMARYVIEW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/GroupView.h"
#include "ctrls/LinkCtrl.h"

class CSiteItem;
class CSiteList;

/////////////////////////////////////////////////////////////////////////////
// CSummaryView class

class CSummaryView : public CGroupView {
public:
	CSummaryView();   // standard constructor
	~CSummaryView();

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSummaryView)
	public:
	virtual void OnInitialUpdate();
	protected:
	//}}AFX_VIRTUAL

	void InsertSites(CSiteList *siteList);
	void InsertItems(HGROUPITEM hParent, CSiteItem *parent);
	void InsertSiteItem(HGROUPITEM hParent, CSiteItem *parent);

// Implementation
protected:
	CLinkCtrl m_ctlSiteManLink;

	// apperance
	void SetReadState(CSiteItem *si, DWORD state);

	virtual void OnDrawItem(CDC &dc, CRect &rc, HGROUPITEM hItem, BOOL selected);
	virtual void OnItemClicked();
	virtual void ContextMenu(CPoint pt);

	// Generated message map functions
	//{{AFX_MSG(CSummaryView)
	//}}AFX_MSG
	afx_msg void OnUpdateChannel();
	afx_msg void OnUpdateUpdateChannel(CCmdUI *pCmdUI);
	afx_msg void OnToolsMarkItemsRead();
	afx_msg void OnToolsMarkItemsUnread();
	afx_msg void OnFileProperies();
	afx_msg void OnUpdateFileProperies(CCmdUI *pCmdUI);
	afx_msg void OnViewHideGroups();
	afx_msg void OnUpdateViewHideGroups(CCmdUI *pCmdUI);
	afx_msg void OnSiteManLink();
	void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	friend class CMainFrame;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_SUMMARYVIEW_H_)
