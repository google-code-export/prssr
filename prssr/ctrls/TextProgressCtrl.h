// UpdateBar.h : interface of the CUpdateBar class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTPROGRESSCTRL__INCLUDED_)
#define AFX_TEXTPROGRESSCTRL__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\net\download.h"

//
// CTextProgressCtrl class
//
class CTextProgressCtrl : public CWnd {
public:
	CTextProgressCtrl();

	static BOOL Register();

	BOOL Create(DWORD dwStyle, CRect &rc, CWnd *parent, UINT nID);

	void SetDownloader(CDownloader *downloader);

	void SetText(const CString &text);
	void Reset();

	void SetRange(int lo, int hi);
	void GetRange(int &lo, int &hi);

	void SetPos(int pos);
	int GetPos();

	void SetStep(int step);
	void StepIt();

	void Redraw(BOOL delayedPaint = FALSE);

protected:
	CDownloader *Downloader;

	int Lo, Hi;
	int Pos;
	int Step;

	UINT RedrawTimer;
	BOOL RedrawPlanned;

	CFont m_fntText;
	CString m_strText;
	CString m_strSize;
	
	void FinishRedraw();

	UINT ProgressRefreshTimer;

    //{{AFX_MSG(CTextProgressCtrl)
	DECLARE_MESSAGE_MAP()
	//}}AFX_MSG

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTPROGRESSCTRL__INCLUDED_)
