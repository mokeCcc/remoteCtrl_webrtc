#pragma once
#include "pch.h"
#pragma pack(push)
#pragma pack(1)
class CPacket
{
public:
	CPacket() :wdHead(0), dwLength(0), wdCmd(0), wdSumCheck(0) {}
	CPacket(WORD nCmd, const BYTE* pData, size_t  nSize) {
		wdHead = 0xfeff;
		dwLength = nSize + 4;
		wdCmd = nCmd;
		if (nSize > 0) {
			strData.resize(nSize);
			memcpy((void*)strData.c_str(), pData, nSize);
		}
		else {
			strData.clear();
		}
		wdSumCheck = 0;
		for (size_t j = 0; j < strData.size(); j++) {
			wdSumCheck += (BYTE)(strData[j]) & 0xff;
		}
	}
	CPacket(const CPacket& pack) {
		wdHead = pack.wdHead;
		dwLength = pack.dwLength;
		wdCmd = pack.wdCmd;
		strData = pack.strData;
		wdSumCheck = pack.wdSumCheck;
	}
	int Size() {
		return dwLength + 6;
	}
	const char* Data() { // FF FE 09 00 00 00 01 00 43 2C 44 2C 45 24 01 
		strOut.resize(dwLength + 6);
		BYTE* pData = (BYTE*)strOut.c_str();
		*(WORD*)pData = wdHead;				// FF FE 
		*(DWORD*)(pData + 2) = dwLength;	// 09 00 00 00
		*(WORD*)(pData + 6) = wdCmd;		// 01 00
		memcpy(pData + 8, strData.c_str(), strData.size()); // 43 2C 44 2C 45
		*(WORD*)(pData + 8 + strData.size()) = wdSumCheck; // 
		return strOut.c_str();
	}

	CPacket& operator=(const CPacket& pack) {
		if (this != &pack) {
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
		for (i = 0; i < nSize; i++) { //由于使用TCP buffer 开头不一定是packet开头 所以需要去找包头
			if (*(WORD*)(pData + i) == 0xFEFF) {
				wdHead = *(WORD*)(pData + i);
				i += 2;
				break;
			}
		}
		if (i + 8 > nSize) { nSize = 0; return; }  //0x8 is dwLength AND wdHead AND wdCmd
		dwLength = *(DWORD*)(pData + i); i += 4;

		if (dwLength + i > nSize) { // packet receive not 
			nSize = 0;
			return;
		}
		wdCmd = *(WORD*)(pData + i); i += 2;
		if (dwLength > 4) {
			strData.resize(dwLength - 4);
			memcpy((void*)strData.c_str(), pData + 8, dwLength - 4);
			i += dwLength - 4;
		}
		wdSumCheck = *(WORD*)(pData + i);
		for (unsigned int j = 0; j < strData.size(); j++)
		{
			wdSumCheck -= BYTE(strData[j]) & 0xff;
		}
		if (wdSumCheck == 0) {
			nSize = i;  // length4  head 2 a and data
			return;
		}
		nSize = 0;
	}
	~CPacket() {}

	WORD wdHead;		  //fix bytes FE FF
	DWORD dwLength;		  //packet length( packet command and data)
	WORD  wdCmd;		  // packet command
	std::string strData;  // packet data 
	WORD wdSumCheck;
	std::string strOut;


private:
};
#pragma  pack (pop)

typedef struct file_info {
	file_info() {
		IsInvalid = false;
		IsDirectory = -1;
		hasNext = true;
		memset(szFileName, 0, sizeof(szFileName));
	}
	bool IsInvalid; // invalid
	bool hasNext;
	char szFileName[256]; //file name
	bool IsDirectory;   //directory or file
}FILEINFO, * PFILEINFO;

typedef struct MouseEvent {
	MouseEvent() {
		nAction = 0;
		nButton = -1;
		ptXY.x = 0;
		ptXY.y = 0;
	}
	WORD nAction; // Click(1) move(2) dounle Click(4) 
	WORD nButton;// left(1)  right(2) mid(4)
	POINT ptXY;
}MOUSENV, * PMOUSENV;