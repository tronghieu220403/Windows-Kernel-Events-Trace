﻿#include "file_manager.h"

namespace manager
{
    void FileNameObjMap::MapObjectWithPath(const size_t file_object, const std::wstring& file_path)
    {
        obj_to_name_map_[file_object] = file_path;
    }

    void FileNameObjMap::RemoveObject(const size_t file_object)
    {
        obj_to_name_map_.erase(file_object);
    }

    const std::wstring& FileNameObjMap::GetPathByObject(const size_t file_object)
    {
        const std::wstring& null_str = L"";
        auto it = obj_to_name_map_.find(file_object);
        if (it == obj_to_name_map_.end())
        {
            return null_str;
        }
        return it->second;
    }

    void FileIoManager::LockMutex()
    {
        file_io_mutex_.lock();
    }

    void FileIoManager::UnlockMutex()
    {
        file_io_mutex_.unlock();
    }

    FileIoInfo FileIoManager::PopFileIoEvent()
    {
        FileIoInfo file_io_info;
        if (!file_io_queue_.empty())
        {
            file_io_info = file_io_queue_.front();
            file_io_queue_.pop_front();
        }
        return file_io_info;
    }

    size_t FileIoManager::GetQueueSize()
    {
        return file_io_queue_.size();
    }

    void FileIoManager::PushRenameFileEventToQueue(const std::wstring& file_path_new, size_t pid, size_t start_time_ms, const std::wstring& file_path_old)
    {
        // PrintDebugW(L"File I/O, custom Rename event, pid %llu, from %ws to %ws\n", pid, file_path_old.data(), file_path_new.data());

        FileIoInfo file_io_info;
        file_io_info.featured_access_flags = RENAME_FLAG;
        file_io_info.file_path = file_path_new;
        file_io_info.rename_info.file_path_old = file_path_old;
        file_io_info.start_time_ms = start_time_ms;
        file_io_info.pid = pid;
        file_io_queue_.push_back(file_io_info);
    }

    void FileIoManager::PushWriteFileEventToQueue(const std::wstring& file_path, size_t pid, size_t start_time_ms, size_t io_size)
    {
        // PrintDebugW(L"File I/O, custom Write event, pid %llu, file %ws\n", pid, file_path.data());

        FileIoInfo file_io_info;
        file_io_info.featured_access_flags = WRITE_FLAG;
        file_io_info.file_path = file_path;
        file_io_info.start_time_ms = start_time_ms;
        file_io_info.pid = pid;
        file_io_info.write_info.size = io_size;
        file_io_queue_.push_back(file_io_info);
    }

    std::wstring GetNativePath(const std::wstring& win32_path)
    {
        std::wstring drive_name = win32_path.substr(0, win32_path.find_first_of('\\'));
        if (kNativePath.find(drive_name) != kNativePath.end())
        {
            return kNativePath[drive_name] + win32_path.substr(drive_name.length());
        }

        std::wstring device_name;
        device_name.resize(MAX_PATH);
        DWORD status;
        while (QueryDosDeviceW(drive_name.data(), (WCHAR*)device_name.data(), (DWORD)device_name.size()) == 0)
        {
            status = GetLastError();
            if (status != ERROR_INSUFFICIENT_BUFFER)
            {
                PrintDebugW(L"QueryDosDevice failed for Win32 path %ws, error %s", win32_path.c_str(), debug::GetErrorMessage(status).c_str());
                return std::wstring();
            }
            device_name.resize(device_name.size() * 2);
        }
        device_name.resize(wcslen(device_name.data()));
        kNativePath.insert({ drive_name, device_name });
        return device_name + win32_path.substr(win32_path.find_first_of('\\'));
    }

