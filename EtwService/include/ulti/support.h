#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#ifndef ETWSERVICE_ULTI_SUPPORT_H_
#define ETWSERVICE_ULTI_SUPPORT_H_
#include <Windows.h>
#include <iphlpapi.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <wmistr.h>
#include <evntrace.h>
#include <evntcons.h>
#include <comutil.h>
#include <wbemidl.h>
#include <winternl.h>

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
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <list>
#include <regex>
#include <array>
#include <locale>
#include <cctype>
#include <codecvt>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "ntdll.lib")

#pragma warning(disable : 26110)

typedef unsigned int uint32;

#define FlagOn(_Flag, _Bit)  (((_Flag) & (_Bit)) != 0)

#define ZeroMem(data,size) ZeroMemory(data,size)

#define GetCurrentDir _getcwd

namespace fs = std::filesystem;

#define MAIN_DIR L"E:\\hieunt20210330\\"
#define TEMP_DIR L"E:\\hieunt20210330\\temp\\"
#define LOG_PATH L"E:\\hieunt20210330\\log.txt"
#define SERVICE_NAME L"HieuntEtwService"
#define TRID_PATH L"E:\\hieunt20210330\\Trid\\trid.exe"

namespace ulti
{
    std::wstring StrToWStr(const std::string& str);
    std::string CharVectorToString(const std::vector<char>& v);
    std::vector<char> StringToVectorChar(const std::string& s);
    std::vector<unsigned char> StringToVectorUChar(const std::string& s);
    std::string WstrToStr(const std::wstring& wstr);

    bool CreateDir(const std::wstring& dir_path);

    bool CheckPrintableUTF16(const std::vector<unsigned char>& buffer);

    bool CheckPrintableUTF16(const std::wstring& wstr);

    bool CheckPrintableUTF8(const std::vector<unsigned char>& buffer);

    bool CheckPrintableUTF8(const std::wstring& wstr);

    bool CheckPrintableANSI(const std::vector<unsigned char>& buffer);

    bool CheckPrintableANSI(const std::wstring& wstr);

    std::wstring ExecCommand(const std::wstring& cmd);

    bool IsRunningAsSystem();
}

#endif
