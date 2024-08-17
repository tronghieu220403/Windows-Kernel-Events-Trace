#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_FILEIOEVENT_H_
#define ETWSERVICE_ETW_FILEIOEVENT_H_

#include "etw/event.h"
#include "etw/wmieventclass.h"

namespace etw
{
	
	struct FileCreateEvent
	{

	private:
		static inline bool is_positioned_ = false;
		static inline bool positioning_ability = true;
		static inline DWORD irp_ptr_offs = 0;

	public:
		PVOID IrpPtr = NULL;
		PVOID TTID = NULL;
		PVOID FileObject = NULL;
		DWORD CreateOptions = NULL;
		DWORD FileAttributes = NULL;
		DWORD ShareAccess = NULL;
		wchar_t* OpenPath = NULL;

		FileCreateEvent() = default;
		FileCreateEvent(const Event& event);
	};

};

#endif

#endif