#if !defined(_OPTCHANNELSPG_H_INCLUDED_)
#define _OPTCHANNELSPG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptModePg.h : header file
//

#include "../prssr/ctrls/CeListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COptChannelsPg dialog

class COptChannelsPg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptChannelsPg)

// Construction
public:
	COptChannelsPg();
	~COptChannelsPg();

// Dialog Data
	//{{AFX_DATA(COptChannelsPg)
	enum { IDD = IDD_CFG_CHANNELS };
	CCeListCtrl m_ctlChannels;
	CButton m_ctlShowNewChannels;
	BOOL m_bShowNewChannels;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptChannelsPg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(COptChannelsPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTMODEPG_H__A08866BC_2EE9_4624_A4AE_49408C7CA10B__INCLUDED_)
