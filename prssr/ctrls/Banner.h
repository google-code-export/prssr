#if !defined(_BANNER_H_)
#define _BANNER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Banner.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBanner window

class CBanner : public CStatic {
// Construction
public:
	CBanner();

// Attributes
public:
	COLORREF m_clrBackground;
	COLORREF m_clrForeground;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBanner)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBanner();

	void SetTitle(const CString &title) { m_strTitle = title; }
	void SetItems(int pos, int total = -1) { m_nPos = pos; if (total != -1) m_nTotal = total; }
//	void SetIcon(HICON icon) { m_hIcon = icon; }
	void SetIcon(int image) { m_nImage = image; }
	void SetImageList(CImageList *il) { m_pImageList = il; }

	void SetFlagged(int flaggedImg) { m_nFlaggedImage = flaggedImg; }
	void SetSmbImageList(CImageList *il) { m_pSmbImageList = il; }

	// Generated message map functions
protected:
	CFont m_fntBold;
	
	CString m_strTitle;
	int m_nPos;
	int m_nTotal;
//	HICON m_hIcon;
	int m_nImage;
	CImageList *m_pImageList;
	int m_nFlaggedImage;
	CImageList *m_pSmbImageList;

	//{{AFX_MSG(CBanner)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	BOOL m_bClick;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTION_H__B0F01B36_3F07_4A52_85E8_EC4A6C18B76A__INCLUDED_)
