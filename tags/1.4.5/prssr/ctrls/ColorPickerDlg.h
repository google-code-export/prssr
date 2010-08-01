/**
 *  ColorPickerDlg.h
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined(AFX_COLORPICKERDLG_H__0FD6D51A_059D_4270_9E91_23D40FBE1DBD__INCLUDED_)
#define AFX_COLORPICKERDLG_H__0FD6D51A_059D_4270_9E91_23D40FBE1DBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CeDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CColorCtl Control
class CColorCtl : public CStatic {
public:
	CColorCtl();
	virtual ~CColorCtl();
	
	void SetColor(COLORREF clrColor);

protected:
	COLORREF m_clrColor;

	//{{AFX_MSG(CColorCtl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CLightnessCtl Control
class CLightnessCtl : public CStatic {
public:
	CLightnessCtl();
	virtual ~CLightnessCtl();
	
	void SetColor(COLORREF color);
	void SetColor(double h, double s, double l);

protected:
	double m_dH, m_dS, m_dL;

	// caching
	CBitmap *m_bmp;
	CDC *m_dcMem;

	void SetLFromPoint(CPoint &point);
	void PrepareGradient();
	void UpdateMe(CPoint &point);

	//{{AFX_MSG(CLightnessCtl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CColorPickerDlg;
};


/////////////////////////////////////////////////////////////////////////////
// CHSCtl Control
class CHSCtl : public CStatic {
public:
	CHSCtl();
	virtual ~CHSCtl();
	
	void SetColor(COLORREF color);
	void SetColor(double h, double s, double l);

protected:
	double m_dH, m_dS;

	void SetHSFromPoint(CPoint &point);
	void DrawCross(CDC &dc);
	void UpdateMe(CPoint &point);

	// caching
	CDC *m_dcMem;
	CBitmap *m_bmp;

	//{{AFX_MSG(CHSCtl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CColorPickerDlg;
};


/////////////////////////////////////////////////////////////////////////////
// CColorPickerDlg dialog

class CColorPickerDlg : public CCeDialog {
// Construction
public:
	CColorPickerDlg(COLORREF clrInit = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorPickerDlg)
	enum { IDD = IDD_COLOR };
	CSpinButtonCtrl	m_ctlSspin;
	CSpinButtonCtrl	m_ctlRspin;
	CSpinButtonCtrl	m_ctlLspin;
	CSpinButtonCtrl	m_ctlHspin;
	CSpinButtonCtrl	m_ctlBspin;
	CSpinButtonCtrl	m_ctlGspin;
	CColorCtl	m_ctlColor;
	CLightnessCtl m_ctlLigthness;
	CHSCtl	m_ctlHSField;
	//}}AFX_DATA
	COLORREF m_clrColor;

public:
	COLORREF GetColor() { return m_clrColor; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	void UpdateColor();
//	void UpdateControls();

	void UpdateHSL();

	void SetHSL(double h, double s, double l);
	void SetHSLCtrl(double h, double s, double l);

	void UpdateRGB();
	void SetRGB(COLORREF color);

	// Generated message map functions
	//{{AFX_MSG(CColorPickerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposBspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposGspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposRspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposHspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposLspin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	LRESULT OnSelectColor(WPARAM wParam, LPARAM lParam);
	LRESULT OnSelectLightness(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKERDLG_H__0FD6D51A_059D_4270_9E91_23D40FBE1DBD__INCLUDED_)
