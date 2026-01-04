#pragma once
#include "pch.h"
#include "framework.h"
#include "Utils.h"
#include "Packet.h"
#include <list>

typedef void(*SOCK_CALLBACK)(void*,int,std::list<CPacket>&,const CPacket&);
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
	int Run(SOCK_CALLBACK call_back, void* arg) {
		m_callback = call_back;
		m_arg = arg;
		if (this->InitializeSocket() == false) return -1;
		std::list<CPacket> lstPackets;
		int count = 0;
		while (true) {
			if (this->AcceptClient() == false) {
				if (count >= 3) return -2;
				count++;
			}
			int ret = this->DealCommand();
			if (ret > 0) {
				m_callback(m_arg, ret, lstPackets,this->m_packet);
				while (lstPackets.size() > 0) {
					this->Send(lstPackets.front());
					lstPackets.pop_front();
				}
			}
			this->CloseClient();
		}
	}
	bool AcceptClient() {
		sockaddr_in client_addr;
		
		int cli_sz = sizeof(client_addr);
		m_client = accept(m_sock, (sockaddr*)&client_addr, &cli_sz);
		TRACE(" m_client : %d \r\n", m_client);
		if (m_client == -1) return false;
		return true;
	}
#define  BUFFER_SIZE 4096
	int DealCommand() {
		TRACE("Server DealCommand inside!\r\n");
		if (m_client == -1) return false;
		//char buffer[1024] = {};
		char* buffer = new char[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		size_t  idx = 0;
		while (true) {
			size_t len = recv(m_client, buffer + idx, BUFFER_SIZE - idx, 0);
			TRACE("recv data len : %d \r\n", len);
			if (len <= 0) {
				delete[] buffer;
				return -1; 
			}
			idx += len;
			len = idx;
			m_packet = CPacket((BYTE*)buffer, len);
			TRACE("[Client command id : %d] \r\n", m_packet.wdCmd);
			if (len > 0) {
				memmove(buffer, buffer + len, BUFFER_SIZE - len);
				idx -= len;
				TRACE("Server DealCommand result: %d \r\n", m_packet.wdCmd);
				delete[] buffer;
				return m_packet.wdCmd;
			}
		}
		delete[] buffer;
		return -1;
		

	}
	bool Send(const char* pData, int nSize) {
		CUtils::Dump((BYTE*)pData, nSize);
		if (m_client == -1) return false;
		return send(m_client, pData, nSize, 0) > 0;

	}
	bool Send( CPacket& pack) {
		if (m_client == -1) return false;
		return send(m_client, pack.Data(), pack.Size(), 0) > 0;

	}
	void CloseClient() {
		if(m_client != INVALID_SOCKET)
			closesocket(m_client);
		m_client = INVALID_SOCKET;
	}
private:
	
	SOCK_CALLBACK m_callback;
	void* m_arg;
	SOCKET m_sock,m_client;
	CPacket m_packet;
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

