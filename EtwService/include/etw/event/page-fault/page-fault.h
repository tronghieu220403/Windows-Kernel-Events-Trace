#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_PAGEFAULT_PAGEFAULT_H_
#define ETWSERVICE_ETW_ETW_EVENT_PAGEFAULT_PAGEFAULT_H_

#include "etw/event.h"
#include "etw/wmieventclass.h"
#include "ulti/debug.h"

namespace etw
{
	enum PageFaultEventType
	{
		kVirtualAlloc = 98,
		kVirtualFree = 99
	};
	/*
	[EventType{98, 99}, EventTypeName{"VirtualAlloc", "VirtualFree"}]
	class PageFault_VirtualAlloc : PageFault_V2
	{
		size_t BaseAddress;
		size_t RegionSize;
		size_t ProcessId;
		size_t Flags;
	};
	*/
	struct PageFaultVirtualAllocEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD base_address_offs = 0;
		DWORD region_size_offs = 0;
		DWORD process_id_offs = 0;
		DWORD flags_offs = 0;

		DWORD base_address_size = 0;
		DWORD region_size_size = 0;
		DWORD process_id_size = 0;
		DWORD flags_size = 0;
	};

	struct PageFaultVirtualAllocEventMember
	{
	public:
		size_t base_address = 0;
		size_t region_size = 0;
		size_t process_id = 0;
		size_t flags = 0;

		PageFaultVirtualAllocEventMember() = default;
		PageFaultVirtualAllocEventMember(const Event& event, PageFaultVirtualAllocEventOffset* event_offset);
	};

	struct PageFaultVirtualAllocEvent : PageFaultVirtualAllocEventMember
	{
	private:
		static inline PageFaultVirtualAllocEventOffset offset;
	public:
		PageFaultVirtualAllocEvent(const Event& event);
	};

	struct PageFaultVirtualFreeEvent : PageFaultVirtualAllocEventMember
	{
	private:
		static inline PageFaultVirtualAllocEventOffset offset;
	public:
		PageFaultVirtualFreeEvent(const Event& event);
	};

}

#endif

#endif
