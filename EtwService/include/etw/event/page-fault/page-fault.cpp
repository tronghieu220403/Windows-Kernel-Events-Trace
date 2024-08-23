#include "page-fault.h"

namespace etw
{
	PageFaultVirtualAllocEventMember::PageFaultVirtualAllocEventMember(const Event& event, PageFaultVirtualAllocEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kPageFault, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"BaseAddress", event);
			offset->base_address_offs = p.first;
			offset->base_address_size = p.second;

			p = wec.GetPropertyInfo(L"RegionSize", event);
			offset->region_size_offs = p.first;
			offset->region_size_size = p.second;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			offset->process_id_offs = p.first;
			offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"Flags", event);
			offset->flags_offs = p.first;
			offset->flags_size = p.second;

			if (offset->base_address_size == 0 ||
				offset->region_size_size == 0 ||
				offset->process_id_size == 0 ||
				offset->flags_size == 0)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in PageFaultVirtualAllocEvent");
				offset->is_successful = false;
				return;
			}

			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&base_address, p_data + offset->base_address_offs, offset->base_address_size);
		memcpy(&region_size, p_data + offset->region_size_offs, offset->region_size_size);
		memcpy(&process_id, p_data + offset->process_id_offs, offset->process_id_size);
		memcpy(&flags, p_data + offset->flags_offs, offset->flags_size);
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

