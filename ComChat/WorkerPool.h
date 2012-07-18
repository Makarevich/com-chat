


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
		// No need to free the request object, since a call to AbsNotify::Invoke
		// releases the request object.
		request->Invoke();
	}
};