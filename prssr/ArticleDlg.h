/**
 *  ArticleDlg.h
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
#include "ctrls/CeDialog.h"
#include "ctrls/Banner.h"
#include "ctrls/EnclosureBar.h"
#include "ctrls/InfoBar.h"
#include "Site.h"

/////////////////////////////////////////////////////////////////////////////
// CArticleDlg dialog

class CFeedItem;
class CFeedView;
class CEnclosureItem;

class CArticleDlg : public CCeDialog {
// Construction
public:
	CArticleDlg();   // standard constructor
	~CArticleDlg();

	BOOL Create(CFeedView *view, CWnd *pParentWnd = NULL);

// Dialog Data
	enum {
		IDD = IDD_ARTICLE,
		IDD_WIDE = IDD_ARTICLE_WIDE
	};
	//{{AFX_DATA(CArticleDlg)
	CBanner m_ctlBanner;
	CEnclosureBar m_ctlEnclosureBar;
	//}}AFX_DATA
	CInfoBar m_ctlInfoBar;
	CHTMLCtrl m_ctlHTML;

	CFeedItem *m_pFeedItem;				// local copy of feed item (to have this pointer always valid)
										// something bad can happen when doing update on background

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticleDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFeedView *View;
	CImageList m_ilIcons;
	BOOL HotSpot;
	CString m_strContextMnuUrl;
	CString m_strContextMenuLinkName;
	CString m_strImageLoc;

	BOOL InFullScreen;
	void ToNormalMode();
	void ToFullScreenMode();

	void OnContextMenu(NM_HTMLCONTEXT *pnmhc);

	void SetupEnclosureBar(CEnclosureItem *ei);
	void ShowFeedItem();

	// Generated message map functions
	//{{AFX_MSG(CArticleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
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

	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
//	afx_msg BOOL OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);

	void OnScrollUp();
	void OnScrollDown();

	void OnLinkOpen();
	void OnLinkDownload();

	void OnBookmarkLink(UINT nID);
	void AppendBookmarkMenu(CMenu *menu);

	void NoNewMessage();

	void ResizeControls();

	void OnViewImage();
	void OnCopyImageLocation();
	void OnSendByEmail();
	void OnFullscreen();

	BOOL HtmlCached;

	DECLARE_MESSAGE_MAP()
};

void OpenOnlineMessage(const CString &link, CSiteItem *si);

//extern LPCTSTR szHash;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSDLG_H__B3A62D56_611B_4794_89D6_2921D65903C5__INCLUDED_)
