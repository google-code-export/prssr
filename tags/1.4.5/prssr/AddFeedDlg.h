/**
 *  AddFeedDlg.h
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

#if !defined(_ADD_FEED_DLG_H_)
#define _ADD_FEED_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"
#include "ctrls/CeDialog.h"

class CAddProgressDlg;
class CDownloader;
class CSiteItem;
struct CHtmlFeed;

/////////////////////////////////////////////////////////////////////////////
// CAddFeedDlg dialog

class CAddFeedDlg : public CCeDialog
{
// Construction
public:
	CAddFeedDlg(CWnd *pParent = NULL);   // standard constructor
	~CAddFeedDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_ADD_FEED_WIDE };
	//{{AFX_DATA(CAddFeedDlg)
	enum { IDD = IDD_ADD_FEED };
	CCeEdit	m_ctlName;
	CCeEdit	m_ctlURL;
	CButton	m_ctlCheckOnline;
	CString m_strName;
	CString m_strURL;
	BOOL m_bCheckOnline;
	//}}AFX_DATA
//	CString FileName;
//	CString FaviconFileName;
	CList<CSiteItem *, CSiteItem *> SiteItemsToAdd;

protected:
//	CList<CHtmlFeed *, CHtmlFeed *> LFeeds;
	CAddProgressDlg *m_pProgress;
	CDownloader *Downloader;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFeedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	virtual void ResizeControls();

	DWORD WINAPI AddThread();

	// Generated message map functions
	//{{AFX_MSG(CAddFeedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnClose();
	//}}AFX_MSG

	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	friend DWORD AddThreadProc(LPVOID lpParameter);
	friend class CAddProgressDlg;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AddOfflinePg_H__8A9A528F_7C5B_4FED_9F8A_8B800EACCE4C__INCLUDED_)