    std::wstring GetWin32PathCaseSensitive(const std::wstring& path)
    {
        // If the path is empty or it does not start with "\\" (not a device path), return as-is
        if (path.empty() || path[0] != L'\\') {
            return path;
        }

        // Remove Win32 Device Namespace or File Namespace prefix if present
        std::wstring clean_path = path;
        if (clean_path.find(L"\\\\?\\") == 0 || clean_path.find(L"\\\\.\\") == 0) {
            return clean_path.substr(4); // Remove "\\?\" or "\\.\"
        }

        std::wstring device_name = path.substr(0, path.find(L'\\', sizeof("\\Device\\") - 1)); // Extract device name
        auto it = kWin32Path.find(device_name);
        if (it != kWin32Path.end()) {
            return it->second + clean_path.substr(device_name.length());
        }

        // Prepare UNICODE_STRING for the native path
        UNICODE_STRING unicode_path = { (USHORT)(clean_path.length() * sizeof(wchar_t)), (USHORT)(clean_path.length() * sizeof(wchar_t)), (PWSTR)clean_path.c_str() };

        OBJECT_ATTRIBUTES obj_attr;
        InitializeObjectAttributes(&obj_attr, &unicode_path, OBJ_CASE_INSENSITIVE, NULL, NULL);

        HANDLE file_handle;
        IO_STATUS_BLOCK io_status;

        // Attempt to open the file or device
        NTSTATUS status = NtCreateFile(&file_handle, FILE_READ_ATTRIBUTES, &obj_attr, &io_status, NULL, FILE_ATTRIBUTE_READONLY, FILE_SHARE_READ, FILE_OPEN, FILE_NON_DIRECTORY_FILE, NULL, 0);

        if (!NT_SUCCESS(status)) {
            return L"";
        }

        // Get full path using the handle
        WCHAR buffer[MAX_PATH];
        DWORD result = GetFinalPathNameByHandleW(file_handle, buffer, MAX_PATH, FILE_NAME_NORMALIZED);
        CloseHandle(file_handle);

        std::wstring win_api_path;
        if (result != 0 && result < MAX_PATH) {
            win_api_path = std::wstring(buffer);
            if (win_api_path.find(L"\\\\?\\") == 0 || win_api_path.find(L"\\\\.\\") == 0) {
                win_api_path = win_api_path.substr(4); // Remove "\\?\" or "\\.\"
            }
            // Extract and cache the device prefix
            const std::wstring device_prefix = clean_path.substr(0, clean_path.find(L'\\', sizeof("\\Device\\") - 1));
            const std::wstring drive_prefix = win_api_path.substr(0, win_api_path.find_first_of(L'\\')); // Get the drive letter part (e.g., "C:")
            kWin32Path.insert({ device_name, drive_prefix }); // Cache the prefix mapping
        }
        else {
            win_api_path = L""; // Fallback to original path if conversion fails
        }

        return win_api_path;
    }

    std::wstring GetWin32Path(const std::wstring& path)
    {
        return ulti::ToLower(GetWin32PathCaseSensitive(path));
    }

    bool FileExist(const std::wstring& file_path)
    {
        DWORD file_attributes = GetFileAttributesW(file_path.c_str());
        if (file_attributes == INVALID_FILE_ATTRIBUTES || FlagOn(file_attributes, FILE_ATTRIBUTE_DIRECTORY) == true) {
            return false;
        }
        return true;
    }

    bool DirExist(const std::wstring& dir_path)
    {
        DWORD file_attributes = GetFileAttributesW(dir_path.c_str());
        if (file_attributes == INVALID_FILE_ATTRIBUTES || FlagOn(file_attributes, FILE_ATTRIBUTE_DIRECTORY) == false) {
            return false;
        }
        return true;
    }

    size_t GetFileSize(const std::wstring& file_path)
    {
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (!GetFileAttributesEx(file_path.c_str(), GetFileExInfoStandard, &fad))
        {
            PrintDebugW(L"GetFileSize failed for file %ws, error %s", file_path.c_str(), debug::GetErrorMessage(GetLastError()).c_str());
            return 0;
        }
        LARGE_INTEGER size;
        size.HighPart = fad.nFileSizeHigh;
        size.LowPart = fad.nFileSizeLow;
        return size.QuadPart;
    }

    // Function to get file extension
    std::wstring GetFileExtension(const std::wstring& file_name) {
        size_t pos = file_name.find_last_of(L".");
        if (pos == std::wstring::npos) {
            return L""; // No file extension
        }
        return ulti::ToLower(file_name.substr(pos + 1)); // Return the file extension
    }

