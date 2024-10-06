#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_PERFINFO_PERFINFO_H_
#define ETWSERVICE_ETW_ETW_EVENT_PERFINFO_PERFINFO_H_

#include "etw/event.h"
#include "etw/wmieventclass.h"
#include "ulti/debug.h"

namespace etw
{
	enum PerfInfoEventType
	{
		SysClEnter = 51,
		SysClExit = 52
	};
	/*
	[EventType{51}, EventTypeName{"SysClEnter"}]
	class SysCallEnter : PerfInfo
	{
		uint32 SysCallAddress;
	};
	*/
	struct SysCallEnterEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD syscall_address_offs = 0;
		DWORD syscall_address_size = 0;
	};

	struct SysCallEnterEventMember
	{
	public:
		size_t sys_call_address = 0;

		SysCallEnterEventMember() = default;
		SysCallEnterEventMember(const Event& event, SysCallEnterEventOffset* event_offset);
	};

	struct SysCallEnterEvent : SysCallEnterEventMember
	{
	private:
		static inline SysCallEnterEventOffset offset;
	public:
		SysCallEnterEvent(const Event& event);
	};

}

#endif

#endif
