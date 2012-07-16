// ComChat.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "ComChat_i.h"




class CComChatModule : public ATL::CAtlExeModuleT< CComChatModule >
	{
public : 
	DECLARE_LIBID(LIBID_ComChatLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMCHAT, "{94093B4C-46D3-47BB-AD7E-5142A9C0635E}")
	};

CComChatModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

