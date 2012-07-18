// CChatClient.h : Declaration of the CChatClient

#pragma once

class CChatClient;

#include "../ComChat/ComChat_i.h"

#include "../ComChat/Notify.h"


#include "MainDlg.h"

using namespace ATL;


//
// CChatClient - chat client notification object
//
//  Implemented as a singleton.
//
//  It's main purpuse is to receive notifications from chat server. When
//  configured with a CMainDlg, it it relays all the notifications to the dialog.
//
//  Also, it is possible for the client object to receive notifications before it
//  is configured with a dialog. In this case it stores all notification is
//  a queue and reapplies them once it gets confed with a valid dialog.
//  (See CChatClient::Configure.)
//
//  It is also possible for a thread to wait for the singleton object to die
//  (via static CChatClient::WaitForRelease).
//

class ATL_NO_VTABLE CChatClient :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IChatClient
{
protected:
	// CComObject requires this default ctor tor be protected.
	CChatClient() : m_dialog(NULL)
	{
	}

private:
	// Private ass-operator is a part of singleton impelemtation.
	CChatClient& operator= (const CChatClient&);

private:

DECLARE_NOT_AGGREGATABLE(CChatClient)

BEGIN_COM_MAP(CChatClient)
	COM_INTERFACE_ENTRY(IChatClient)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

protected:
	HRESULT FinalConstruct() {
		// Create a release event object.
		if((m_release_event = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) {
			return E_UNEXPECTED;
		}

		return S_OK;
	}

	void FinalRelease()
	{
		// clean up the pending queue
		while(!m_pending.IsEmpty()) {
			delete m_pending.RemoveHead();
		}

		// Signal the release event.
		SetEvent(m_release_event);
	}

public:

	//
	// Singleton interface
	//
	//  The template parameter allows passing almost arbitrary CComPtr<T> object.
	//

	template <class T>
	static HRESULT Create(CComPtr<T>& ppClient) {
		if(m_instance == NULL) {
			typedef CComObject<CChatClient>		Client;

			Client		*pClient;
			HRESULT		hr = Client::CreateInstance(&pClient);

			if(FAILED(hr)) return hr;

			m_instance = pClient;
		}

		ppClient = m_instance;		// implicit AddRef call
		return S_OK;
	}

	//
	// other utils
	//

	static DWORD WaitForRelease() {
		return WaitForSingleObject(m_release_event, INFINITE);
	}

	void Configure(CMainDlg* dlg) {
		ATL_LOCKER(CChatClient);

		m_dialog = dlg;

		// if a new dialog is configured,
		// then we apply pending requests
		if(m_dialog != NULL) {
			while(!m_pending.IsEmpty()) {
				m_pending.RemoveHead()->Invoke();
				// NOTE: Invoke automatically destroys the request
			}
		}
	}

public:

	// IChatClient interface

	STDMETHOD(notifyUserJoin) (BSTR name);
	STDMETHOD(notifyUserLeave)(BSTR name);
	STDMETHOD(notifyMessage)  (ChatMessage* msg);

private:

	//
	// dialogNotify
	//
	//  A common notification routing that implements both dialog notification
	//  and (in case an unconfigured dialog) notification queueing.
	//
	//  All the methods of IChatClient implementation are supposed to call it.
	//

	template<class T, void (CMainDlg::*method)(T), class N>
	HRESULT dialogNotify(T item) {
		for(;;){
			// If a dialog is confgured, notify it.
			if(m_dialog != NULL) {
				(m_dialog->*method)(item);
				return S_OK;
			}

			// If the dialog doesnot seem to be configured,
			// lock self, check dialog again, and if it is still NULL,
			// queue the notification.

			ATL_LOCKER(CChatClient);

			if(m_dialog == NULL) {
				m_pending.AddTail(new N(this, item));
				return S_OK;
			}
		}
	}

private:
	// singleton stuff
	static CChatClient*		m_instance;
	static HANDLE			m_release_event;	// NOTE: this handle never gets released

	// dialog
	CMainDlg*				m_dialog;

	// pending queue
	CAtlList<AbsNotify*>	m_pending;
};



