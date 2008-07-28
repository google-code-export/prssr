/**
 *  TextProgressCtrl.h
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

#if !defined(AFX_TEXTPROGRESSCTRL__INCLUDED_)
#define AFX_TEXTPROGRESSCTRL__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../net/Download.h"

//
// CTextProgressCtrl class
//
class CTextProgressCtrl : public CWnd {
public:
	CTextProgressCtrl();

	static BOOL Register();

	BOOL Create(DWORD dwStyle, CRect &rc, CWnd *parent, UINT nID);

	void SetDownloader(CDownloader *downloader);

	void SetText(const CString &text);
	void Reset();

	void SetRange(int lo, int hi);
	void GetRange(int &lo, int &hi);

	void SetPos(int pos);
	int GetPos();

	void SetStep(int step);
	void StepIt();

	void Redraw(BOOL delayedPaint = FALSE);

protected:
	CDownloader *Downloader;

	int Lo, Hi;
	int Pos;
	int Step;

	UINT RedrawTimer;
	BOOL RedrawPlanned;

	CFont m_fntText;
	CString m_strText;
	CString m_strSize;

	void FinishRedraw();

	UINT ProgressRefreshTimer;

    //{{AFX_MSG(CTextProgressCtrl)
	DECLARE_MESSAGE_MAP()
	//}}AFX_MSG

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTPROGRESSCTRL__INCLUDED_)
