#include "support.h"
#include "debug.h"

namespace ulti
{
    std::wstring StrToWStr(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(str);
    }

    std::string CharVectorToString(const std::vector<char>& v)
    {
        return std::string(v.begin(), v.end());
    }

    std::vector<char> StringToVectorChar(const std::string& s)
    {
        return std::vector<char>(s.begin(), s.end());
    }

    std::vector<unsigned char> StringToVectorUChar(const std::string& s)
    {
        return std::vector<unsigned char>(s.begin(), s.end());
    }

    std::string WstrToStr(const std::wstring& wstr)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> myconv;
        return myconv.to_bytes(wstr);
    }

    bool CreateDir(const std::wstring& dir_path)
    {
        BOOL status = ::CreateDirectory(dir_path.c_str(), NULL);
        if (status == TRUE || GetLastError() == ERROR_ALREADY_EXISTS)
        {
            return true;
        }
        else if (GetLastError() == ERROR_PATH_NOT_FOUND)
        {
            try {
                fs::create_directories(dir_path);
            }
            catch (const fs::filesystem_error&) {
                return false;
            }
            return true;
        }
        else {
            return false;
        }

        return false;
    }

    bool CheckPrintableUTF16(const std::vector<unsigned char>& buffer) {
        std::streamsize printable_chars = 0;
        std::streamsize total_chars = 0;

        for (std::size_t i = 0; i + 1 < buffer.size(); i += 2) {
            wchar_t wchar = buffer[i] | (buffer[i + 1] << 8);
            total_chars++;
            if (iswprint(wchar) || iswspace(wchar)) {
                printable_chars++;
            }
        }

        if (total_chars == 0) {
            return true;
        }

        return (static_cast<double>(printable_chars) / total_chars) >= 0.97;
    }

    bool CheckPrintableUTF16(const std::wstring& wstr)
    {
        return CheckPrintableUTF16(StringToVectorUChar(WstrToStr(wstr)));
    }

    bool CheckPrintableUTF8(const std::vector<unsigned char>& buffer) {
        std::streamsize printable_chars = 0;
        std::streamsize total_chars = 0;
        std::size_t i = 0;
        while (i < buffer.size()) {
            unsigned char c = buffer[i];
            if (c < 0x80) { // 1-byte ASCII (7-bit)
                total_chars++;
                if (isprint(c) || isspace(c)) {
                    printable_chars++;
                }
                i++;
            }
            else if ((c & 0xE0) == 0xC0) { // 2-byte UTF-8
                if (i + 1 < buffer.size()) {
                    wchar_t wchar = ((c & 0x1F) << 6) | (buffer[i + 1] & 0x3F);
                    total_chars++;
                    if (iswprint(wchar) || iswspace(wchar)) {
                        printable_chars++;
                    }
                }
                i += 2;
            }
            else if ((c & 0xF0) == 0xE0) { // 3-byte UTF-8
                if (i + 2 < buffer.size()) {
                    wchar_t wchar = ((c & 0x0F) << 12) | ((buffer[i + 1] & 0x3F) << 6) | (buffer[i + 2] & 0x3F);
                    total_chars++;
                    if (iswprint(wchar) || iswspace(wchar)) {
                        printable_chars++;
                    }
                }
                i += 3;
            }
            else if ((c & 0xF8) == 0xF0) { // 4-byte UTF-8
                if (i + 3 < buffer.size()) {
                    wchar_t wchar = ((c & 0x07) << 18) | ((buffer[i + 1] & 0x3F) << 12) | ((buffer[i + 2] & 0x3F) << 6) | (buffer[i + 3] & 0x3F);
                    total_chars++;
                    if (iswprint(wchar) || iswspace(wchar)) {
                        printable_chars++;
                    }
                }
                i += 4;
            }
            else {
                i++; // Skip invalid bytes
            }
        }

        if (total_chars == 0) {
            return true; // If there is no character, return true
        }

        return (static_cast<double>(printable_chars) / total_chars) >= 0.97;
    }

    bool CheckPrintableUTF8(const std::wstring& wstr)
    {
        return CheckPrintableUTF8(StringToVectorUChar(WstrToStr(wstr)));
    }

    bool CheckPrintableANSI(const std::vector<unsigned char>& buffer) {
        std::streamsize printable_chars = 0;
        std::streamsize total_chars = 0;

        for (unsigned char c : buffer) {
            total_chars++;
            if (isprint(c) || isspace(c)) {
                printable_chars++;
            }
        }

        if (total_chars == 0) {
            return true;
        }

        return (static_cast<double>(printable_chars) / total_chars) >= 0.97;
    }

    bool CheckPrintableANSI(const std::wstring& wstr)
    {
        return CheckPrintableANSI(StringToVectorUChar(WstrToStr(wstr)));
    }

    // Execute command and return output
    std::wstring ExecCommand(const std::wstring& cmd) {
        std::wstring result;
        HANDLE stdout_read = nullptr;
        HANDLE stdout_write = nullptr;

        // Security attributes to allow handle inheritance
        SECURITY_ATTRIBUTES security_attributes = {};
        security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
        security_attributes.bInheritHandle = TRUE;
        security_attributes.lpSecurityDescriptor = nullptr;

        // Create a pipe for the child process's standard output
        if (!CreatePipe(&stdout_read, &stdout_write, &security_attributes, 0)) {
            PrintDebugW(L"CreatePipe failed with error %d", GetLastError());
            return result;
        }
        else
        {
            PrintDebugW(L"CreatePipe succeeded");
        }
        defer{
            if (stdout_read != nullptr)
            {
                CloseHandle(stdout_read);
            }
            if (stdout_write != nullptr)
            {
                CloseHandle(stdout_write);
            }
        };

        // Ensure the read handle is not inheritable
        if (!SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0)) {
            PrintDebugW(L"SetHandleInformation failed with error %d", GetLastError());
            return result;
        }
        else
        {
            PrintDebugW(L"SetHandleInformation succeeded");
        }

        // Set up the STARTUPINFO structure
        STARTUPINFOW startup_info = {};
        startup_info.cb = sizeof(STARTUPINFOW);
        startup_info.hStdOutput = stdout_write;
        startup_info.hStdError = stdout_write;
        startup_info.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startup_info.wShowWindow = SW_HIDE;

        PROCESS_INFORMATION process_info = {};

        if (IsRunningAsSystem() == false) {
            // Create the child process
            if (!CreateProcessW(
                nullptr,                            // Application name
                const_cast<LPWSTR>(cmd.c_str()),    // Command line
                nullptr,                            // Process security attributes
                nullptr,                            // Thread security attributes
                TRUE,                               // Inherit handles
                CREATE_NEW_CONSOLE,
                nullptr,                            // Use parent's environment block
                nullptr,                            // Use parent's starting directory
                &startup_info,                      // Pointer to STARTUPINFO
                &process_info))                     // Pointer to PROCESS_INFORMATION
            {
                PrintDebugW(L"CreateProcessW failed with error %d", GetLastError());
                return result;
            }
            defer{
                CloseHandle(process_info.hProcess);
                CloseHandle(process_info.hThread);
            };
        }
        else
        {
            DWORD session_id = GetCurrentSessionId();
            if (session_id == 0)
            {
                return result;
            }
            else
            {
                PrintDebugW(L"Session ID: %d", session_id);
            }

            HANDLE h_token = NULL;
            if (!WTSQueryUserToken(session_id, &h_token))
            {
                PrintDebugW(L"WTSQueryUserToken failed with error %d", GetLastError());
                return result;
            }
            else
            {
                PrintDebugW(L"WTSQueryUserToken succeeded");
            }
            defer{
                CloseHandle(h_token);
            };

            void* env = nullptr;
            if (!CreateEnvironmentBlock(&env, h_token, TRUE))
            {
                PrintDebugW(L"CreateEnvironmentBlock failed with error %d", GetLastError());
                return result;
            }
            else
            {
                PrintDebugW(L"CreateEnvironmentBlock succeeded");
            }
            defer{
                DestroyEnvironmentBlock(env);
            };
            startup_info.lpDesktop = (LPWSTR)L"winsta0\\default";
            DWORD dw_creation_flags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
            std::wstring cmd_editable = cmd;
            if (!CreateProcessAsUserW(
                h_token,                            // User token
                nullptr,                            // Application name
                (LPWSTR)(cmd_editable.c_str()),     // Command line
                nullptr,                            // Process security attributes
                nullptr,                            // Thread security attributes
                TRUE,                               // Inherit handles
                dw_creation_flags,
                env,
                nullptr,                            // Use parent's starting directory
                &startup_info,                      // Pointer to STARTUPINFO
                &process_info))                     // Pointer to PROCESS_INFORMATION
            {
                PrintDebugW(L"CreateProcessAsUserW failed with error %d", GetLastError());
                return result;
            }
            else
            {
                PrintDebugW(L"CreateProcessAsUserW succeeded");
                DWORD exit_code = 0;
                PrintDebugW(L"Process ID: %d", process_info.dwProcessId);
                PrintDebugW(L"Wating for process to finish");
                // Successfully created the process.  Wait for it to finish.
                WaitForSingleObject(process_info.hProcess, INFINITE);

                // Get the exit code.
                result = GetExitCodeProcess(process_info.hProcess, &exit_code);
                PrintDebugW(L"Exit code %d", exit_code);
            }
            defer{
                CloseHandle(process_info.hProcess);
                CloseHandle(process_info.hThread);
            };
        }

        CloseHandle(stdout_write);
        stdout_write = nullptr;
        std::string result_str;

        // Read the child process's output from the pipe
        char buffer[4096];
        DWORD bytes_read = 0;
        while (true)
        {
            if (ReadFile(stdout_read, buffer, sizeof(buffer) - sizeof(char), &bytes_read, nullptr) == TRUE || bytes_read > 0)
            {
                buffer[bytes_read / sizeof(char)] = '\0';  // Null-terminate the string
                result_str += buffer;
                PrintDebugW(L"Read %d bytes", bytes_read);
            }
            else
            {
                //PrintDebugW(L"ReadFile failed with error %d", GetLastError());
                break;
            }
        }
        PrintDebugW(L"Total bytes read %d", result_str.size());
        // Clean up handles
        return ulti::StrToWStr(result_str);
    }

    bool IsRunningAsSystem() {
        HANDLE h_token = NULL;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &h_token)) {
            return false;
        }

        DWORD size = 0;
        TOKEN_USER* token_user = nullptr;
        GetTokenInformation(h_token, TokenUser, NULL, 0, &size);
        token_user = (TOKEN_USER*)malloc(size);

        if (!token_user || !GetTokenInformation(h_token, TokenUser, token_user, size, &size)) {
            if (h_token) CloseHandle(h_token);
            if (token_user) free(token_user);
            return false;
        }

        SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
        PSID system_sid = NULL;
        if (!AllocateAndInitializeSid(&nt_authority, 1, SECURITY_LOCAL_SYSTEM_RID, 0, 0, 0, 0, 0, 0, 0, &system_sid)) {
            if (h_token) CloseHandle(h_token);
            if (token_user) free(token_user);
            return false;
        }

        BOOL is_system = EqualSid(system_sid, token_user->User.Sid);
        FreeSid(system_sid);
        CloseHandle(h_token);
        free(token_user);

        return is_system;
    }

    DWORD GetCurrentSessionId()
    {
        WTS_SESSION_INFO* p_session_info;
        DWORD n_sessions = 0;
        BOOL ok = WTSEnumerateSessions(WTS_CURRENT_SERVER, 0, 1, &p_session_info, &n_sessions);
        if (!ok)
        {
            PrintDebugW(L"WTSEnumerateSessions failed with error %d", GetLastError());
            return 0;
        }

        DWORD session_id = 0;

        for (DWORD i = 0; i < n_sessions; ++i)
        {
            if (p_session_info[i].State == WTSActive)
            {
                session_id = p_session_info[i].SessionId;
                break;
            }
        }
        WTSFreeMemory(p_session_info);
        return session_id;
    }
}
