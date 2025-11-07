// RemoteCtrl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"
#include "CServerSocket.h"
#include <list>
#include <io.h>
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
            int Cmd = 1;
            switch (Cmd)
            {
            case 1:
                MakeDriverInfo();
            	break;
            case 2:
                MakeDirectoryInfo();
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
