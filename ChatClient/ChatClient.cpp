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


CAppModule _Module;


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
	COCALL(CoInitializeEx(NULL, COINIT_MULTITHREADED), L"initializing multithreaded COM") {
		return 2;
	}

	ComUninitializer	_com_uninit;

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	COCALL(_Module.Init(NULL, hInstance), L"initializing ATL module") {
		return 2;
	}

	ModuleTerminator	__mod_term;

	//
	// 2. Spawn server
	//

	CComPtr<IChatServer>		pIs;

	COCALL(pIs.CoCreateInstance(CLSID_ChatServer, NULL, CLSCTX_LOCAL_SERVER),
		L"connecting to chat server")
	{
		return 2;
	}

	//
	// 3. Create client notification object
	//

	CComPtr<CChatClient>		pIc;

	COCALL(CChatClient::Create(pIc), L"creating client object") {
		return 2;
	}

	//
	// 4. Create main modal dialog
	//

	CMainDlg	dlgMain;

	pIc->Configure(&dlgMain);

	//
	// 5. Attempt to login
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
	// 6. Show main interface
	//

	{
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

CChatClient*		CChatClient::m_instance = NULL;
HANDLE				CChatClient::m_release_event = NULL;