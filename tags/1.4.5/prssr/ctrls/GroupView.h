/**
 *  GroupView.h
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

#if !defined(_GROUPVIEW_H_)
#define _GROUPVIEW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CeScrollBar.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupView window

enum EGVIS {
	GVIS_EXPANDED = 0,
	GVIS_COLLAPSED = 1
};


#define GVI_ROOT						((HGROUPITEM) 0xFFFF0001)
#define GVI_FIRST						((HGROUPITEM) 0xFFFF0002)
#define GVI_LAST						((HGROUPITEM) 0xFFFF0003)


typedef
	POSITION HGROUPITEM;


class CGroupView : public CView {
// Construction
public:
	CGroupView();

// Attributes
public:

protected:
	struct GVITEM {
		HGROUPITEM Parent;							// position in Items
		CList<HGROUPITEM, HGROUPITEM> Childs;		// position in Items

		int yTop;
		BYTE State;

		CString Text;
		DWORD Data;					// application defined value
		int Image;
	};

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupView();

	static BOOL Register();
	static void Unregister();

	virtual void OnDraw(CDC *pDC);

	UINT GetCount() const;

	void SetImageList(CImageList *il) { m_pImageList = il; }
	CImageList *GetImageList() const { return m_pImageList; }

	BOOL ItemHasChildren(HGROUPITEM hItem) const;
	HGROUPITEM GetChildItem(HGROUPITEM hItem) const;
	HGROUPITEM GetNextSiblingItem(HGROUPITEM hItem) const;
	HGROUPITEM GetPrevSiblingItem(HGROUPITEM hItem) const;
	HGROUPITEM GetParentItem(HGROUPITEM hItem) const;
	HGROUPITEM GetSelectedItem() const { return m_hSelectedItem; }
	HGROUPITEM GetRootItem() const;
	HGROUPITEM GetPrevVisibleItem(HGROUPITEM hItem) const;
	HGROUPITEM GetNextVisibleItem(HGROUPITEM hItem) const;

	UINT GetItemState(HGROUPITEM hItem, UINT nStateMask) const;
	BOOL SetItemState(HGROUPITEM hItem, UINT nState, UINT nStateMask);

	BOOL GetItemImage(HGROUPITEM hItem, int &nImage) const;
	BOOL SetItemImage(HGROUPITEM hItem, int nImage);

	CString GetItemText(HGROUPITEM hItem) const;
	BOOL SetItemText(HGROUPITEM hItem, LPCTSTR lpszItem);

	DWORD GetItemData(HGROUPITEM hItem) const;
	BOOL SetItemData(HGROUPITEM hItem, DWORD dwData);

	HGROUPITEM InsertItem(LPCTSTR lpszItem, HGROUPITEM hParent = GVI_ROOT, HGROUPITEM hInsertAfter = GVI_LAST);
	HGROUPITEM InsertItem(LPCTSTR lpszItem, int nImage, HGROUPITEM hParent = GVI_ROOT, HGROUPITEM hInsertAfter = GVI_LAST);
	BOOL DeleteItem(HGROUPITEM hItem);
	BOOL DeleteAllItems();
	void ExpandItem(HGROUPITEM hItem);
	void CollapseItem(HGROUPITEM hItem);
	void ToggleItem(HGROUPITEM hItem);
	void SelectItem(HGROUPITEM hItem);

	BOOL EnsureVisible(HGROUPITEM hItem);

	void UpdateScrollBars();
	void UpdateItemHeight();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual void ContextMenu(CPoint *pt);
	virtual void OnItemClicked();

	void OnLButtonDownTouch(UINT nFlags, CPoint point);
	void OnLButtonUpTouch(UINT nFlags, CPoint point);
	void OnMouseMoveTouch(UINT nFlags, CPoint point);

	void OnLButtonDownNormal(UINT nFlags, CPoint point);
	void OnLButtonUpNormal(UINT nFlags, CPoint point);
	void OnMouseMoveNormal(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

	CImageList *m_pImageList;

	GVITEM RootItem;
	CList<GVITEM *, GVITEM *> Items;

	UINT NoItemsId;

	//
	CRect m_rcScroll;
	CCeScrollBar m_oVScrollBar;

	int m_nViewTop;								// in pixels
	int m_nTotalHeight;							// in pixels
	int m_nClientHeight;						// in pixels
	int m_nClientWidth;							// in pixels
	HGROUPITEM m_hSelectedItem;

	CFont m_fntBase;
	CFont m_fntBold;
	CImageList m_oIcons;

	CPoint LastCursorPos;

	HGROUPITEM m_hClickedItem;

	void UpdateItemsY(HGROUPITEM hParent, int &top);

	void CreateFonts();
	void DrawIcon(CDC &dc, int icon, BOOL selected);
	void DrawItems(CDC &dc, CRect &rc, HGROUPITEM hParent);
	virtual void OnDrawItem(CDC &dc, CRect &rc, HGROUPITEM hItem, BOOL selected);

	void InvalidateItem(HGROUPITEM hItem, BOOL erase = TRUE);

	HGROUPITEM ItemFromPoint(CPoint pt);
	void AdjustViewTop();

	UINT CtxMenuTimer;
	BOOL m_bOpenCtxMenu;

protected:
	static const int ITEM_MARGIN;
	int ItemHeight;

	friend DWORD WINAPI ScrollThread(LPVOID lpParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_GROUPVIEW_H_)
