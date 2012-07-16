#pragma once


//
// COCALL
//

#include "stdafx.h"

#include <atlstr.h>		// strangely, this should be outside the PCH


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