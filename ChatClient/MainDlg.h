// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once


class CMainDlg;

#include "utils.h"



#include "ChatClient.h"

//
// CMainDlg - the main dialog
//

class CMainDlg :
	public CDialogImpl<CMainDlg>,
	public CWinDataExchange<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	// The main dialog is configured with the client object and a server port.

	CMainDlg(CChatClient&				client,
		const CComPtr<IChatServerPort>& pIsp)
		: m_chat_client(client), m_server_port(pIsp)
	{
	}

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDC_SEND, OnSend)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	BEGIN_DDX_MAP(CMainDlg)
		DDX_CONTROL_HANDLE(IDC_MESSAGE, m_msg)
		DDX_CONTROL_HANDLE(IDC_LOG,     m_log)
		DDX_CONTROL_HANDLE(IDC_PEERS,   m_peers)
	END_DDX_MAP()


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(FALSE);	// Init ddx

		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// minor GUI stuff
		m_msg.SetFocus();

		m_peers.AddString(_T("--"));	// That line would indicate "send to all"
		m_peers.SetCurSel(0);

		// tell the client object we're ready
		m_chat_client.Configure(this);

		return TRUE;
	}

	LRESULT OnSend(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// message buffer
		const int	buf_size = 256;
		TCHAR		buf[buf_size + 1];

		CComBSTR	dest;

		{
			// fetch message
			*buf = 0;
			int cnt = m_msg.GetLine(0, buf, buf_size);
			buf[cnt] = 0;

			// do nothing for empty messages
			if(cnt == 0) {
				return 0;
			}
		}

		{
			// get message destination
			const int dest_id = m_peers.GetCurSel();

			if(dest_id != 0) {
				m_peers.GetLBTextBSTR(dest_id, dest.m_str);
			}
		}

		// TODO: here a COM CALL is made by the GUI thread, which is BAAAD
		COCALL(m_server_port->sendMessage(dest, CComBSTR(buf)), _T("sending message")) {
		}

		m_msg.SetWindowText(_T(""));
		m_msg.SetFocus();

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// tell the client object that we're done
		m_chat_client.Configure(NULL);

		EndDialog(0);
		return 0;
	}

	//////////

	//
	// NOTE:
	//	The following methods are called asynchronously by the free-threaded
	//  client object. Implicit synchronization is done via dialog's message loop.
	//

	void notifyUserJoin(BSTR name) {
		m_peers.AddString(name);

		/////

		CString		s;

		s = s + _T("User \"") + name + _T("\" joined");

		add_log_message(s);
	}

	void notifyUserLeave(BSTR name) {
		const int ind = m_peers.FindString(0, name);

		if(ind != 0 && ind != CB_ERR) {
			m_peers.DeleteString(ind);
		}

		/////
		CString		s;

		s = s + _T("User \"") + name + _T("\" left");

		add_log_message(s);
	}

	void notifyMessage(ChatMessage m) {
		SYSTEMTIME	st;

		FileTimeToSystemTime(&m.time, &st);

		CString		s;

		s.Format(_T("%02i:%02i:%02i %s%s: %s"),
			st.wHour, st.wMinute, st.wSecond,
			m.src,
			(m.dst ? CString(" to ") + m.dst : CString()),
			m.msg);

		add_log_message(s);
	}

private:
	void add_log_message(const CString& s) {
#if 0
		// TODO: lock self???

		// fetch scroll info of the message log
		SCROLLINFO	si = { sizeof(SCROLLINFO), SIF_POS | SIF_RANGE };
		const BOOL	si_ok = m_log.GetScrollInfo(SB_VERT, &si);

		//err(_T("%u < %u"), si.nPos, si.nMax);

		// add new string
		const int index = m_log.AddString(s);

		// scroll the log, if necessary
		if(si_ok && si.nPos == si.nMax ) {
			//const int h = m_log.GetItemHeight(index);
			//m_log.ScrollWindow(0, h);
			//m_log.SetScrollPos(SB_VERT, si.nPos + 1);
			m_log.SetTopIndex(index);
		}
#else
		m_log.SetTopIndex(m_log.AddString(s));
#endif
	}

	// dialog widgets
	CListBox					m_log;
	CEdit						m_msg;
	CComboBox					m_peers;

	CChatClient&				m_chat_client;		// used for the client object configuration

	// server port pointer
	CComPtr<IChatServerPort>	m_server_port;

};
