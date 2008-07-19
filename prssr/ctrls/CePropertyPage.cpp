// CePropertyPage.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\prssr.h"
#include "CePropertyPage.h"
#include "../../share/helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCePropertyPage property page

IMPLEMENT_DYNCREATE(CCePropertyPage, CPropertyPage)

CCePropertyPage::CCePropertyPage()
{
}


CCePropertyPage::CCePropertyPage(UINT nIDTemplate, UINT nIDCaption/* = 0*/) : CPropertyPage(nIDTemplate, nIDCaption)
{
	//{{AFX_DATA_INIT(CCePropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	WideMode = InWideMode();
}

CCePropertyPage::~CCePropertyPage()
{
}

void CCePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCePropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCePropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCePropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCePropertyPage message handlers

void CCePropertyPage::ResizeControls() {
}

void CCePropertyPage::OnSize(UINT nType, int cx, int cy) {
	CPropertyPage::OnSize(nType, cx, cy);

	ResizeControls();
}
