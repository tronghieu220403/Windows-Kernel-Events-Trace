#include "manager.h"

namespace manager {
	void Init()
	{
		PrintDebugW(L"Manager initialized");
		kCurrentPid = GetCurrentProcessId();
		kProcMan = new ProcessManager();
		kFileNameObjMap = new FileNameObjMap();
		kFileIoManager = new FileIoManager();
		kDriverComm = new DriverComm();
	}

	void Cleanup()
	{
		PrintDebugW(L"Manager cleaned up");
		delete kDriverComm;
		delete kFileIoManager;
		delete kFileNameObjMap;
		delete kProcMan;
	}

    void EvaluateProcess()
    {
		PrintDebugW(L"Start evaluating processes");

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
		std::unordered_map<size_t, size_t> pid_file_cnt;
		std::unordered_set<size_t> unique_paths;
		for (const FileIoInfo& io : file_io_list)
        {
			const std::wstring file_path = io.file_path_cur;
			size_t file_path_hash = std::hash<std::wstring>{}(file_path);
			if (manager::FileExist(file_path) == false || manager::IsExecutableFile(file_path) || unique_paths.find(file_path_hash) != unique_paths.end())
 			{
				continue;
			}
			size_t pid = io.pid;
			pid_file_cnt[pid]++;
			unique_paths.insert(file_path_hash);
        }
		for (const FileIoInfo& io : file_io_list)
		{
			const std::wstring file_path = io.file_path_cur;
			size_t file_path_hash = std::hash<std::wstring>{}(file_path);
			if (unique_paths.find(file_path_hash) == unique_paths.end())
			{
				continue;
			}
			size_t pid = io.pid;
			if (pid_file_cnt[pid] < MIN_FILE_COUNT)
			{
				continue;
			}
			unique_paths.erase(file_path_hash);

			size_t size = manager::GetFileSize(file_path);
			if (size != 0)
			{
				std::wstring tmp_path = file_path;
				if (!ulti::CheckPrintableANSI(file_path))
				{
					tmp_path = CopyToTmp(file_path, min(size, FILE_MAX_SIZE_CHECK));
					if (tmp_path.empty())
					{
						continue;
					}
				}
				file_list.push_back(FileInfo(file_path, tmp_path, pid));
				paths.push_back(tmp_path);
			}
		}

		// Check TrID
		std::vector<std::pair<std::wstring, bool>> trid_output;
		if (!paths.empty())
		{
			trid_output = manager::CheckTrID(paths);
		}

		std::unordered_map<size_t, bool> trid_map;
		for (int i = 0; i < trid_output.size(); i++)
		{
			trid_map[std::hash<std::wstring>{}(paths[i])] = trid_output[i].second;
		}

		std::unordered_map<size_t, std::pair<size_t, size_t>> proc_check_results; // <pid> -> <recognized, total>

		for (int i = 0; i < file_list.size(); i++)
		{
			if (manager::FileExist(file_list[i].current_path) == false)
			{
				continue;
			}
			const std::wstring& file_path = file_list[i].current_path;
			size_t tmp_path_hash = std::hash<std::wstring>{}(file_list[i].tmp_path);
			bool is_recognized = false;
			if (trid_map.find(tmp_path_hash) == trid_map.end())
			{
				if (FileExist(file_path) == true && GetFileExtension(file_path) == L"txt")
				{
					is_recognized = manager::IsPrintableFile(file_path);
				}
				else
				{
					continue;
				}
			}
			is_recognized = trid_map[tmp_path_hash];
			auto pid = file_list[i].pid;
			PrintDebugW(L"PID %d%srecognized, %s", pid, is_recognized ? L", " : L", not ", file_path.c_str());
			proc_check_results[pid].first += is_recognized;
			proc_check_results[pid].second++;
		}

        // Clear temporary files
        manager::ClearTmpFiles();

        for (const auto& [pid, stats] : proc_check_results)
        {
            size_t num_recognized = stats.first;
            size_t total = stats.second;
            if (total >= MIN_FILE_COUNT && BelowThreshold(num_recognized, total))
            {
				PrintDebugW(L"PID %d is ransomware.", pid);
			}
        }
		PrintDebugW(L"Process evaluation done");
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