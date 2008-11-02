/**
 *  InfoBar.h
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

#if !defined(AFX_INFOBAR__INCLUDED_)
#define AFX_INFOBAR__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// CInfoBar class
//
class CInfoBar : public CControlBar {
// Construction
public:
	CInfoBar();
	virtual ~CInfoBar();

//	BOOL Create(DWORD dwStyle, CRect &rc, CWnd *pParentWnd, UINT nID);
	BOOL Create(CWnd *parentWnd);
	void OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	void StartTimer();

	static BOOL Register();

// Implementation
public:
	void SetText(UINT nID);

protected:
	CBitmapButton m_ctlStopBtn;
	CString m_strText;
	UINT HideTimer;

    //{{AFX_MSG(CInfoBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnTextClicked();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOBAR__INCLUDED_)
