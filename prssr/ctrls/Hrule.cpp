// Hrule.cpp : implementation file
//

#include "../stdafx.h"
#include "../prssr.h"
#include "../../share/uihelper.h"
#include "Hrule.h"

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
// CHrule

CHrule::CHrule() {
	Flags = 0;
}

CHrule::~CHrule() {
}


BEGIN_MESSAGE_MAP(CHrule, CStatic)
	//{{AFX_MSG_MAP(CHrule)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHrule message handlers

void CHrule::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	if (IsWindowVisible()) {
		GetClientRect(&rcClient);
		dc.FillSolidRect(&rcClient, GetSysColor(COLOR_STATIC));

		CPen blackPen(PS_SOLID, SCALEY(1), GetSysColor(COLOR_3DSHADOW));

		CPen *oldPen  = dc.SelectObject(&blackPen);

		if (Flags != 0) {
			DWORD masks[] = { HR_TOP, HR_LEFT, HR_BOTTOM, HR_RIGHT };
			for (int i = 0; i < countof(masks); i++) {
				if ((Flags & HR_TOP) == HR_TOP) {
					dc.MoveTo(rcClient.left, rcClient.top);
					dc.LineTo(rcClient.right, rcClient.top);
				}
				else if ((Flags & HR_LEFT) == HR_LEFT) {
					dc.MoveTo(rcClient.left, rcClient.top);
					dc.LineTo(rcClient.left, rcClient.bottom);
				}
				else if ((Flags & HR_BOTTOM) == HR_BOTTOM) {
					dc.MoveTo(rcClient.left, rcClient.bottom);
					dc.LineTo(rcClient.right, rcClient.bottom);
				}
				else if ((Flags & HR_RIGHT) == HR_RIGHT) {
					dc.MoveTo(rcClient.right, rcClient.top);
					dc.LineTo(rcClient.right, rcClient.bottom);
				}
			}
		}
		else {
			int nHalf = (rcClient.bottom + rcClient.top) / 2;
			dc.MoveTo(rcClient.left, nHalf);
			dc.LineTo(rcClient.right, nHalf);
		}

		dc.SelectObject(oldPen);
	}

	ValidateRect(NULL);

	// Do not call CStatic::OnPaint() for painting messages
}

