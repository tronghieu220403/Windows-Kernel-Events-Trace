#include "manager.h"

namespace manager {
	void Init()
	{
		debug::DebugPrintW(L"Manager initialized");
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
		debug::DebugPrintW(L"Manager cleaned up");
		delete kPageFaultEventCache;
		delete kDriverComm;
		delete kProcMan;
		delete kFileMan;
	}

    void EvaluateProcess()
    {
        debug::DebugPrintW(L"Evaluating processes");
        std::vector<std::pair<size_t, std::vector<std::pair<size_t, std::wstring>>>> file_list_for_processes; // pid -> (file_size, file_path)
        std::vector<std::pair<std::pair<std::wstring, size_t>, bool>> file_check_results; // <file_path, pid>, is_recognized
        size_t total_size = 0;

        std::vector<std::pair<size_t, ProcessInfo>> process_list; // <pid, process_info>

        manager::kProcMan->process_map_mutex_.lock();
        for (auto& process : kProcMan->GetProcessMap())
        {
            if (process.second.is_evaluated == true)
            {
                continue;
            }
            process_list.push_back(process);
            kProcMan->UpdateEvaluationStatus(process.first, true);
        }
        manager::kProcMan->process_map_mutex_.unlock();

        // TODO: Phân chia theo tỉ lệ số file mà mỗi process có, process thực hiện nhiều thì lấy nhiều file, thực hiện ít thì lấy ít file.
        std::unordered_map<size_t, std::wstring> tmp_to_ori_map; // <tmp_path_hash, original_path>

        size_t total_files_needing_evaluation = 0;
        std::unordered_map<size_t, size_t> process_file_counts; // pid -> number of files needing evaluation
        std::unordered_map<size_t, std::vector<std::pair<size_t, std::wstring>>> process_files; // pid -> list of (file_size, file_path)

        // First pass: Collect files needing evaluation and count totals
        for (auto& process : process_list)
        {
            size_t pid = process.first;
            std::vector<std::pair<size_t, std::wstring>> file_list; // file_size, file_path

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
                        const std::wstring path = file.second.current_path;
                        size_t file_size = manager::GetFileSize(path);
                        if (file_size > FILE_MAX_SIZE_CHECK || file_size == 0)
                        {
                            continue;
                        }
                        file_list.push_back({ file_size, path });
                    }
                }
                else
                {
                    // TODO: Thống kê số false và true để cuối hàm sẽ tính hoặc in ra.
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

            /*
            std::wstring wstr;
            wstr.resize(2000);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"Process %d, %lld files, %lld overwrite, %lld delete and rewrite, %lld smash and rewrite, %lld overwrite and rename, %lld read and delete, %lld create and write", pid, file_list.size(), overwrite_count, min(read_delete_count, create_write_count), min(overwrite_count, read_delete_count), overwrite_rename_count, read_delete_count, create_write_count));
            debug::DebugPrintW(wstr);
            std::wcout << wstr << std::endl;
            */

            if (file_list.size() == 0)
            {
                continue;
            }

            // Sort files by size in ascending order
            std::sort(file_list.begin(), file_list.end());

            // Store the file list and counts for this process
            process_files[pid] = file_list;
            process_file_counts[pid] = file_list.size();
            total_files_needing_evaluation += file_list.size();
        }

        // Compute number of files to select per process
        struct ProcessSelection
        {
            size_t pid;
            size_t num_files_in_process;
            double exact_files_to_select;
            size_t integer_part;
            double fractional_part;
        };

        std::vector<ProcessSelection> process_selections;
        for (auto& [pid, num_files_in_process] : process_file_counts)
        {
            double exact_files_to_select = (num_files_in_process / (double)total_files_needing_evaluation) * 100.0;
            size_t integer_part = (size_t)exact_files_to_select;
            double fractional_part = exact_files_to_select - integer_part;

            process_selections.push_back({ pid, num_files_in_process, exact_files_to_select, integer_part, fractional_part });
        }

        // Adjust for rounding errors
        size_t total_integer_parts = 0;
        for (auto& ps : process_selections)
        {
            total_integer_parts += ps.integer_part;
        }

        size_t remaining_files = MAX_TOTAL_SIZE_CHECK / FILE_MAX_SIZE_CHECK - total_integer_parts;

        // Distribute remaining files based on largest fractional parts
        std::sort(process_selections.begin(), process_selections.end(), [](const ProcessSelection& a, const ProcessSelection& b) {
            return a.fractional_part > b.fractional_part;
            });

        size_t index = 0;
        while (remaining_files > 0 && index < process_selections.size())
        {
            process_selections[index].integer_part += 1;
            remaining_files -= 1;
            index += 1;
        }

