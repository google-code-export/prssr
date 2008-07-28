/**
 *  Caption.cpp
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

#if defined PRSSR_APP
	#include "../StdAfx.h"
	#include "../prssr.h"
	#include "../Appearance.h"
#elif defined PRSSR_TODAY
	#include "../../prssrtoday/StdAfx.h"
#endif

#include "Caption.h"
#include "../../share/UIHelper.h"
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
