#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

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

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Shell32.lib") 
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "Pdh.lib")
#pragma comment(lib, "Advapi32.lib")
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
    inline std::wstring StrToWStr(const std::string& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    inline std::string CharVectorToString(const std::vector<char>& v)
    {
        return std::string(v.begin(), v.end());
    }

    inline std::vector<char> StringToVectorChar(const std::string& s)
    {
        return std::vector<char>(s.begin(), s.end());
    }

	inline std::vector<unsigned char> StringToVectorUChar(const std::string& s)
	{
		return std::vector<unsigned char>(s.begin(), s.end());
	}

	inline std::string WstrToStr(const std::wstring& wstr)
	{
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> myconv;
        return myconv.to_bytes(wstr);
    }

    inline bool CreateDir(const std::wstring& dir_path)
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

    // Kiểm tra ký tự in được cho UTF-16
    inline bool CheckPrintableUTF16(const std::vector<unsigned char>& buffer) {
        std::streamsize printable_chars = 0;
        std::streamsize total_chars = 0;

        // UTF-16 đọc từng cặp 2 byte
        for (std::size_t i = 0; i + 1 < buffer.size(); i += 2) {
            wchar_t wchar = buffer[i] | (buffer[i + 1] << 8);
            total_chars++;
            if (iswprint(wchar) || iswspace(wchar)) {
                printable_chars++;
            }
        }

        if (total_chars == 0) {
            return true; // Nếu không có ký tự nào, mặc định trả về true
        }

        return (static_cast<double>(printable_chars) / total_chars) >= 0.97;
    }

	inline bool CheckPrintableUTF16(const std::wstring& wstr)
	{
		return CheckPrintableUTF16(StringToVectorUChar(WstrToStr(wstr)));
	}

    // Kiểm tra ký tự in được cho UTF-8
    inline bool CheckPrintableUTF8(const std::vector<unsigned char>& buffer) {
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
                i++; // Bỏ qua các byte không hợp lệ
            }
        }

        if (total_chars == 0) {
            return true; // Nếu không có ký tự nào, mặc định trả về true
        }

        return (static_cast<double>(printable_chars) / total_chars) >= 0.97;
    }

	inline bool CheckPrintableUTF8(const std::wstring& wstr)
	{
		return CheckPrintableUTF8(StringToVectorUChar(WstrToStr(wstr)));
	}

    // Kiểm tra ký tự in được cho ANSI (ASCII 1-byte)
    inline bool CheckPrintableANSI(const std::vector<unsigned char>& buffer) {
        std::streamsize printable_chars = 0;
        std::streamsize total_chars = 0;

        for (unsigned char c : buffer) {
            total_chars++;
            if (isprint(c) || isspace(c)) {
                printable_chars++;
            }
        }

        if (total_chars == 0) {
            return true; // Nếu không có ký tự nào, mặc định trả về true
        }

        return (static_cast<double>(printable_chars) / total_chars) >= 0.97;
    }

    inline bool CheckPrintableANSI(const std::wstring& wstr)
    {
		return CheckPrintableANSI(StringToVectorUChar(WstrToStr(wstr)));
    }

    // Hàm thực thi command và trả về output dưới dạng một chuỗi
    inline std::wstring ExecCommand(const std::wstring& cmd) {
        std::array<wchar_t, 4096> buffer; // Buffer để lưu từng dòng output của command
        std::wstring result; // Chuỗi lưu toàn bộ output của command
        // Dùng _wpopen để mở pipe và chạy command, _pclose để đóng pipe sau khi xong
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_wpopen(cmd.c_str(), L"r"), _pclose);
        try
        {
            result.reserve(cmd.size() * 10); // Dự trữ cho chuỗi result (tùy theo kích thước output của command)
        }
		catch (const std::exception& e)
		{
			std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << e.what() << std::endl;
			result.clear();
			return result;
		}
        if (!pipe) {
            std::wcerr << L"Failed to run command." << std::endl; // Lỗi nếu không chạy được command
            result.clear();
            return result;
        }
        // Đọc từng dòng từ pipe và lưu vào result
        while (fgetws(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result; // Trả về toàn bộ output của command
    }

    inline bool IsRunningAsSystem() {
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

#endif
