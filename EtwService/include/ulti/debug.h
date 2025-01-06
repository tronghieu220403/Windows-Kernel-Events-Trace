#pragma once

#ifndef ETWSERVICE_ULTI_DEBUG_H_
#define ETWSERVICE_ULTI_DEBUG_H_

#include "include.h"

#define DEBUG_LOG_THRESHOLD 1000
#define LOG_PATH L"E:\\hieunt20210330\\log.txt"

namespace debug {

    // Initializes the debug log file.
    void InitDebugLog();

    // Cleans up the debug log file.
    void CleanupDebugLog();

    // Writes a debug message to the log file.
    void WriteDebugToFileW(const std::wstring& s);

    // Logs a formatted debug message.
    void DebugPrintW(const wchar_t* pwsz_format, ...);

    // Retrieves a formatted error message from a Windows error code.
    std::wstring GetErrorMessage(DWORD errorCode);

    // Displays a message box, depending on the session context.
    bool PopUpMessageBox(const std::wstring& title, const std::wstring& message);

}  // namespace debug

#define PrintDebugW(str, ...) \
    debug::DebugPrintW((std::wstring(L"[") + __FUNCTIONW__ + L":" + std::to_wstring(__LINE__) + L"] " + str).c_str(), __VA_ARGS__)

#endif