#include "support.h"

namespace ulti
{
    std::wstring StrToWStr(const std::string& str)
    {
        return std::wstring(str.begin(), str.end());
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
        std::array<wchar_t, 4096> buffer;
        std::wstring result;
		// Use _wpopen to run command and get output
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_wpopen(cmd.c_str(), L"r"), _pclose);
        try
        {
            result.reserve(cmd.size() * 10);
        }
        catch (const std::exception& e)
        {
            result.clear();
            return result;
        }
        if (!pipe) {
            result.clear();
            return result;
        }
        while (fgetws(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
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
}
