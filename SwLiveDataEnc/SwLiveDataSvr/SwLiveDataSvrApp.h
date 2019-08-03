// T01.h : main header file for the T01 application
//

#if !defined(AFX_T01_H__B94D1CAE_CF3D_4EB4_937A_0CFE4D54F033__INCLUDED_)
#define AFX_T01_H__B94D1CAE_CF3D_4EB4_937A_0CFE4D54F033__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include ".\Ado.h"
/////////////////////////////////////////////////////////////////////////////
// CT01App:
// See T01.cpp for the implementation of this class
//

class CSwLiveDataSvrApp : public CWinApp
{
public:
	CSwLiveDataSvrApp();
	~CSwLiveDataSvrApp();

	CString SZAPPNAME;
	CString SZSERVICENAME;
	CString SZSERVICEDISPLAYNAME;
	CString SZDEPENDENCIES;

	//
	static CADODatabase* m_pDataBase;
	static BOOL OpenDB();
	static BOOL CloseDB();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CT01App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CT01App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_T01_H__B94D1CAE_CF3D_4EB4_937A_0CFE4D54F033__INCLUDED_)
