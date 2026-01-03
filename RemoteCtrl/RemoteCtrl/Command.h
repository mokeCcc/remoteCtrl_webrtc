#pragma once
#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"
#include "CServerSocket.h"
#include "Utils.h"
#include<map>
#include <list>
#include <io.h>
#include <direct.h>
#include <atlimage.h> 
class CCommand
{
public:
	CCommand();
	~CCommand(){

	}
protected:
	typedef int(CCommand::* CMDFUNC)();
	std::map<int, CMDFUNC> m_mapFunction;

	int MakeDriverInfo() {
		std::string res;
		for (int i = 1; i < 26; i++)
		{
			if (_chdrive(i) == 0) {
				if (res.size() > 0) res += ',';
				res += 'A' + i - 1;
			}
		}
		res += ',';
		CPacket packet(1, (BYTE*)res.c_str(), res.size());
		CUtils::Dump((BYTE*)packet.Data(), packet.Size());
		CServerSocket::getInstance()->Send(packet);
		return 0;
	}
    int SendScreen() {
        // TODO: multi screen 
        CImage  screen;
        HDC hScreen = GetDC(NULL);
        int nWidth = GetSystemMetrics(SM_CXSCREEN);
        int nHeight = GetSystemMetrics(SM_CYSCREEN);
        int nBitPerPixel = GetDeviceCaps(hScreen, BITSPIXEL);

        screen.Create(nWidth, nHeight, nBitPerPixel);
        HDC hImgScreen = screen.GetDC();
        BitBlt(hImgScreen, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);
        IStream* pStream = nullptr;

        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
        if (hMem == NULL) return -1;

        HRESULT hr = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
        if (FAILED(hr)) return -2;
        screen.Save(pStream, Gdiplus::ImageFormatJPEG);
        //screen.Save(_T("TEST20125.jpg"), Gdiplus::ImageFormatJPEG);
        LARGE_INTEGER liZero = { 0 };
        pStream->Seek(liZero, STREAM_SEEK_SET, nullptr);
        PBYTE pData = (PBYTE)GlobalLock(hMem);
        SIZE_T nSz = GlobalSize(hMem);
        CPacket pack(5, pData, nSz);
        CServerSocket::getInstance()->Send(pack);
        //Dump(pData, nSz);

        GlobalUnlock(hMem);
        pStream->Release();
        ReleaseDC(NULL, hScreen);
        screen.ReleaseDC();
        return 0;
    }
    int MakeDirectoryInfo() {
        std::string strPath;
        //  std::list<FILEINFO> lstFileInfos;
        if (!CServerSocket::getInstance()->GetFilePath(strPath)) return -1;  //command phrase error

        if (_chdir(strPath.c_str()) != 0) {
            FILEINFO finfo;
            //finfo.IsInvalid = true;
            //finfo.IsDirectory = true;
            finfo.hasNext = false;
            // memcpy(finfo.szFileName, st rPath.c_str(), strPath.size());
           // lstFileInfos.push_back(finfo);
           // TRACE("[%s] isdir: %d", finfo.szFileName, finfo.IsDirectory);
            OutputDebugString(_T("has no valid  to access the directory!"));
            CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
            CServerSocket::getInstance()->Send(pack);
            return -2;
        }// can not to access dir
        _finddata_t fdata;
        intptr_t hfind = 0;
        hfind = _findfirst("*", &fdata);
        if (hfind == -1) {
            OutputDebugString(_T("can not find any file"));
            FILEINFO finfo;
            finfo.hasNext = false;
            CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
            CServerSocket::getInstance()->Send(pack);
            return -3;
        }

        do {
            FILEINFO finfo;
            //ZeroMemory(&finfo, sizeof(finfo));
            finfo.IsDirectory = (fdata.attrib & _A_SUBDIR) != 0;
            // finfo.IsInvalid = false;
            memcpy(finfo.szFileName, fdata.name, strlen(fdata.name));
            TRACE("transport :[%s] \r\n", finfo.szFileName);
            CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
            CServerSocket::getInstance()->Send(pack);
            //  lstFileInfos.push_back(finfo);
        } while (_findnext(hfind, &fdata) == 0);

        FILEINFO finfo;
        finfo.hasNext = false;
        CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->Send(pack);
        return 0;
    }
    int RunFile() {
        std::string strPath;
        CServerSocket::getInstance()->GetFilePath(strPath);
        ShellExecuteA(NULL, NULL, strPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        CPacket pack(3, NULL, 0);
        CServerSocket::getInstance()->Send(pack);
        return 0;
    }
    int DownloadFile() {
        std::string strPath;
        LONGLONG  data = 0;
        CServerSocket::getInstance()->GetFilePath(strPath);
        FILE* pFile = NULL;
        errno_t err = fopen_s(&pFile, strPath.c_str(), "rb");
        if (err != 0 || pFile == NULL) {
            CPacket pack(4, (BYTE*)&data, 8);
            CServerSocket::getInstance()->Send(pack);
            return -1;
        }
        fseek(pFile, 0, SEEK_END);
        data = _ftelli64(pFile);
        CPacket head(4, (BYTE*)&data, 8);
        CServerSocket::getInstance()->Send(head);
        fseek(pFile, 0, SEEK_SET);
        char buffer[1024] = {};
        size_t rlen = 0;
        size_t send_len = 0;
        do {
            rlen = fread(buffer, 1, 1024, pFile);
            send_len += rlen;
            CPacket pack(4, (BYTE*)buffer, rlen);
            CServerSocket::getInstance()->Send(pack);
        } while (rlen >= 1024);
        TRACE("[Send file  length : %lld, this file size : %lld] \r\n", send_len, data);
        CPacket pack(4, NULL, 0);
        CServerSocket::getInstance()->Send(pack);
        fclose(pFile);
        return 0;
    }
    int LockMachine() {
        return 0;
    }
    int UnlockMachine() {
        return 0;

    }
    int MouseEvent() {
        MOUSENV event;
        WORD nCode = 0;
        if (CServerSocket::getInstance()->GetMouseEvent(event)) {
            SetCursorPos(event.ptXY.x, event.ptXY.y);
            TRACE("[Mouse Control xPos: %d yPos:%d]\r\n", event.ptXY.x, event.ptXY.y);
            switch (event.nAction) {
            case 0:  //Click
                nCode |= 0;
                break;
            case 1:     //double Click
                nCode |= 1;
                break;
            case 2:   // UP
                nCode |= 2;
                break;
            case 4:  //move
                nCode |= 4;
                break;
            }
            switch (event.nButton) {
            case 0:
                nCode |= 8;
                break;
            case 1:
                nCode |= 16;
                break;
            case 2:
                nCode |= 32;
                break;
            }
            switch (nCode) {
            case 8:
                mouse_event(MOUSEEVENTF_LEFTDOWN, event.ptXY.x, event.ptXY.y, 0, 0);
                break;
            case 9:
                mouse_event(MOUSEEVENTF_LEFTDOWN, event.ptXY.x, event.ptXY.y, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, event.ptXY.x, event.ptXY.y, 0, 0);
                Sleep(10);
                mouse_event(MOUSEEVENTF_LEFTDOWN, event.ptXY.x, event.ptXY.y, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, event.ptXY.x, event.ptXY.y, 0, 0);
                break;
            case 10:
                mouse_event(MOUSEEVENTF_LEFTUP, event.ptXY.x, event.ptXY.y, 0, 0);
                break;
            case 16:
                mouse_event(MOUSEEVENTF_RIGHTDOWN, event.ptXY.x, event.ptXY.y, 0, 0);
                //mouse_event(MOUSEEVENTF_RIGHTUP, event.ptXY.x, event.ptXY.y, 0, 0);

                break;
            case 17:
                mouse_event(MOUSEEVENTF_RIGHTDOWN, event.ptXY.x, event.ptXY.y, 0, 0);
                mouse_event(MOUSEEVENTF_RIGHTUP, event.ptXY.x, event.ptXY.y, 0, 0);
                Sleep(10);
                mouse_event(MOUSEEVENTF_RIGHTDOWN, event.ptXY.x, event.ptXY.y, 0, 0);
                mouse_event(MOUSEEVENTF_RIGHTUP, event.ptXY.x, event.ptXY.y, 0, 0);
                break;
            default:
                break;
            }
        }
        return 0;
    }
    int TestConnect() {
        TRACE("TestConnect send command id: 1981\r\n");
        CPacket packet(1981, NULL, 0);
        CServerSocket::getInstance()->Send(packet);
        return 0;
    }
    int DeleteFileByCommand() {
        std::string strPath;
        CServerSocket::getInstance()->GetFilePath(strPath);
        DeleteFile(strPath.c_str());
        CPacket pack(9, NULL, 0);
        CServerSocket::getInstance()->Send(pack);
        return 0;
    }
    int ExcuteCommand(WORD nCmd) {
        TRACE("ExcuteCommand!  command id: %d\r\n", nCmd);
        int ret = 0;
        switch (nCmd)
        {
        case 1:
            ret = MakeDriverInfo();
            break;
        case 2:
            ret = MakeDirectoryInfo();
            break;
        case 3:
            ret = RunFile();
            break;
        case 4:
            ret = DownloadFile();
            break;
        case 5:
            ret = SendScreen();
            break;
        case 6:
            ret = LockMachine();
            break;
        case 7:
            ret = UnlockMachine();
            break;
        case 1981:
            ret = TestConnect();
            break;
        case 8:
            ret = DeleteFileByCommand();
            break;
        case 9:
            ret = MouseEvent();
            break;
        }

        return ret;
    }
};

