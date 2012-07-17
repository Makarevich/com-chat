// ChatServer.cpp : Implementation of CChatServer

#include "stdafx.h"
#include "ChatServer.h"
#include "ChatServerPort.h"


// CChatServer


STDMETHODIMP CChatServer::registerClient(
	BSTR				name_bstr,
	IChatClient*		pClient,
	IChatServerPort**	ppPort)
{
	*ppPort = NULL;

	typedef CComObject<CChatServerPort>		Port;
	Port		*pPort = NULL;

	CComBSTR	name(name_bstr);	// allocate a copy

	// check the login
	{
		const UINT	len = name.Length();

		for(UINT i = 0; i < len; i++) {
			const OLECHAR	c = name.m_str[i];

			if(_T('A') <= c && c <= _T('Z')) continue;
			if(_T('a') <= c && c <= _T('z')) continue;
			if(_T('0') <= c && c <= _T('9')) continue;

			return S_OK;	// deny the login
		}
	}


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

		if(FAILED(hr)) return hr;

		pPort->Setup(name, this);

		// announce the registration
		{
			CComBSTR	n2;
			ClientPtr	pCl;

			for(POSITION pos = m_clients.GetStartPosition(); pos != NULL; ) {
				m_clients.GetNextAssoc(pos, n2, pCl);

				// mutual announce
				m_worker.QueueRequest(new JoinNotify(pCl, name));

				m_worker.QueueRequest(new JoinNotify(cl, n2));
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
	if(!m_clients.RemoveKey(name)) {
		return;
	}

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
