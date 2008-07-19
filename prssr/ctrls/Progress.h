#if !defined(AFX_PROGRESS_H__723B67CB_8E71_4BF1_B1B3_A2D7C0D61A69__INCLUDED_)
#define AFX_PROGRESS_H__723B67CB_8E71_4BF1_B1B3_A2D7C0D61A69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Progress.h : header file
//

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
