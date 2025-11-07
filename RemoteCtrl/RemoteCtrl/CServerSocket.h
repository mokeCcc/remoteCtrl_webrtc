#pragma once
#include "pch.h"
//#include "framework.h"




class CPacket
{
public:
	CPacket() :wdHead(0), dwLength(0), wdCmd(0), wdSumCheck(0) {}

	CPacket(const CPacket& pack) {
		wdHead = pack.wdHead;
		dwLength = pack.dwLength;
		wdCmd = pack.wdCmd;
		strData = pack.strData;
		wdSumCheck = pack.wdSumCheck;
	}

	CPacket& operator=(const CPacket& pack) {
		if (this != &pack){
			wdHead = pack.wdHead;
			dwLength = pack.dwLength;
			wdCmd = pack.wdCmd;
			strData = pack.strData;
			wdSumCheck = pack.wdSumCheck;
		}
		return *this;
	}
	CPacket(const BYTE* pData, size_t& nSize) {
		size_t i;
		for (i = 0; i < nSize; i++) {
			if (*(WORD*)(pData + i) == 0xFEFF) {
				wdHead = *(WORD*)(pData + i);
				i += 2;
				break;
			}
		}
		if (i + 8 >= nSize) { nSize = 0; return; }  //0x8 is dwLength AND wdHead AND wdCmd
		dwLength = *(DWORD*)(pData + i); i += 4;
		wdCmd = *(WORD*)(pData + i); i += 2;
		if (dwLength + i > nSize) { // packet receive not 
			nSize = 0;
			return;
		}

		if (dwLength > 4) {
			strData.resize(dwLength - 4);
			memcpy((void*)strData.c_str(), pData + 6, dwLength - 4);
			i += dwLength - 4;
		}
		wdSumCheck = *(WORD*)(pData + i); i += 2;
		for (size_t j =0; j<strData.size();j++)
		{
			wdSumCheck -= BYTE(strData[i]) & 0xff;
		}
		if (wdSumCheck == 0) {
			nSize = i;  // length4  head 2 a and data

		}
		nSize = 0;
	}
	~CPacket(){}

	WORD wdHead;		  //fix bytes FE FF
	DWORD dwLength;		  //packet length( packet command and data)
	WORD  wdCmd;		  // packet command
	std::string strData;  // packet data 
	WORD wdSumCheck;

private:
};



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
#define  BUFFER_SIZE 4096
	int DealCommand() {
		if (m_client == -1) return false;
		//char buffer[1024] = {};
		char* buffer = new char[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		size_t idx = 0;
		while (true) {
			size_t len = recv(m_sock, buffer + idx, BUFFER_SIZE - idx, 0);
			if (len <= 0) return -1;
			idx += len;
			len = idx;
			m_packet = CPacket((BYTE*)buffer, len);
			if (len > 0) {
				memmove(buffer, buffer + len, BUFFER_SIZE - len);
				idx -= len;
				return m_packet.wdCmd;
			}
		}
		return -1;
		

	}
	bool Send(const char* pData, int nSize) {
		if (m_client == -1) return false;
		return send(m_client, pData, nSize, 0) > 0;

	}
private:
	
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

