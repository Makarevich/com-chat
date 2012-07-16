// ChatServer.h : Declaration of the CChatServer

#pragma once



#include "ComChat_i.h"



using namespace ATL;

#include "utils.h"



// CChatServer

class ATL_NO_VTABLE CChatServerPort :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IChatServerPort
{
public:
	CChatServerPort()
	{
	}

DECLARE_NOT_AGGREGATABLE(CChatServerPort)

BEGIN_COM_MAP(CChatServerPort)
	COM_INTERFACE_ENTRY(IChatServerPort)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	void Setup(BSTR login, CChatServer* serv) {
		m_login = login;
		m_serv = serv;
	}

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		//err(L"Port dying");
		m_serv->onUnregister(m_login);
	}

public:

	// STDMETHOD(registerClient)(BSTR name, IChatClient* pClient, IChatServerPort** ppPort);

private:
	CComBSTR				m_login;

	CComPtr<CChatServer>	m_serv;
};
