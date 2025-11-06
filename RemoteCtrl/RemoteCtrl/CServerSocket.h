#pragma once
#include "pch.h"
//#include "framework.h"
class CServerSocket
{
public:
	static CServerSocket* getInstance() {
		if (!m_instance) { m_instance = new CServerSocket(); }
		return m_instance;
	}
	bool InitializeSocket() {
		if (m_sock == -1) return false;
		// TODO: check socket value
		sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY; // listen all ip address
		serv_addr.sin_port = htons(8086);

		if (bind(m_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) return false;
		if (listen(m_sock, 1) == -1) return false;

		return true;
	}
	bool AcceptClient() {
		sockaddr_in client_addr;
		
		int cli_sz = sizeof(client_addr);
		m_client = accept(m_sock, (sockaddr*)&client_addr, &cli_sz);
		if (m_client == -1) return false;
		return true;
	}
	int DealCommand() {
		if (m_client == -1) return false;
		char buffer[1024] = {};
		while (true) {
			int len = recv(m_sock, buffer, sizeof(buffer), 0);
			if (len <= 0) return -1;
		}
		// TODO : handle command 

	}
	bool Send(const char* pData, int nSize) {
		if (m_client == -1) return false;
		return send(m_client, pData, nSize, 0) > 0;

	}
private:
	
	SOCKET m_sock,m_client;
	CServerSocket& operator=(const CServerSocket&ss) {
		m_sock = ss.m_sock;
		m_client = ss.m_client;
	}
	CServerSocket() {
		m_sock = -1;
		m_client = INVALID_SOCKET;
		if (!InitSocketEnv()) {
			MessageBox(NULL, _T("can not initialized socket environment, please check network settings"), _T("socket initialize error!"), MB_OK | MB_ICONERROR);
			exit(0);
		}
		m_sock = socket(PF_INET, SOCK_STREAM, 0);
	}
	~CServerSocket() {
		closesocket(m_sock);
		WSACleanup();


	}
	BOOL InitSocketEnv() {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
			return false;
		}
		return true;
	}
	
	static void ReleaseInstance() {
		if (m_instance) {
			CServerSocket* tmp = m_instance;
			m_instance = nullptr;
			delete tmp;
		}

	}
	static CServerSocket* m_instance;

	class CHelper
	{
	public:
		CHelper() { CServerSocket::getInstance(); }
		~CHelper() { CServerSocket::ReleaseInstance(); }

	private:

	};
	static CHelper m_helper;
};


//extern CServerSocket server;

