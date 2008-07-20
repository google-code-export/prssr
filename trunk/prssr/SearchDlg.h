/**
 *  SearchDlg.h
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

#if !defined(_SITESEARCHPG_H_)
#define _SITESEARCHPG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"
#include "ctrls/CeDialog.h"

#include "net/Searcher.h"
//#include "Session.h"

class CSearchProgressDlg;
class CSyndic8ComSearcher;

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

class CSearchDlg : public CCeDialog//, public CRestorable
{
// Construction
public:
	CSearchDlg(CWnd *pParent = NULL);   // standard constructor
	~CSearchDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_SEARCH_WIDE };
	//{{AFX_DATA(CSearchDlg)
	enum { IDD = IDD_SEARCH };
	CCeEdit	m_ctlWhat;
	CString m_strWhat;
	//}}AFX_DATA
	CList<CSearchResultItem *, CSearchResultItem *> *SearchResults;

protected:
	CSearchProgressDlg *m_pProgress;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	virtual void ResizeControls();

	DWORD WINAPI SearchThread();

	CSyndic8ComSearcher *Searcher;

	// Generated message map functions
	//{{AFX_MSG(CSearchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSearch();
	afx_msg void OnChangeWhat();
	//}}AFX_MSG

	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	friend DWORD SearchThreadProc(LPVOID lpParameter);
	friend CSearchProgressDlg;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_SITESEARCHPG_H_)
