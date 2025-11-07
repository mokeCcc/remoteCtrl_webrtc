// RemoteCtrl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"
#include "CServerSocket.h"
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
std::string MakeDriverInfo() {
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
