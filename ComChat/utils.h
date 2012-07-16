#pragma once

#include "stdafx.h"

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
// BSTR converter
//

#define BSTR2TSTR(b)		((LPTSTR)_bstr_t(b))

//
// err
//

void err(LPCTSTR msg, ... );