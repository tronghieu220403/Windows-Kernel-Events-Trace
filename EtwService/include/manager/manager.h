#ifndef MANAGER_H_
#define MANAGER_H_

#include "../ulti/support.h"
#include "file_manager.h"
#include "process_manager.h"
#include "../com/driver_comm.h"

namespace manager {
	inline ProcessManager* kProcMan = nullptr;
	inline FileNameObjMap* kFileNameObjMap = nullptr;
	inline FileIoManager* kFileIoManager = nullptr;
	inline DriverComm* kDriverComm = nullptr;
	inline uint64_t kCurrentPid = 0;

	void Init();
	void Cleanup();

	void EvaluateProcess();

	bool OverallEventFilter(uint64_t issuing_pid);
	bool PageFaultEventFilter(uint64_t issuing_pid, uint64_t allocated_pid, uint64_t time_ms);
	bool RegistryEventFilter(uint64_t status, uint64_t handle);
}
#endif  // MANAGER_H_
