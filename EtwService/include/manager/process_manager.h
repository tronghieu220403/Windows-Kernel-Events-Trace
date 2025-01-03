#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"
#include "file_manager.h"
#include "com/driver_comm.h"

namespace manager {

    struct ProcessInfo {
        size_t pid = 0;
        size_t ppid_real = 0;
        size_t ppid_adopted = 0;
        size_t creation_time = 0;
        std::wstring image_file_name;
    };

    class ProcessManager {
    public:
        // Add or remove a process by its PID
        void AddProcess(size_t pid, size_t ppid_real, size_t ppid_adopted);
        void RemoveProcess(size_t pid);

        const std::unordered_map<size_t, ProcessInfo>& GetProcessMap();

        bool IsChild(size_t ppid, size_t pid);

        // TODO: Tìm không ra thì hỏi kernel
        // Add image file name of a process
        void UpdateImageFileName(size_t pid, const std::wstring& image_file_name);

        // Get image file name of a process
        std::wstring GetImageFileName(size_t pid);

        void UpdateProcessCreationTime(size_t pid, size_t creation_time);

        const ProcessInfo& GetProcessInfo(size_t pid);

        void LockMutex();
        void UnlockMutex();

    private:
        std::unordered_map<size_t, ProcessInfo> process_map_; // PID -> ProcessInfo
        std::mutex process_map_mutex_;
    };
}
#endif  // PROCESS_MANAGER_H_
