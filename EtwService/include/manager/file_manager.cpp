﻿#include "file_manager.h"

namespace manager
{
	void FileManager::AddFileObject(const size_t file_object, const std::wstring& file_path)
	{
		obj_to_name_map_[file_object] = file_path;
	}

	void FileManager::RemoveFileObject(const size_t file_object)
	{
		obj_to_name_map_.erase(file_object);
	}

    const std::wstring& FileManager::GetFilePath(const size_t file_object)
    {
        auto it = obj_to_name_map_.find(file_object);
        if (it == obj_to_name_map_.end())
        {
			it = obj_to_name_map_.find(0);
            if (it == obj_to_name_map_.end())
            {
				obj_to_name_map_[0] = L"";
				it = obj_to_name_map_.find(0);
            }
        }
        return it->second;
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
                debug::DebugLogW(L"QueryDosDevice failed: " + debug::GetErrorMessage(status));
                return std::wstring();
            }
            device_name.resize(device_name.size() * 2);
        }
        device_name.resize(wcslen(device_name.data()));
		kNativePath.insert({ drive_name, device_name });
        return device_name + win32_path.substr(win32_path.find_first_of('\\'));
    }

    std::wstring GetWin32Path(const std::wstring& path) {

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

	// Hàm lấy kích thước file
    size_t GetFileSize(const std::wstring& file_path)
    {
        // Open the file
		HANDLE file_handle = CreateFileW(file_path.c_str(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

        if (file_handle == INVALID_HANDLE_VALUE) {
            std::wcerr << L"Failed to open file: " << file_path << std::endl;
            return 0;
        }

        // Get the file size
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(file_handle, &file_size) == 0)
        {
            file_size.QuadPart = 0;
        }

        // Close the handle
        CloseHandle(file_handle);

        return file_size.QuadPart;

    }

    // Hàm lấy đuôi file
    std::wstring GetFileExtension(const std::wstring& file_name) {
        size_t pos = file_name.find_last_of(L".");
        if (pos == std::wstring::npos) {
            return L""; // Không có đuôi file
        }
        return file_name.substr(pos + 1); // Trả về phần đuôi file
    }

    // Check if a file is an executable (filters by extension)
    bool IsExecutableFile(const std::wstring& file_path) {
        static const std::wregex exe_pattern(L".*\\.(exe|dll|msi|bat|cmd)$", std::regex_constants::icase);
        return std::regex_match(file_path, exe_pattern);
    }

    std::wstring CopyToTmp(const std::wstring& path, const size_t& copy_size) {

        std::filesystem::path src(path);
        std::filesystem::path tmp_dir = TEMP_DIR;
		std::wstring new_file_name = std::to_wstring(std::hash<std::wstring>{}(path)) + L"." + GetFileExtension(src.filename());
        std::filesystem::path dest = tmp_dir / new_file_name;

        std::ifstream infile(path, std::ios::binary);
        std::ofstream outfile(dest, std::ios::binary);

		std::vector<char> buffer(copy_size);
        infile.read(buffer.data(), copy_size);
        outfile.write(buffer.data(), infile.gcount());

        return dest.wstring();
    }

	// Hàm xóa các file tạm
    void ClearTempFiles() {
        std::filesystem::path tmp_dir = TEMP_DIR;
        // Chuyển đổi đường dẫn sang kiểu wstring để dùng với Windows API
        std::wstring temp_path = tmp_dir.wstring();
		if (temp_path[temp_path.size() - 1] != L'\\') {
			temp_path += L"\\";
		}

        // Thiết lập bộ lọc để lấy tất cả các file và thư mục trong thư mục tạm
        std::wstring search_path = temp_path + L"*";
        WIN32_FIND_DATAW find_file_data;
        HANDLE h_find = FindFirstFileW(search_path.c_str(), &find_file_data);

        if (h_find == INVALID_HANDLE_VALUE) {
            return;  // Không thể mở thư mục tạm
        }

        do {
            // Bỏ qua các file và thư mục đặc biệt "." và ".."
            if (wcscmp(find_file_data.cFileName, L".") == 0 || wcscmp(find_file_data.cFileName, L"..") == 0) {
                continue;
            }

            // Tạo đường dẫn đầy đủ đến file hoặc thư mục
            std::wstring full_path = temp_path + find_file_data.cFileName;

            // Kiểm tra và xóa file hoặc thư mục
            if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Nếu là thư mục, xóa thư mục (không đệ quy)
                RemoveDirectoryW(full_path.c_str());
            }
            else {
                // Nếu là file, xóa file
                DeleteFileW(full_path.c_str());
            }
        } while (FindNextFileW(h_find, &find_file_data) != 0);

        // Đóng handle tìm kiếm
        FindClose(h_find);
    }

    // Hàm kiểm tra xem file có chủ yếu chứa ký tự in được hay không (kiểm tra cả UTF-8, UTF-16 và ANSI)
    bool IsPrintableFile(const std::wstring& file_path, std::streamsize max_size) {
        std::ifstream file(file_path, std::ios::binary); // Mở file dưới dạng nhị phân
        if (!file.is_open()) {
            //std::wcerr << L"Failed to open file: " << file_path << std::endl;
            return false; // Trả về false nếu không mở được file
        }

        // Xác định kích thước file để đọc (lấy FILE_MAX_SIZE_CHECK đầu tiên hoặc kích thước file, tùy cái nào nhỏ hơn)
        std::streamsize file_size = std::min<std::streamsize>(max_size, static_cast<std::streamsize>(file.seekg(0, std::ios::end).tellg()));
        file.seekg(0, std::ios::beg); // Đưa con trỏ file về đầu

        std::vector<unsigned char> buffer(file_size); // Bộ đệm để chứa dữ liệu từ file
        file.read(reinterpret_cast<char*>(buffer.data()), file_size); // Đọc toàn bộ dữ liệu vào bộ đệm
        file.close();

        // Kiểm tra theo các mã hóa khác nhau và trả về true nếu bất kỳ cái nào khớp
        if (ulti::CheckPrintableUTF8(buffer) || ulti::CheckPrintableUTF16(buffer) || ulti::CheckPrintableANSI(buffer)) {
            return true;
        }

        return false; // Nếu không có mã hóa nào đạt yêu cầu, trả về false
    }

    // Hàm phân tích output của trid và trả về kết quả kiểm tra từng file
    std::vector<std::pair<std::wstring, bool>> AnalyzeTridOutput(const std::wstring& output) {
        std::wistringstream stream(output);
        std::wstring line;
        std::wstring current_file;
        std::vector<std::wstring> found_extensions; // Đuôi file tìm thấy từ `trid`
        std::vector<std::pair<std::wstring, bool>> results; // Kết quả kiểm tra từng file
        for (int i = 0; i < 5; i++)
        {
            std::getline(stream, line);
        }
        int cnt = 0;
        // Duyệt từng dòng của output
        while (std::getline(stream, line)) {
            if (line.size() > 0 && line[line.size() - 1] == L'\r')
            {
                line.resize(line.size() - 1);
            }
            if (line.find(L"ile: ") != std::wstring::npos) { // Nếu là dòng thông tin file
                current_file = line.substr(line.find(L"ile: ") + sizeof(L"ile: ") / sizeof(WCHAR) - 1);
                //std::wcout << L"\nChecking file: " << current_file << std::endl;
                cnt++;
                found_extensions.clear(); // Reset danh sách đuôi file tìm thấy
            }
            if (!line.empty() && line.find(L"% (.") != std::wstring::npos) { // Nếu là dòng chứa phần trăm tỉ lệ của đuôi file
                size_t start_pos = line.find(L"% (.") + (sizeof(L"% (.")) / sizeof(WCHAR) - 1; // Vị trí bắt đầu của đuôi file
                size_t end_pos = line.find(L")", start_pos);
                if (end_pos != std::wstring::npos) {
                    std::wstring exts = line.substr(start_pos, end_pos - start_pos);
                    if (exts.size() != 0)
                    {
                        std::transform(exts.begin(), exts.end(), exts.begin(), ::towlower);
                        // Extension có thể chứa nhiều đuôi file (ví dụ JPG/JPEG, cần phân tách ra)
                        std::wstringstream ss(exts);
                        std::wstring ext;
                        while (std::getline(ss, ext, L'/'))
                        {
                            found_extensions.push_back(ext); // Lưu đuôi file tìm thấy
                        }
                    }
                }
            }
            else if (line.empty())
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
                    if (IsPrintableFile(current_file)) {
                        results.push_back({ current_file, true });
                        //std::wcout << L"True:  File contains mainly printable characters." << std::endl;
                    }
                    else {
                        results.push_back({ current_file, false });
                        //std::wcout << L"False: File extension mismatch!" << std::endl;
                    }
                }
                else {
                    results.push_back({ current_file, true });
                    //std::wcout << L"True:  File extension matches one of the detected extensions." << std::endl;
                }
                current_file.resize(0);
            }
        }
        //std::cout << "Number of files: " << cnt << std::endl;
        return results;
    }

    // Hàm kiểm tra danh sách file
    std::vector<std::pair<std::wstring, bool>> CheckFileList(const std::vector<std::wstring>& file_list) {
        //std::wstring cmd = L"dir E:\\Download /b /s | E:\\Code\\TrID\\trid.exe -@ -n:5"; // Lệnh trid với option -n:5 để lấy 5 loại đuôi phổ biến nhất

        std::wstring cmd = L"C:\\hieunt210330\\TrID\\trid.exe -n:5 "; // Lệnh trid với option -n:5 để lấy 5 loại đuôi phổ biến nhất
        for (const auto& file : file_list) {
            cmd += L"\"" + file + L"\" "; // Nối các file vào command
        }

        std::wstring output(ulti::ExecCommand(cmd)); // Chạy command và lấy output
        std::vector<std::pair<std::wstring, bool>> trid_output;
		if (output.empty()) {
			debug::DebugLogW(L"TrID failed to run.");
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

        // Phân tích output
        trid_output = std::move(AnalyzeTridOutput(output));
        return trid_output;
    }
}