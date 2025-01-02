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
        try
        {
			if (outfile.is_open())
			{
				outfile.close();
			}
			outfile.open(LOG_PATH, std::ios_base::app);
			debug_cnt = 0;
		}
		catch (...)
		{
        }
    }

    inline void CleanupDebugLog()
    {
        try
        {
            outfile.close();
        }
        catch (...)
        {

        }
    }

    inline void WriteDebugToFileW(const std::wstring& s)
    {
        mt.lock();
        try {
            if (outfile.is_open() == false)
            {
                InitDebugLog();
            }
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
        }
		catch (...)
		{
		}
        mt.unlock();
    }

#define PrintDebugW(str, ...) debug::DebugPrintW((std::wstring(L"[") + __FUNCTIONW__ + L":" + std::to_wstring(__LINE__) + L"] " + str).c_str(), __VA_ARGS__)

    inline void DebugPrintW(const wchar_t* pwsz_format, ...)
    {
        if (pwsz_format == nullptr) return;

        std::wstring log;
        log.resize(16384 * 512);

        va_list args;
        va_start(args, pwsz_format);
        int len = 0;
        while (true)
        {
            try {
                len = vswprintf_s(&log[0], log.size(), pwsz_format, args);
            }
            catch (...)
            {
                if (errno == ERANGE)
                {
                    log.resize(log.size() * 2);
                    continue;
                }
            }
            break;
        }
        if (len > 0)
        {
            SYSTEMTIME time;
            GetLocalTime(&time);

            wchar_t time_str[64];
            swprintf_s(time_str, 64, L"[%d/%02d/%02d - %02d:%02d:%02d][HieuEtwService] ",
                time.wYear, time.wMonth, time.wDay,
                time.wHour, time.wMinute, time.wSecond);

            log.insert(0, time_str);
            OutputDebugStringW(log.c_str());
			WriteDebugToFileW(log.c_str());
        }

        va_end(args);
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