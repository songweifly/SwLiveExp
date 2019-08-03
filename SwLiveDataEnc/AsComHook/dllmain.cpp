// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "mhook.h"
#include "CreationHook.h"
#include "UDPSocket.h"
#include "RegisterKey.h"

//////////////////////////////////////////////////////////////////////////
CUDPSocket m_socket;
BOOL DllInit(HMODULE module)
{
	//================
	InitializeSocket(); 

	UINT nPort;
	DWORD dwPort;
	CRegisterKey RegKey;
	if(RegKey.RegOpen(HKEY_CURRENT_USER, L"SOFTWARE\\HOOKDLL\\CONFIG") == ERROR_SUCCESS)
	{
		RegKey.RegRead(L"PORT", dwPort);
		//规范化端口范围
		if(dwPort < CLI_NODE_MIN_PORT || dwPort>CLI_NODE_MAX_PORT)
			nPort = CLI_NODE_MIN_PORT; 
		else
			nPort = dwPort;

		while(1)
		{
			if(m_socket.Create(nPort) || nPort>CLI_NODE_MAX_PORT) 
				break;

			nPort++;
			Sleep(1000);
		}

		RegKey.RegWrite(L"PORT", nPort+1);
		RegKey.RegClose();   
	}
	//=======================

    InstallHooks();
    return TRUE;
}

BOOL DllFree()
{

    UninstallHooks();
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    try
    {
        switch (reason)
        {
        case DLL_PROCESS_ATTACH:
            return DllInit(module);

        case DLL_PROCESS_DETACH:
            return DllFree();
        }

        return TRUE;
    }
    catch (const std::exception& )
    {
        return FALSE;
    }
}

