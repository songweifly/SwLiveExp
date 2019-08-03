// MyInsecter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
//#include "afxdialogex.h"
////////////////////////////////////////
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>         
#include <TlHelp32.h> 
#include <StrSafe.h>
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"wmainCRTStartup\"")
//////////////////////////////////////
//是否需要写Log日志文件的开关，不需要就将他mark掉
#define  WRITELOG
//LOG日志文件的路径
#define  MYDLLLOGPATH  "c:\\windows\\MyInsecter.log"
void WriteLog(const char *fmt,...)
{
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
//#endif
}
 
DWORD GetPidByProcessName(LPCTSTR strName)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 lppe;
	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if(hSnapshot == NULL)
		return 0;

	lppe.dwSize = sizeof(lppe);
	if(!::Process32First(hSnapshot, &lppe))
		return 0;

	do
	{
		if(wcscmp(lppe.szExeFile, strName) == 0)
		{
			return lppe.th32ProcessID;
		}
	}
	while(::Process32Next(hSnapshot,&lppe));

	return 0;
	
}



#ifdef UNICODE
   #define InjectLib InjectLibW
   #define EjectLib  EjectLibW
#else
   #define InjectLib InjectLibA
   #define EjectLib  EjectLibA
#endif   // !UNICODE


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {

   BOOL bOk = FALSE; // Assume that the function fails
   HANDLE hProcess = NULL, hThread = NULL;
   PWSTR pszLibFileRemote = NULL;


   //// Get a handle for the target process.
   //hProcess = OpenProcess(
	  // PROCESS_QUERY_INFORMATION |   // Required by Alpha
	  // PROCESS_CREATE_THREAD     |   // For CreateRemoteThread
	  // PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
	  // PROCESS_VM_WRITE,             // For WriteProcessMemory
	  // FALSE, dwProcessId);
   //if (hProcess == NULL) 
	  // return FALSE;

      // Get a handle for the target process.
   hProcess = OpenProcess(PROCESS_ALL_ACCESS,             // For WriteProcessMemory
	   FALSE, dwProcessId);
   if (hProcess == NULL) 
	   return FALSE;

   // Calculate the number of bytes needed for the DLL's pathname
   int cch = 1 + lstrlenW(pszLibFile);
   int cb  = cch * sizeof(wchar_t);

   // Allocate space in the remote process for the pathname
   pszLibFileRemote = (PWSTR) 
	   VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
   if (pszLibFileRemote == NULL) 
	   return FALSE;

   // Copy the DLL's pathname to the remote process' address space
   if (!WriteProcessMemory(hProcess, pszLibFileRemote, 
	   (PVOID) pszLibFile, cb, NULL)) 
	   return FALSE;

   // Get the real address of LoadLibraryW in Kernel32.dll
   PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
	   GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
   if (pfnThreadRtn == NULL) 
	   return FALSE;

   // Create a remote thread that calls LoadLibraryW(DLLPathname)
   hThread = CreateRemoteThread(hProcess, NULL, 0, 
	   pfnThreadRtn, pszLibFileRemote, 0, NULL);
   if (hThread == NULL) 
	   return FALSE;
   // Wait for the remote thread to terminate
   WaitForSingleObject(hThread, INFINITE);

   bOk = TRUE; // Everything executed successfully



   // Free the remote memory that contained the DLL's pathname
   if (pszLibFileRemote != NULL) 
	   VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

   if (hThread  != NULL) 
	   CloseHandle(hThread);

   if (hProcess != NULL) 
	   CloseHandle(hProcess);


   return(bOk);
}


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI InjectLibA(DWORD dwProcessId, PCSTR pszLibFile) {

   // Allocate a (stack) buffer for the Unicode version of the pathname
   SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
   PWSTR pszLibFileW = (PWSTR) 
      _alloca(cchSize * sizeof(wchar_t));

   // Convert the ANSI pathname to its Unicode equivalent
   StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);

   // Call the Unicode version of the function to actually do the work.
   return(InjectLibW(dwProcessId, pszLibFileW));
}


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI EjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {

   BOOL bOk = FALSE; // Assume that the function fails
   HANDLE hthSnapshot = NULL;
   HANDLE hProcess = NULL, hThread = NULL;


      // Grab a new snapshot of the process
      hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
      if (hthSnapshot == INVALID_HANDLE_VALUE) 
	   return FALSE;

      // Get the HMODULE of the desired library
      MODULEENTRY32W me = { sizeof(me) };
      BOOL bFound = FALSE;
      BOOL bMoreMods = Module32FirstW(hthSnapshot, &me);
      for (; bMoreMods; bMoreMods = Module32NextW(hthSnapshot, &me)) {
         PCWSTR pFilename = wcsrchr(pszLibFile, TEXT('\\'));
		 if(pFilename != NULL)
		 {			 
			 pFilename = pFilename + 1;
			 bFound = (_wcsicmp(me.szModule,  pFilename) == 0) || 
				 (_wcsicmp(me.szExePath, pFilename) == 0);
		 }
		 else
		 {
			 bFound = (_wcsicmp(me.szModule,  pszLibFile) == 0) || 
				 (_wcsicmp(me.szExePath, pszLibFile) == 0);
		 }
         if (bFound) break;
      }
      if (!bFound) 
	   return FALSE;

      //// Get a handle for the target process.
      //hProcess = OpenProcess(
      //   PROCESS_QUERY_INFORMATION |   
      //   PROCESS_CREATE_THREAD     | 
      //   PROCESS_VM_OPERATION,  // For CreateRemoteThread
      //   FALSE, dwProcessId);
      //if (hProcess == NULL) 
	     //return FALSE;

      // Get a handle for the target process.
      hProcess = OpenProcess(PROCESS_ALL_ACCESS,  // For CreateRemoteThread
         FALSE, dwProcessId);
      if (hProcess == NULL) 
	     return FALSE;

      // Get the real address of FreeLibrary in Kernel32.dll
      PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
         GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "FreeLibrary");
      if (pfnThreadRtn == NULL) 
	      return FALSE;

      // Create a remote thread that calls FreeLibrary()
      hThread = CreateRemoteThread(hProcess, NULL, 0, 
         pfnThreadRtn, me.modBaseAddr, 0, NULL);
      if (hThread == NULL) 
	      return FALSE;

      // Wait for the remote thread to terminate
      WaitForSingleObject(hThread, INFINITE);

      bOk = TRUE; // Everything executed successfully


      if (hthSnapshot != NULL) 
         CloseHandle(hthSnapshot);

      if (hThread     != NULL) 
         CloseHandle(hThread);

      if (hProcess    != NULL) 
         CloseHandle(hProcess);


   return(bOk);
}


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI EjectLibA(DWORD dwProcessId, PCSTR pszLibFile) {

   // Allocate a (stack) buffer for the Unicode version of the pathname
   SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
   PWSTR pszLibFileW = (PWSTR) 
      _alloca(cchSize * sizeof(wchar_t));

   // Convert the ANSI pathname to its Unicode equivalent
      StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);

   // Call the Unicode version of the function to actually do the work.
   return(EjectLibW(dwProcessId, pszLibFileW));
}

 
int _tmain(int argc, _TCHAR* argv[])
{
	//Sleep(90000);
	DWORD id = GetPidByProcessName(L"explorer.exe");
	if(id != 0)
	{
		BOOL bRet = InjectLib(id, L"C:\\Windows\\AsComHook.dll");
		char sTemp[256];
		memset(sTemp, 0x00, sizeof(sTemp));
		sprintf(sTemp, "PID==[%d]\nbRet==[%d]", id, bRet);
		WriteLog(sTemp);
	}

	id = GetPidByProcessName(L"FeiQ.exe");	
	if(id != 0)
	{
		BOOL bRet = InjectLib(id, L"C:\\Windows\\AsComHook.dll");
		char sTemp[256];
		memset(sTemp, 0x00, sizeof(sTemp));
		sprintf(sTemp, "PID==[%d]\nbRet==[%d]", id, bRet);
		WriteLog(sTemp);
	}

	id = GetPidByProcessName(L"RTX.exe");	
	if(id != 0)
	{
		BOOL bRet = InjectLib(id, L"C:\\Windows\\AsComHook.dll");
		char sTemp[256];
		memset(sTemp, 0x00, sizeof(sTemp));
		sprintf(sTemp, "PID==[%d]\nbRet==[%d]", id, bRet);
		WriteLog(sTemp);
	}


	return 0;
}

