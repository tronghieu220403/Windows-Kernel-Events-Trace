#include "file_manager.h"

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

    std::wstring GetDosPath(const std::wstring* wstr)
    {
        std::wstring device_name = wstr->substr(0, wstr->find_first_of('\\'));
        if (kDosPath.find(device_name) != kDosPath.end())
        {
			return kDosPath[device_name] + wstr->substr(wstr->find_first_of('\\'));
        }
        std::wstring dos_name;
        dos_name.resize(MAX_PATH);
        DWORD status;
        while (QueryDosDeviceW(device_name.data(), (WCHAR*)dos_name.data(), (DWORD)dos_name.size()) == 0)
        {
            status = GetLastError();
            if (status != ERROR_INSUFFICIENT_BUFFER)
            {
                debug::DebugLogW(L"QueryDosDevice failed: " + debug::GetErrorMessage(status));
                return std::wstring();
            }
            dos_name.resize(dos_name.size() * 2);
        }
        dos_name.resize(wcslen(dos_name.data()));
		kDosPath[device_name] = dos_name;
        return dos_name + wstr->substr(wstr->find_first_of('\\'));
    }

	// Hàm lấy kích thước file
    size_t GetFileSize(const std::wstring& file_path)
    {
        size_t file_size = 0;
        try
        {
            file_size = std::filesystem::file_size(file_path);
        }
        catch (...)
        {
            file_size = 0;
        }
        return file_size;
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
        std::filesystem::path tmp_dir = std::filesystem::temp_directory_path();
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
		std::filesystem::path tmp_dir = std::filesystem::temp_directory_path();
		for (const auto& entry : std::filesystem::directory_iterator(tmp_dir))
		{
            try
            {
                std::filesystem::remove(entry.path());
            }
            catch (const std::exception& e)
            {

            }
		}
	}

    // Hàm kiểm tra xem file có chủ yếu chứa ký tự in được hay không (kiểm tra cả UTF-8, UTF-16 và ANSI)
    bool IsPrintableFile(const std::wstring& file_path, std::streamsize max_size) {
        std::ifstream file(file_path, std::ios::binary); // Mở file dưới dạng nhị phân
        if (!file.is_open()) {
            std::wcerr << L"Failed to open file: " << file_path << std::endl;
            return false; // Trả về false nếu không mở được file
        }

        // Xác định kích thước file để đọc (lấy 10MB đầu tiên hoặc kích thước file, tùy cái nào nhỏ hơn)
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

        std::wstring cmd = L"E:\\Code\\TrID\\trid.exe -n:5 "; // Lệnh trid với option -n:5 để lấy 5 loại đuôi phổ biến nhất
        for (const auto& file : file_list) {
            cmd += L"\"" + file + L"\" "; // Nối các file vào command
        }

        std::wstring output = ulti::ExecCommand(cmd); // Chạy command và lấy output
        if (output[output.size() - 1] != L'\n')
        {
            output += L"\n";
        }
        if (output[output.size() - 2] != L'\n')
        {
            output += L"\n";
        }

        // Phân tích output
        std::vector<std::pair<std::wstring, bool>> trid_output = AnalyzeTridOutput(output);
        return trid_output;
    }
}