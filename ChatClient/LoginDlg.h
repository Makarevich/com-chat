// LoginDlg.h : interface of the CLoginDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once


#include "utils.h"

//
// CLoginDlg -- the dialog that is used to login the user.
//

class CLoginDlg :
	public CDialogImpl<CLoginDlg>,
	public CWinDataExchange<CLoginDlg>
{
public:
	enum { IDD = IDD_LOGINDLG };

	BEGIN_MSG_MAP(CLoginDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnLogin)
		COMMAND_ID_HANDLER(IDCANCEL, OnExit)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)



	BEGIN_DDX_MAP(CLoginDlg)
		DDX_TEXT(IDC_LOGIN, m_login)
	END_DDX_MAP()

	//
	// The only ctor cofigures refs to a bunch of CComPtr objects.
	//

	CLoginDlg(
		CComPtr<IChatClient>&		m_client,
		CComPtr<IChatServer>&		m_server,
		CComPtr<IChatServerPort>&	m_port
		) : m_client(m_client), m_server(m_server), m_port(m_port)
	{
	}

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

		return TRUE;
	}

	LRESULT OnLogin(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(TRUE);		// fetch login

		COCALL(m_server->registerClient(m_login, m_client, &m_port), _T("registering")) {
			EndDialog(IDABORT);
			return 0;
		}

		if(m_port == NULL) {
			::MessageBox(this->m_hWnd,
				_T("A user with the same name already registered"),
				_T("Login error"), 0);
			return 0;
		}

		// Close the dialog on successful login.
		EndDialog(IDOK);
		return 0;
	}

	LRESULT OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);		// That usually sends IDCANCEL
		return 0;
	}

private:
	CComBSTR					m_login;

	CComPtr<IChatClient>&		m_client;
	CComPtr<IChatServer>&		m_server;
	CComPtr<IChatServerPort>&	m_port;
};
