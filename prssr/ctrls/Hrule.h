#if !defined(AFX_HRULE_H__55BA9C64_11B9_494C_9E56_BEF44FAB8F8A__INCLUDED_)
#define AFX_HRULE_H__55BA9C64_11B9_494C_9E56_BEF44FAB8F8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Hrule.h : header file
//

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
