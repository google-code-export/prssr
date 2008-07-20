/**
 *  SiteManagerDlg.h
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

#if !defined(AFX_SITEMANAGERDLG_H__D24084E2_B01C_4F43_97CB_BEE8FD391F50__INCLUDED_)
#define AFX_SITEMANAGERDLG_H__D24084E2_B01C_4F43_97CB_BEE8FD391F50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeDialog.h"
#include "ctrls/hrule.h"
//#include "Session.h"
#include "ctrls/CeTreeCtrl.h"

class CSiteItem;
class CSiteList;

/////////////////////////////////////////////////////////////////////////////
// CSiteTreeCtrl control

class CSiteTreeCtrl : public CCeTreeCtrl {
public:
	CSiteTreeCtrl(UINT nID);

protected:
	//{{AFX_MSG(CSiteTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSiteManagerDlg dialog

class CSiteManagerDlg : public CCeDialog { //, public CRestorable {
// Construction
public:
	CSiteManagerDlg(CWnd* pParent = NULL);   // standard constructor
	~CSiteManagerDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_SITE_MANAGER_WIDE };
	//{{AFX_DATA(CSiteManagerDlg)
	enum { IDD = IDD_SITE_MANAGER };
	CSiteTreeCtrl	m_ctlSites;
	CButton	m_ctlMoveUp;
	CButton	m_ctlMoveDown;
	CHrule m_ctlHrule;
	//}}AFX_DATA
	CSiteItem *Root;
	BOOL ShowNewChannelsOnToday;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSiteManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nGroupIcon;
	BOOL m_bAddingGroup;	// used in EndLabelEdit to determine fi the user is adding new folder or editing group name

	CImageList m_oImageList;

	HTREEITEM InsertSite(HTREEITEM hParent, CSiteItem *item, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertGroup(HTREEITEM hParent, CSiteItem *group, HTREEITEM hInsertAfter = TVI_LAST);
	void UpdateControls();

	HTREEITEM AddItem(HTREEITEM hParent, CSiteItem *item, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM MergeItem(HTREEITEM hParent, CSiteItem *item, HTREEITEM hInsertAfter = TVI_LAST);
	void DeleteItem(HTREEITEM hItem, BOOL free = TRUE);

	HTREEITEM GetGroupItem();

	void CreateMoveToMenu(CMenu &mnuMove);
	void AppendMoveToMenu(CMenu &mnuMove, HTREEITEM hParent);
	CArray<HTREEITEM, HTREEITEM> CtxMenuMoveToIDs;

//	CMap<CSiteItem *, CSiteItem *, HTREEITEM, HTREEITEM> MapSiteToTreeItem;

	// for merging
	CMapStringToPtr MapGroupToTreeItem;
	CMapStringToPtr MapUrlToTreeItem;

	void CreateSiteList(HTREEITEM hItem, CSiteItem *parent = NULL);

	// Generated message map functions
	//{{AFX_MSG(CSiteManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
//	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnRename();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnEdit();
	afx_msg void OnSelchangedSites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditSites(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg void OnAddFeed();
	afx_msg void OnAddGroup();
	afx_msg void OnSearch();
	afx_msg void OnExportOpml();
	afx_msg void OnImportOpml();

	void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnMoveToGroup(UINT nID);

	virtual void ResizeControls();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SITEMANAGERDLG_H__D24084E2_B01C_4F43_97CB_BEE8FD391F50__INCLUDED_)
