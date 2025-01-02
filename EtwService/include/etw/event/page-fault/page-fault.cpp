#include "page-fault.h"

namespace etw
{
	PageFaultVirtualAllocEventMember::PageFaultVirtualAllocEventMember(const Event& event, PageFaultVirtualAllocEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kPageFault, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"BaseAddress", event);
			event_offset->base_address_offs = p.first;
			event_offset->base_address_size = p.second;

			p = wec.GetPropertyInfo(L"RegionSize", event);
			event_offset->region_size_offs = p.first;
			event_offset->region_size_size = p.second;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			event_offset->process_id_offs = p.first;
			event_offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"Flags", event);
			event_offset->flags_offs = p.first;
			event_offset->flags_size = p.second;

			if (event_offset->base_address_size == 0 ||
				event_offset->region_size_size == 0 ||
				event_offset->process_id_size == 0 ||
				event_offset->flags_size == 0)
			{
				PrintDebugW(L"Event type %d GetPropertyInfo failed", event.GetType());
				event_offset->is_successful = false;
				return;
			}

			PrintDebugW(L"Event type %d GetPropertyInfo completed", event.GetType());
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&base_address, p_data + event_offset->base_address_offs, event_offset->base_address_size);
		memcpy(&region_size, p_data + event_offset->region_size_offs, event_offset->region_size_size);
		memcpy(&process_id, p_data + event_offset->process_id_offs, event_offset->process_id_size);
		memcpy(&flags, p_data + event_offset->flags_offs, event_offset->flags_size);
	}

	PageFaultVirtualAllocEvent::PageFaultVirtualAllocEvent(const Event& event)
	{
		this->PageFaultVirtualAllocEventMember::PageFaultVirtualAllocEventMember(event, &offset);
	}
	PageFaultVirtualFreeEvent::PageFaultVirtualFreeEvent(const Event& event)
	{
		this->PageFaultVirtualAllocEventMember::PageFaultVirtualAllocEventMember(event, &offset);
	}
}

