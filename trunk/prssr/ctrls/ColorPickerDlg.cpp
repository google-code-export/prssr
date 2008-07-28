/**
 *  ColorPickerDlg.cpp
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

#include "../StdAfx.h"
#include "../prssr.h"
#include "ColorPickerDlg.h"
#include "../../share/UIHelper.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SELECT_COLOR_MESSAGE		(WM_USER + 100)
#define SELECT_LIGHTNESS_MESSAGE	(WM_USER + 101)

//

COLORREF hsl2rgb(double h, double sl, double l) {
	double v;
	double r, g, b;

	r = l;   // default to gray
	g = l;
	b = l; 
	v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
	if (v > 0) {
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = (int) h; 
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) {
			case 0: 
				r = v; 
				g = mid1; 
				b = m; 
				break;
			case 1: 
				r = mid2; 
				g = v; 
				b = m; 
				break;
			case 2: 
				r = m; 
				g = v; 
				b = mid1; 
				break;
			case 3: 
				r = m; 
				g = mid2; 
				b = v; 
				break;
			case 4: 
				r = mid1; 
				g = m; 
				b = v; 
				break;
			case 5: 
				r = v; 
				g = m; 
				b = mid2; 
				break;
		}
	}

	return RGB((BYTE) (r * 255.0f), (BYTE) (g * 255.0f), (BYTE) (b * 255.0f));
}

void rgb2hsl(COLORREF rgb, double &h, double &s, double &l) {
	double r = GetRValue(rgb) / 255.0;
	double g = GetGValue(rgb) / 255.0;
	double b = GetBValue(rgb) / 255.0;
	double v;
	double m;
	double vm;
	double r2, g2, b2;

	h = 0; // default to black
	s = 0;
	l = 0;
	v = max(r, g);
	v = max(v, b);
	m = min(r, g);
	m = min(m, b);
	l = (m + v) / 2.0;
	if (l <= 0.0) {
		return;
	}
	vm = v - m;
	s = vm;
	if (s > 0.0) {
		s /= (l <= 0.5) ? (v + m ) : (2.0 - v - m);
	} 
	else {
		return;
	}
	r2 = (v - r) / vm;
	g2 = (v - g) / vm;
	b2 = (v - b) / vm;
	if (r == v) {
		h = (g == m ? 5.0 + b2 : 1.0 - g2);
	}
	else if (g == v) {
		h = (b == m ? 1.0 + r2 : 3.0 - b2);
	}
	else {
		h = (r == m ? 3.0 + g2 : 5.0 - r2);
	}
	h /= 6.0;
}

/////////////////////////////////////////////////////////////////////////////
// CColorCtl Control

CColorCtl::CColorCtl() {
}

CColorCtl::~CColorCtl() {
}

BEGIN_MESSAGE_MAP(CColorCtl, CStatic)
	//{{AFX_MSG_MAP(CColorCtl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorCtl::SetColor(COLORREF clrColor) {
	m_clrColor = clrColor;
}

void CColorCtl::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	// space
	dc.FrameRect(&rc, &CBrush(::GetSysColor(COLOR_WINDOW)));

	// fill
//	rc.DeflateRect(SCALEX(1), SCALEY(1), SCALEX(1), SCALEY(1));
	rc.DeflateRect(1, 1, 1, 1);
	dc.FillSolidRect(&rc, m_clrColor);

	ValidateRect(NULL);	
}


/////////////////////////////////////////////////////////////////////////////
// CLightnessCtl Control

CLightnessCtl::CLightnessCtl() {
	m_dL = 0.5;

	m_bmp = NULL;
	m_dcMem = NULL;
}

CLightnessCtl::~CLightnessCtl() {
	if (m_dcMem != NULL && m_bmp != NULL)
		m_dcMem->SelectObject(m_bmp);	// select original bitmap
	delete m_dcMem;
}

BEGIN_MESSAGE_MAP(CLightnessCtl, CStatic)
	//{{AFX_MSG_MAP(CLightnessCtl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLightnessCtl::SetColor(COLORREF color) {
	double l;
	rgb2hsl(color, m_dH, m_dS, l);

	PrepareGradient();
}

void CLightnessCtl::SetColor(double h, double s, double l) {
	m_dH = h;
	m_dS = s;
	m_dL = l;
	PrepareGradient();
}


#define THUMB_WD		SCALEX(10)

void CLightnessCtl::PrepareGradient() {
	if (m_dcMem != NULL) {
		CRect rc;
		GetClientRect(&rc);

		CRect rcFrame = CRect(rc.left, rc.top, rc.right - THUMB_WD, rc.bottom);
		CPen blackPen(PS_SOLID, SCALEY(1), RGB(0, 0, 0));
		m_dcMem->SelectObject(&blackPen);
		HIDPI_BorderRectangle(m_dcMem->GetSafeHdc(), rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.bottom);

		// fill
		rcFrame.DeflateRect(SCALEX(1), SCALEY(1), SCALEX(2), SCALEY(1));
		for (int i = 0; i < rc.Height(); i += SCALEY(3)) {
			double l = 1.0 - ((double) (i) / rc.Height());
			COLORREF rgb = hsl2rgb(m_dH, m_dS, l);
			m_dcMem->FillSolidRect(rcFrame.left, rcFrame.top + i, rcFrame.right, rcFrame.top + (i + SCALEY(3)), rgb);
		}

		i -= SCALEY(3);
		double l = 0.0;
		COLORREF rgb = hsl2rgb(m_dH, m_dS, l);
		m_dcMem->FillSolidRect(rcFrame.left, rcFrame.top + i, rcFrame.right, rcFrame.bottom, rgb);
	}
}

void CLightnessCtl::OnPaint() {
	CPaintDC dc(this); // device context for painting

	int save = dc.SaveDC();

	CRect rc;
	GetClientRect(&rc);

	if (m_dcMem == NULL) {
		m_dcMem = new CDC;

		CDC *pDC = GetDC();
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC, rc.Width() - THUMB_WD, rc.Height());

		m_dcMem->CreateCompatibleDC(pDC);
		m_bmp = m_dcMem->SelectObject(&bmp);
		PrepareGradient();

		ReleaseDC(pDC);
	}

	// copy gradient to screen
	dc.BitBlt(0, 0, rc.Width() - THUMB_WD, rc.Height(), m_dcMem, 0, 0, SRCCOPY);

	// thumb
	dc.FillSolidRect(rc.right - THUMB_WD, rc.top, rc.right, rc.bottom, GetSysColor(COLOR_WINDOW));
	int y = (int) ((1.0 - m_dL) * rc.Height());

	CPen blackPen(PS_SOLID, SCALEY(1), RGB(0, 0, 0));
	dc.SelectObject(&blackPen);

	dc.MoveTo(rc.right - THUMB_WD + SCALEX(1), y);
	dc.LineTo(rc.right, y);

	for (int i = 1; i<= SCALEY(2); i++) {
		dc.MoveTo(rc.right - THUMB_WD + SCALEX(1) + (2 * i), y - i);
		dc.LineTo(rc.right, y - i);

		dc.MoveTo(rc.right - THUMB_WD + SCALEX(1) + (2 * i), y + i);
		dc.LineTo(rc.right, y + i);
	}

	dc.RestoreDC(save);

	ValidateRect(NULL);	
}

void CLightnessCtl::SetLFromPoint(CPoint &point) {
	CRect rc;
	GetClientRect(&rc);
	m_dL = (double) (rc.bottom - point.y) / rc.Height();
}

void CLightnessCtl::UpdateMe(CPoint &point) {
	SetLFromPoint(point);
	
	CRect rc;
	GetClientRect(&rc);
	rc.left = rc.right - THUMB_WD;
	InvalidateRect(&rc);
	UpdateWindow();
}

void CLightnessCtl::OnLButtonDown(UINT nFlags, CPoint point) {
	SetCapture();
	UpdateMe(point);
	GetParent()->SendMessage(SELECT_LIGHTNESS_MESSAGE);
}

void CLightnessCtl::OnMouseMove(UINT nFlags, CPoint point) {
	CRect rc;
	GetClientRect(&rc);

	if (point.y < rc.top)
		point.y = rc.top;
	else if (point.y > rc.bottom)
		point.y = rc.bottom;

	UpdateMe(point);
	GetParent()->SendMessage(SELECT_LIGHTNESS_MESSAGE);
}

void CLightnessCtl::OnLButtonUp(UINT nFlags, CPoint point) {
	ReleaseCapture();
}


/////////////////////////////////////////////////////////////////////////////
// CHSCtl Control

CHSCtl::CHSCtl() {
	m_dcMem = NULL;
	m_bmp = NULL;
}

CHSCtl::~CHSCtl() {
	if (m_dcMem != NULL && m_bmp != NULL)
		m_dcMem->SelectObject(m_bmp);	// select original bitmap
	delete m_dcMem;
}

BEGIN_MESSAGE_MAP(CHSCtl, CStatic)
	//{{AFX_MSG_MAP(CHSCtl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHSCtl::SetColor(double h, double s, double l) {
	m_dH = h;
	m_dS = s;
}

void CHSCtl::SetColor(COLORREF color) {
	double l;
	rgb2hsl(color, m_dH, m_dS, l);
}

#define CROSS_SIZE SCALEX(5)
#define CROSS_INSIDE SCALEX(1)
#define CROSS_WD SCALEX(2)

void CHSCtl::DrawCross(CDC &dc) {
	CRect rc;
	GetClientRect(&rc);
	int x = (int) (m_dH * rc.Width());
	int y = (int) ((1.0 - m_dS) * rc.Height());

	CPen blackPen(PS_SOLID, SCALEY(1), RGB(0, 0, 0));
	dc.SelectObject(&blackPen);

	// left
	dc.MoveTo(x - CROSS_SIZE, y);
	dc.LineTo(x - CROSS_INSIDE, y);
	dc.MoveTo(x - CROSS_SIZE, y + SCALEY(1));
	dc.LineTo(x - CROSS_INSIDE, y + SCALEY(1));

	// right
	dc.MoveTo(x + CROSS_WD + CROSS_INSIDE, y);
	dc.LineTo(x + CROSS_WD + CROSS_SIZE, y);
	dc.MoveTo(x + CROSS_WD + CROSS_INSIDE, y + SCALEY(1));
	dc.LineTo(x + CROSS_WD + CROSS_SIZE, y + SCALEY(1));

	// up
	dc.MoveTo(x, y - CROSS_SIZE);
	dc.LineTo(x, y - CROSS_INSIDE);
	dc.MoveTo(x + SCALEX(1), y - CROSS_SIZE);
	dc.LineTo(x + SCALEX(1), y - CROSS_INSIDE);

	// down
	dc.MoveTo(x,     y + CROSS_WD + CROSS_INSIDE);
	dc.LineTo(x,     y + CROSS_WD + CROSS_SIZE);
	dc.MoveTo(x + SCALEX(1), y + CROSS_WD + CROSS_INSIDE);
	dc.LineTo(x + SCALEX(1), y + CROSS_WD + CROSS_SIZE);
}

void CHSCtl::OnPaint() {
	CPaintDC dc(this); // device context for painting

	int save = dc.SaveDC();

	CRect rc;
	GetClientRect(&rc);

	// ---
	if (m_dcMem == NULL) {
		m_dcMem = new CDC();

		CDC *pDC = GetDC();
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		m_dcMem->CreateCompatibleDC(pDC);
		m_bmp = m_dcMem->SelectObject(&bmp);

		// fill
		for (int i = 0; i < rc.Height(); i += SCALEY(5)) {
			for (int j = 0; j < rc.Width(); j += SCALEX(5)) {
				double h = (double) j / rc.Width();
				double s = 1.0 - (double) i / rc.Height();
				COLORREF rgb = hsl2rgb(h, s, 0.5);
				m_dcMem->FillSolidRect(rc.left + j, rc.top + i, rc.left + j + SCALEX(5), rc.top + i + SCALEY(5), rgb);
			}
		}

		ReleaseDC(pDC);
	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), m_dcMem, 0, 0, SRCCOPY);
	DrawCross(dc);

	dc.RestoreDC(save);

	ValidateRect(NULL);	
}

void CHSCtl::SetHSFromPoint(CPoint &point) {
	CRect rc;
	GetClientRect(&rc);

	m_dH = (double) (point.x - rc.left) / rc.Width();
	if (m_dH < 0.0) m_dH = 0.0;
	if (m_dH > 1.0) m_dH = 1.0;

	m_dS = (double) (rc.bottom - point.y) / rc.Height();
	if (m_dS < 0.0) m_dS = 0.0;
	if (m_dS > 1.0) m_dS = 1.0;
}

void CHSCtl::UpdateMe(CPoint &point) {
	// old position
	CRect rc;
	GetClientRect(&rc);
	int x = (int) (m_dH * rc.Width());
	int y = (int) ((1.0 - m_dS) * rc.Height());
	InvalidateRect(CRect(x - CROSS_SIZE, y - CROSS_SIZE, x + CROSS_SIZE + CROSS_WD, y + CROSS_SIZE + CROSS_WD));

	SetHSFromPoint(point);

	InvalidateRect(CRect(point.x - CROSS_SIZE, point.y - CROSS_SIZE,
		point.x + CROSS_SIZE + CROSS_WD, point.y + CROSS_SIZE + CROSS_WD));
	UpdateWindow();
}

void CHSCtl::OnLButtonDown(UINT nFlags, CPoint point) {
	SetCapture();
	UpdateMe(point);
	GetParent()->SendMessage(SELECT_COLOR_MESSAGE);
}

void CHSCtl::OnMouseMove(UINT nFlags, CPoint point) {
	CRect rc;
	GetClientRect(&rc);

	if (point.y < rc.top - SCALEY(1))
		point.y = rc.top - SCALEY(1);
	else if (point.y > rc.bottom - CROSS_INSIDE - SCALEY(1))
		point.y = rc.bottom - CROSS_INSIDE - SCALEY(1);

	if (point.x < rc.left)
		point.x = rc.left;
	else if (point.x > rc.right - CROSS_INSIDE - SCALEX(1))
		point.x = rc.right - CROSS_INSIDE - SCALEX(1);

	UpdateMe(point);
	GetParent()->SendMessage(SELECT_COLOR_MESSAGE);
}

void CHSCtl::OnLButtonUp(UINT nFlags, CPoint point) {
	ReleaseCapture();
}


/////////////////////////////////////////////////////////////////////////////
// CColorPickerDlg dialog


CColorPickerDlg::CColorPickerDlg(COLORREF clrInit/* = 0*/)
	: CCeDialog(CColorPickerDlg::IDD)
{
	m_clrColor = clrInit;
	//{{AFX_DATA_INIT(CColorPickerDlg)
	//}}AFX_DATA_INIT

	SetMenu(IDR_CANCEL);
//	m_strCaption.LoadString(IDS_SELECT_COLOR);
}


