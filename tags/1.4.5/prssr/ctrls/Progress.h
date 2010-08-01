/**
 *  Progress.h
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

#if !defined(AFX_PROGRESS_H__723B67CB_8E71_4BF1_B1B3_A2D7C0D61A69__INCLUDED_)
#define AFX_PROGRESS_H__723B67CB_8E71_4BF1_B1B3_A2D7C0D61A69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CProgress window

class CProgress : public CStatic
{
// Construction
public:
	CProgress();

// Attributes
public:

// Operations
public:
	void Run();
	void Stop();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgress)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProgress();

	// Generated message map functions
protected:
	void DrawProg();

	//{{AFX_MSG(CProgress)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	BOOL m_bRunning;
	UINT m_nTimer;
	int m_nOffset;

	// caching
	CBitmap *m_bmp;
	CDC *m_dcMem;
	
	CRect m_rcBar;
	int m_nPcWd;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESS_H__723B67CB_8E71_4BF1_B1B3_A2D7C0D61A69__INCLUDED_)
