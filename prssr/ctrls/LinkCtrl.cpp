// LinkCtrl.cpp : implementation file
//

#include "../stdafx.h"
#include "../prssr.h"
#include "LinkCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const static COLORREF rgbLinkColor = RGB(0, 0, 0xff);

/////////////////////////////////////////////////////////////////////////////
// CLinkCtrl

CLinkCtrl::CLinkCtrl() {
	// create an underlined font
	HFONT hSystemFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	ASSERT(hSystemFont);
	LOGFONT lf;
	VERIFY(GetObject(hSystemFont, sizeof(lf), &lf));
	lf.lfHeight = SCALEY(12) + 1;
	lf.lfUnderline = TRUE;
	VERIFY(m_fntUnderlined.CreateFontIndirect(&lf));
}

CLinkCtrl::~CLinkCtrl() {
}


BEGIN_MESSAGE_MAP(CLinkCtrl, CStatic)
	//{{AFX_MSG_MAP(CLinkCtrl)
//	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkCtrl message handlers

/*void CLinkCtrl::OnClicked() {
}
*/

void CLinkCtrl::OnPaint() {
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);

	int saveDC = dc.SaveDC();

	dc.FillSolidRect(rc, ::GetSysColor(COLOR_STATIC));

	dc.SetBkColor(::GetSysColor(COLOR_STATIC));
	dc.SetTextColor(rgbLinkColor);
	dc.SelectObject(&m_fntUnderlined);

	DWORD style = GetStyle();
	int fmt = DT_SINGLELINE | DT_TOP;
	if (style & SS_LEFT)
		fmt |= DT_LEFT;
	else if (style & SS_RIGHT)
		fmt |= DT_RIGHT;
	else if (style & SS_CENTER)
		fmt |= DT_CENTER;

	CString sText;
	GetWindowText(sText);
	dc.DrawText(sText, rc, fmt);
	
	dc.RestoreDC(saveDC);

	// Do not call CStatic::OnPaint() for painting messages
}
