/**
 *  SearchDlg.cpp
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
#include "SearchDlg.h"
#include "prssr.h"
#include "Config.h"
//#include "Session.h"
#include "../share/helpers.h"

#include "net/Searcher.h"
#include "net/Connection.h"

#include "ProgressDlg.h"

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

static CRITICAL_SECTION CSSearcher; 

DWORD WINAPI SearchThreadProc(LPVOID lpParameter) {
	CSearchDlg *dlg = (CSearchDlg *) lpParameter;
	return dlg->SearchThread();
}

/////////////////////////////////////////////////////////////////////////////
// CSearchProgressDlg dialog

class CSearchProgressDlg : public CProgressDlg {
public:
	CSearchProgressDlg(CSyndic8ComSearcher *searcher, CSearchDlg *parent);
	virtual ~CSearchProgressDlg();

protected:
	CSyndic8ComSearcher *Searcher;
	UINT Timer;
	ESearchState State;
	CSearchDlg *Parent;

	int RangeHi;

    //{{AFX_MSG(CSearchProgressDlg)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual BOOL OnInitDialog();

	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

CSearchProgressDlg::CSearchProgressDlg(CSyndic8ComSearcher *searcher, CSearchDlg *parent) {
	Searcher = searcher;
	State = SEARCH_STATE_NONE;
	Timer = 1;

	Parent = parent;
	RangeHi = 150000;
}

CSearchProgressDlg::~CSearchProgressDlg() {
}

BEGIN_MESSAGE_MAP(CSearchProgressDlg, CProgressDlg)
	//{{AFX_MSG_MAP(CSearchProgressDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSearchProgressDlg::OnInitDialog() {
	LOG0(3, "CSearchProgressDlg::OnInitDialog()");

	SetTimer(Timer, 500, NULL);
	CProgressDlg::OnInitDialog();

	SetRange(0, RangeHi);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchProgressDlg::OnDestroy() {
	KillTimer(Timer);
}

void CSearchProgressDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == Timer) {
		ESearchState state = SEARCH_STATE_NONE;

		EnterCriticalSection(&CSSearcher);
		if (Searcher != NULL) {
			state = Searcher->State;

			int newPos = (int) Searcher->GetDownloadedFileSize();
			if (newPos > RangeHi) {
				RangeHi *= 10;
				SetRange(0, RangeHi);
			}
			SetPos(newPos);
		}
		LeaveCriticalSection(&CSSearcher);
		
		if (State != state) {
			State = state;

			CString sText;
			switch (state) {
				case SEARCH_STATE_CONNECTING:         sText.LoadString(IDS_CONNECTING); break;
				case SEARCH_STATE_CONNECTED:          sText.LoadString(IDS_CONNECTED); break;
				case SEARCH_STATE_SENDING_REQUEST:    sText.LoadString(IDS_SENDING_REQUEST); break;
				case SEARCH_STATE_RECEIVING_RESPONSE: sText.LoadString(IDS_GETTING_RESPONSE); break;
				case SEARCH_STATE_DATA_TRANSFER:      sText.LoadString(IDS_DATA_TRANSFER); break;
				default:                              sText.LoadString(IDS_SEARCHING); break;
			}

			sText += _T("...");
			m_ctlLabel.SetWindowText(sText);
		}
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CSearchProgressDlg::OnCancel() {
	LOG0(3, "CSearchProgressDlg::OnCancel()");

	EnterCriticalSection(&CSSearcher);
	if (Searcher != NULL)
		Searcher->Terminate();
	LeaveCriticalSection(&CSSearcher);	

	Parent->m_pProgress = NULL;

	CProgressDlg::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

CSearchDlg::CSearchDlg(CWnd *pParent/* = NULL*/)
	: CCeDialog(InWideMode() ? CSearchDlg::IDD_WIDE : CSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchDlg)
	//}}AFX_DATA_INIT
	m_nMenuID = IDR_SEARCH;

	m_pProgress = NULL;
	Searcher = NULL;
	InitializeCriticalSection(&CSSearcher);
}

CSearchDlg::~CSearchDlg() {
	DeleteCriticalSection(&CSSearcher);
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX) {
	LOG0(5, "CSearchDlg::DoDataExchange()");

	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchDlg)
	DDX_Control(pDX, IDC_WHAT, m_ctlWhat);
	DDX_Text(pDX, IDC_WHAT, m_strWhat);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		if (m_strWhat.IsEmpty())
			pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CSearchDlg, CCeDialog)
	//{{AFX_MSG_MAP(CSearchDlg)
	ON_EN_CHANGE(IDC_WHAT, OnChangeWhat)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SEARCH, OnSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers

