#if !defined(AFX_COLORBUTTON_H__EAA36042_2DD7_4F40_A5BF_60DEF206607E__INCLUDED_)
#define AFX_COLORBUTTON_H__EAA36042_2DD7_4F40_A5BF_60DEF206607E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
// Construction
public:
	CColorButton();

	COLORREF m_clrColor;

// Attributes
public:

// Operations
public:
	void SetColor(COLORREF clrColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__EAA36042_2DD7_4F40_A5BF_60DEF206607E__INCLUDED_)
