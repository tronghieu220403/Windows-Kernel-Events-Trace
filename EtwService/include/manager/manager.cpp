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

		// TODO: Check if the issing_pid has been terminated, if so, remove the pid from the cache.
		if (kPageFaultEventCache->Query({ issuing_pid, allocated_pid }) == true)
		{
			return false;
		}

		// Accecpt allocation from an ancestor process if the time of the operation is less than 0.5 second after the process creation.
		// The case for an ancestor process is only accepted if the issuing pid is still safe
		// TODO: Check if the issuing_pid is still safe, create a safe process list.
		if (manager::kProcMan->IsAncestor(issuing_pid, allocated_pid))
		{
			if (time_ms - manager::kProcMan->GetProcessInfo(allocated_pid).creation_time < 400)
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