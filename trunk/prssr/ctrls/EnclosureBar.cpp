/**
 *  EnclosureBar.cpp
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

#include "../../share/UIHelper.h"
#include "../../share/helpers.h"
#include "EnclosureBar.h"
#include "../Appearance.h"

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
// CEnclosureBar

CEnclosureBar::CEnclosureBar() {
	LOGFONT lf;
	Appearance.BaseFont.GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lf);

	m_nSize = 0;
}

CEnclosureBar::~CEnclosureBar() {
	m_fntBold.DeleteObject();	
}


BEGIN_MESSAGE_MAP(CEnclosureBar, CStatic)
	//{{AFX_MSG_MAP(CEnclosureBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnclosureBar message handlers

void CEnclosureBar::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CFont *oldFont;
	CRect rcClient;

	GetClientRect(&rcClient);
	dc.FillSolidRect(&rcClient, GetSysColor(COLOR_3DFACE));

	// icon
	if (m_hIcon != NULL)
		dc.DrawIcon(SCALEX(1), SCALEY(1) + 1, m_hIcon);

	// title
	int oldBkMode = dc.SetBkMode(TRANSPARENT);
	CRect rcText = rcClient;
	rcText.DeflateRect(SCALEX(20), SCALEY(3), SCALEX(2), SCALEY(1));

//	oldFont = dc.SelectObject(&m_fntBold);
	if (Cached)
		oldFont = dc.SelectObject(&m_fntBold);
	else
		oldFont = dc.SelectObject(&Appearance.BaseFont);
	dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	if (m_nSize > 0) {
		CString s = FormatFileSize(m_nSize);
		CString strSize;
		strSize.Format(_T(" (%s)"), s);

		CRect rcSize = rcText;
		dc.DrawText(strSize, rcSize, DT_RIGHT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);
		rcSize.OffsetRect(rcText.Width() - rcSize.Width(), 0);
		dc.DrawText(strSize, rcSize, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		rcText.right -= rcSize.Width();
	}
	DrawTextEndEllipsis(dc, m_strName, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);
	dc.SelectObject(oldFont);

	dc.SetBkMode(oldBkMode);

	// line
	CPen blackPen(PS_SOLID, SCALEY(1), GetSysColor(COLOR_WINDOWFRAME));

	CPen *oldPen  = dc.SelectObject(&blackPen);
	dc.MoveTo(rcClient.left, rcClient.top + SCALEY(1) - 1);			// VGA hack: SCALEY(1) - 1
	dc.LineTo(rcClient.right, rcClient.top + SCALEY(1) - 1);
	dc.SelectObject(oldPen);

	ValidateRect(NULL);
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CEnclosureBar::OnLButtonDown(UINT nFlags, CPoint point) {
	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcIcon(rcClient.left, rcClient.top, rcClient.left + SCALEX(20), rcClient.bottom);
	CRect rcText(rcClient.left + SCALEX(21), rcClient.top, rcClient.right, rcClient.bottom);
	if (rcIcon.PtInRect(point)) {
		CMenu mnu;
		mnu.LoadMenu(IDR_ENCLOSURES);
		CMenu *popup = mnu.GetSubMenu(0);

		// track popup menu
		CPoint ptMnu(rcClient.left, rcClient.top + 1);
		ClientToScreen(&ptMnu);
		popup->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTALIGN, ptMnu.x, ptMnu.y, GetParent());
	}
	else if (rcText.PtInRect(point)) {
		// open enclosure
		GetParent()->PostMessage(WM_COMMAND, ID_ENCLOSURE_OPEN);
	}
}
