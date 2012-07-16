


#pragma once

#include "utils.h"


class AbsNotify {
protected:
	// note those SHOULD be objects (not refs)
	CComPtr<IChatClient>	pCl;
	CComBSTR				name;

	AbsNotify(const AbsNotify&);

	AbsNotify(
		const CComPtr<IChatClient>& pCl, 
		const CComBSTR&				name)
		: pCl(pCl), name(name) {}
public:
	virtual void Invoke() = 0;
};

#define DEFINE_COMMAND_START(n)	\
	class n ## Notify : public AbsNotify {		\
	private:									\
		n ## Notify(const n ## Notify&);		\
	public:										\
		n ## Notify(							\
			const CComPtr<IChatClient>& pCl,	\
			const CComBSTR&				name)	\
			: AbsNotify(pCl, name) {}			\
		virtual void Invoke(){


#define DEFINE_COMMAND_END()		\
		delete this;				\
	}};


DEFINE_COMMAND_START(Join)
	pCl->notifyUserJoin(name);
DEFINE_COMMAND_END()

DEFINE_COMMAND_START(Leave)
	pCl->notifyUserLeave(name);
DEFINE_COMMAND_END()

/////////////////////////////////////////

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