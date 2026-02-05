#include "pch.h"
#include "ClientSocket.h"
#include "ClientController.h"
std::map<UINT, CClientController::MSGFUNC> CClientController::m_mapFunc;


int CClientController::InitController()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, 
		&CClientController::threadEntry, 
		this, 0, &m_nThreadId); //CreateThread
	m_statusDlg.Create(IDD_DIG_INFO, &m_clientDlg);
	return 0;
}

int CClientController::Invoke(CWnd*& pMainWnd)
{
	pMainWnd = &m_clientDlg;
	return m_clientDlg.DoModal();
}

LRESULT CClientController::SendMessage(MSG msg)
{
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) return -2;	
	MSGINFO info(msg);
	PostThreadMessage(m_nThreadId, WM_SEND_MESSAGE, (WPARAM)&info,
		(LPARAM)hEvent);
	WaitForSingleObject(hEvent, -1);
	return info.result;
}

LRESULT CClientController::OnSendPack(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPacket* pPacket = (CPacket*)wParam;
	return CClientSocket::getInstance()->Send(*pPacket);
}

LRESULT CClientController::OnSendData(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	char* pBuffer = (char*)wParam;
	return CClientSocket::getInstance()->Send(pBuffer,(int)lParam);
}

LRESULT CClientController::OnShowStatus(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return m_statusDlg.ShowWindow(SW_SHOW);
}

LRESULT CClientController::OnShowWatcher(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return m_showDlg.DoModal();
}

unsigned __stdcall CClientController::threadEntry(void* arg)
{
	CClientController* thiz = (CClientController*)arg;
	thiz->threadFunc();
	_endthreadex(0);
	return 0;
}

void CClientController::threadFunc()
{
	MSG msg;
	while (::GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_SEND_MESSAGE) {
			MSGINFO* pMsgInfo = (MSGINFO*)msg.wParam;
			HANDLE hEvent = (HANDLE)msg.lParam;
			std::map<UINT, MSGFUNC>::iterator it = m_mapFunc.find(msg.message);
			if (it != m_mapFunc.end()) {
				pMsgInfo->result = (this->*it->second)(pMsgInfo->msg.message,
					pMsgInfo->msg.wParam, pMsgInfo->msg.lParam);
			}
			else {
				pMsgInfo->result = -1;
			}
			SetEvent(hEvent);
		}
		
	}
}
