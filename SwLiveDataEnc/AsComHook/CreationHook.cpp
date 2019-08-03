#include "StdAfx.h"
#include "CreationHook.h"

#include "Factory.h"
#include "VtableHooks.h"
#include "mhook.h"

//////////////////////////////////////////////////////////////////////////

typedef HRESULT (WINAPI *CoCreateInstance_T)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef HRESULT (WINAPI *CoGetClassObject_T)(REFCLSID rclsid, DWORD dwClsContext, COSERVERINFO *pServerInfo, REFIID riid, LPVOID *ppv);

namespace Original
{
    CoCreateInstance_T  CoCreateInstance    = NULL;
    CoGetClassObject_T  CoGetClassObject    = NULL;
}

namespace Hook
{
    HRESULT WINAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv);
    HRESULT WINAPI CoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, COSERVERINFO *pServerInfo, REFIID riid, LPVOID *ppv);
};

//////////////////////////////////////////////////////////////////////////

struct FunctionInfo
{
    char*  FunctionModule;
    char*  FunctionName;
    void** OriginalFunction;
    void*  HookFunction;
};

FunctionInfo g_Functions[] = 
{
    {"ole32.dll", "CoCreateInstance", (void**)&Original::CoCreateInstance, (void*)Hook::CoCreateInstance},
    {"ole32.dll", "CoGetClassObject", (void**)&Original::CoGetClassObject, (void*)Hook::CoGetClassObject}
};

const size_t g_FunctionsCount = sizeof(g_Functions)/sizeof(FunctionInfo);

//////////////////////////////////////////////////////////////////////////
static const IID CLSID_FileOperation = {0x3ad05575, 0x8857, 0x4850, {0x92, 0x77, 0x11, 0xb8, 0x5b, 0xdb, 0x8e, 0x09}};
static const IID CLSID_FileSaveDialog ={0xC0B4E2F3, 0xBA21, 0x4773, {0x8D, 0xBA, 0x33, 0x5E, 0xC9, 0x46, 0xEB, 0x8B}};
static const IID CLSID_FileOpenDialog ={0xDC1C5A9C, 0xE88A, 0x4dde, {0xA5, 0xA1, 0x60, 0xF8, 0x2A, 0x20, 0xAE, 0xF7}};
HRESULT WINAPI Hook::CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv)
{
	//CLSID_FileOperation
    if (rclsid == CLSID_FileOperation)
    {        
        if (pUnkOuter)
            return CLASS_E_NOAGGREGATION;

        HRESULT hr = Original::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
        if (FAILED(hr))
            return hr;

        return InstallComInterfaceHooks((IUnknown*)*ppv);
    }

	//CLSID_FileSaveDialog
    if (rclsid == CLSID_FileSaveDialog)
    {       
        if (pUnkOuter)
            return CLASS_E_NOAGGREGATION;

        HRESULT hr = Original::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
        if (FAILED(hr))
            return hr;

        return InstallComInterfaceHooksExt((IUnknown*)*ppv);
    }

    return Original::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

HRESULT WINAPI Hook::CoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, COSERVERINFO *pServerInfo, REFIID riid, LPVOID *ppv)
{
    if (riid == IID_IClassFactory)
    {
        ATL::CComPtr<IClassFactory> originalFactory;
        HRESULT hr = Original::CoGetClassObject(rclsid, dwClsContext, pServerInfo, riid, (void**)&originalFactory);
        if (FAILED(hr))
            return hr;

        return CSampleObjectProxyFactory::CreateFactory(originalFactory, ppv);
    }

    return Original::CoGetClassObject(rclsid, dwClsContext, pServerInfo, riid, ppv);
}
//////////////////////////////////////////////////////////////////////////

void InstallHooks()
{
    for(size_t i = 0; i < g_FunctionsCount; ++i)
    {
        if(!GetModuleHandleA(g_Functions[i].FunctionModule))
            throw std::runtime_error("Cannot find the module ");
		

        *g_Functions[i].OriginalFunction = GetProcAddress(GetModuleHandleA(g_Functions[i].FunctionModule), g_Functions[i].FunctionName);

        if(*g_Functions[i].OriginalFunction == NULL)
            throw std::runtime_error("Cannot find the function ");

        if(!Mhook_SetHook(g_Functions[i].OriginalFunction, g_Functions[i].HookFunction))
            throw std::runtime_error("Cannot set hook on the function ");
    }
}

void UninstallHooks()
{
    BOOL failed = false;

    for(size_t i = 0; i < g_FunctionsCount; ++i)
    {
        if (g_Functions[i].OriginalFunction)
            failed = failed || !Mhook_Unhook(g_Functions[i].OriginalFunction);
    }

    if (failed)
        throw std::runtime_error("UninstallHooks was failed to remove one or more hooks");
}