#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_PROVIDER_H_
#define ETWSERVICE_ETW_PROVIDER_H_

#define INITGUID

#include "ulti/support.h"

#if defined(_UNICODE) || defined(UNICODE)
#define HIEUNT_LOGGER_NAME L"Hieunt Kernel Logger"
#else
#define HIEUNT_LOGGER_NAME "Hieunt Kernel Logger"
#endif

//
// HieuntTraceControlGuid.
//
DEFINE_GUID( /* 501b69a5-3f1e-46d3-ac55-a9892855f1dc */
	HieuntTraceControlGuid,
	0x501b69a5,
	0x3f1e,
	0x46d3,
	0xac, 0x55, 0xa9, 0x89, 0x28, 0x55, 0xf1, 0xdc
);

namespace etw
{
	class KernelProvider
	{
	private:
		TRACEHANDLE session_handle_ = NULL;
		DWORD flags_ = NULL;
		int buffer_size_ = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(HIEUNT_LOGGER_NAME) * 2;
		EVENT_TRACE_PROPERTIES* session_properties_;

	public:

		KernelProvider();
		KernelProvider(ULONG flags);

		void Initialize();

		void SetFlags(ULONG flags);

		TRACEHANDLE GetSessionHandle() const;
		void SetSessionHandle(TRACEHANDLE session_handle);
		EVENT_TRACE_PROPERTIES GetSessionProperties() const;

		ULONG BeginTrace();
		ULONG CloseTrace();

		~KernelProvider();
	};

}


#endif

#endif
