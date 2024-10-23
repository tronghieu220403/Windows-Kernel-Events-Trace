#include "process_manager.h"

namespace manager
{
    void ProcessManager::AddProcess(size_t pid, size_t ppid) {
        if (process_map_.find(pid) == process_map_.end()) {
            ProcessInfo process_info;
			process_info.pid = pid;
			process_info.ppid = ppid;
            process_map_[pid] = process_info;
        }
        if (ppid != 0) {
			if (process_map_.find(ppid) == process_map_.end()) {
                ProcessInfo process_info;
				process_info.pid = ppid;
				process_info.ppid = 0;
				process_map_[ppid] = process_info;
			}
            process_map_[ppid].cpid_list.insert(pid);
			process_map_[pid].ppid = ppid;
        }
    }

    void ProcessManager::RemoveProcess(size_t pid) 
    {
        if (process_map_.find(pid) == process_map_.end())
        {
            return;
        }

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

    void ProcessManager::PendingRemoveProcess(size_t pid)
    {
		const std::lock_guard<std::mutex> lock(pending_remove_mutex_);
        pending_remove_.push_back(pid);
    }

    void ProcessManager::RemovePendingProcesses()
    {
        const std::lock_guard<std::mutex> lock(pending_remove_mutex_);
        for (size_t pid : pending_remove_)
        {
            RemoveProcess(pid);
        }
		pending_remove_.clear();
    }

    void ProcessManager::AddChild(size_t pid, size_t cpid) 
    {
        if (process_map_.find(pid) == process_map_.end()) {
			ProcessInfo process_info;
			process_info.pid = pid;
			process_map_[pid] = process_info;
        }
        if (process_map_.find(cpid) == process_map_.end()) {
			ProcessInfo process_info;
			process_info.ppid = pid;
			process_info.pid = cpid;
			process_map_[cpid] = process_info;
        }
        process_map_[pid].cpid_list.insert(cpid);
        process_map_[cpid].ppid = pid;
    }

    void ProcessManager::RemoveChild(size_t pid, size_t cpid) 
    {
        if (process_map_.find(pid) != process_map_.end()) {
            process_map_[pid].cpid_list.erase(cpid);
        }
    }

    bool ProcessManager::IsAncestor(size_t ancestor_pid, size_t descendant_pid) 
    {
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
            ProcessInfo process_info;
			process_info.image_file_name = image_file_name;
            process_map_[pid] = process_info;
        }
    }

    std::wstring ProcessManager::GetImageFileName(size_t pid) 
    {
        // TODO: Always ask drivers.
        if (process_map_.find(pid) != process_map_.end())
        {
		    return process_map_[pid].image_file_name;
        }
        
        /*
        DWORD error = 0;
        std::wstring image_file_name_w;
        HANDLE hProcess = nullptr;
        std::string image_file_name_a;
        size_t size_tmp = 0;
        image_file_name_w.resize(MAX_PATH);
        image_file_name_a.resize(MAX_PATH);

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
                            debug::DebugLogW(L"[+] PID " + std::to_wstring(pid) + L" GetProcessImageFileName failed: " + debug::GetErrorMessage(error));
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
                                debug::DebugLogW(L"[+] PID " + std::to_wstring(pid) + L" QueryFullProcessImageName failed: " + debug::GetErrorMessage(error));
                                break; // Log and stop on any other error
                            }
                        }
                    }
                }
            }

            if (error != ERROR_SUCCESS) {
                while (true) {
                    if (size_tmp = GetModuleFileNameExW(hProcess, nullptr, image_file_name_w.data(), (DWORD)image_file_name_w.size())) {
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
                        if (size_tmp = GetModuleFileNameExA(hProcess, nullptr, image_file_name_a.data(), (DWORD)image_file_name_a.size())) {
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
                                debug::DebugLogW(L"[+] PID " + std::to_wstring(pid) + L" GetModuleFileNameEx failed: " + debug::GetErrorMessage(error));
                                break; // Log and stop on any other error
                            }
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
                debug::DebugLogW(L"[+] PID " + std::to_wstring(pid) + L" OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) failed: " + debug::GetErrorMessage(error));
            }
        }
        */

		// TODO: Ask driver from DriverComm to get image file name and set error to ERROR_SUCCESS
        std::wstring image_file_name_w(kDriverComm->GetProcessImageFromPid(pid));
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
        if (process_map_.find(pid) != process_map_.end())
        {
            return ProcessInfo();
        }
		return process_map_[pid];
    }

    void ProcessManager::PushCreateFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
        std::wstring wstr;
        wstr.resize(2000);
        wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, custom Create event, pid %llu, file %ws\n", pid, file_path.data()));
        debug::DebugLogW(wstr);
    }

    void ProcessManager::PushDeleteFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
        std::wstring wstr;
        wstr.resize(2000);
        wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, custom Delete event, pid %llu, file %ws\n", pid, file_path.data()));
        debug::DebugLogW(wstr);
    }

    void ProcessManager::PushRenameFileEventToProcess(size_t pid, const std::wstring& old_file_path, const std::wstring& new_file_path)
    {
        std::wstring wstr;
        wstr.resize(2000);
        wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, custom Rename event, pid %llu, from %ws to %ws\n", pid, old_file_path.data(), new_file_path.data()));
        debug::DebugLogW(wstr);
    }

    void ProcessManager::PushWriteFileEventToProcess(size_t pid, const std::wstring& file_path)
    {

    }
}
