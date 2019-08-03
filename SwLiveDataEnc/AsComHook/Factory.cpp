#include "stdafx.h"
#include "Factory.h"
#include "VtableHooks.h"

STDMETHODIMP CSampleObjectProxyFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    HRESULT hr = m_OriginalFactory->CreateInstance(pUnkOuter, riid, ppvObject);
    if (FAILED(hr))
        return hr;

	WriteLog("Factory.cpp");
    return InstallComInterfaceHooks((IUnknown*)*ppvObject);
}

STDMETHODIMP CSampleObjectProxyFactory::LockServer(BOOL fLock)
{
    return m_OriginalFactory->LockServer(fLock);
}

HRESULT CSampleObjectProxyFactory::CreateFactory(IClassFactory* originalFactory, void** proxyFactory)
{
    try
    {
        ATL::CComObjectNoLock<CSampleObjectProxyFactory>* self = new ATL::CComObjectNoLock<CSampleObjectProxyFactory>;

        self->SetVoid(NULL);

        self->InternalFinalConstructAddRef();
        HRESULT hr = self->_AtlInitialConstruct();
        if (SUCCEEDED(hr))
            hr = self->FinalConstruct();
        if (SUCCEEDED(hr))
            hr = self->_AtlFinalConstruct();
        self->InternalFinalConstructRelease();

        self->m_OriginalFactory = originalFactory;

        if (SUCCEEDED(hr))
            hr = self->QueryInterface(IID_IClassFactory, proxyFactory);

        if (hr != S_OK)
            delete self;

        return hr;
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
}