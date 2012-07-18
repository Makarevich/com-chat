#include "utils.h"


//
// COCALL implementation
//
//  Diaplays an error if the HRESULT of the call is somewhat invalid
//

bool CoCallImplementation(HRESULT hr, LPCTSTR msg) {
	if(FAILED(hr)) {
		TCHAR		buf[200];

		wsprintf(buf, _T("%s: %08X"), msg, hr);

		::MessageBox(0, buf, _T("Com Error"), 0);

		return true;
	}else if(hr != S_OK) {
		TCHAR		buf[200];

		wsprintf(buf, _T("%s: %08X"), msg, hr);

		::MessageBox(0, buf, _T("Com Warning"), 0);

		return true;
	}

	return false;
}



//
// err -- vararg message notification
//

void err(LPCTSTR msg, ... ) {
	TCHAR		buf[200];

	va_list	list;

	va_start(list, msg);

	wvsprintf(buf, msg, list);

	va_end(list);

	::MessageBox(0, buf, _T("Chat Client"), 0);
}