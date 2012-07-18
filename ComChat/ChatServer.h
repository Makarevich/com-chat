// ChatServer.h : Declaration of the CChatServer

#pragma once
#include "resource.h"       // main symbols



#include "ComChat_i.h"



using namespace ATL;


#include "utils.h"

#include "WorkerPool.h"

//
// CChatServer
//

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
		// Initialize the thread pool
		HRESULT hr = m_worker.Initialize();

		if(FAILED(hr)) return hr;

		return S_OK;
	}

	void FinalRelease()
	{
		// Shutdown the thread pool
		m_worker.Shutdown();
	}

public:
	// onUnregister - called when a server port dies
	void onUnregister(BSTR name);

	// onSendMessage - called by a server port when a client's message is received
	// NOTE: failes with E_UNEXPECTED, if "dest" is not NULL is not found.
	HRESULT onSendMessage(BSTR name, BSTR dest, BSTR msg);

	//
	// IChatServer interface
	//

	STDMETHOD(registerClient)(BSTR name, IChatClient* pClient, IChatServerPort** ppPort);

private:
	// LogItem - an item of the message log
	struct LogItem {
		// TODO: switch those to _bstr_t to optimize everything
		CComBSTR		src;
		CComBSTR		dst;
		CComBSTR		msg;
	};

	typedef CComPtr<IChatClient>	ClientPtr;

	// NOTE: leave CComBSTR here, because CAtlMap knows how to hash it
	CAtlMap<CComBSTR, ClientPtr>	m_clients;		// client map

	CAtlList<LogItem>				m_log;			// message log

	CThreadPool<Worker>				m_worker;		// thread pool

};

OBJECT_ENTRY_AUTO(__uuidof(ChatServer), CChatServer)