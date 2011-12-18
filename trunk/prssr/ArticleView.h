/**
 *  ArticleView.h
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

#if !defined(AFX_ARTICLEDLG_H__B3A62D56_611B_4794_89D6_2921D65903C5__INCLUDED_)
#define AFX_ARTICLEDLG_H__B3A62D56_611B_4794_89D6_2921D65903C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/HtmlCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CArticleView dialog

class CSiteItem;
class CFeedItem;
class CFeedView;
class CEnclosureItem;

class CArticleView : public CHTMLCtrl {
// Construction
public:
	CArticleView();   // standard constructor
	~CArticleView();

	void CreateMenu(HWND hwndCmdBar);
	void SetFeedItem(CFeedItem *fi);
	void ShowArticle();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticleView)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFeedView *View;
	CFeedItem *m_pArticle;				// local copy of feed item (to have this pointer always valid)
										// something bad can happen when doing update on background

//	CImageList m_ilIcons;
	BOOL HotSpot;
	CString m_strContextMnuUrl;
	CString m_strContextMenuLinkName;
	CString m_strImageLoc;

	BOOL InFullScreen;
	void ToNormalMode();
	void ToFullScreenMode();

	void OnContextMenu(NM_HTMLCONTEXT *pnmhc);
	void ContextMenu(CPoint point);

	// Generated message map functions
	//{{AFX_MSG(CArticleView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

	afx_msg void OnItemNext();
	afx_msg void OnItemPrev();

	afx_msg void OnItemOpen();
	afx_msg void OnItemFlag();
	afx_msg void OnUpdateItemFlag(CCmdUI *pCmdUI);

	afx_msg void OnRefresh();
	afx_msg void OnCopyUrl();
	afx_msg void OnCopy();
	afx_msg void OnUpdateCopy(CCmdUI *pCmdUI);

	afx_msg void OnEnclosureOpen();
	afx_msg void OnUpdateEnclosureOpen(CCmdUI *pCmdUI);
	afx_msg void OnEnclosureGet();
	afx_msg void OnUpdateEnclosureGet(CCmdUI *pCmdUI);
	afx_msg void OnEnclosureDelete();
	afx_msg void OnUpdateEnclosureDelete(CCmdUI *pCmdUI);

	void OnScrollUp();
	void OnScrollDown();

	afx_msg void OnLinkOpen();
	afx_msg void OnLinkDownload();

	afx_msg void OnBookmarkLink(UINT nID);
	void AppendBookmarkMenu(CMenu *menu);

	afx_msg void OnViewImage();
	afx_msg void OnCopyImageLocation();
	afx_msg void OnSendByEmail();
	afx_msg void OnFullscreen();
	afx_msg void OnUpdateFullscreen(CCmdUI *pCmdUI);
	afx_msg void OnDisableGestures();
	afx_msg void OnUpdateDisableGestures(CCmdUI *pCmdUI);
	afx_msg void OnSelectAndCopyAll();

	friend class CMainFrame;

	DECLARE_MESSAGE_MAP()
};

void OpenOnlineMessage(const CString &link, CSiteItem *si);

//extern LPCTSTR szHash;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSDLG_H__B3A62D56_611B_4794_89D6_2921D65903C5__INCLUDED_)
