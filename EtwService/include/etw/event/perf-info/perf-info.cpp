#include "perf-info.h"

namespace etw
{
	SysCallEnterEventMember::SysCallEnterEventMember(const Event& event, SysCallEnterEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kPerfInfo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"SysCallAddress", event);
			event_offset->syscall_address_offs = p.first;
			event_offset->syscall_address_size = p.second;

			if (event_offset->syscall_address_size == 0)
			{
				debug::DebugLogW(std::wstring(L"[+] [") + __FUNCTIONW__ + L":" + std::to_wstring(__LINE__) + L"] Event type " + std::to_wstring(event.GetType()) + L" GetPropertyInfo failed\n");
				event_offset->is_successful = false;
				return;
			}

			debug::DebugLogW(std::wstring(L"[+] [") + __FUNCTIONW__ + L":" + std::to_wstring(__LINE__) + L"] Event type " + std::to_wstring(event.GetType()) + L" GetPropertyInfo completed\n");
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&sys_call_address, p_data + event_offset->syscall_address_offs, event_offset->syscall_address_size);
	}

	SysCallEnterEvent::SysCallEnterEvent(const Event& event)
	{
		this->SysCallEnterEventMember::SysCallEnterEventMember(event, &offset);
	}

}