BOOL CSearchDlg::OnInitDialog() {
	LOG0(3, "CSearchDlg::OnInitDialog()");

	CCeDialog::OnInitDialog();

//	m_ctlWhat.SetWindowText(_T("gadget"));

	SetForegroundWindow();
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchDlg::OnSearch() {
	LOG0(1, "CSearchDlg::OnSearch()");

	if (UpdateData())
		OnOK();
}

void CSearchDlg::UpdateControls() {
	LOG0(5, "CSearchDlg::UpdateControls()");

}

void CSearchDlg::OnChangeWhat() {
	LOG0(5, "CSearchDlg::OnChangeWhat()");

	UpdateControls();	
}

void CSearchDlg::ResizeControls() {
	LOG0(3, "CSearchDlg::ResizeControls()");

	RelayoutDialog(AfxGetInstanceHandle(), GetSafeHwnd(), InWideMode() ?
		MAKEINTRESOURCE(IDD_WIDE) :
		MAKEINTRESOURCE(IDD));
}

void CSearchDlg::OnOK() {
	LOG0(1, "CSearchDlg::OnOK()");

	if (m_pProgress == NULL) {
		CString what;
		m_ctlWhat.GetWindowText(what);

		EnterCriticalSection(&CSSearcher);
		Searcher = new CSyndic8ComSearcher(what, Config.CacheLocation);
		LeaveCriticalSection(&CSSearcher);

		m_pProgress = new CSearchProgressDlg(Searcher, this);
		m_pProgress->OpenDialog(IDS_SEARCHING, this);
		CloseHandle(CreateThread(NULL, 0, SearchThreadProc, this, 0, NULL));
	}
	else
		m_pProgress->SetActiveWindow();

/*
//	SearchResults->AddTail(new CSearchResultItem(
//		_T("Tech Tips Today - Gadget and Gizmo News"), _T("http://www.techtipstoday.com/feed/atom/")));
	CSearchResultItem *sri = new CSearchResultItem(
		_T("GADGET"), _T("http://www.netcityonline.com/commy/prodotti_lista_public_xml.asp?CM_CATEGORIA_ID=143&amp;FEED_ID=1&amp;RSS_DISP=1"));
	sri->Description = _T("tohle je silene dlouhy Description, ktery by se mel zlomit na vic radek, abych moh otestovat, lamani v editu, ktery jsem pouzil mito staticu, protoze static je nejkay vadny a nefunguje moc dobre, previt jeden");
	SearchResults->AddTail(sri);

	sri = new CSearchResultItem(
		_T("GADGET2"), _T("http://www.netcityonline.com/commy/prodotti_lista_public_xml.asp?CM_CATEGORIA_ID=143&amp;FEED_ID=1&amp;RSS_DISP=1"));
	sri->Description = _T("Descr");
	SearchResults->AddTail(sri);

	SearchResults->AddTail(new CSearchResultItem(
		_T("ecoblog"), _T("http://www.ecoblog.it/categoria/gadget/rss2.xml")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Computer Alchemy"), _T("http://www.computeralchemy.com/feed/atom/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Design News - Gadget Freak"), _T("http://www.designnews.com/DN_GadgetFreak.xml")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("All Gadget"), _T("http://www.allgadget.com/feed/rss2/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("The Tech Gadget Mag"), _T("http://aboi.uplies.co.uk/wp-rss2.php")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("GadgetRoad - Daily Gadget News"), _T("http://www.gadgetroad.com/feed/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("The Tech Gadget Mag"), _T("http://bob.uplies.co.uk/wp-atom.php")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Gadget Universe Deals at DealShake.com"), _T("http://www.dealshake.com/storedealrss.php?store_id=389")));

	SearchResults->AddTail(new CSearchResultItem(
		_T("GADGET"), _T("http://www.netcityonline.com/commy/prodotti_lista_public_xml.asp?CM_CATEGORIA_ID=143&amp;FEED_ID=1&amp;RSS_DISP=1")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("ecoblog"), _T("http://www.ecoblog.it/categoria/gadget/rss2.xml")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Computer Alchemy"), _T("http://www.computeralchemy.com/feed/atom/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Design News - Gadget Freak"), _T("http://www.designnews.com/DN_GadgetFreak.xml")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("All Gadget"), _T("http://www.allgadget.com/feed/rss2/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("The Tech Gadget Mag"), _T("http://aboi.uplies.co.uk/wp-rss2.php")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("GadgetRoad - Daily Gadget News"), _T("http://www.gadgetroad.com/feed/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("The Tech Gadget Mag"), _T("http://bob.uplies.co.uk/wp-atom.php")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("GADGET"), _T("http://www.netcityonline.com/commy/prodotti_lista_public_xml.asp?CM_CATEGORIA_ID=143&amp;FEED_ID=1&amp;RSS_DISP=1")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("ecoblog"), _T("http://www.ecoblog.it/categoria/gadget/rss2.xml")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Computer Alchemy"), _T("http://www.computeralchemy.com/feed/atom/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("Design News - Gadget Freak"), _T("http://www.designnews.com/DN_GadgetFreak.xml")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("All Gadget"), _T("http://www.allgadget.com/feed/rss2/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("The Tech Gadget Mag"), _T("http://aboi.uplies.co.uk/wp-rss2.php")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("GadgetRoad - Daily Gadget News"), _T("http://www.gadgetroad.com/feed/")));
	SearchResults->AddTail(new CSearchResultItem(
		_T("The Tech Gadget Mag"), _T("http://bob.uplies.co.uk/wp-atom.php")));

	CDialog::OnOK();
*/
}

DWORD CSearchDlg::SearchThread() {
	LOG0(1, "CSearchDlg::SearchThread() - start");

	// empty previous search results
	while (!SearchResults->IsEmpty())
		delete SearchResults->RemoveHead();

	BOOL bOK = TRUE;
	BOOL disconnect;
	if (CheckConnection(Config.AutoConnect, disconnect)) {
		if (Searcher->Search(Config.SearchLimit, *SearchResults)) {
			// ok
		}
		else {
			bOK = FALSE;
		}

		if (disconnect)
			Connection.HangupConnection();
	}
	else {
		// error: can not connect to the Internet
		bOK = FALSE;
	}

	// done
	if (bOK) {
		SetForegroundWindow();
		CDialog::OnOK();
	}
	else {
		if (Searcher->IsTerminated()) {
			// terminated
		}
		else {
			SetForegroundWindow();
			m_pProgress->CloseDialog();
		}
	}
	m_pProgress = NULL;

	EnterCriticalSection(&CSSearcher);
	delete Searcher;
	Searcher = NULL;
	LeaveCriticalSection(&CSSearcher);

	LOG0(1, "CSearchDlg::AddThread() - end");

	return 0;
}
