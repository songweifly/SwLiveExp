// SwLiveDataSvrDlg.h : header file
//

#if !defined(AFX_SwLiveDataSvrDLG_H__811145B9_28F4_42B1_895F_A1C622839911__INCLUDED_)
#define AFX_SwLiveDataSvrDLG_H__811145B9_28F4_42B1_895F_A1C622839911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSwLiveDataSvrDlg dialog
#include "UDPSocket.h"
class CSwLiveDataSvrDlg : public CDialog
{
public:
	CUDPSocket m_socket;
	HANDLE  m_hRecvThread;
	static DWORD WINAPI RecvThreadFunc(LPVOID lpParam);	
	/*  读取竖线分割的第nIndex个值，nIndex的值从零开始*/
    static BOOL GetXField(const CString& strIn, int nIndex, CString& strOut); 
	static void WriteLog(const char *fmt,...);

// Construction
public:
	CSwLiveDataSvrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSwLiveDataSvrDlg)
	enum { IDD = IDD_SwLiveDataSvr_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwLiveDataSvrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSwLiveDataSvrDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SwLiveDataSvrDLG_H__811145B9_28F4_42B1_895F_A1C622839911__INCLUDED_)
