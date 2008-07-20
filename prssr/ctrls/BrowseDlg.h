/**
 *  BrowseDlg.h
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

#if !defined(AFX_BROWSEDLG_H__DD2268BD_8D65_4766_8BB4_AC41909FCDB5__INCLUDED_)
#define AFX_BROWSEDLG_H__DD2268BD_8D65_4766_8BB4_AC41909FCDB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Caption.h"
#include "CeEdit.h"
#include "CeDialog.h"
//#include "Session.h"

/////////////////////////////////////////////////////////////////////////////
// CBrowseDlg dialog

class CBrowseDlg : public CCeDialog //, public CRestorable
{
// Construction
public:
	CBrowseDlg(BOOL bFiles, CWnd* pParent = NULL);   // standard constructor
	~CBrowseDlg();

// Dialog Data
	enum { IDD_WIDE = IDD_BROWSE_WIDE };
	//{{AFX_DATA(CBrowseDlg)
	enum { IDD = IDD_BROWSE };
//	CButton	m_ctlNewFolder;
//	CCaption	m_ctlCaption;
	CTreeCtrl	m_ctlDirs;
	CCeEdit	m_ctlPath;
//	CString	m_strCaption;
	CString	m_strPath;
	//}}AFX_DATA
	CImageList m_oImageList;
//	BOOL m_bNewFolderBtn;
	// add only files with this extension
	CString m_strExts;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bFiles;
	HTREEITEM m_hRootItem;
	CStringArray m_arExts;
	
	// internal flag (delete items on expanding if TRUE), used 
	// when adding new item (to prevent deleteting of newly crated items)
	BOOL m_bExpandDelete;

	CString GetFullPath(HTREEITEM hItem);
	void UpdateControls();
	void FillMe();
	void SelectPath(const CString &path);

	HTREEITEM AddDirItem(HTREEITEM hParent, CString strItem, int nIcon, BOOL bPerm, HTREEITEM hInsertAfter = TVI_LAST);
	void AddPossibleChild(HTREEITEM hParent, CString strPath);
	void AddPathItems(HTREEITEM hParent, CString strPath);

//	BOOL IsSelectedItemValid();
	BOOL HasExt(const CString &strFileName);
//	virtual void Restore();

	// Generated message map functions
	//{{AFX_MSG(CBrowseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedDirs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOk();
	afx_msg void OnNewFolder();
	afx_msg void OnEndlabeleditDirs(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnItemexpandingDirs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSEDLG_H__DD2268BD_8D65_4766_8BB4_AC41909FCDB5__INCLUDED_)
