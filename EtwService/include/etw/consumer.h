#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_COMSUMER_H_
#define ETWSERVICE_ETW_COMSUMER_H_

#define INITGUID

#include "ulti/support.h"
#include "ulti/collections.h"
#include "mutex/mutex.h"

#include "etw/event.h"

#include "etw/event/file/file.h"
#include "etw/event/process/process.h"
#include "etw/event/page-fault/page-fault.h"
#include "etw/event/perf-info/perf-info.h"

namespace etw
{

	class KernelConsumer
	{
	private:
		EVENT_TRACE_LOGFILE trace_ = { 0 };
		TRACEHANDLE handle_trace_ = NULL;
		inline static int pointer_size_ = 0;

	public:
		KernelConsumer();

		DWORD Open();
		DWORD StartProcessing();

		static void SetPointerSize(int pointer_size);
		static int GetPointerSize();

		static ULONG WINAPI ProcessBuffer(PEVENT_TRACE_LOGFILE p_buffer);
		
		static VOID WINAPI ProcessEvent(PEVENT_TRACE p_event);

		static VOID WINAPI ProcessFileIoEvent(Event event);
		static VOID WINAPI ProcessProcessEvent(Event event);
		static VOID WINAPI ProcessThreadEvent(Event event);
		static VOID WINAPI ProcessPageFaultEvent(Event event);
		static VOID WINAPI ProcessPerfInfoEvent(Event event);

		/*
		static VOID WINAPI ProcessImageLoadEvent(Event event);
		static VOID WINAPI ProcessRegistryEvent(Event event);
		static VOID WINAPI ProcessTcpIpEvent(Event event);
		static VOID WINAPI ProcessNetworkEvent(Event event);
		*/

		ULONG Close();

		~KernelConsumer();
	};

};

#endif

#endif