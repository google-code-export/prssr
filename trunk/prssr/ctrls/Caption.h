#if !defined(AFX_CAPTION_H__B0F01B36_3F07_4A52_85E8_EC4A6C18B76A__INCLUDED_)
#define AFX_CAPTION_H__B0F01B36_3F07_4A52_85E8_EC4A6C18B76A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Caption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCaption window

class CCaption : public CStatic
{
// Construction
public:
	CCaption();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaption)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCaption();

	// Generated message map functions
protected:
	CFont *m_fntBold;

	//{{AFX_MSG(CCaption)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTION_H__B0F01B36_3F07_4A52_85E8_EC4A6C18B76A__INCLUDED_)
