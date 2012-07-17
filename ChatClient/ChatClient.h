// CChatClient.h : Declaration of the CChatClient

#pragma once

class CChatClient;

#include "../ComChat/ComChat_i.h"

#include "../ComChat/Notify.h"


#include "MainDlg.h"

using namespace ATL;



// CChatClient

class ATL_NO_VTABLE CChatClient :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IChatClient
{
protected:
	CChatClient() : m_dialog(NULL)		// sort of, private
	{
	}

private:
	CChatClient& operator= (const CChatClient&);

public:

DECLARE_NOT_AGGREGATABLE(CChatClient)

BEGIN_COM_MAP(CChatClient)
	COM_INTERFACE_ENTRY(IChatClient)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		if((m_release_event = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
			return E_UNEXPECTED;
		}

		return S_OK;
	}

	void FinalRelease()
	{
		//err(L"CChatClient destroyed");

		// make sure the pending queue is empty
		while(!m_pending.IsEmpty()) {
			delete m_pending.RemoveHead();
		}

		SetEvent(m_release_event);
	}

	//
	// Singleton interface
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
		//err(L"Waiting for release: %u", m_instance->m_dwRef);
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
			}
		}
	}

public:

	STDMETHOD(notifyUserJoin) (BSTR name);
	STDMETHOD(notifyUserLeave)(BSTR name);
	STDMETHOD(notifyMessage)  (ChatMessage* msg);

private:
	template<class T, void (CMainDlg::*method)(T), class N>
	HRESULT dialogNotify(T item) {
		for(;;){
			if(m_dialog != NULL) {
				(m_dialog->*method)(item);
				return S_OK;
			}

			ATL_LOCKER(CChatClient);

			if(m_dialog == NULL) {
				m_pending.AddTail(new N(this, item));
				return S_OK;
			}
		}
	}

private:
	static CChatClient*		m_instance;
	static HANDLE			m_release_event;

	CMainDlg*				m_dialog;

	CAtlList<AbsNotify*>	m_pending;
};



