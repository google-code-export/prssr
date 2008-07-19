#if !defined(_GROUPVIEW_H_)
#define _GROUPVIEW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupView window

/*
enum EGVIT {
	GVIT_ITEM = 0,
	GVIT_GROUP_EXPANDED = 1,
	GVIT_GROUP_COLLAPSED = 2
};
*/

enum EGVIS {
	GVIS_EXPANDED = 0,
	GVIS_COLLAPSED = 1
};

/*enum EGVI {
	GVI_ROOT,
	GVI_LAST
};
*/

/*
enum EGVE {
	GVE_COLLAPSE = 0,
	GVE_EXPAND = 1,
	GVE_TOGGLE = 2
};
*/


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
//	BOOL Create(DWORD dwStyle, const RECT &rect, CView *pParentWnd, UINT nID);

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

//	HGROUPITEM GetNextItem(HGROUPITEM pos, UINT code);
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
//	BOOL Expand(HGROUPITEM hItem, UINT nCode);
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
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual void ContextMenu(CPoint pt);
	virtual void OnItemClicked();

	DECLARE_MESSAGE_MAP()

	CImageList *m_pImageList;

	GVITEM RootItem;
	CList<GVITEM *, GVITEM *> Items;

	UINT NoItemsId;

	//
	CRect m_rcScroll;
/*	BOOL m_bScrolling;
	int m_nScrollDelta;
*/
	CScrollBar m_oVScrollBar;
//	UINT m_nOldKeys;
//	CPoint m_ptOldCursorPos;

	int m_nViewTop;								// in pixels
	int m_nTotalHeight;							// in pixels
	int m_nClientHeight;						// in pixels
	int m_nClientWidth;							// in pixels
	HGROUPITEM m_hSelectedItem;

	CFont m_fntBase;
	CFont m_fntBold;
	CImageList m_oIcons;

	UINT TapAndHoldTimer;
	CPoint LastCursorPos;

	UINT CtxMenuTimer;
//	BOOL IgnoreUp;

	HGROUPITEM m_hClickedItem;

	void UpdateItemsY(HGROUPITEM hParent, int &top);

	void CreateFonts();
	void DrawIcon(CDC &dc, int icon, BOOL selected);
	void DrawItems(CDC &dc, CRect &rc, HGROUPITEM hParent);
	virtual void OnDrawItem(CDC &dc, CRect &rc, HGROUPITEM hItem, BOOL selected);

	void InvalidateItem(HGROUPITEM hItem, BOOL erase = TRUE);

	HGROUPITEM ItemFromPoint(CPoint pt);
	void AdjustViewTop();

//	void UpdateScroll();

protected:
/*
	static const int LABEL_MARGIN;
	static const int LABEL_X_PADDING;
	static const int LABEL_Y_PADDING;
	static const int LABEL_MSG_SKIP;
	static const int INTERMSG_SKIP;

*/
//	static const int ITEM_HEIGHT;
	static const int ITEM_MARGIN;
	int ItemHeight;

	friend DWORD WINAPI ScrollThread(LPVOID lpParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_GROUPVIEW_H_)
