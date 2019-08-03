// T01.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SwLiveDataSvrApp.h"
#include "SwLiveDataSvrDlg.h"

extern BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
extern void __cdecl main(int argc, char **argv);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SwLiveDataSvr
CADODatabase* CSwLiveDataSvrApp::m_pDataBase = NULL; 
BEGIN_MESSAGE_MAP(CSwLiveDataSvrApp, CWinApp)
	//{{AFX_MSG_MAP(SwLiveDataSvr)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SwLiveDataSvr construction
CSwLiveDataSvrApp::CSwLiveDataSvrApp()
{ 

}

CSwLiveDataSvrApp::~CSwLiveDataSvrApp()
{
    CloseDB();

}
/////////////////////////////////////////////////////////////////////////////
// The one and only SwLiveDataSvr object

CSwLiveDataSvrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SwLiveDataSvr initialization

BOOL CSwLiveDataSvrApp::InitInstance()
{
	// 设置当前目录
	TCHAR szPath[MAX_PATH];
	GetModuleFileName( NULL, szPath, MAX_PATH );
	TCHAR drive[MAX_PATH],dir[MAX_PATH],fname[MAX_PATH],ext[MAX_PATH];
	_tsplitpath( szPath,drive,dir,fname,ext );
	strcpy( szPath, drive );
	strcat( szPath, dir );
	SetCurrentDirectory( szPath );

	//===============================
	if(!OpenDB())
	{
		AfxMessageBox("open database failed!");
	}
	//========================

	main( __argc, __argv );

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//
//  FUNCTION: ServiceStart
//
//  PURPOSE: Actual code of the service
//           that does the work.
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
VOID ServiceStart (DWORD dwArgc, LPTSTR *lpszArgv)
{
	ReportStatusToSCMgr( SERVICE_RUNNING, NO_ERROR, 0 );

	CSwLiveDataSvrDlg dlg;
	theApp.m_pMainWnd = &dlg;
	theApp.m_nCmdShow=SW_HIDE;
	
	int nResponse = dlg.DoModal();
	//dlg.ShowWindow(SW_HIDE);
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
		dlg.ShowWindow(SW_HIDE);
	} 
}


//
//  FUNCTION: ServiceStop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a ServiceStop procedure is going to
//    take longer than 3 seconds to execute,
//    it should spawn a thread to execute the
//    stop code, and return.  Otherwise, the
//    ServiceControlManager will believe that
//    the service has stopped responding.
//    
VOID ServiceStop()
{
	((CSwLiveDataSvrDlg*)theApp.m_pMainWnd)->EndDialog( 0L );
}


BOOL CSwLiveDataSvrApp::OpenDB()
{
	CString strConn; 
	if(m_pDataBase == NULL)
	{
		m_pDataBase = new CADODatabase;
		if(m_pDataBase == NULL)
			return FALSE;

		//strConn = "Provider = Microsoft.Jet.OLEDB.4.0; Data Source = SwLiveDataOutDB.mdb";
		//if(!m_pDataBase->Open(strConn))
		//	return FALSE;

		if(!m_pDataBase->Open("Driver=MySQL ODBC 8.0 Unicode Driver;Server= x.x.x.x;Database=sid", "root", "root"))
			return FALSE;
	}
	
	return TRUE;
}


BOOL CSwLiveDataSvrApp::CloseDB()
{
	if(m_pDataBase != NULL)
	{
		m_pDataBase->Close();
		delete m_pDataBase ;
		m_pDataBase = NULL;
	}

	return TRUE;
}