    // Check if a file is an executable (filters by extension)
    bool IsExecutableFile(const std::wstring& file_path) {
        static const std::wregex exe_pattern(L".*\\.(exe|dll|msi|bat|cmd)$", std::regex_constants::icase);
        return std::regex_match(ulti::ToLower(file_path), exe_pattern);
    }

    std::wstring CopyToTmp(const std::wstring& path, size_t copy_size) {
        std::wstring dest = TEMP_DIR + std::to_wstring(std::hash<std::wstring>{}(ulti::ToLower(path))) + L"." + path.substr(path.find_last_of(L".") + 1);
        HANDLE h_src = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h_src == INVALID_HANDLE_VALUE)
        {
            return L"";
        }
        HANDLE h_dest = CreateFileW(dest.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h_dest == INVALID_HANDLE_VALUE)
        {
            CloseHandle(h_src);
            return L"";
        }
        char* buf = new char[copy_size];
        DWORD r = 0, w = 0;
        if (ReadFile(h_src, buf, (DWORD)copy_size, &r, NULL))
        {
            WriteFile(h_dest, buf, r, &w, NULL);
        }
        delete[] buf;
        CloseHandle(h_src);
        CloseHandle(h_dest);
        return dest;
    }

    // Function to clear temporary files
    void ClearTmpFiles() {
        std::filesystem::path tmp_dir = TEMP_DIR;
        // Convert path to wstring for Windows API
        std::wstring temp_path = tmp_dir.wstring();
        if (temp_path[temp_path.size() - 1] != L'\\') {
            temp_path += L"\\";
        }

        // Set up filter to get all files and directories in the temporary directory
        std::wstring search_path = temp_path + L"*";
        WIN32_FIND_DATAW find_file_data;
        HANDLE h_find = FindFirstFileW(search_path.c_str(), &find_file_data);

        if (h_find == INVALID_HANDLE_VALUE) {
            return;  // Cannot open temporary directory
        }

        do {
            // Skip special files and directories "." and ".."
            if (wcscmp(find_file_data.cFileName, L".") == 0 || wcscmp(find_file_data.cFileName, L"..") == 0) {
                continue;
            }

            // Create full path to file or directory
            std::wstring full_path = temp_path + find_file_data.cFileName;

            // Check and delete file or directory
            if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // If it's a directory, delete it (not recursive)
                RemoveDirectoryW(full_path.c_str());
            }
            else {
                // If it's a file, delete it
                DeleteFileW(full_path.c_str());
            }
        } while (FindNextFileW(h_find, &find_file_data) != 0);

        // Close search handle
        FindClose(h_find);
    }

    // Function to check if a file contains mostly printable characters (checks UTF-8, UTF-16, and ANSI)
    bool IsPrintableFile(const std::wstring& file_path, std::streamsize max_size) {
        try
        {
            std::ifstream file(file_path, std::ios::binary); // Open file in binary mode
            if (!file.is_open()) {
                return false; // File cannot be opened
            }

            std::streamsize file_size = std::min<std::streamsize>(max_size, static_cast<std::streamsize>(file.seekg(0, std::ios::end).tellg()));
            file.seekg(0, std::ios::beg); // Move file pointer to the beginning

            std::vector<unsigned char> buffer(file_size); // Buffer to hold file data
            file.read(reinterpret_cast<char*>(buffer.data()), file_size); // Read all data into buffer
            file.close();

            // Check against different encodings and return true if any matches
            if (ulti::CheckPrintableUTF8(buffer) || ulti::CheckPrintableUTF16(buffer) || ulti::CheckPrintableANSI(buffer)) {
                return true;
            }
        }
        catch (...)
        {

        }
        return false; // If no encoding matches, return false
    }

    // Function to analyze TrID output and return the result of each file check
    std::vector<std::pair<std::wstring, bool>> AnalyzeTridOutput(const std::wstring& output) {
        std::wistringstream stream(output);
        std::wstring line;
        std::wstring current_file;
        std::vector<std::wstring> found_extensions; // Extensions found by `trid`
        std::vector<std::pair<std::wstring, bool>> results; // Result of each file check
        for (int i = 0; i < 5; i++)
        {
            std::getline(stream, line);
        }
        int cnt = 0;
        // Iterate through each line of the output
        while (std::getline(stream, line)) {
            if (line.size() > 0 && line[line.size() - 1] == L'\r')
            {
                line.resize(line.size() - 1);
            }
            line = ulti::ToLower(line);
#ifdef _DEBUG
            PrintDebugW(L"TrID text: %ws", line.c_str());
#endif // _DEBUG

            if (line.empty())
            {
                if (current_file.empty()) {
                    continue;
                }
                std::wstring actual_extension = GetFileExtension(current_file);
                bool extension_matched = std::any_of(found_extensions.begin(), found_extensions.end(),
                    [&](const std::wstring& ext) {
                        return ext == actual_extension;
                    });

                if (!extension_matched) {
                    if (IsPrintableFile(current_file))
                    {
#ifdef _DEBUG
                        PrintDebugW(L"IsPrintableFile: %ws", current_file.c_str());
#endif // _DEBUG
                        results.push_back({ current_file, true });
                    }
                    else {
#ifdef _DEBUG
                        PrintDebugW(L"extension_not_matched: %ws", current_file.c_str());
#endif // _DEBUG
                        results.push_back({ current_file, false });
                    }
                }
                else {
#ifdef _DEBUG
                    PrintDebugW(L"extension_matched: %ws", current_file.c_str());
#endif // _DEBUG
                    results.push_back({ current_file, true });
                }
                current_file.resize(0);
            }
            else if (line.find(L"file: ") != std::wstring::npos)
            { // If it's a file information line
                current_file = line.substr(line.find(L"ile: ") + sizeof(L"ile: ") / sizeof(WCHAR) - 1);
#ifdef _DEBUG
                PrintDebugW(L"Checking file: %ws", current_file.c_str());
#endif // _DEBUG
                cnt++;
                found_extensions.clear(); // Reset found extensions list
            }
            else if (line.find(L"% (.") != std::wstring::npos) { // If it's a line containing the percentage of the file extension
                if (line.find(L"ransom") != std::wstring::npos || line.find(L"encrypt") != std::wstring::npos) {
                    continue;
                }
                size_t start_pos = line.find(L"% (.") + (sizeof(L"% (.")) / sizeof(WCHAR) - 1; // Start position of the file extension
                size_t end_pos = line.find(L")", start_pos);
                if (end_pos != std::wstring::npos) {
                    std::wstring exts = line.substr(start_pos, end_pos - start_pos);
                    if (exts.size() != 0)
                    {
                        // Extension may contain multiple file extensions (e.g., JPG/JPEG, need to split)
                        std::wstringstream ss(exts);
                        std::wstring ext;
                        while (std::getline(ss, ext, L'/'))
                        {
                            found_extensions.push_back(ext); // Save found extension
                        }
                    }
                }
            }
        }
        return results;
    }

    std::vector<std::pair<std::wstring, bool>> CheckTrID(const std::vector<std::wstring>& file_list) {

        std::wstring cmd = L"\"" TRID_PATH L"\" -n:5 ";
        for (const auto& file : file_list) {
            cmd += L"\"" + file + L"\" ";
        }

        PrintDebugW(L"Running TrID command: %ws", cmd.c_str());

        std::wstring output;
        try {
            output = ulti::ExecCommand(cmd);
        }
        catch (const std::exception& e) {
            PrintDebugW(L"Failed to run TrID: %hs", e.what());
            return {};
        }
        std::vector<std::pair<std::wstring, bool>> trid_output;
        if (output.empty()) {
            PrintDebugW(L"TrID failed to run.");
            return trid_output;
        }
        if (output[output.size() - 1] != L'\n')
        {
            output += L"\n";
        }
        if (output[output.size() - 2] != L'\n')
        {
            output += L"\n";
        }

        // Analyze output
        trid_output = std::move(AnalyzeTridOutput(output));
        return trid_output;
    }
}