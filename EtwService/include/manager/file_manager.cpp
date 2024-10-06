#include "file_manager.h"

namespace manager
{
	void FileManager::AddFile(const std::wstring& file_path)
	{
		std::wstring dos_path = GetDosPath(&file_path);
		if (dos_path.empty()) return;

		size_t hash = std::hash<std::wstring>{}(dos_path);
		file_map_[hash] = dos_path;
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
        while (QueryDosDeviceW(device_name.data(), (WCHAR*)dos_name.data(), dos_name.size()) == 0)
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