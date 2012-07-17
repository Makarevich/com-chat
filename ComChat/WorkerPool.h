


#pragma once

#include "utils.h"

#include "Notify.h"

class Worker {
public:
	typedef AbsNotify*	RequestType;

	BOOL Initialize(void* pvParam) {
		// Enter MTA
		return SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED));
	}

	void Terminate(void* pvWorkerParam){
		CoUninitialize();
	}
	
	void Execute(
		RequestType request,
		void* pvWorkerParam,
		OVERLAPPED* pOverlapped
	){
		request->Invoke();
	}
};