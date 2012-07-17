// ChatServer.h : Declaration of the CChatServer

#pragma once
#include "resource.h"       // main symbols



#include "ComChat_i.h"



using namespace ATL;


#include "utils.h"

#include "WorkerPool.h"


// CChatServer

class ATL_NO_VTABLE CChatServer :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CChatServer, &CLSID_ChatServer>,
	public IChatServer
{
public:
	CChatServer()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CHATSERVER)

DECLARE_NOT_AGGREGATABLE(CChatServer)

BEGIN_COM_MAP(CChatServer)
	COM_INTERFACE_ENTRY(IChatServer)
END_COM_MAP()

	// Specify singleton class factory
	DECLARE_CLASSFACTORY_SINGLETON(CChatServer)

	DECLARE_PROTECT_FINAL_CONSTRUCT()



	HRESULT FinalConstruct()
	{
		HRESULT hr = m_worker.Initialize();

		if(FAILED(hr)) return hr;

		return S_OK;
	}

	void FinalRelease()
	{
		//err(L"Server dying");
		m_worker.Shutdown();
	}

public:
	void onUnregister(BSTR name);

	HRESULT onSendMessage(BSTR name, BSTR dest, BSTR msg);

	STDMETHOD(registerClient)(BSTR name, IChatClient* pClient, IChatServerPort** ppPort);

private:

	typedef CComPtr<IChatClient>	ClientPtr;

	CAtlMap<CComBSTR, ClientPtr>	m_clients;


	CThreadPool<Worker>				m_worker;

};

OBJECT_ENTRY_AUTO(__uuidof(ChatServer), CChatServer)