#include "manager.h"

namespace manager {
	void Init()
	{
		debug::DebugPrintW(L"Manager initialized");
		kCurrentPid = GetCurrentProcessId();
		kProcMan = new ProcessManager();
		kFileNameObjMap = new FileNameObjMap();
		kFileIoManager = new FileIoManager();
		kDriverComm = new DriverComm();
	}

	void Cleanup()
	{
		debug::DebugPrintW(L"Manager cleaned up");
		delete kDriverComm;
		delete kFileIoManager;
		delete kFileNameObjMap;
		delete kProcMan;
	}

    void EvaluateProcess()
    {
        debug::DebugPrintW(L"%ws: Evaluating processes", __FUNCTIONW__);

		std::vector<FileIoInfo> file_io_list;

		kFileIoManager->LockMutex();
		while (kFileIoManager->GetQueueSize() > 0)
		{
			file_io_list.push_back(kFileIoManager->PopFileIoEvent());
		}
		kFileIoManager->UnlockMutex();

		struct FileInfo
		{
			std::wstring current_path;
			std::wstring tmp_path;
            size_t pid;
		};
		std::vector<FileInfo> file_list;
		std::vector<std::wstring> paths;

		for (const FileIoInfo& io : file_io_list)
        {
			const std::wstring file_path = io.file_path_cur;
			if (manager::FileExist(file_path) == false || manager::IsExecutableFile(file_path))
			{
				continue;
			}
			size_t pid = io.pid;
            size_t size = manager::GetFileSize(file_path);
            if (size != 0)
            {
				std::wstring tmp_path;
				if (!ulti::CheckPrintableANSI(file_path))
				{
					std::wstring tmp_path = CopyToTmp(file_path, min(size, FILE_MAX_SIZE_CHECK));
				}
                file_list.push_back(FileInfo(file_path, tmp_path, pid));
				paths.push_back(tmp_path.empty() ? file_path : tmp_path);
            }
        }

        // Check files and store results
        auto check_results = manager::CheckFileList(paths);

		std::unordered_map<size_t, std::pair<size_t, size_t>> proc_check_results; // <pid> -> <recognized, total>

		for (int i = 0; file_list.size(); i++)
		{
			if (manager::FileExist(file_list[i].current_path) == false)
			{
				continue;
			}
			bool is_recognized = check_results[i].second;
			const std::wstring& file_path = file_list[i].current_path;
			auto pid = file_list[i].pid;
			debug::DebugPrintW(L"%ws: PID %d%srecognized, %s", __FUNCTIONW__, pid, is_recognized ? L", " : L", not ", file_path.c_str());
			proc_check_results[pid].first += is_recognized;
			proc_check_results[pid].second++;
			size_t path_hash = std::hash<std::wstring>{}(file_path);
		}

        // Clear temporary files
        manager::ClearTempFiles();

        for (const auto& [pid, stats] : proc_check_results)
        {
            size_t recognized = stats.first;
            size_t total = stats.second;
            if (total > 20 && recognized * 100 <= total * 80)
            {
                debug::DebugPrintW(L"%ws: PID %d is ransomware.\n", __FUNCTIONW__, pid);
            }
        }
    }


	bool OverallEventFilter(size_t issuing_pid)
	{
		if (issuing_pid == 0 || issuing_pid == 4 || issuing_pid == kCurrentPid)
		{
			return false;
		}
		return true;
	}

	bool PageFaultEventFilter(size_t issuing_pid, size_t allocated_pid, size_t time_ms)
	{
		if (issuing_pid == 0 || allocated_pid == 0)
		{
			return false;
		}
		if (issuing_pid == allocated_pid)
		{
			return false;
		}
		if (issuing_pid == 4)
		{
			return false;
		}

		if (manager::kProcMan->GetProcessInfo(allocated_pid).pid == 0)
		{
			return false;
		}

		// Accecpt allocation from a father process if the time of the operation is less than 0.2 second after the process creation.
		if (manager::kProcMan->IsChild(issuing_pid, allocated_pid))
		{
			if (time_ms - manager::kProcMan->GetProcessInfo(allocated_pid).creation_time < 200)
			{
				return false;
			}
		}

		std::wstring issuing_image = manager::kProcMan->GetImageFileName(issuing_pid);
		std::wstring allocated_image = manager::kProcMan->GetImageFileName(allocated_pid);
		if (issuing_image.empty() || allocated_image.empty())
		{
			return false;
		}
		if (issuing_image == allocated_image)
		{
			return false;
		}
		return true;
	}
	bool RegistryEventFilter(size_t status, size_t handle)
	{
		if (status != 0 || handle == 0)
		{
			return false;
		}
		return true;
	}
}