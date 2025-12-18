// RemoteCtrl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"
#include "CServerSocket.h"
#include <list>
#include <io.h>
#include <atlimage.h> 
#include <stdio.h>
#include <direct.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object
// branch main
CWinApp theApp;
VOID Dump(BYTE* pData, unsigned int  nSize) {
    std::string strOut;
    for (unsigned int  i = 0; i < nSize; i++) {
        char buf[8] = {};
        if (i > 0 && (i % 16 == 0)) strOut += "\n";
        snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xff);
        strOut += buf;
    }
    strOut += "\n";
    OutputDebugStringA(strOut.c_str());
}
int  MakeDriverInfo() {
   std::string res;
   for (int i = 1; i< 26; i++)
   {
       if(_chdrive(i)  == 0){
           if (res.size() > 0) res += ',';
           res += 'A' + i - 1;
       }
   }
   CPacket packet(1, (BYTE*)res.c_str(), res.size());
   Dump((BYTE*)packet.Data(), packet.Size());
  // CServerSocket::getInstance()->Send(packet);
   return 1;
}
typedef struct file_info{
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
}FILEINFO,*PFINEINFO;

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
        finfo.IsInvalid = true;
        finfo.IsDirectory = true;
        finfo.hasNext = false;
        memcpy(finfo.szFileName, strPath.c_str(), strPath.size());
       // lstFileInfos.push_back(finfo);
        CPacket pack(2,(BYTE*) & finfo, sizeof(finfo));
        CServerSocket::getInstance()->Send(pack);
        return -2;
    }// can not to access dir
    _finddata_t fdata;
    int hfind = 0;
    if ((hfind = _findfirst("*", &fdata))== -1) {
        OutputDebugString(_T("can not find any file"));
        return -3;
    }
    do {
        FILEINFO finfo; 
        finfo.IsDirectory = (fdata.attrib & _A_SUBDIR) != 0;
       // finfo.IsInvalid = false;
        memcpy(finfo.szFileName, fdata.name, strlen(fdata.name));
		CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
		CServerSocket::getInstance()->Send(pack);
      //  lstFileInfos.push_back(finfo);
    } while (!_findnext(hfind,&fdata));

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
	CPacket pack(3, NULL,0);
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
    fseek(pFile, 0, SEEK_SET);
    char buffer[1024] = {};
    size_t rlen = 0;
    do{
        rlen = fread(buffer, 1, 1024, pFile);
        CPacket pack(4, (BYTE*)buffer, rlen);
        CServerSocket::getInstance()->Send(pack);
    } while (rlen >= 1024);

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
int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);
    
    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
             /*
            CServerSocket* pserver = CServerSocket::getInstance();
            int count = 0;
            if (!pserver->InitializeSocket()) {
                MessageBox(NULL, _T("network initialized error!"), _T("network  initialized error!"), MB_OK | MB_ICONERROR);
                exit(0);
            }
            while (CServerSocket::getInstance() != NULL) {
                if (pserver->AcceptClient() == false) {
                    if (count >= 3) exit(0);
                    MessageBox(NULL, _T("can not accept client,reagian!"), _T("accept client error"), MB_OK | MB_ICONERROR);
                    count++;
                }
                int ret = pserver->DealCommand();
            }*/
            int Cmd = 5;
            switch (Cmd)
            {
            case 1:
                MakeDriverInfo();
            	break;
            case 2:
                MakeDirectoryInfo();
                break;
            case 3:
                RunFile();
                break;
            case 4:
                DownloadFile();
                break;
            case 5:
                SendScreen();
                break;
            case 6:
                LockMachine();
                break;
            case 7:
                UnlockMachine();
                break;

            }
           
            
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
