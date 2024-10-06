#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"
#include "file_manager.h"
#include "com/driver_comm.h"

namespace manager {

    struct ProcessInfo {
        size_t pid = 0;
        size_t ppid = 0;
		size_t creation_time = 0;
        std::wstring image_file_name;
        std::set<size_t> cpid_list;

		bool is_evaluated = false;

        // TODO: Chuyển sang class riêng để đánh giá cho ransomware
		std::unordered_map<size_t, FileIoManager> file_io; // Current: file_path_hash -> file_path
        /*
        1) Ghi đè file A
        2) Đọc file A, tạo + viết file B, xóa file A (vẫn còn nội dung ở disk)
        3) Đọc file A, tạo + viết file B (rewrite), ghi đè file A (xóa nội dung khỏi disk - smash)
        4) Ghi đè file A, đổi tên thành file B
        */
		size_t overwrite_count = 0;
		size_t delete_and_rewrite_count = 0;
        size_t smash_and_rewrite_count = 0;
		size_t overwrite_and_rename_count = 0;
    };

    class ProcessManager {
    public:
        // Add or remove a process by its PID
        void AddProcess(size_t pid, size_t ppid);
        void RemoveProcess(size_t pid);

        const std::unordered_map<size_t, ProcessInfo>& GetProcessMap();

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
        
		void UpdateProcessCreationTime(size_t pid, size_t creation_time);

        const ProcessInfo& GetProcessInfo(size_t pid);

		void PushCreateFileEventToProcess(size_t pid, const std::wstring& file_path);

		void PushDeleteFileEventToProcess(size_t pid, const std::wstring& file_path);

		void PushRenameFileEventToProcess(size_t pid, const std::wstring& old_file_path, const std::wstring& new_file_path);

		void PushWriteFileEventToProcess(size_t pid, const std::wstring& file_path);

		void PushReadFileEventToProcess(size_t pid, const std::wstring& file_path);

		void PushSetInfoFileEventToProcess(size_t pid, const std::wstring& file_path);

		void UpdateFileEvaluationInProcess(size_t pid, size_t file_hash, bool evaluation_needed, bool is_regconized);
        
		void UpdateEvaluationStatus(size_t pid, bool is_evaluated);

    private:
		std::unordered_map<size_t, ProcessInfo> process_map_; // PID -> ProcessInfo
    public:
		std::mutex process_map_mutex_;
    };
}
#endif  // PROCESS_MANAGER_H_
