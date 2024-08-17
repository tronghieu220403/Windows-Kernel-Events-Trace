#pragma once
#define _CRT_SECURE_NO_DEPRECATE

#ifndef ETWSERVICE_ULTI_SUPPORT_H_
#define ETWSERVICE_ULTI_SUPPORT_H_

#include <string.h>
#include <time.h>

#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <direct.h>
#include <stdio.h>
#include <conio.h>
#include <strsafe.h>
#include <tchar.h>
#include <locale.h>
#include <Windows.h>
#include <Shlobj.h>
#include <Pdh.h>
#include <PdhMsg.h>
#include <wmistr.h>
#include <evntrace.h>
#include <evntcons.h>
#include <comutil.h>
#include <wbemidl.h>
#include <aclapi.h>    

#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <functional>
#include <chrono>
#include <deque>
#include <syncstream>
#include <mutex>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Shell32.lib") 
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib,"Pdh.lib")
#pragma comment(lib,"Advapi32.lib")

typedef unsigned int uint32;

#define ZeroMem(data,size) ZeroMemory(data,size)

#define GetCurrentDir _getcwd

namespace etw
{
    inline std::mutex mt;
    inline void WriteDebugA(std::string s)
    {
    #ifdef _DEBUG
        mt.lock();
        std::ofstream outfile("C:\\debug.txt", std::ios_base::app);
        outfile << s << "\n";
        outfile.close();
        mt.unlock();
    #endif // DEBUG
    }
    inline void WriteDebugW(std::wstring s)
    {
    #ifdef _DEBUG
        mt.lock();
        std::wofstream outfile("C:\\debug.txt", std::ios_base::app);
        outfile << s << "\n";
        outfile.close();
        mt.unlock();
    #endif // DEBUG
    }


    inline std::string WStrToStr(const std::wstring& w_str)
    {
        return std::string(w_str.begin(), w_str.end());
    }

    inline std::wstring StrToWStr(const std::string& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    inline std::string CharVectorToString(std::vector<char> v)
    {
        return std::string(v.begin(), v.end());
    }

    inline std::vector<char> StringToVectorChar(std::string s)
    {
        return std::vector<char>(s.begin(), s.end());
    }

}

#endif
