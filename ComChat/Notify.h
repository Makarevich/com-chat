#pragma once


#include <atlcom.h>

#include "ComChat_i.h"

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

	//
	// NOTE: all notification object free themselves upon Invoke
	//

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


//
// MessageNotify
//

class MessageNotify : public AbsNotify {
private:
	MessageNotify (const MessageNotify&);

	CComBSTR				dest;
	CComBSTR				msg;

public:
	MessageNotify(
		const CComPtr<IChatClient>& pCl,
		const CComBSTR&				name, 
		const CComBSTR&				dest, 
		const CComBSTR&				msg)
		: AbsNotify(pCl, name), dest(dest), msg(msg) {}

	MessageNotify(
		const CComPtr<IChatClient>& pCl,
		const ChatMessage* const	m)
		: AbsNotify(pCl, m->src), dest(m->dst), msg(m->msg) {}

	virtual void Invoke(){
		ChatMessage		m = { name, dest, msg };

		pCl->notifyMessage(&m);
	}
};