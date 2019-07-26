
// usbdemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "usbdemo.h"
#include "usbdemoDlg.h"
#include "afxdialogex.h"
#include "WinIoCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CusbdemoDlg 对话框




CusbdemoDlg::CusbdemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CusbdemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CusbdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CusbdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CusbdemoDlg 消息处理程序

BOOL CusbdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	UsbCtrl();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CusbdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CusbdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CusbdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CusbdemoDlg::UsbCtrl()
{
	HANDLE hDevice;               // handle to the drive to be examined 
	BOOL bResult;                 // results flag
	DWORD junk;                   // discard results
	DWORD dwError;
	// Open the volume
	hDevice = CreateFile(L"\\\\.\\H:", // drive to open
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
		NULL,    // default security attributes
		OPEN_EXISTING,  // disposition
		0,       // file attributes
		NULL);   // don't copy any file's attributes

	if (hDevice == INVALID_HANDLE_VALUE) // can't open the drive
	{
		dwError = GetLastError();
		AfxMessageBox(L"Fail to create file!");
		return ;
	}

	//Dismount the volume 
	bResult = DeviceIoControl(
		hDevice,  // handle to volume
		FSCTL_DISMOUNT_VOLUME,       // dwIoControlCode
		NULL,                        // lpInBuffer
		0,                           // nInBufferSize
		NULL,                        // lpOutBuffer
		0,                           // nOutBufferSize
		&junk, // discard count of bytes returned
		(LPOVERLAPPED) NULL);  // synchronous I/O
	if (!bResult) // IOCTL failed
	{
		dwError = GetLastError();
		AfxMessageBox(L"Fail to dismount the volume!");
	}

	// Close the volume handle 
	bool bRet = CloseHandle(hDevice); 
	if (!bRet)
	{
		dwError = GetLastError();
		AfxMessageBox(L"Fail to close handle!");
	}
}

