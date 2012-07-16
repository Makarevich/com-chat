// ChatServer.cpp : Implementation of CChatServer

#include "stdafx.h"
#include "ChatServer.h"
#include "ChatServerPort.h"


// CChatServer


STDMETHODIMP CChatServer::registerClient(
	BSTR				name,
	IChatClient*		pClient,
	IChatServerPort**	ppPort)
{
	*ppPort = NULL;

	typedef CComObject<CChatServerPort>		Port;
	Port		*pPort = NULL;

	{
		// Lock self
		ATL_LOCKER(CChatServer);

		ClientPtr		cl(pClient);
		ClientPtr		cl2;

		// Check if the login has already been registered.
		if(m_clients.Lookup(name, cl2)) {
			//err(L"%s already reggd", BSTR2TSTR(name));
			return S_OK;
		}

		// Create and setup a port object
		HRESULT hr = Port::CreateInstance(&pPort);

		if(FAILED(hr)) {
			return hr;
		}

		pPort->Setup(name, this);

		// announce the registration
		{
			CComBSTR	n2;
			ClientPtr	pCl;

			for(POSITION pos = m_clients.GetStartPosition(); pos != NULL; ) {
				m_clients.GetNextAssoc(pos, n2, pCl);

				m_worker.QueueRequest(new JoinNotify(pCl, name));
			}
		}

		// Register the login
		m_clients.SetAt(name, cl);
	}

	// Return the port
	pPort->AddRef();
	*ppPort = pPort;

	return S_OK;
}


void CChatServer::onUnregister(BSTR name) {
	// Lock self
	ATL_LOCKER(CChatServer);

	// Effectively release IChatClient
	m_clients.RemoveKey(name);

	// announce
	{
		CComBSTR	n2;
		ClientPtr	pCl;

		for(POSITION pos = m_clients.GetStartPosition(); pos != NULL; ) {
			m_clients.GetNextAssoc(pos, n2, pCl);
			//pCl->notifyUserLeave(name);

			//err(_T("Notify %s that %s left"), BSTR2TSTR(n2), BSTR2TSTR(name));
			m_worker.QueueRequest(new LeaveNotify(pCl, name));
		}
	}
		

}
