﻿#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"
#include "file_manager.h"

namespace manager {

    struct ProcessInfo {
        size_t pid = 0;
        size_t ppid = 0;
        std::wstring image_file_name;
        std::set<size_t> cpid_list;
        bool pending_remove = false;

        // TODO: Chuyển sang class riêng để đánh giá cho ransomware
        std::unordered_map<size_t, FileIoManager> file_io;
		size_t overwrite_count = 0;
		size_t smash_and_rewrite_count = 0;
		size_t delete_and_rewrite_count = 0;

    };

    class ProcessManager {
    public:
        // Add or remove a process by its PID
        void AddProcess(size_t pid, size_t ppid);
        void RemoveProcess(size_t pid);
        void PendingRemoveProcess(size_t pid);
		void RemovePendingProcesses();

        // Add or remove a child PID from a process
        void AddChild(size_t pid, size_t cpid);
        void RemoveChild(size_t pid, size_t cpid);

        // Check if a process is an ancestor of another process
        bool IsAncestor(size_t ancestor_pid, size_t descendant_pid);

        // TODO: Tìm không ra thì hỏi kernel
		// Add image file name of a process
		void UpdateImageFileName(size_t pid, const std::wstring& image_file_name);

        // Get image file name of a process
        std::wstring GetImageFileName(size_t pid);
        
		ProcessInfo GetProcessInfo(size_t pid);

		// Add or remove a file I/O event from a process
		void PushFileIoEventToProcess(size_t pid, size_t file_object, etw::FileIoEventType file_io_event_type);

    private:
        std::unordered_map<size_t, ProcessInfo> process_map_;
		std::vector<size_t> pending_remove_;
		std::mutex pending_remove_mutex_;
    };
}
#endif  // PROCESS_MANAGER_H_
