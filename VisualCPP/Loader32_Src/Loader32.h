// Loader32.h : main header file for the SIMPLESTRING application
//

#if !defined(AFX_LOADER32_H__A8197645_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_)
#define AFX_LOADER32_H__A8197645_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLoader32App:
// See SimpleString.cpp for the implementation of this class
//

class CLoader32App : public CWinApp
{
public:
	CLoader32App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoader32App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLoader32App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADER32_H__A8197645_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_)
