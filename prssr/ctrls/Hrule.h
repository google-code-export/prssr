/**
 *  Hrule.h
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

#if !defined(AFX_HRULE_H__55BA9C64_11B9_494C_9E56_BEF44FAB8F8A__INCLUDED_)
#define AFX_HRULE_H__55BA9C64_11B9_494C_9E56_BEF44FAB8F8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HR_TOP							1
#define HR_LEFT							2
#define HR_RIGHT						4
#define HR_BOTTOM						8

/////////////////////////////////////////////////////////////////////////////
// CHrule window

class CHrule : public CStatic {
// Construction
public:
	CHrule();

// Attributes
public:

// Operations
public:
	void SetFrame(DWORD flags) { Flags = flags; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHrule)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHrule();

	// Generated message map functions
protected:
	DWORD Flags;

	//{{AFX_MSG(CHrule)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HRULE_H__55BA9C64_11B9_494C_9E56_BEF44FAB8F8A__INCLUDED_)
