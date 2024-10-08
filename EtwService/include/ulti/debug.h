#pragma once

#include "support.h"

#define DEBUG_LOG_THRESHOLD 1000

namespace debug
{
    inline std::mutex mt;

    inline std::wofstream outfile;
    inline int debug_cnt = 0;
    inline void InitDebugLog()
    {
        outfile.open("C:\\debug.txt", std::ios_base::app);
        debug_cnt = 0;
    }

    inline void CleanupDebugLog()
    {
        outfile.close();
    }

    inline void DebugLogW(const std::wstring& s)
    {
        mt.lock();
        debug_cnt++;
        if (s.at(s.length() - 1) == L'\n')
            outfile << s;
        else
            outfile << s << "\n";
        outfile.flush();
        if (debug_cnt >= DEBUG_LOG_THRESHOLD)
        {
            CleanupDebugLog();
            InitDebugLog();
        }
        mt.unlock();
    }

    inline std::wstring GetErrorMessage(DWORD errorCode) {
        LPWSTR messageBuffer = nullptr;
        size_t size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer, 0, nullptr);

        std::wstring message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
    }
}