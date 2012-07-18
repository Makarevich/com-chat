// ChatServer.cpp : Implementation of CChatServer

#include "stdafx.h"
#include "ChatServer.h"
#include "ChatServerPort.h"

#include <time.h>

//
// IChatServer implementation
//

STDMETHODIMP CChatServer::registerClient(
	BSTR				name_bstr,
	IChatClient*		pClient,
	IChatServerPort**	ppPort)
{
	typedef CComObject<CChatServerPort>		Port;


	*ppPort = NULL;

	Port	*pPort = NULL;

	CComBSTR	name(name_bstr);	// allocate a copy of the client's login

	// check the login for some sort of sanity
	{
		const UINT	len = name.Length();

		if(len == 0) return S_OK;

		for(UINT i = 0; i < len; i++) {
			const OLECHAR	c = name.m_str[i];

			// allow the following char ranges
			if(_T('A') <= c && c <= _T('Z')) continue;
			if(_T('a') <= c && c <= _T('z')) continue;
			if(_T('0') <= c && c <= _T('9')) continue;

			return S_OK;	// deny the login
		}
	}

	CAtlArray<CComBSTR>		init_names;
	CAtlArray<ChatMessage>	init_msgs;

	ClientPtr		cl(pClient);

	{
		// Lock self
		ATL_LOCKER(CChatServer);

		ClientPtr		cl2;

		// Check if the login has already been registered.
		if(m_clients.Lookup(name, cl2)) {
			return S_OK;
		}

		// Create and setup a server port object
		HRESULT hr = Port::CreateInstance(&pPort);

		if(FAILED(hr)) return hr;

		pPort->Setup(name, this);

		// announce the registration to other clients,
		// as well as collect other login
		{
			CComBSTR	n2;
			ClientPtr	pCl;

			for(POSITION pos = m_clients.GetStartPosition(); pos != NULL; ) {
				m_clients.GetNextAssoc(pos, n2, pCl);

				// mutual announce
				m_worker.QueueRequest(new JoinNotify(pCl, name));

				//m_worker.QueueRequest(new JoinNotify(cl, n2));
				init_names.Add(n2);
			}
		}

		// Register the login
		m_clients.SetAt(name, cl);

		// scan the message log and collect appropriate messages
		// for newly connected client
		for(POSITION pos = m_log.GetHeadPosition(); pos; ) {
			const LogItem&	item = m_log.GetNext(pos);

			if(item.dst != _T("") &&
				item.src != name &&
				item.dst != name)
			{
				continue;
			}

			//m_worker.QueueRequest(new MessageNotify(cl,
			//	item.src, item.dst, item.msg, item.time));

			init_msgs.Add(item);
		}
	}

	// Send the initialization message
	m_worker.QueueRequest(new InitializeNotify(cl, init_names, init_msgs));

	// Return the port object
	pPort->AddRef();
	*ppPort = pPort;

	return S_OK;
}


void CChatServer::onUnregister(BSTR name) {
	// Lock self
	ATL_LOCKER(CChatServer);

	// Effectively release IChatClient
	if(!m_clients.RemoveKey(name)) {
		ATLASSERT(false && "Attempt to remove an unknown login");
		return;
	}

	// announce the unregistration
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


HRESULT CChatServer::onSendMessage(BSTR name, BSTR dest, BSTR msg) {

	FILETIME	t;

	// determine current timestamp
	{
		SYSTEMTIME		st;

		GetLocalTime(&st);
		SystemTimeToFileTime(&st, &t);
	}

	// Lock self
	ATL_LOCKER(CChatServer);

	//err(_T("onSendMessage: %s"), BSTR2TSTR(msg));

	if(dest == NULL) {
		// announce to everybody
		CComBSTR	n2;
		ClientPtr	pCl;

		for(POSITION pos = m_clients.GetStartPosition(); pos != NULL; ) {
			m_clients.GetNextAssoc(pos, n2, pCl);

			m_worker.QueueRequest(new MessageNotify(pCl, name, dest, msg, t));
		}
	}else{
		// announce to author and recipient only
		ClientPtr		cla, clb;

		if(!m_clients.Lookup(name, cla)) {
			return E_UNEXPECTED;
		}

		if(!m_clients.Lookup(dest, clb)) {
			return E_UNEXPECTED;
		}

		// announce to both parties
		m_worker.QueueRequest(new MessageNotify(cla, name, dest, msg, t));
		m_worker.QueueRequest(new MessageNotify(clb, name, dest, msg, t));
	}

	// register the messege in the message log
	LogItem		item = { name, dest, msg, t };

	m_log.AddTail(item);

	return S_OK;
}
