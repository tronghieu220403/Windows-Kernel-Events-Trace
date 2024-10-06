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
		auto it = process_map_.find(pid);
        if (it == process_map_.end())
        {
            return;
        }

        size_t ppid = it->second.ppid;
        if (ppid != 0) {
			RemoveChild(ppid, pid);
        }

        // Reassign children to PID 0
        for (size_t child_pid : it->second.cpid_list) {
            process_map_[child_pid].ppid = 0;
        }

        process_map_.erase(it);
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
                            debug::DebugPrintW((L"PID " + std::to_wstring(pid) + L" GetProcessImageFileName failed: " + debug::GetErrorMessage(error)).c_str());
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
                debug::DebugPrintW((L"PID " + std::to_wstring(pid) + L" OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) failed, status : " + debug::GetErrorMessage(error)).c_str());
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

    void ProcessManager::PushCreateFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
        debug::DebugPrintW(L"File I/O, custom Create event, pid %llu, file %ws\n", pid, file_path.data());

		auto it = process_map_.find(pid);
		if (it != process_map_.end())
		{
            it->second.is_evaluated = false;
			size_t file_name_hash = std::hash<std::wstring>{}(file_path);
            auto file_io_it = it->second.file_io.find(file_name_hash);
            if (file_io_it == it->second.file_io.end())
            {
                it->second.file_io.insert({ file_name_hash, {} });
                file_io_it = it->second.file_io.find(file_name_hash);
                file_io_it->second.current_path = file_path;
            }
            file_io_it->second.evaluation_needed = true;
            file_io_it->second.is_recognized = false;
			file_io_it->second.featured_access_flags |= CREATE_FLAG;
		}
    }

    void ProcessManager::PushDeleteFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
        debug::DebugPrintW(L"File I/O, custom Delete event, pid %llu, file %ws\n", pid, file_path.data());

        auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            it->second.is_evaluated = false;
            size_t file_name_hash = std::hash<std::wstring>{}(file_path);
            auto file_io_it = it->second.file_io.find(file_name_hash);
            if (file_io_it == it->second.file_io.end())
            {
                it->second.file_io.insert({ file_name_hash, {} });
                file_io_it = it->second.file_io.find(file_name_hash);
                file_io_it->second.current_path = file_path;
            }
            file_io_it->second.evaluation_needed = true;
            file_io_it->second.is_recognized = false;
            file_io_it->second.featured_access_flags |= DELETE_FLAG;
        }
    }

    void ProcessManager::PushRenameFileEventToProcess(size_t pid, const std::wstring& old_file_path, const std::wstring& new_file_path)
    {
        debug::DebugPrintW(L"File I/O, custom Rename event, pid %llu, from %ws to %ws\n", pid, old_file_path.data(), new_file_path.data());

        auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            it->second.is_evaluated = false;
            size_t old_file_name_hash = std::hash<std::wstring>{}(old_file_path);
			auto old_file_io_it = it->second.file_io.find(old_file_name_hash);
            if (old_file_io_it != it->second.file_io.end())
            {
				size_t new_file_name_hash = std::hash<std::wstring>{}(new_file_path);
                it->second.file_io.insert({ new_file_name_hash, {} });
				auto new_file_io_it = it->second.file_io.find(new_file_name_hash);
				new_file_io_it->second.current_path = new_file_path;
                new_file_io_it->second.featured_access_flags = old_file_io_it->second.featured_access_flags | RENAME_FLAG;
                new_file_io_it->second.old_path = old_file_path;
                new_file_io_it->second.evaluation_needed = true;
                new_file_io_it->second.is_recognized = false;

				it->second.file_io.erase(old_file_io_it);
            }

        }
    }

    void ProcessManager::PushWriteFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
		debug::DebugPrintW(L"File I/O, custom Write event, pid %llu, file %ws\n", pid, file_path.data());

		auto it = process_map_.find(pid);
		if (it != process_map_.end())
		{
            it->second.is_evaluated = false;
            size_t file_name_hash = std::hash<std::wstring>{}(file_path);
            auto file_io_it = it->second.file_io.find(file_name_hash);
            if (file_io_it == it->second.file_io.end())
            {
                it->second.file_io.insert({ file_name_hash, {} });
                file_io_it = it->second.file_io.find(file_name_hash);
                file_io_it->second.current_path = file_path;
            }
            file_io_it->second.evaluation_needed = true;
            file_io_it->second.is_recognized = false;
            file_io_it->second.featured_access_flags |= WRITE_FLAG;
		}   
    }
    void ProcessManager::PushReadFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
		debug::DebugPrintW(L"File I/O, custom Read event, pid %llu, file %ws\n", pid, file_path.data());

		auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            //it->second.is_evaluated = false;
            size_t file_name_hash = std::hash<std::wstring>{}(file_path);
            auto file_io_it = it->second.file_io.find(file_name_hash);
            if (file_io_it == it->second.file_io.end())
            {
                it->second.file_io.insert({ file_name_hash, {} });
                file_io_it = it->second.file_io.find(file_name_hash);
                file_io_it->second.current_path = file_path;
            }
            //file_io_it->second.evaluation_needed = true;
            //file_io_it->second.is_recognized = false;
            file_io_it->second.featured_access_flags |= READ_FLAG;
        }
    }
    void ProcessManager::PushSetInfoFileEventToProcess(size_t pid, const std::wstring& file_path)
    {
        debug::DebugPrintW(L"File I/O, custom Set Info event, pid %llu, file %ws\n", pid, file_path.data());

		auto it = process_map_.find(pid);
		if (it != process_map_.end())
		{
            it->second.is_evaluated = false;
            size_t file_name_hash = std::hash<std::wstring>{}(file_path);
			auto file_io_it = it->second.file_io.find(file_name_hash);
			if (file_io_it == it->second.file_io.end())
			{
				it->second.file_io.insert({ file_name_hash, {} });
				file_io_it = it->second.file_io.find(file_name_hash);
                file_io_it->second.current_path = file_path;
			}
            file_io_it->second.evaluation_needed = true;
            file_io_it->second.is_recognized = false;
		}
    }

    const std::unordered_map<size_t, ProcessInfo>& ProcessManager::GetProcessMap()
    {
		return process_map_;
    }


    void ProcessManager::UpdateFileEvaluationInProcess(size_t pid, size_t file_hash, bool evaluation_needed, bool is_regconized)
    {
		auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
			auto file_io_it = it->second.file_io.find(file_hash);
            if (file_io_it != it->second.file_io.end())
            {
				file_io_it->second.evaluation_needed = evaluation_needed;
				file_io_it->second.is_recognized = is_regconized;
            }
        }
    }
    void ProcessManager::UpdateEvaluationStatus(size_t pid, bool is_evaluated)
    {
		auto it = process_map_.find(pid);
        if (it != process_map_.end())
        {
            it->second.is_evaluated = is_evaluated;
        }
    }
}
