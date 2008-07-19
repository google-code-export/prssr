#if !defined(AFX_CETOOLBAR_H__E5AC3314_DCFF_44BC_8440_D50430A6A5D6__INCLUDED_)
#define AFX_CETOOLBAR_H__E5AC3314_DCFF_44BC_8440_D50430A6A5D6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CeToolBar.h : header file
//

#define IDC_ACT_SITE					0x0900
#define ID_TOOLBAR_SEPARATOR			0x0901
#define IDC_SORT_BUTTON					0x0902

/////////////////////////////////////////////////////////////////////////////
// CCeToolBar window

class CCeToolBar : public CToolBar
{
// Construction
public:
	CCeToolBar();

// Attributes
public:

// Operations
public:
	void SetText(const CString &strText) { m_strText = strText; }
	void SetImageIdx(int nImageIdx) { m_nImageIdx = nImageIdx; }
	void EnableSortButton(BOOL enable = TRUE) { m_bSortButton = enable; }
	void SetSortButton(int imageIdx) { m_nImageSortIdx = imageIdx; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeToolBar();

	// Generated message map functions
protected:
	int m_nImageIdx;
	CString m_strText;
	CRect m_rcText;
	BOOL m_bSortButton;
	int m_nImageSortIdx;

	//{{AFX_MSG(CCeToolBar)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CETOOLBAR_H__E5AC3314_DCFF_44BC_8440_D50430A6A5D6__INCLUDED_)
