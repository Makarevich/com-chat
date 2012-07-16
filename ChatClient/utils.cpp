#include "utils.h"


bool CoCallImplementation(HRESULT hr, LPCTSTR msg) {
	if(FAILED(hr)) {
		TCHAR		buf[200];

		wsprintf(buf, L"%s: %08X", msg, hr);

		::MessageBox(0, buf, L"Com Error", 0);

		return true;
	}else if(hr != S_OK) {
		TCHAR		buf[200];

		wsprintf(buf, L"%s: %08X", msg, hr);

		::MessageBox(0, buf, L"Com Warning", 0);

		return true;
	}

	return false;
}





void err(LPCTSTR msg, ... ) {
	TCHAR		buf[200];

	va_list	list;

	va_start(list, msg);

	wvsprintf(buf, msg, list);

	va_end(list);

	::MessageBox(0, buf, L"Chat Client", 0);
}