#pragma once



#include "stdafx.h"

#include <atlstr.h>		// strangely, this should be outside the PCH



//
// Locker
//
//		Simple utility that wraps calls to Lock/Unlock into an object


template<class T>
class Locker {
private:
	T*	obj;
public:

	Locker(T* obj) : obj(obj) {
		obj->Lock();
	}

	~Locker() {
		obj->Unlock();
	}
};


#define ATL_LOCKER(cl)		Locker<cl>	_ ## cl ## __locker(this)


//
// COCALL
//


#include "../ComChat/ComChat_i.h"

#define COCALL(c, msg)			\
	if(CoCallImplementation((c), L"Error while " msg))

extern bool CoCallImplementation(HRESULT, LPCTSTR);


//
// BSTR converter
//

#define BSTR2TSTR(b)		((LPTSTR)_bstr_t(b))

//
// err
//

void err(LPCTSTR msg, ... );