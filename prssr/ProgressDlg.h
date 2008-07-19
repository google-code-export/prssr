#if !defined(_PROGRESSDLG_H_)
#define _PROGRESSDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// progresdlg.h : header file
//

//#include "Caption.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog {
// Construction
public:
	CProgressDlg();   // standard constructor
	CProgressDlg(CWnd *parent);   // standard constructor

// Dialog Data
	CProgressCtrl m_ctlProgress;
	CStatic m_ctlLabel;
	CButton m_ctlCancel;
//	CStatic CtlSubText;

	virtual void OpenDialog(const CString &strText, CWnd *parent = NULL);
	virtual void OpenDialog(UINT nID, CWnd *parent = NULL);
	virtual void SetTotalSize(DWORD start, DWORD size);
	virtual BOOL AddSize(DWORD amount, BOOL delayedPaint = TRUE);
	virtual void CloseDialog();

	virtual void ShowError(UINT nID);
	virtual void ShowError(const CString &strText);

	virtual void SetRange(int lo, int hi);
	virtual void SetPos(int pos);

	virtual BOOL GetTerminated() { return Terminate; }

//	virtual void SetLabel(const CString &label);
//	virtual void SetLabel(UINT nID);

//	virtual void SetSubText(const CString &label);
//	virtual void SetSubText(UINT nID);
//	virtual void SetProgress(DWORD size);

/*	virtual void StartWorkIndicator();
	virtual void StopWorkIndicator();
	virtual void StartSubWorkIndicator();
	virtual void StopSubWorkIndicator();
*/
// Overrides
	// ClassWizard generated virtual function overrides
//	enum { IDD_WIDE = IDD_PROGRESS_WIDE };
	//{{AFX_VIRTUAL(CProgressDlg)
	enum { IDD = IDD_PROGRESS };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
//	CWnd *Parent;
	BOOL Terminate;
	HWND m_hwndCmdBar;

	CString m_strLabel;

/*	UINT LabelTimer;
	int LabelDots;
	CString Label;

	UINT SubTextTimer;
	int SubTextDots;
	CString SubText;
*/

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
//	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized); 
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

//	virtual UINT ProgressThread();

//	friend UINT ProgressThreadProc(LPVOID lpParameter);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_PROGRESSDLG_H_)
