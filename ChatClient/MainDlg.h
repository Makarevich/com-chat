// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once


#include "utils.h"



class CMainDlg :
	public CDialogImpl<CMainDlg>,
	public CWinDataExchange<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

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

		m_msg.SetFocus();

		return TRUE;
	}

	LRESULT OnSend(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const int	buf_size = 256;

		TCHAR		buf[buf_size + 1];

		*buf = 0;

		int cnt = m_msg.GetLine(0, buf, buf_size);

		buf[cnt] = 0;

		err(_T("Sending: %s"), buf);

		m_msg.SetWindowText(_T(""));
		m_msg.SetFocus();

		//EndDialog(wID);
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		EndDialog(0);
		return 0;
	}

	//////////

	//
	// NOTE:
	//	The following methods are called asynchronously
	//	by the free-threaded client object. Implicit
	//	serialization is performed via dialog's message loop.
	//

	void notifyUserJoin(BSTR name) {
		CString		s;

		s = s + _T("User \"") + name + _T("\" joined");

		m_log.AddString(s);
	}

	void notifyUserLeave(BSTR name) {
		CString		s;

		s = s + _T("User \"") + name + _T("\" left");

		m_log.AddString(s);
	}

private:
	CListBox		m_log;
	CEdit			m_msg;
};
