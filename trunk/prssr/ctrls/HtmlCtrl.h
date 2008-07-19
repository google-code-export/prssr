#if !defined(_HTMLCTRL_H_)
#define _HTMLCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHTMLCtrl

class CHTMLCtrl {//: public CWnd {
public:
	CHTMLCtrl();

	HWND GetHwnd() { return HWnd; }

	BOOL Create(DWORD dwStyle, CRect &rc, HWND parent, UINT id);

	void Clear();
	void EnableContextMenu(BOOL enable = TRUE);
	void EnableClearType(BOOL enable = TRUE);
	void EnableScripting(BOOL enable = TRUE);
	void ZoomLevel(int level);
	void AddText(LPWSTR str);
	void AddText(const CString &str);
	void EndOfSource();
	BOOL IsSelection();
	void CopySelectionToNewIStream(DWORD *rsd, LPSTREAM *stream);

protected:
	static HINSTANCE HHtmlDLL;
	HWND HWnd;

/*	// Generated message map functions
	//{{AFX_MSG(CHTMLCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
*/
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_HTMLCTRL_H_)
