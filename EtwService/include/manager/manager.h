#ifndef MANAGER_H_
#define MANAGER_H_

#include "../ulti/support.h"
#include "file_manager.h"
#include "process_manager.h"
#include "../com/driver_comm.h"

namespace manager {
	inline ProcessManager* kProcMan = nullptr;
	inline FileManager* kFileMan = nullptr;
	inline DriverComm* kDriverComm = nullptr;
	inline size_t kCurrentPid = 0;
	
	void Init();
	void Cleanup();

	bool OverallEventFilter(size_t issuing_pid);
	bool PageFaultEventFilter(size_t issuing_pid, size_t allocated_pid);
}
#endif  // MANAGER_H_
