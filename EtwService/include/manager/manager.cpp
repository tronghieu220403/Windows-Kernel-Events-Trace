#include "manager.h"

namespace manager {
	void Init()
	{
		ulti::DebugLogW(L"Manager initialized");
		kProcMan = ProcessManager();
	}
}