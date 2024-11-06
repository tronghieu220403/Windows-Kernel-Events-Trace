#include "manager.h"

namespace manager {
	void Init()
	{
		debug::DebugLogW(L"Manager initialized");
		kCurrentPid = GetCurrentProcessId();
		kFileMan = new FileManager();
		kProcMan = new ProcessManager();
		kDriverComm = new DriverComm();
		kPageFaultEventCache = new Cache<std::pair<size_t, size_t>>(30000, [] (std::pair<size_t, size_t> element){
			return element.first * element.second;
			});
	}

	void Cleanup()
	{
		debug::DebugLogW(L"Manager cleaned up");
		delete kPageFaultEventCache;
		delete kDriverComm;
		delete kProcMan;
		delete kFileMan;
	}

	void EvaluateProcess()
	{
		debug::DebugLogW(L"Evaluating processes");
		std::lock_guard<std::mutex> lock(manager::kProcMan->process_map_mutex_);
		std::vector<std::pair<size_t, std::vector<std::pair<int, std::wstring>>>> file_list_for_processes; // pid -> (file_size, file_path)
		std::map<std::wstring, bool> file_check_results; // native_file_hash -> is_recognized
		size_t total_size = 0;

		std::vector<std::pair<size_t, ProcessInfo>> process_list; // <pid, process_info>

		for (auto& process : kProcMan->GetProcessMap())
		{
			if (process.second.is_evaluated == true)
			{
				continue;
			}
			process_list.push_back(process);
			kProcMan->UpdateEvaluationStatus(process.first, true);
		}

		// TODO: Phân chia theo tỉ lệ số file mà mỗi process có, process thực hiện nhiều thì lấy nhiều file, thực hiện ít thì lấy ít file.

		for (auto& process: process_list)
		{
			int pid = (int)process.first;
			std::vector<std::pair<int, std::wstring>> file_list; // file_size, file_path

			size_t overwrite_count = 0;
			size_t create_write_count = 0;
			size_t read_delete_count = 0;
			size_t overwrite_rename_count = 0;

			for (auto& file : process.second.file_io)
			{
				if (file.second.evaluation_needed == true)
				{
					if (file.second.featured_access_flags == OVERWRITE_FLAG ||
						file.second.featured_access_flags == CREATE_WRITE_FLAG ||
						file.second.featured_access_flags == OVERWRITE_RENAME_FLAG)
					{
						const std::wstring win32_file_path = manager::GetWin32Path(file.second.current_native_file_path);
						size_t file_size = manager::GetFileSize(win32_file_path);
						if (file_size > FILE_MAX_SIZE_CHECK || file_size == 0)
						{
							continue;
						}
						file_list.push_back({ file_size, win32_file_path });
					}
				}
				
				// Đếm số truy cập đặc trưng
				if (file.second.featured_access_flags == OVERWRITE_FLAG)
				{
					overwrite_count += 1;
				}
				else if (file.second.featured_access_flags == CREATE_WRITE_FLAG)
				{
					create_write_count += 1;
				}
				else if (file.second.featured_access_flags == READ_DELETE_FLAG)
				{
					read_delete_count += 1;
				}
				else if (file.second.featured_access_flags == OVERWRITE_RENAME_FLAG)
				{
					overwrite_rename_count += 1;
				}
			}

			std::wstring wstr;
			wstr.resize(2000);
			wstr.resize(swprintf(wstr.data(), wstr.size(), L"Process %d, %lld files, %lld overwrite, %lld delete and rewrite, %lld smash and rewrite, %lld overwrite and rename, %lld read and delete, %lld create and write", pid, file_list.size(), overwrite_count, min(read_delete_count, create_write_count), min(overwrite_count, read_delete_count), overwrite_rename_count, read_delete_count, create_write_count));
			debug::DebugLogW(wstr);
			std::wcout << wstr << std::endl;

			if (file_list.size() == 0)
			{
				continue;
			}

			// Sort files by size in ascending order
			std::sort(file_list.begin(), file_list.end());

			// Select up to 5 files with ANSI-only characters, each not exceeding 3MB
			std::vector<std::pair<int, std::wstring>> selected_files;

			for (auto& [size, path] : file_list)
			{
				// Check if the file size is under 3MB and avoid executables
				if (size <= FILE_MAX_SIZE_CHECK)
				{
					if (!manager::IsExecutableFile(path) && ulti::CheckPrintableANSI(path))
					{
						selected_files.push_back({ size, path });
						size_t native_file_hash = std::hash<std::wstring>{}(path);
						manager::kProcMan->UpdateFileEvaluationInProcess(pid, native_file_hash, false, false);
						total_size += size;

						if (selected_files.size() == 5) break;
					}
				}
				else
				{
					break;
				}
			}

			// If fewer than 5 ANSI files were found, copy non-ANSI files into tmp
			if (selected_files.size() < 5)
			{
				for (auto& [size, path] : file_list)
				{
					if (size <= FILE_MAX_SIZE_CHECK)
					{
						if (!manager::IsExecutableFile(path))
						{
							std::wstring new_path = CopyToTmp(path, size);
							selected_files.push_back({ size, new_path });
							size_t native_file_hash = std::hash<std::wstring>{}(path);
							manager::kProcMan->UpdateFileEvaluationInProcess(pid, native_file_hash, false, false);
							total_size += size;

							if (selected_files.size() == 5) break;
						}
					}
					else
					{
						break;
					}
				}
			}

			// If still fewer than 5 files, copy the first 3MB of larger files
			if (selected_files.size() < 5)
			{
				for (auto& [size, path] : file_list)
				{
					if (size > FILE_MAX_SIZE_CHECK)
					{
						std::wstring new_path = CopyToTmp(path);
						selected_files.push_back({ FILE_MAX_SIZE_CHECK, new_path });
						size_t native_file_hash = std::hash<std::wstring>{}(path);
						manager::kProcMan->UpdateFileEvaluationInProcess(pid, native_file_hash, false, false);
						total_size += FILE_MAX_SIZE_CHECK;
						if (selected_files.size() == 5) break;
					}
				}
			}

			// Add to global list if it meets criteria
			file_list_for_processes.push_back({ pid, selected_files });
		}
		
		debug::DebugLogW(L"Check files and store results");

		// Check files and store results
		for (const auto& [pid, files] : file_list_for_processes)
		{
			std::vector<std::wstring> paths;
			for (const auto& [_, path] : files)
			{
				paths.push_back(path);
			}

			auto check_results = manager::CheckFileList(paths);
			file_check_results.insert(check_results.begin(), check_results.end());
		}
		
		// Clear temporary files
		manager::ClearTempFiles();

		// TODO: (Debug first) In kết quả ra để đánh giá

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

		// TODO: Check if the issing_pid has been terminated, if so, remove the pid from the cache.
		if (kPageFaultEventCache->Query({ issuing_pid, allocated_pid }) == true)
		{
			return false;
		}

		// Accecpt allocation from an ancestor process if the time of the operation is less than 0.2 second after the process creation.
		// The case for an ancestor process is only accepted if the issuing pid is still safe
		// TODO: Check if the issuing_pid is still safe, create a safe process list.
		if (manager::kProcMan->IsAncestor(issuing_pid, allocated_pid))
		{
			if (time_ms - manager::kProcMan->GetProcessInfo(allocated_pid).creation_time < 200)
			{
				return false;
			}
		}

		/*
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
		*/
		kPageFaultEventCache->Push({ issuing_pid, allocated_pid });

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