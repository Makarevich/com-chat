#include "stdafx.h"

//
// err -- vararg message notification
//

void err(LPCTSTR msg, ... ) {
	TCHAR		buf[200];

	va_list	list;

	va_start(list, msg);

	wvsprintf(buf, msg, list);

	va_end(list);

	::MessageBox(0, buf, _T("Chat Server"), 0);
}