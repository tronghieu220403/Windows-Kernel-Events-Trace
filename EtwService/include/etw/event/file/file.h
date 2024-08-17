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

		static inline DWORD irp_ptr_offs = 0;
		static inline DWORD ttid_offs = 0;
		static inline DWORD file_object_offs = 0;
		static inline DWORD create_options_offs = 0;
		static inline DWORD file_attributes_offs = 0;
		static inline DWORD share_access_offs = 0;
		static inline DWORD open_path_offs = 0;

		static inline DWORD irp_ptr_size = sizeof(PVOID);
		static inline DWORD ttid_size = sizeof(DWORD);
		static inline DWORD file_object_size = sizeof(PVOID);
		static inline DWORD create_options_size = sizeof(DWORD);
		static inline DWORD file_attributes_size = sizeof(DWORD);
		static inline DWORD share_access_size = sizeof(DWORD);

	public:
		PVOID IrpPtr = NULL;
		DWORD TTID = NULL;
		PVOID FileObject = NULL;
		DWORD CreateOptions = NULL;
		DWORD FileAttributes = NULL;
		DWORD ShareAccess = NULL;
		wchar_t* OpenPath = NULL;

		FileCreateEvent(const Event& event);
	};

};

#endif

#endif