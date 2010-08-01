/**
 *  FeedsTreeCtrl.cpp
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

#include "StdAfx.h"
#include "prssr.h"
#include "FeedsTreeCtrl.h"

#include "Site.h"
#include "../share/helpers.h"
#include "Appearance.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Import Feeds icons
//

// site
#define ICON_SITE						0


CFeedsTreeCtrl::CFeedsTreeCtrl(UINT nID/* = IDS_NO_ITEMS_TO_DISPLAY*/) :
	CCeTreeCtrl(nID)
{
	m_nGroupIconIdx = -1;
}

BEGIN_MESSAGE_MAP(CFeedsTreeCtrl, CCeTreeCtrl)
	//{{AFX_MSG_MAP(CFeedsTreeCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFeedsTreeCtrl::SetGroupCheck(HTREEITEM hItem, BOOL check) {
	LOG2(5, "CFeedsTreeCtrl::SetGroupCheck(%d, %d)", hItem, check);

	SetCheck(hItem, check);

	HTREEITEM hChild = GetChildItem(hItem);
	while (hChild != NULL) {
		SetGroupCheck(hChild, check);
		hChild = GetNextSiblingItem(hChild);
	}
}

void CFeedsTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	LOG0(5, "CFeedsTreeCtrl::OnLButtonDown()");

	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if (hItem != NULL && (uFlags & TVHT_ONITEMSTATEICON)) {
		// checked -> unchecked, unchecked -> checked
		BOOL check;
		if (GetCheck(hItem))
			check = FALSE;					// set the check for all childs
		else
			check = TRUE;					// uncheck all childs

		CSiteItem *si = (CSiteItem *) GetItemData(hItem);
		if (si->Type == CSiteItem::Group) {
			SetGroupCheck(hItem, check);
			return;
		}
		else if (si->Type == CSiteItem::Site) {
			SetCheck(hItem, check);
			return;
		}
	}

	CCeTreeCtrl::OnLButtonDown(nFlags, point);
}

