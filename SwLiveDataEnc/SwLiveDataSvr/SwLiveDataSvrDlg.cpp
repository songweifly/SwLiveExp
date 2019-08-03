// T01Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "SwLiveDataSvrApp.h"
#include "SwLiveDataSvrDlg.h"



#include <Iads.h>  
#include <Adshlp.h>  
#include <activeds.h>  
#include <adserr.h>  
#include <comutil.h>  
#include <Lmaccess.h>
#include <Lm.h>
#pragma comment(lib,"Activeds.lib")  
#pragma comment(lib,"adsiid.lib")  
//#pragma comment(lib,"comsuppw.lib")  
#pragma comment(lib,"netapi32.lib")

extern CSwLiveDataSvrApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CT01Dlg dialog

CSwLiveDataSvrDlg::CSwLiveDataSvrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSwLiveDataSvrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CT01Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	
}

void CSwLiveDataSvrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CT01Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSwLiveDataSvrDlg, CDialog)
	//{{AFX_MSG_MAP(CT01Dlg)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CT01Dlg message handlers

/*  读取竖线分割的第nIndex个值，nIndex的值从零开始*/
BOOL CSwLiveDataSvrDlg::GetXField(const CString& strIn, int nIndex, CString& strOut)
{
	if(strIn.IsEmpty())
		return FALSE;
	
	int nCounter = 0;
	int nId;
	CString strLine, strEnd;
	strLine = strIn;
	while(nCounter<nIndex)
	{
		nId = strLine.Find("|");
		if(nId == -1)
			return FALSE;
		strEnd = strLine.Mid(nId+1);
		if(strEnd.IsEmpty())
			return FALSE;

		strLine = strEnd;	
		nCounter ++;
	}

	nId = strLine.Find("|");
	if(nId == -1)
	{
		strOut = strLine;
	}
	else
	{
		strOut = strLine.Left(nId);
	}

	strOut.TrimRight();
	strOut.TrimRight();

	return TRUE;
}

void CSwLiveDataSvrDlg::WriteLog(const char *fmt,...)
{
	CTime mTimeNew;
	mTimeNew = CTime::GetCurrentTime(); 
	int nYear = mTimeNew.GetYear();
	int nMonth = mTimeNew.GetMonth();
	int nDay = mTimeNew.GetDay();
	int nHour = mTimeNew.GetHour();
	int nMin = mTimeNew.GetMinute();
	int nSec = mTimeNew.GetSecond();

	va_list args;
	char modname[200];
	char temp[5000]={0};
	HANDLE hFile;
	GetModuleFileNameA(NULL, modname, sizeof(modname));
	if((hFile =CreateFileA("SwLiveDataSvr.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0) return;
	int pos = SetFilePointer(hFile, 0, NULL, FILE_END);
	if (pos != -1) // Test for failure
	{
		wsprintfA(temp, "%d-%02d-%02d %02d:%02d:%02d|%s: [", nYear,nMonth,nDay,nHour,nMin,nSec, modname);
		DWORD dw;
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
		va_start(args,fmt);
		vsprintf(temp, fmt, args);
		va_end(args);
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
		wsprintfA(temp, "]\r\n");
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	}

	CloseHandle(hFile);
}



BOOL CSwLiveDataSvrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here  

	WriteLog("system initialize!!!!");
	
	//network
	InitializeSocket();
	if(!this->m_socket.Create(SVR_NODE_PORT))
	{
		WriteLog("create socket failed");
	}
	
	DWORD id;
	this->m_hRecvThread = ::CreateThread(NULL,0,RecvThreadFunc,NULL,0,&id);
	if(this->m_hRecvThread)
	{
		CloseHandle(m_hRecvThread);
	}
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


 
DWORD WINAPI CSwLiveDataSvrDlg::RecvThreadFunc(LPVOID lpParam)
{
	CSwLiveDataSvrApp* pApp = (CSwLiveDataSvrApp*)AfxGetApp();
	if(pApp == NULL)
		return -1;

	CSwLiveDataSvrDlg *pDlg = (CSwLiveDataSvrDlg *)pApp->GetMainWnd();   //::AfxGetMainWnd(); 
	if(pDlg == NULL)
		return -1;

	WriteLog("work thread start!!!!");

	int len=0;
	char buf[MAX_PACKET_SIZE];
	memset(buf,0,MAX_PACKET_SIZE);
	sockaddr_in sock;
	while(TRUE)
	{		
		ZeroMemory(buf, MAX_PACKET_SIZE);
		len = pDlg->m_socket.RecvFrom((char *)buf, MAX_PACKET_SIZE, (SOCKADDR *)&sock);
		if(len > 0)
		{
			CStringArray strItemArray;		
			int i = 1 ;
			CString strItem;
			CString strData = buf;

			GetXField(strData, i, strItem);
			if(strItem.Compare("111111") == 0)
			{
				while(GetXField(strData, i++, strItem))
				{
					strItemArray.Add(strItem);
				}

				if(i == 5)
				{
					CString strModSQL ;
					strModSQL.Format("update CloudPasswd  set 密码='%s' where 账号='%s'", strItemArray.GetAt(2), strItemArray.GetAt(1));					
					pDlg->WriteLog(strModSQL);
					CString ErrMsg;
					if(!CSwLiveDataSvrApp::m_pDataBase->Execute((LPCTSTR)strModSQL, ErrMsg))
					{
						pDlg->WriteLog("update CloudPasswd error");
						pDlg->WriteLog(ErrMsg);
					}
				}
			}
			else
			{
				while(GetXField(strData, i++, strItem))
				{
					strItemArray.Add(strItem);
				}
				if(i == 7) 
				{ 
					CString strAddSQL ;
					strAddSQL.Format("INSERT INTO CloudDataOut (姓名,日期,类型,拷出文件,拷入目录) VALUES ('%s','%s','%s','%s','%s')", 
						strItemArray.GetAt(0), strItemArray.GetAt(1), strItemArray.GetAt(2), strItemArray.GetAt(3), strItemArray.GetAt(4));

					strAddSQL.Replace("\\", "\\\\");
					pDlg->WriteLog(strAddSQL);
                    CString ErrMsg;
					if(!CSwLiveDataSvrApp::m_pDataBase->Execute((LPCTSTR)strAddSQL, ErrMsg))
					{
						pDlg->WriteLog("INSERT INTO CloudDataOut error");						
						pDlg->WriteLog(ErrMsg);
					}
				}
			}
	
		}
	}

	WriteLog("work thread end!!!!");

	return 0;
}
