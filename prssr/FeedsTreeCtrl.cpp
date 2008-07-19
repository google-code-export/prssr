// FeedsTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "prssr.h"
#include "FeedsTreeCtrl.h"

#include "Site.h"
#include "../share\helpers.h"
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

