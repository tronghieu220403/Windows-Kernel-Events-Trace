#include "process_manager.h"
#include <psapi.h>
#include <tchar.h>

namespace manager
{
    void ProcessManager::AddProcess(size_t pid, size_t ppid) {
        if (process_map_.find(pid) == process_map_.end()) {
            ProcessInfo process_info = { pid, ppid, L"", {} };
            process_map_[pid] = process_info;
        }
        if (ppid != 0) {
			if (process_map_.find(ppid) == process_map_.end()) {
				ProcessInfo process_info = { ppid, 0, L"", {} };
				process_map_[ppid] = process_info;
			}
            process_map_[ppid].cpid_list.insert(pid);
			process_map_[pid].ppid = ppid;
        }
    }

    void ProcessManager::RemoveProcess(size_t pid) {
        if (process_map_.find(pid) == process_map_.end()) return;

        size_t ppid = process_map_[pid].ppid;
        if (ppid != 0) {
			RemoveChild(ppid, pid);
        }

        // Reassign children to PID 0
        for (size_t child_pid : process_map_[pid].cpid_list) {
            process_map_[child_pid].ppid = 0;
        }

        process_map_.erase(pid);
    }

    void ProcessManager::AddChild(size_t pid, size_t cpid) {
        if (process_map_.find(pid) == process_map_.end()) {
			ProcessInfo process_info = { pid, 0, L"", {} };
			process_map_[pid] = process_info;
        }
        if (process_map_.find(cpid) == process_map_.end()) {
			ProcessInfo process_info = { cpid, pid, L"", {} };
			process_map_[cpid] = process_info;
        }
        process_map_[pid].cpid_list.insert(cpid);
        process_map_[cpid].ppid = pid;
    }

    void ProcessManager::RemoveChild(size_t pid, size_t cpid) {
        if (process_map_.find(pid) != process_map_.end()) {
            process_map_[pid].cpid_list.erase(cpid);
        }
    }

    bool ProcessManager::IsAncestor(size_t ancestor_pid, size_t descendant_pid) {
        while (process_map_.find(descendant_pid) != process_map_.end()) {
            if (process_map_[descendant_pid].ppid == ancestor_pid) {
                return true;
            }
            descendant_pid = process_map_[descendant_pid].ppid;
			if (descendant_pid == 0) {
				break;
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
            ProcessInfo process_info = { pid, 0, image_file_name, {} };
            process_map_[pid] = process_info;
        }
    }

    std::wstring ProcessManager::GetImageFileName(size_t pid) {
        if (process_map_.find(pid) != process_map_.end())
        {
		    return process_map_[pid].image_file_name;
        }
        HANDLE hProcess = nullptr;
        std::wstring image_file_name_w;
        std::string image_file_name_a;
        int size_tmp = 0;
        DWORD error = 0;
        image_file_name_w.resize(MAX_PATH);
        image_file_name_a.resize(MAX_PATH);

        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(pid));
        if (hProcess) {
            // Try GetProcessImageFileNameW with resizing loop
            while (true) {
                if (size_tmp = GetProcessImageFileNameW(hProcess, image_file_name_w.data(), image_file_name_w.size())) {
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
                    if (size_tmp = GetProcessImageFileNameA(hProcess, image_file_name_a.data(), image_file_name_a.size())) {
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
                            break; // Move to the next step on any other error
                        }
                    }
                }
            }

            // Try QueryFullProcessImageNameW if previous methods failed
            if (error != ERROR_SUCCESS) {
                while (true) {
                    size_tmp = image_file_name_w.size();
                    if (QueryFullProcessImageNameW(hProcess, PROCESS_NAME_NATIVE, image_file_name_w.data(), (LPDWORD)&size_tmp)) {
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

                // Try QueryFullProcessImageNameA if QueryFullProcessImageNameW failed
                if (error != ERROR_SUCCESS) {
                    size_tmp = image_file_name_a.size();
                    while (true) {
                        if (QueryFullProcessImageNameA(hProcess, PROCESS_NAME_NATIVE, image_file_name_a.data(), (LPDWORD)&size_tmp)) {
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
                                break; // Log and stop on any other error
                            }
                        }
                    }
                }
            }
            CloseHandle(hProcess);
        }
        else {
            ulti::DebugLogW(L"PID " + std::to_wstring(pid) + L" OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) failed: " + ulti::GetErrorMessage(GetLastError()));
        }

        if (error != ERROR_SUCCESS) {
            // Try PROCESS_QUERY_INFORMATION | PROCESS_VM_READ path
            hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));
            if (hProcess) {
                while (true) {
                    if (size_tmp = GetModuleFileNameExW(hProcess, nullptr, image_file_name_w.data(), image_file_name_w.size())) {
                        error = ERROR_SUCCESS;
                        image_file_name_w.resize(size_tmp);
                        image_file_name_w = GetDosPath(&image_file_name_w);
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
                    while (true) {
                        if (size_tmp = GetModuleFileNameExA(hProcess, nullptr, image_file_name_a.data(), image_file_name_a.size())) {
                            error = ERROR_SUCCESS;
                            image_file_name_a.resize(size_tmp);
                            image_file_name_w = std::wstring(image_file_name_a.begin(), image_file_name_a.end());
                            image_file_name_w = GetDosPath(&image_file_name_w);
                            break;
                        }
                        else {
                            error = GetLastError();
                            if (error == ERROR_INSUFFICIENT_BUFFER) {
                                image_file_name_a.resize(image_file_name_a.size() * 2);
                            }
                            else {
                                ulti::DebugLogW(L"PID " + std::to_wstring(pid) + L" GetModuleFileNameEx failed: " + ulti::GetErrorMessage(error));
                                break; // Log and stop on any other error
                            }
                        }
                    }
                }
                CloseHandle(hProcess);
            }
            else {
                ulti::DebugLogW(L"PID " + std::to_wstring(pid) + L" OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ) failed: " + ulti::GetErrorMessage(GetLastError()));
            }
        }
        if (error == ERROR_SUCCESS)
        {
			UpdateImageFileName(pid, image_file_name_w);
			return image_file_name_w;
        }
        return L"";
    }

    const ProcessInfo& ProcessManager::GetProcessInfo(size_t pid)
    {
        if (process_map_.find(pid) != process_map_.end())
        {
            return {};
        }
		return process_map_[pid];
    }
}
