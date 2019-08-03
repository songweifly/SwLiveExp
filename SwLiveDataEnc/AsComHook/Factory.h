#pragma once

class ATL_NO_VTABLE CSampleObjectProxyFactory :
    public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>,
    public IClassFactory
{
public:
    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CSampleObjectProxyFactory)
        COM_INTERFACE_ENTRY(IClassFactory)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
    HRESULT FinalConstruct(){return S_OK;}
    void FinalRelease(){}

    static HRESULT CreateFactory(IClassFactory* originalFactory, void** proxyFactory);

public:
    STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);

    STDMETHODIMP LockServer(BOOL fLock);

private:
    ATL::CComPtr<IClassFactory> m_OriginalFactory;
};