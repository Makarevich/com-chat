#pragma once


#include <atlcom.h>

#include "ComChat_i.h"

class AbsNotify {
protected:
	// NOTE: those SHOULD be objects (not refs) to make sure they are not released
	// somewhere else.
	CComPtr<IChatClient>	pCl;
	CComBSTR				name;

	AbsNotify(const AbsNotify&);

	AbsNotify(
		const CComPtr<IChatClient>& pCl, 
		const CComBSTR&				name)
		: pCl(pCl), name(name) {}
public:

	//
	// NOTE: all notification objects free themselves upon Invoke
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

	FILETIME				time;

public:
	MessageNotify(
		const CComPtr<IChatClient>& pCl,
		const CComBSTR&				name, 
		const CComBSTR&				dest, 
		const CComBSTR&				msg,
		const FILETIME&				time)
		: AbsNotify(pCl, name), dest(dest), msg(msg), time(time) {}

	MessageNotify(
		const CComPtr<IChatClient>& pCl,
		const ChatMessage&			m)
		: AbsNotify(pCl, m.src), dest(m.dst), msg(m.msg), time(m.time) {}

	virtual void Invoke(){
		ChatMessage		m = { name, dest, msg, time };

		pCl->notifyMessage(m);

		delete this;
	}
};

//
// InitializeNotify
//

class InitializeNotify : public AbsNotify {
private:
	InitializeNotify (const MessageNotify&);

	CAtlArray<CComBSTR>		init_names;
	CAtlArray<ChatMessage>	init_msgs;

public:
	InitializeNotify(
		const CComPtr<IChatClient>&		pCl,
		const CAtlArray<CComBSTR>&		names,
		const CAtlArray<ChatMessage>&	msgs)
		: AbsNotify(pCl, NULL), init_names(), init_msgs()
	{
		init_names.Copy(names);
		init_msgs .Copy(msgs);
	}

	virtual void Invoke(){
		// convert names from CComBSTR to BSTR
		CAtlArray<BSTR>	bn;

		bn.SetCount(init_names.GetCount());

		for(int i = init_names.GetCount() - 1; i >= 0; i--) {
			bn[i] = init_names[i];
		}

		pCl->initialize(
			bn.GetData(), bn.GetCount(),
			init_msgs.GetData(), init_msgs.GetCount());

		delete this;
	}
};