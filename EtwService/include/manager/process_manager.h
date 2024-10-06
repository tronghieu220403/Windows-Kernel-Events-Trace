#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "../ulti/support.h"
#include "file_manager.h"

namespace manager {

    struct ProcessInfo {
        size_t pid;
        size_t ppid;
        std::wstring image_file_name;
        std::set<size_t> cpid_list;
    };

    class ProcessManager {
    public:
        // Add or remove a process by its PID
        void AddProcess(size_t pid, size_t ppid);
        void RemoveProcess(size_t pid);

        // Add or remove a child PID from a process
        void AddChild(size_t pid, size_t cpid);
        void RemoveChild(size_t pid, size_t cpid);

        // Check if a process is an ancestor of another process
        bool IsAncestor(size_t ancestor_pid, size_t descendant_pid);

		// Add image file name of a process
		void UpdateImageFileName(size_t pid, const std::wstring& image_file_name);

        // Get image file name of a process
        std::wstring GetImageFileName(size_t pid);
        
		const ProcessInfo& GetProcessInfo(size_t pid);

    private:
        std::map<size_t, ProcessInfo> process_map_;
    };
}
#endif  // PROCESS_MANAGER_H_