void CColorPickerDlg::DoDataExchange(CDataExchange* pDX)
{
	LOG0(5, "CColorPickerDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPickerDlg)
	DDX_Control(pDX, IDC_SSPIN, m_ctlSspin);
	DDX_Control(pDX, IDC_RSPIN, m_ctlRspin);
	DDX_Control(pDX, IDC_LSPIN, m_ctlLspin);
	DDX_Control(pDX, IDC_HSPIN, m_ctlHspin);
	DDX_Control(pDX, IDC_BSPIN, m_ctlBspin);
	DDX_Control(pDX, IDC_GSPIN, m_ctlGspin);
	DDX_Control(pDX, IDC_COLOR, m_ctlColor);
	DDX_Control(pDX, IDC_LIGHTNESS, m_ctlLigthness);
	DDX_Control(pDX, IDC_HSFIELD, m_ctlHSField);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPickerDlg, CCeDialog)
	//{{AFX_MSG_MAP(CColorPickerDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BSPIN, OnDeltaposBspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GSPIN, OnDeltaposGspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RSPIN, OnDeltaposRspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HSPIN, OnDeltaposHspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SSPIN, OnDeltaposSspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LSPIN, OnDeltaposLspin)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SELECT_COLOR_MESSAGE, OnSelectColor)
	ON_MESSAGE(SELECT_LIGHTNESS_MESSAGE, OnSelectLightness)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerDlg message handlers

BOOL CColorPickerDlg::OnInitDialog() {
	LOG0(3, "CColorPickerDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();
	
	// better spin buttons	
	m_ctlRspin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlGspin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlBspin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);

	m_ctlHspin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlSspin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);
	m_ctlLspin.SendMessage(CCM_SETVERSION, COMCTL32_VERSION);

	// set ranges
	m_ctlRspin.SetRange(0, 255);
	m_ctlGspin.SetRange(0, 255);
	m_ctlBspin.SetRange(0, 255);
	
	m_ctlHspin.SetRange(0, 360);
	m_ctlSspin.SetRange(0, 255);
	m_ctlLspin.SetRange(0, 255);

	// set values
	SetRGB(m_clrColor);
	double h, s, l;
	rgb2hsl(m_clrColor, h, s, l);
	SetHSL(h, s, l);

	m_ctlColor.SetColor(m_clrColor);

	m_ctlLigthness.SetColor(m_clrColor);
	m_ctlLigthness.m_dL = l;

	m_ctlHSField.m_dH = h;
	m_ctlHSField.m_dS = s;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CColorPickerDlg::OnSelectColor(WPARAM wParam, LPARAM lParam) {
	double h = m_ctlHSField.m_dH;
	double s = m_ctlHSField.m_dS;
	double l = m_ctlLigthness.m_dL;

	COLORREF color = hsl2rgb(h, s, l);
	m_ctlColor.SetColor(color);
	m_ctlColor.Invalidate();

//	m_ctlLigthness.SetColor(color);
	m_ctlLigthness.SetColor(h, s, l);
	m_ctlLigthness.Invalidate();

	// HSL
	m_ctlHspin.SetPos((int) (h * 360));
	m_ctlSspin.SetPos((int) (s * 255));

	// RGB
	SetRGB(color);
	m_clrColor = color;

	return TRUE;
}

LRESULT CColorPickerDlg::OnSelectLightness(WPARAM wParam, LPARAM lParam) {
	double h = m_ctlHSField.m_dH;
	double s = m_ctlHSField.m_dS;
	double l = m_ctlLigthness.m_dL;

	COLORREF color = hsl2rgb(h, s, l);
	m_ctlColor.SetColor(color);
	m_ctlColor.Invalidate();

	// HSL
	m_ctlLspin.SetPos((int) (l * 255));

	// RGB
	SetRGB(color);
	m_clrColor = color;

	return TRUE;
}

void CColorPickerDlg::SetHSL(double h, double s, double l) {
	m_ctlHspin.SetPos((int) (h * 360));
	m_ctlSspin.SetPos((int) (s * 255));
	m_ctlLspin.SetPos((int) (l * 255));
}

void CColorPickerDlg::SetHSLCtrl(double h, double s, double l) {
	m_ctlLigthness.SetColor(h, s, l);
	m_ctlLigthness.Invalidate();

	m_ctlHSField.SetColor(h, s, l);
	m_ctlHSField.Invalidate();
}

void CColorPickerDlg::UpdateHSL() {
	int r = m_ctlRspin.GetPos();
	int g = m_ctlGspin.GetPos();
	int b = m_ctlBspin.GetPos();

	m_clrColor = RGB(r, g, b);
	double h, s, l;
	rgb2hsl(m_clrColor, h, s, l);
	SetHSL(h, s, l);

	m_ctlColor.SetColor(m_clrColor);
	m_ctlColor.Invalidate();

	SetHSLCtrl(h, s, l);
}

void CColorPickerDlg::OnDeltaposBspin(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateHSL();
	*pResult = 0;
}

void CColorPickerDlg::OnDeltaposGspin(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateHSL();
	*pResult = 0;
}

void CColorPickerDlg::OnDeltaposRspin(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateHSL();
	*pResult = 0;
}

void CColorPickerDlg::SetRGB(COLORREF color) {
	m_ctlRspin.SetPos(GetRValue(color));
	m_ctlGspin.SetPos(GetGValue(color));
	m_ctlBspin.SetPos(GetBValue(color));
}

void CColorPickerDlg::UpdateRGB() {
	int h = m_ctlHspin.GetPos();
	int s = m_ctlSspin.GetPos();
	int l = m_ctlLspin.GetPos();

	m_clrColor = hsl2rgb(h / 360.0, s / 255.0, l / 255.0);
	SetRGB(m_clrColor);

	m_ctlColor.SetColor(m_clrColor);
	m_ctlColor.Invalidate();

	SetHSLCtrl(h / 360.0, s / 255.0, l / 255.0);
}

void CColorPickerDlg::OnDeltaposHspin(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateRGB();	
	*pResult = 0;
}

void CColorPickerDlg::OnDeltaposSspin(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateRGB();	
	*pResult = 0;
}

void CColorPickerDlg::OnDeltaposLspin(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateRGB();	
	*pResult = 0;
}
