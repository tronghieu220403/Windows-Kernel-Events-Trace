#include "manager.h"

namespace manager {
	void Init()
	{
		debug::DebugLogW(L"Manager initialized");
		kCurrentPid = GetCurrentProcessId();
		kFileMan = new FileManager();
		kProcMan = new ProcessManager();
		kDriverComm = new DriverComm();
	}

	void Cleanup()
	{
		debug::DebugLogW(L"Manager cleaned up");
		delete kDriverComm;
		delete kProcMan;
		delete kFileMan;
	}

	std::set<std::pair<size_t, size_t>> kPageFaultEventCache;

	bool OverallEventFilter(size_t issuing_pid)
	{
		if (issuing_pid == 0 || issuing_pid == 4 || issuing_pid == kCurrentPid)
		{
			return false;
		}
		return true;
	}

	bool PageFaultEventFilter(size_t issuing_pid, size_t allocated_pid)
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

		// The case for an ancestor process is only accepted if the issuing pid is still safe
		// TODO: Check if the issuing_pid is still safe, create a safe process list
		if (manager::kProcMan->IsAncestor(issuing_pid, allocated_pid))
		{
			return false;
		}

		if (kPageFaultEventCache.find({issuing_pid, allocated_pid}) != kPageFaultEventCache.end())
		{
			return false;
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
		kPageFaultEventCache.insert({ issuing_pid, allocated_pid });
		// TODO: Mark the issuing_pid and allocated_pid as unsafe
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