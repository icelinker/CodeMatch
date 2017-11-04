// Match.h : main header file for the MATCH application
//

#if !defined(AFX_MATCH_H__D8B34AD1_8FFB_4D46_82DE_D0D15B84FA45__INCLUDED_)
#define AFX_MATCH_H__D8B34AD1_8FFB_4D46_82DE_D0D15B84FA45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMatchApp:
// See Match.cpp for the implementation of this class
//

class CMatchApp : public CWinApp
{
public:
	CMatchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatchApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMatchApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATCH_H__D8B34AD1_8FFB_4D46_82DE_D0D15B84FA45__INCLUDED_)
