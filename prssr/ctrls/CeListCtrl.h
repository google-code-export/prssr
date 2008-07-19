#if !defined(AFX_CELISTCTRL_H__FC01E3E9_ED8C_4DCF_8591_C7EFE5AE020C__INCLUDED_)
#define AFX_CELISTCTRL_H__FC01E3E9_ED8C_4DCF_8591_C7EFE5AE020C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CeListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCeListCtrl window

class CCeListCtrl : public CListCtrl
{
// Construction
public:
	CCeListCtrl(UINT nID = IDS_NO_ITEMS_TO_DISPLAY);

// Attributes
public:

protected:
	UINT m_nID;
	CFont m_oFont;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeListCtrl();

	// Generated message map functions
protected:
	UINT KeyCtxTimer;

	void CreateFont();
	//{{AFX_MSG(CCeListCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CELISTCTRL_H__FC01E3E9_ED8C_4DCF_8591_C7EFE5AE020C__INCLUDED_)
