#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"
#include "file_manager.h"
#include "com/driver_comm.h"

namespace manager {

    struct ProcessInfo {
        uint64_t pid = 0;
        uint64_t ppid_real = 0;
        uint64_t ppid_adopted = 0;
        uint64_t creation_time = 0;
        std::wstring image_file_name;
    };

    class ProcessManager {
    public:
        // Add or remove a process by its PID
        void AddProcess(uint64_t pid, uint64_t ppid_real, uint64_t ppid_adopted);
        void RemoveProcess(uint64_t pid);

        const std::unordered_map<uint64_t, ProcessInfo>& GetProcessMap();

        bool IsChild(uint64_t ppid, uint64_t pid);

        // TODO: Tìm không ra thì hỏi kernel
        // Add image file name of a process
        void UpdateImageFileName(uint64_t pid, const std::wstring& image_file_name);

        // Get image file name of a process
        std::wstring GetImageFileName(uint64_t pid);

        void UpdateProcessCreationTime(uint64_t pid, uint64_t creation_time);

        const ProcessInfo& GetProcessInfo(uint64_t pid);

        void LockMutex();
        void UnlockMutex();

    private:
        std::unordered_map<uint64_t, ProcessInfo> process_map_; // PID -> ProcessInfo
        std::mutex process_map_mutex_;
    };
}
#endif  // PROCESS_MANAGER_H_