        // Second pass: Select files per process based on calculated counts
        for (const auto& ps : process_selections)
        {
            size_t pid = ps.pid;
            size_t num_files_to_select = ps.integer_part;

            // Ensure we do not select more files than available
            num_files_to_select = min(num_files_to_select, ps.num_files_in_process);

            if (num_files_to_select == 0)
            {
                continue;
            }

            auto& file_list = process_files[pid];

            // The file_list is already sorted by size in ascending order.

            // Now, select files according to the criteria, up to num_files_to_select.
            std::vector<std::pair<size_t, std::wstring>> selected_files;

            for (auto& [size, path] : file_list)
            {
                // Check if the file size is under 3MB and avoid executables
                if (size <= FILE_MAX_SIZE_CHECK)
                {
                    if (!manager::IsExecutableFile(path) && ulti::CheckPrintableANSI(path))
                    {
                        selected_files.push_back({ size, path });
                        size_t file_hash = std::hash<std::wstring>{}(path);
                        manager::kProcMan->process_map_mutex_.lock();
                        manager::kProcMan->UpdateFileEvaluationInProcess(pid, file_hash, false, false);
                        manager::kProcMan->process_map_mutex_.unlock();
                        total_size += size;

                        if (selected_files.size() == num_files_to_select) break;
                    }
                }
                else
                {
                    break;
                }
            }

            // If fewer files were found, copy non-ANSI files into tmp
            if (selected_files.size() < num_files_to_select)
            {
                for (auto& [size, path] : file_list)
                {
                    if (size <= FILE_MAX_SIZE_CHECK)
                    {
                        if (!manager::IsExecutableFile(path))
                        {
                            size_t path_hash = std::hash<std::wstring>{}(path);
                            std::wstring new_path = CopyToTmp(path);
                            // Copy vào temp folder thì phải map lại đường dẫn cũ
                            tmp_to_ori_map[std::hash<std::wstring>{}(new_path)] = path;
                            selected_files.push_back({ size, new_path });
                            manager::kProcMan->process_map_mutex_.lock();
                            manager::kProcMan->UpdateFileEvaluationInProcess(pid, path_hash, false, false);
                            manager::kProcMan->process_map_mutex_.unlock();
                            total_size += size;

                            if (selected_files.size() == num_files_to_select) break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // If still fewer files, copy the first FILE_MAX_SIZE_CHECK of larger files
            if (selected_files.size() < num_files_to_select)
            {
                for (auto& [size, path] : file_list)
                {
                    if (size > FILE_MAX_SIZE_CHECK)
                    {
                        size_t path_hash = std::hash<std::wstring>{}(path);
                        std::wstring new_path = CopyToTmp(path);
                        // Copy vào temp folder thì phải map lại đường dẫn cũ
                        tmp_to_ori_map[std::hash<std::wstring>{}(new_path)] = path;
                        selected_files.push_back({ FILE_MAX_SIZE_CHECK, new_path });
                        manager::kProcMan->process_map_mutex_.lock();
                        manager::kProcMan->UpdateFileEvaluationInProcess(pid, path_hash, false, false);
                        manager::kProcMan->process_map_mutex_.unlock();
                        total_size += FILE_MAX_SIZE_CHECK;
                        if (selected_files.size() == num_files_to_select) break;
                    }
                }
            }

            /*
            std::wstring wstr;
            wstr.resize(2000);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"Process %d selected %lld files", pid, selected_files.size()));
            debug::DebugPrintW(wstr);
            std::wcout << wstr << std::endl;
            */

            // Add to global list if it meets criteria
            if (selected_files.size() > 0)
            {
                file_list_for_processes.push_back({ pid, selected_files });
            }
        }

        // Collect all paths and store mapping from path to pid
        std::vector<std::wstring> all_paths;
		std::unordered_map<size_t, size_t> path_to_pid; // path_hash -> pid

        for (const auto& [pid, files] : file_list_for_processes)
        {
            for (const auto& [_, path] : files)
            {
                all_paths.push_back(path);
                size_t path_hash = std::hash<std::wstring>{}(path);
                path_to_pid[path_hash] = pid;
            }
        }

        // Check files and store results
        auto check_results = manager::CheckFileList(all_paths);

        for (auto& [path, is_recognized] : check_results)
        {
            size_t path_hash = std::hash<std::wstring>{}(path);
            // Lấy pid từ mapping path_to_pid
            size_t pid = path_to_pid[path_hash];

            // Khôi phục lại đường dẫn gốc nếu file nằm trong temp folder
            auto it = tmp_to_ori_map.find(path_hash);
            if (it != tmp_to_ori_map.end())
            {
                path_hash = std::hash<std::wstring>{}(it->second);
                path = it->second;
            }

            manager::kProcMan->process_map_mutex_.lock();
            manager::kProcMan->UpdateFileEvaluationInProcess(pid, path_hash, false, is_recognized);
            manager::kProcMan->process_map_mutex_.unlock();
            file_check_results.push_back({ { path, pid }, is_recognized });
        }

        // Clear temporary files
        manager::ClearTempFiles();

        for (const auto& [p, is_recognized] : file_check_results)
        {
            const auto& [path, pid] = p;
            debug::DebugPrintW(L"PID %d%srecognized, %s\n",
                pid,
                is_recognized ? L", " : L", not ",
                path.c_str());
        }

        // TODO: Evaluate the results and take actions
		std::unordered_map<size_t, std::pair<size_t, size_t>> process_stats; // pid -> <recognized, total>
		for (const auto& [p, is_recognized] : file_check_results)
		{
			const auto& [path, pid] = p;
			auto it = process_stats.find(pid);
			if (it == process_stats.end())
			{
				process_stats[pid] = { 0, 0 };
				it = process_stats.find(pid);
			}
			it->second.second += 1;
			if (is_recognized == true)
			{
                it->second.first += 1;
			}
		}

        for (const auto& [pid, stats] : process_stats)
        {
            size_t recognized = stats.first;
            size_t total = stats.second;
            if (total > 20 && recognized * 100 <= total * 80)
            {
                debug::DebugPrintW(L"PID %d is ransomware.\n", pid);
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