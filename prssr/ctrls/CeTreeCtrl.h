#if !defined(AFX_CETREECTRL_H__DF29C8F9_0C86_4917_9CCD_4C41D0954C34__INCLUDED_)
#define AFX_CETREECTRL_H__DF29C8F9_0C86_4917_9CCD_4C41D0954C34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CeTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCeTreeCtrl window

class CCeTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CCeTreeCtrl(UINT nID = IDS_NO_ITEMS_TO_DISPLAY);

// Attributes
public:

protected:
	UINT m_nID;
//	CFont m_oSmallFont;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeTreeCtrl();

	// Generated message map functions
protected:
//	void CreateFont();
	//{{AFX_MSG(CCeTreeCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CETREECTRL_H__DF29C8F9_0C86_4917_9CCD_4C41D0954C34__INCLUDED_)
