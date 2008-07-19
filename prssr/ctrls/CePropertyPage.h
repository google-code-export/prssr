#if !defined(AFX_CEPROPERTYPAGE_H__C2B9A61A_7ED0_40D8_807D_73441E2CA384__INCLUDED_)
#define AFX_CEPROPERTYPAGE_H__C2B9A61A_7ED0_40D8_807D_73441E2CA384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CePropertyPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCePropertyPage dialog

class CCePropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCePropertyPage)

// Construction
public:
	CCePropertyPage();
	CCePropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
	~CCePropertyPage();

// Dialog Data
	//{{AFX_DATA(CCePropertyPage)
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCePropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL WideMode;	

	virtual void ResizeControls();

	// Generated message map functions
	//{{AFX_MSG(CCePropertyPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEPROPERTYPAGE_H__C2B9A61A_7ED0_40D8_807D_73441E2CA384__INCLUDED_)
