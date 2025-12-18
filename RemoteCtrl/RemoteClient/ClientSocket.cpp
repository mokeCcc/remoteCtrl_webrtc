#include "pch.h"
#include "ClientSocket.h"


CClientSocket* CClientSocket::m_instance = nullptr;
CClientSocket::CHelper CClientSocket::m_helper;

std::string GetwsaErrInfo(int wsaErrorCode) {
	std::string ret;
	LPVOID lpMessageBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		wsaErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMessageBuf, 0, NULL
	);
	ret = (char*)lpMessageBuf;
	LocalFree(lpMessageBuf);
	return ret;
}
