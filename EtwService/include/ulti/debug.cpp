#include "debug.h"
#include "support.h"

namespace debug {

    std::mutex mt;
    std::wofstream outfile;
    int debug_cnt = 0;

    void InitDebugLog() {
        try {
            if (outfile.is_open()) {
                outfile.close();
            }
            outfile.open(LOG_PATH, std::ios_base::app);
            debug_cnt = 0;
        }
        catch (...) {
        }
    }

    void CleanupDebugLog() {
        try {
            outfile.close();
        }
        catch (...) {
        }
    }

    void WriteDebugToFileW(const std::wstring& s) {
        std::lock_guard<std::mutex> lock(mt);
        try {
            if (!outfile.is_open()) {
                InitDebugLog();
            }
            debug_cnt++;
            if (!s.empty() && s.back() == L'\n') {
                outfile << s;
            }
            else {
                outfile << s << L"\n";
            }
            outfile.flush();
            if (debug_cnt >= DEBUG_LOG_THRESHOLD) {
                CleanupDebugLog();
                InitDebugLog();
            }
        }
        catch (...) {
        }
    }

    void DebugPrintW(const wchar_t* pwsz_format, ...) {
        if (pwsz_format == nullptr) return;

        std::wstring log;
        log.resize(16384 * 512);

        va_list args;
        va_start(args, pwsz_format);
        int len = 0;
        while (true) {
            try {
                len = vswprintf_s(&log[0], log.size(), pwsz_format, args);
            }
            catch (...) {
                if (errno == ERANGE) {
                    log.resize(log.size() * 2);
                    continue;
                }
            }
            break;
        }
        if (len > 0) {
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

    std::wstring GetErrorMessage(DWORD errorCode) {
        LPWSTR messageBuffer = nullptr;
        size_t size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer, 0, nullptr);

        std::wstring message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
    }

    bool PopUpMessageBox(const std::wstring& title, const std::wstring& message) {
        if (ulti::IsRunningAsSystem()) {
            DWORD session_id = ulti::GetCurrentSessionId();

            if (!session_id) {
                DebugPrintW(L"PopUpServiceMessageBox: No active sessions");
                return false;
            }
            DWORD response = NULL;
            auto status = WTSSendMessageW(WTS_CURRENT_SERVER_HANDLE, session_id, (LPWSTR)title.c_str(),
                title.size() * sizeof(WCHAR), (LPWSTR)message.c_str(),
                message.size() * sizeof(WCHAR), MB_ICONINFORMATION | MB_OK, 0, &response, FALSE);
            if (!status) {
                PrintDebugW(L"WTSSendMessageW err %d", GetLastError());
                return false;
            }
            return true;
        }
        else {
            auto ret = MessageBoxW(NULL, message.c_str(), title.c_str(), MB_ICONINFORMATION | MB_OK);
            if (ret != IDOK) {
                PrintDebugW(L"MessageBoxW err %d", GetLastError());
                return false;
            }
            return true;
        }
        return false;
    }

}  // namespace debug
