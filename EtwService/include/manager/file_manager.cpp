#include "file_manager.h"

namespace manager
{
	void FileManager::AddFile(const size_t file_object, const std::wstring& file_path)
	{
		file_map_[file_object] = file_path;
	}

	void FileManager::RemoveFile(const size_t file_object)
	{
		file_map_.erase(file_object);
	}

    std::wstring FileManager::GetFilePath(const size_t file_object) const
    {
        auto it = file_map_.find(file_object);
        if (it == file_map_.end())
        {
            return std::wstring();
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
                ulti::DebugLogW(L"QueryDosDevice failed: " + ulti::GetErrorMessage(status));
                return std::wstring();
            }
            dos_name.resize(dos_name.size() * 2);
        }
        dos_name.resize(wcslen(dos_name.data()));
		kDosPath[device_name] = dos_name;
        return dos_name + wstr->substr(wstr->find_first_of('\\'));
    }

}