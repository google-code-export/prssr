/**
 *  FeedView.h
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

#if !defined(AFX_FEEDVIEW_H__1F3F6BC2_F088_4028_94E7_5467DE65F7C2__INCLUDED_)
#define AFX_FEEDVIEW_H__1F3F6BC2_F088_4028_94E7_5467DE65F7C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeScrollBar.h"

class CSiteItem;
class CFeedItem;
//class CArticleDlg;
class CSortInfo;

// icons
#define NEW_FEED_ICON					0
#define UNREAD_FEED_ICON				1
#define READ_FEED_ICON					2
#define KEYWORD_ICON					3
#define CACHED_ENCLOSURE_ICON			4
#define NOT_CACHED_ENCLOSURE_ICON		5
#define FLAG_ICON						6


/////////////////////////////////////////////////////////////////////////////
// CFeedView window

class CFeedView : public CWnd //View
{
// Construction
public:
	CFeedView();

// Attributes
public:

// Operations
public:
//	BOOL Create(DWORD dwStyle, const RECT &rect, CView *pParentWnd, UINT nID);

//	void AddMessage(CChatMsg *msg);
	void SelectAll();
	void DeleteItem(int idx);
	void DeleteAllItems();
	void InsertItems(CSiteItem *si);
	int GetItemCount() { return m_oItems.GetSize(); }
	CFeedItem *GetItem(int idx) { return m_oItems.GetAt(idx); }
	void GetSelectedRange(int &start, int &end) { start = m_nSelectStart; end = m_nSelectEnd; }
	void UpdateItemHeight();

	void UpdateScrollBars();

	void EnsureVisible(int item);

	void MarkAllRead();
	void MarkAllUnread();
	void MarkItem(int item, DWORD mask);

	void SortItems();
	void OnSortAscending();
	void OnSortDescending();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFeedView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFeedView();

	static BOOL Register();
	static void Unregister();

	virtual void OnDraw(CDC *pDC);

	// Generated message map functions

protected:
	BOOL InsertItem(int i, CFeedItem *item);

	//{{AFX_MSG(CFeedView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	void OnLButtonDownTouch(UINT nFlags, CPoint point);
	void OnLButtonUpTouch(UINT nFlags, CPoint point);
	void OnMouseMoveTouch(UINT nFlags, CPoint point);

	void OnLButtonDownNormal(UINT nFlags, CPoint point);
	void OnLButtonUpNormal(UINT nFlags, CPoint point);
	void OnMouseMoveNormal(UINT nFlags, CPoint point);

	void ContextMenu(CPoint *pt);

	afx_msg void OnItemOpen();
	afx_msg void OnItemMarkRead();
	afx_msg void OnItemMarkUnread();
	afx_msg void OnItemMarkNew();
	afx_msg void OnItemDelete();
	afx_msg void OnSelectAll();
	afx_msg void OnItemFlag();
	afx_msg void OnUpdateItemFlag(CCmdUI *pCmdUI);
	afx_msg void OnUpdateItemSelected(CCmdUI *pCmdUI);

	afx_msg void OnViewHideReadItems();
	afx_msg void OnUpdateViewHideReadItems(CCmdUI *pCmdUI);

	afx_msg void OnCopyUrl();
	afx_msg void OnSendByEmail();

	DECLARE_MESSAGE_MAP()

	CArray<CFeedItem *, CFeedItem *> m_oItems;

	CRect m_rcScroll;
	BOOL m_bScrolling;
	int m_nScrollDelta;
	CCeScrollBar m_oVScrollBar;
	UINT m_nOldKeys;
	CPoint m_ptOldCursorPos;

	int m_nViewTop;								// in pixels
	int m_nTotalHeight;							// in pixels
	int m_nClientHeight;						// in pixels
//	int m_nSelectedItem;
	int m_nSelectStart;
	int m_nSelectEnd;
	int m_nSelectFirst;
	BOOL m_bSelecting;
	BOOL m_bClick;
	int m_nFlagItem;

	CFont m_fntBase;
	CFont m_fntBold;
	CFont m_fntSmall;
	CImageList m_oIcons;

	CPoint LastCursorPos;

	void CreateFonts();
	void DrawIcon(CDC &dc, int icon, BOOL selected);
	void DrawItem(CDC &dc, CRect &rc, int idx);
	void InvalidateItem(int idx, BOOL erase = TRUE);

	int ItemFromPoint(CPoint pt);
	void AdjustViewTop();

	int GetSelectedItem() { return m_nSelectFirst; }

	void DeselectAllItems();

	int MoveToNextItem();
	int MoveToPrevItem();
	int MoveToNextChannel();
	int MoveToPrevChannel();

//	void SetupArticleDlg(int item);
	void OpenItem(int item);
	void FlagItem(int item);
	void UnflagItem(int item);
	void OpenItem(CFeedItem *feedItem);

	// article dialog
//	CArticleDlg *m_pArticleDlg;

	CSiteItem *SiteItem;

	// sort
//	CSortInfo *SortInfo;
	void Sort(int (__cdecl *compare)(const void *elem1, const void *elem2));

protected:
	int ItemHeight;

	static const int LABEL_MARGIN;
	static const int LABEL_X_PADDING;
	static const int LABEL_Y_PADDING;
	static const int LABEL_MSG_SKIP;
	static const int ITEM_MARGIN;

	friend DWORD WINAPI ScrollThread(LPVOID lpParam);

	friend class CArticleView;
	friend class CMainFrame;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHATCTRL_H__1F3F6BC2_F088_4028_94E7_5467DE65F7C2__INCLUDED_)
