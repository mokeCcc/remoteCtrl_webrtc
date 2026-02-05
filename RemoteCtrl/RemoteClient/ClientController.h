#pragma once
#include <map>
#include "ClientSocket.h"
#include "RemoteClientDlg.h"
#include "RemoteDlgShow.h"
#include "Resource.h"
#define WM_SEND_PACK (WM_USER + 1)
#define WM_SEND_DATA (WM_USER + 2)
#define WM_SHOW_STATUS (WM_USER + 3)
#define WM_SHOW_WATCH (WM_USER + 4)
#define WM_SEND_MESSAGE (WM_USER + 5)
class CClientController
{
public:
	static CClientController& getInstance() {
		static CClientController m_instance;
		return m_instance;
	}

	int InitController();
	int Invoke(CWnd*& pMainWnd);
	LRESULT SendMessage(MSG msg);
	//更新网络服务器地址
	void UpdateAddress(int nIP, int nPort) {
		CClientSocket::getInstance()->UpdateAddress(nIP, nPort);
	}
	int DealCommand() {
		return CClientSocket::getInstance()->DealCommand();
	}
	void CloseSocket() {
		CClientSocket::getInstance()->CloseServerSocket();
	}
	bool SendPakcet(const CPacket& packet) {
		CClientSocket* pClient = CClientSocket::getInstance();
		if (pClient->InitializeSocket() == FALSE) return false;
		pClient->Send(packet);
	}
protected:
	LRESULT OnSendPack(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendData(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowStatus(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowWatcher(UINT nMsg, WPARAM wParam, LPARAM lParam);
	static unsigned __stdcall threadEntry(void* arg);
	void threadFunc();
private:
	typedef LRESULT(CClientController::* MSGFUNC)(UINT nMsg, WPARAM wParam, LPARAM lParam);
	CRemoteClientDlg m_clientDlg;
	CRemoteDlgShow m_showDlg;
	CStatusDlg m_statusDlg;
	HANDLE m_hThread;	
	unsigned  m_nThreadId;
	static std::map<UINT, MSGFUNC> m_mapFunc;
	CClientController():m_statusDlg(&m_clientDlg),
		m_showDlg(&m_clientDlg){
		m_hThread = INVALID_HANDLE_VALUE;
		m_nThreadId = -1;
		struct { UINT nMsg; MSGFUNC func; }msgFuncs[] = {
			{WM_SEND_PACK,&CClientController::OnSendPack},
			{WM_SEND_DATA,&CClientController::OnSendData},
			{WM_SHOW_STATUS,&CClientController::OnShowStatus},
			{WM_SHOW_WATCH,&CClientController::OnShowWatcher},
			{-1,NULL}
		};
		for (int i = 0; msgFuncs[i].func != NULL; i++) {
			m_mapFunc.insert(std::pair<UINT, MSGFUNC>(msgFuncs[i].nMsg, 
				msgFuncs[i].func));
		}
	}
	CClientController(const CClientController&) = delete;
	CClientController(CClientController&&) = delete;
	CClientController& operator=(const CClientController&) = delete;
	~CClientController() {
		WaitForSingleObject(m_hThread, 100);
	}
private:
	typedef struct MsgInfo {
		MSG msg;
		LRESULT result;
		MsgInfo(MSG m) { result = 0; memcpy(&msg, &m, sizeof MSG); }
		MsgInfo(const MsgInfo& m) { result = m.result; memcpy(&msg, &m.msg, sizeof MSG); }
		MsgInfo& operator= (const MsgInfo& m) {
			if (this != &m)
			{
				result = m.result;
				memcpy(&msg, &m.msg, sizeof MSG);
			}
			return *this;
		}
	}MSGINFO;
};

