// ChatClient.cpp : main source file for ChatClient.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "utils.h"

#include "MainDlg.h"
#include "LoginDlg.h"
#include "ChatClient.h"


// ATL module
CAppModule _Module;


//
// A couple of utility classes that handle uninitialization
//

struct ComUninitializer {
	~ComUninitializer() { ::CoUninitialize(); }
};

struct ModuleTerminator {
	~ModuleTerminator() { _Module.Term(); }
};



int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	//
	// 1. Initialize COM/ATL
	//

	// HRESULT hRes = ::CoInitialize(NULL);
	COCALL(CoInitializeEx(NULL, COINIT_MULTITHREADED), _T("initializing multithreaded COM")) {
		return 2;
	}

	ComUninitializer	_com_uninit;

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	COCALL(_Module.Init(NULL, hInstance), _T("initializing ATL module")) {
		return 2;
	}

	ModuleTerminator	__mod_term;

	//
	// 2. Spawn server
	//

	CComPtr<IChatServer>		pIs;

	COCALL(pIs.CoCreateInstance(CLSID_ChatServer, NULL, CLSCTX_LOCAL_SERVER),
		_T("connecting to chat server"))
	{
		return 2;
	}

	//
	// 3. Create client notification object
	//

	CComPtr<CChatClient>		pIc;

	COCALL(CChatClient::Create(pIc), _T("creating client object")) {
		return 2;
	}

	//
	// 4. Attempt to login
	//

	CComPtr<IChatServerPort>	pIsp;

	{
		CComPtr<IChatClient>	pIcc = pIc;

		CLoginDlg	dlg(pIcc, pIs, pIsp);

		if(dlg.DoModal() != IDOK) {
			return 0;
		}

		// Having obtained a server port, we do not need 
		// the server itself anymore. So we release it.

		pIs.Release();
	}

	//
	// 5. Show main dialog
	//

	{
		CMainDlg	dlgMain(*pIc, pIsp);

		dlgMain.DoModal();
	}

	// T1. Release the server port
	pIsp.Release();

	// T2. Release the client object
	pIc.Release();

	// T3. Wait for the client object to die
	CChatClient::WaitForRelease();

	return 0;
}


//////////////////////////////////////////////
// class CChatClient
//

//
// IChatClient implementation
//

STDMETHODIMP CChatClient::notifyUserJoin (BSTR name){
	return dialogNotify<BSTR, &CMainDlg::notifyUserJoin, JoinNotify>(name);
}

STDMETHODIMP CChatClient::notifyUserLeave (BSTR name){
	return dialogNotify<BSTR, &CMainDlg::notifyUserLeave, LeaveNotify>(name);
}

STDMETHODIMP CChatClient::notifyMessage (ChatMessage* msg) {
	//err(_T("Message recved: %s"), BSTR2TSTR(msg->msg));
	return dialogNotify<ChatMessage*, &CMainDlg::notifyMessage, MessageNotify>(msg);
}

//
// Singleton stuff
//

CChatClient*		CChatClient::m_instance = NULL;
HANDLE				CChatClient::m_release_event = NULL;

