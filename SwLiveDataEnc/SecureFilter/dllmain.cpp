// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "UDPSocket.h"

CUDPSocket m_socket;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH: 
		//network
		InitializeSocket();
		if(!m_socket.Create(CLI_NODE_PORT))
		{
			//WriteLog("create socket failed");
		}
		//sw====end
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

