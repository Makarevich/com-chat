// CChatClient.h : Declaration of the CChatClient

#pragma once



#include "../ComChat/ComChat_i.h"



using namespace ATL;


// CChatClient

class ATL_NO_VTABLE CChatClient :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IChatClient
{
protected:
	CChatClient()		// sort of, private
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
		m_dialog = dlg;
	}

public:

	STDMETHOD(notifyUserJoin) (BSTR name){
		m_dialog->notifyUserJoin(name);
		return S_OK;
	}

	STDMETHOD(notifyUserLeave)(BSTR name){
		m_dialog->notifyUserLeave(name);
		return S_OK;
	}

private:
	static CChatClient*		m_instance;
	static HANDLE			m_release_event;

	CMainDlg*				m_dialog;
};
