#pragma once
#include "framework.h"
class CUtils
{
public:
   static  VOID Dump(BYTE* pData, unsigned int  nSize) {
        std::string strOut;
        for (unsigned int i = 0; i < nSize; i++) {
            char buf[8] = {};
            if (i > 0 && (i % 16 == 0)) strOut += "\n";
            snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xff);
            strOut += buf;
        }
        strOut += "\n";
        OutputDebugStringA(strOut.c_str());
    }
};

