// ChatServer.h : Declaration of the CChatServer

#pragma once



#include "ComChat_i.h"



using namespace ATL;

#include "utils.h"


//
// CChatServerPort
//
//  Used for all the stuff the client would do after it has registered on the server.
//  Exposes IChatServerPort.
//

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

	// Setup - configures the port
	void Setup(const CComBSTR& login, CChatServer* serv) {
		m_login = login;
		m_serv = serv;
	}

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		// announce our death to the server
		m_serv->onUnregister(m_login);
	}

public:

	// IChatServerPort interface
	STDMETHOD(sendMessage)(BSTR dest, BSTR msg) {
		return m_serv->onSendMessage(m_login, dest, msg);
	}

private:
	CComBSTR				m_login;

	CComPtr<CChatServer>	m_serv;
};
