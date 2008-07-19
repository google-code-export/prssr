// Caption.cpp : implementation file
//

#if defined PRSSR_APP
	#include "../stdafx.h"
	#include "../prssr.h"
	#include "../Appearance.h"
#elif defined PRSSR_TODAY
	#include "../../prssrtoday/stdafx.h"
#endif

#include "Caption.h"
#include "../../share/uihelper.h"
#include "../../share/helpers.h"

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
// CCaption

CCaption::CCaption() {
	m_fntBold = NULL;
}

CCaption::~CCaption() {
	delete m_fntBold;
}


BEGIN_MESSAGE_MAP(CCaption, CStatic)
	//{{AFX_MSG_MAP(CCaption)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaption message handlers

void CCaption::OnPaint() {
	LOG0(1, "CCaption::OnPaint()");

	CPaintDC dc(this); // device context for painting

	CRect rcClient;

	GetClientRect(&rcClient);
	dc.FillSolidRect(&rcClient, GetSysColor(COLOR_STATIC));

	// caption
	CRect rcText = rcClient;
	CString strText;
	GetWindowText(strText);

	if (m_fntBold == NULL) {
		CFont *font = GetFont();
		LOGFONT lf;
		font->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;

		m_fntBold = new CFont();
		m_fntBold->CreateFontIndirect(&lf);
	}

	CFont *oldFont = dc.SelectObject(m_fntBold);
	dc.SetTextColor(GetSysColor(COLOR_STATICTEXT));
	DrawTextEndEllipsis(dc, strText, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(oldFont);

/*	// line
	CPen blackPen(PS_SOLID, SCALEY(1), GetSysColor(COLOR_3DSHADOW));

	CPen *oldPen  = dc.SelectObject(&blackPen);
	int nHalf = (rcClient.bottom + rcClient.top) / SCALEY(2);
	dc.MoveTo(rcClient.left, rcClient.bottom - SCALEY(1));
	dc.LineTo(rcClient.right, rcClient.bottom - SCALEY(1));
	
	dc.SelectObject(oldPen);
*/
	// Do not call CStatic::OnPaint() for painting messages
}
