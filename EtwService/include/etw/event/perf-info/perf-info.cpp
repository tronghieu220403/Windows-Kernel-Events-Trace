#include "perf-info.h"

namespace etw
{
	SysCallEnterEventMember::SysCallEnterEventMember(const Event& event, SysCallEnterEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kPerfInfo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"SysCallAddress", event);
			offset->syscall_address_offs = p.first;
			offset->syscall_address_size = p.second;

			if (offset->syscall_address_size == 0)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in SysCallEnterEvent");
				offset->is_successful = false;
				return;
			}

			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&sys_call_address, p_data + offset->syscall_address_offs, offset->syscall_address_size);
	}

	SysCallEnterEvent::SysCallEnterEvent(const Event& event)
	{
		this->SysCallEnterEventMember::SysCallEnterEventMember(event, &offset);
	}

}
