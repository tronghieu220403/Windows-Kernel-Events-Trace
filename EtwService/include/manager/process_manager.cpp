#include "process_manager.h"

namespace manager
{
    void ProcessManager::AddProcess(size_t pid, size_t ppid_real, size_t ppid_adopted) {
        auto it = process_map_.find(pid);
        if (it == process_map_.end()) {
            ProcessInfo process_info;
            process_map_[pid] = process_info;
            it = process_map_.find(pid);
        }
        it->second.pid = pid;
        it->second.ppid_real = ppid_real == (size_t)(-1) ? 0 : ppid_real;
        it->second.ppid_adopted = ppid_adopted == (size_t)(-1) ? 0 : ppid_adopted;
    }

    void ProcessManager::RemoveProcess(size_t pid)
    {
        auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            process_map_.erase(it);
        }
    }

    bool ProcessManager::IsChild(size_t ppid, size_t pid)
    {
        auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            if (it->second.ppid_real == ppid || it->second.ppid_adopted == ppid)
            {
                return true;
            }
        }
        return false;
    }

    void ProcessManager::UpdateImageFileName(size_t pid, const std::wstring& image_file_name)
    {
        if (process_map_.find(pid) != process_map_.end())
        {
            process_map_[pid].image_file_name = image_file_name;
        }
        else
        {
            ProcessInfo process_info;
            process_info.image_file_name = image_file_name;
            process_info.pid = pid;
            process_map_[pid] = process_info;
        }
    }

    std::wstring ProcessManager::GetImageFileName(size_t pid)
    {
        auto it = process_map_.find(pid);
        if (it != process_map_.end() && it->second.image_file_name.size() > 0)
        {
            return it->second.image_file_name;
        }

        DWORD error = 0;
        std::wstring image_file_name_w;
        HANDLE hProcess = nullptr;
        std::string image_file_name_a;
        size_t size_tmp = 0;
        image_file_name_w.resize(MAX_PATH * 4);
        image_file_name_a.resize(MAX_PATH * 4);

        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(pid));
        if (hProcess)
        {
            // Try GetProcessImageFileNameW with resizing loop
            while (true) {
                if (size_tmp = GetProcessImageFileNameW(hProcess, image_file_name_w.data(), (DWORD)image_file_name_w.size())) {
                    error = ERROR_SUCCESS;
                    image_file_name_w.resize(size_tmp);
                    break;
                }
                else {
                    error = GetLastError();
                    if (error == ERROR_INSUFFICIENT_BUFFER) {
                        image_file_name_w.resize(image_file_name_w.size() * 2);
                    }
                    else {
                        break; // Move to the next step on any other error
                    }
                }
            }

            if (error != ERROR_SUCCESS) {
                // Try GetProcessImageFileNameA with resizing loop
                while (true) {
                    if (size_tmp = GetProcessImageFileNameA(hProcess, image_file_name_a.data(), (DWORD)image_file_name_a.size())) {
                        error = ERROR_SUCCESS;
                        image_file_name_a.resize(size_tmp);
                        image_file_name_w = std::wstring(image_file_name_a.begin(), image_file_name_a.end());
                        break;
                    }
                    else {
                        error = GetLastError();
                        if (error == ERROR_INSUFFICIENT_BUFFER) {
                            image_file_name_a.resize(image_file_name_a.size() * 2);
                        }
                        else {
                            PrintDebugW((L"PID " + std::to_wstring(pid) + L" GetProcessImageFileName failed: " + debug::GetErrorMessage(error)).c_str());
                            break; // Move to the next step on any other error
                        }
                    }
                }
            }

            CloseHandle(hProcess);
        }
        else
        {
            error = GetLastError();
            if (error != ERROR_INVALID_PARAMETER)
            {
                PrintDebugW((L"PID " + std::to_wstring(pid) + L" OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) failed, status : " + debug::GetErrorMessage(error)).c_str());
            }
        }
        image_file_name_w = manager::GetWin32Path(image_file_name_w);
        if (image_file_name_w.size() > 0)
        {
            UpdateImageFileName(pid, image_file_name_w);
        }
        return image_file_name_w;
    }

    void ProcessManager::UpdateProcessCreationTime(size_t pid, size_t creation_time)
    {
        auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            it->second.creation_time = creation_time;
        }
    }

    const ProcessInfo& ProcessManager::GetProcessInfo(size_t pid)
    {
        return process_map_[pid];
    }

    const std::unordered_map<size_t, ProcessInfo>& ProcessManager::GetProcessMap()
    {
        return process_map_;
    }

    void ProcessManager::LockMutex()
    {
        process_map_mutex_.lock();
    }

    void ProcessManager::UnlockMutex()
    {
        process_map_mutex_.unlock();
    }
}
