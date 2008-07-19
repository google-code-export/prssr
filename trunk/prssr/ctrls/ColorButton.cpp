// ColorButton.cpp : implementation file
//

#include "../stdafx.h"
#include "../prssr.h"
#include "../../share/uihelper.h"
#include "ColorButton.h"

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

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
//	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::SetColor(COLORREF clrColor) {
	m_clrColor = clrColor;
	Invalidate();
}

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	UINT uStyle = DFCS_BUTTONPUSH;

	// This code only works with buttons.
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	// If drawing selected, add the pushed style to DrawFrameControl.
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uStyle |= DFCS_PUSHED;

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	CRect rcClient;
	rcClient = lpDrawItemStruct->rcItem;

	// Draw the button frame.
	dc.DrawFrameControl(&rcClient, DFC_BUTTON, uStyle);

	rcClient.DeflateRect(SCALEX(4), SCALEY(4), SCALEX(4), SCALEY(4));
	dc.FillSolidRect(&rcClient, m_clrColor);
	dc.FrameRect(&rcClient, &CBrush(RGB(0, 0, 0)));
	ValidateRect(NULL);
}

