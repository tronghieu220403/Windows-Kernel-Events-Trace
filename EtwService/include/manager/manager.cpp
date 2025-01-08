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

		// Since write event is rarer than rename event in case of ransomware, we can not use file_rename_map to see if a file is written-and-renamed

		PrintDebugW(L"Number of file I/O events: %d", file_io_list.size());

		struct FileInfo
		{
			std::wstring current_path;
			std::wstring tmp_path;
			size_t pid;
		};

		std::vector<FileInfo> file_list;
		std::vector<std::wstring> paths;
		struct FileCount
		{
			size_t file_count = 0;
			size_t total_size = 0;
			std::unordered_set<size_t> unique_dir_hashes;
		};
		std::unordered_map<size_t, FileCount> pid_file_cnt; // <pid> -> <file count, total_size>
		std::unordered_set<size_t> unique_paths;
		std::unordered_set<size_t> white_list_pid;
		std::unordered_map<size_t, size_t> file_size_map; // <file_path_hash> -> <file_size>
		for (const FileIoInfo& io : file_io_list)
		{
			const std::wstring file_path = io.file_path;
			size_t file_path_hash = std::hash<std::wstring>{}(file_path);
			if (manager::FileExist(file_path) == false || manager::IsExecutableFile(file_path) == true)
			{
				continue;
			}
			size_t file_size = 0;
			if (file_size_map.find(file_path_hash) == file_size_map.end())
			{
				file_size = manager::GetFileSize(file_path);
				file_size_map[file_path_hash] = file_size;
				if (file_size == 0)
				{
					continue;
				}
				size_t pid = io.pid;
				auto it = pid_file_cnt.find(pid);
				if (it == pid_file_cnt.end())
				{
					pid_file_cnt[pid] = {};
					it = pid_file_cnt.find(pid);
				}
				if ((io.featured_access_flags & WRITE_FLAG) || (io.featured_access_flags & RENAME_FLAG))
				{
					it->second.total_size += file_size;
					it->second.file_count++;
					it->second.unique_dir_hashes.insert(std::hash<std::wstring>{}(fs::path(file_path).parent_path().wstring()));
					unique_paths.insert(file_path_hash);
				}
				PrintDebugW(L"PID %d changed %ws", pid, file_path.c_str());
			}
		}
		for (auto& it : pid_file_cnt)
		{
			const auto& total_size = it.second.total_size;
			const auto& file_count = it.second.file_count;
			const auto& unique_dir_count = it.second.unique_dir_hashes.size();
			if (file_count >= MIN_FILE_COUNT && total_size >= MIN_TOTAL_SIZE_CHECK && unique_dir_count >= MIN_DIR_COUNT)
			{
				PrintDebugW(L"PID %d changed %d files in %d folders, total io %d bytes", it.first, file_count, unique_dir_count, total_size);
			}
			else
			{
				PrintDebugW(L"PID %d is skipped, changed %d files in %d folders, total io %d bytes", it.first, file_count, unique_dir_count, total_size);
				white_list_pid.insert(it.first);
			}
			it.second.file_count = 0;
            it.second.total_size = 0;
		}

		for (const FileIoInfo& io : file_io_list)
		{
			const std::wstring file_path = io.file_path;
			size_t file_path_hash = std::hash<std::wstring>{}(file_path);
			if (unique_paths.find(file_path_hash) == unique_paths.end())
			{
				continue;
			}
			size_t pid = io.pid;

			if (white_list_pid.find(pid) != white_list_pid.end())
			{
				continue;
			}
			size_t file_size = file_size_map[file_path_hash];
#ifndef _DEBUG
			if (pid_file_cnt[pid].total_size > FILE_MAX_TOTAL_SIZE_SCAN)
			{
				continue;
			}
            pid_file_cnt[pid].total_size += file_size;
#endif // !_DEBUG
			unique_paths.erase(file_path_hash);

			if (file_size != 0)
			{
				std::wstring tmp_path = file_path;
				if (!ulti::CheckPrintableANSI(file_path) || file_path.size() >= MAX_PATH)
				{
					tmp_path = CopyToTmp(file_path, file_size);
				}
				if (tmp_path.empty())
				{
					continue;
				}
				pid_file_cnt[pid].file_count++;
				file_list.push_back(FileInfo(file_path, tmp_path, pid));
				paths.push_back(tmp_path);
				//PrintDebugW(L"Checking %ws", tmp_path.c_str());
			}
			else
			{
				//PrintDebugW(L"File %ws is empty", file_path.c_str());
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
				manager::kProcMan->LockMutex();
				const auto& image_name = manager::kProcMan->GetImageFileName(pid);
				manager::kProcMan->UnlockMutex();
                if (!image_name.empty())
                {
                    debug::PopUpMessageBox(L"Ransomware detected", L"PID " + std::to_wstring(pid) + L" (" + image_name + L")" + L" is ransomware.");
                }
                else
                {
                    debug::PopUpMessageBox(L"Ransomware detected", L"PID " + std::to_wstring(pid) + L" is ransomware.");
                }
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