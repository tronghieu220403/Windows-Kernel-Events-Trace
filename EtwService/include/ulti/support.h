#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#ifndef ETWSERVICE_ULTI_SUPPORT_H_
#define ETWSERVICE_ULTI_SUPPORT_H_

#include "include.h"

#define MAIN_DIR L"E:\\hieunt20210330\\"
#define TEMP_DIR L"E:\\hieunt20210330\\temp\\"
#define SERVICE_NAME L"HieuntEtwService"
#define TRID_PATH L"E:\\hieunt20210330\\Trid\\trid.exe"

namespace ulti
{
    std::wstring StrToWStr(const std::string& str);
    std::string CharVectorToString(const std::vector<char>& v);
    std::vector<char> StringToVectorChar(const std::string& s);
    std::vector<unsigned char> StringToVectorUChar(const std::string& s);
    std::string WstrToStr(const std::wstring& wstr);
    std::wstring ToLower(const std::wstring& wstr);

    bool CreateDir(const std::wstring& dir_path);

    bool CheckPrintableUTF16(const std::vector<unsigned char>& buffer);

    bool CheckPrintableUTF16(const std::wstring& wstr);

    bool CheckPrintableUTF8(const std::vector<unsigned char>& buffer);

    bool CheckPrintableUTF8(const std::wstring& wstr);

    bool CheckPrintableANSI(const std::vector<unsigned char>& buffer);

    bool CheckPrintableANSI(const std::wstring& wstr);

    std::wstring ExecCommand(const std::wstring& cmd);

    bool IsRunningAsSystem();

    DWORD GetCurrentSessionId();

}

#endif
