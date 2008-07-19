// InfoBar.h : interface of the CInfoBar class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFOBAR__INCLUDED_)
#define AFX_INFOBAR__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// CInfoBar class
//
class CInfoBar : public CWnd {
// Construction
public:
	CInfoBar();
	virtual ~CInfoBar();

	BOOL Create(DWORD dwStyle, CRect &rc, CWnd *pParentWnd, UINT nID);

	void StartTimer();

	static BOOL Register();

// Implementation
public:
	void SetText(UINT nID);

protected:
	CBitmapButton m_ctlStopBtn;
//	CStatic m_ctlText;
	CString m_strText;
	UINT HideTimer;

    //{{AFX_MSG(CInfoBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnTextClicked();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOBAR__INCLUDED_)
