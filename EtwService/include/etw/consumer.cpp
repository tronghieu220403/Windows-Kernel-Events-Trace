#ifdef _WIN32

#include "consumer.h"
namespace etw
{
	KernelConsumer::KernelConsumer()
	{
        trace_.LogFileName = NULL;
        trace_.LoggerName = (LPWSTR)KERNEL_LOGGER_NAME;
        trace_.BufferCallback = (PEVENT_TRACE_BUFFER_CALLBACK)(ProcessBuffer);
        trace_.EventCallback = (PEVENT_CALLBACK)(ProcessEvent);
        trace_.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_RAW_TIMESTAMP;// | PROCESS_TRACE_MODE_EVENT_RECORD; // create real time sesion + event should be represented as EVENT_RECORD structure

	}

    DWORD KernelConsumer::Open()
    {
        handle_trace_ = OpenTrace(&trace_);
        TRACE_LOGFILE_HEADER* p_header_ = &trace_.LogfileHeader;
        if ((TRACEHANDLE)INVALID_HANDLE_VALUE == handle_trace_)
        {
            Close();
            return GetLastError();
        }

        SetPointerSize(p_header_->PointerSize);

        if (p_header_->PointerSize != sizeof(PVOID))
        {
            p_header_ = (PTRACE_LOGFILE_HEADER)((PUCHAR)p_header_ +
                2 * (p_header_->PointerSize - sizeof(PVOID)));
        }

        return ERROR_SUCCESS;
    }

    DWORD KernelConsumer::StartProcessing()
    {
        ULONG status = ProcessTrace(&handle_trace_, 1, 0, 0);
        if (status != ERROR_SUCCESS)
        {
            Close();
        }
        return status;
    }

    void KernelConsumer::SetPointerSize(int pointer_size)
    {
        pointer_size_ = pointer_size;
    }

    int KernelConsumer::GetPointerSize()
    {
        return pointer_size_;
    }

    ULONG WINAPI KernelConsumer::ProcessBuffer(PEVENT_TRACE_LOGFILE p_buffer)
    {
        UNREFERENCED_PARAMETER(p_buffer);

        return TRUE;
    }

    inline bool wstrcmp(const std::wstring& a, const std::wstring& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }
        for (int i = 0; i < a.size(); i++)
        {
            if (a[i] != b[i])
            {
                return false;
            }
        }
        return true;
    }

    VOID WINAPI KernelConsumer::ProcessEvent(PEVENT_TRACE p_event)
    {
        if (IsEqualGUID(p_event->Header.Guid, EventTraceGuid) &&
            p_event->Header.Class.Type == EVENT_TRACE_TYPE_INFO)
        {
            ; // Skip this event.
        }
        else
        {
            event_count_++;
            Event event(p_event);
            
            if (IsEqualGUID(event.GetGuid(), FileIoGuid))
            {
                //ProcessFileIoEvent(event);
            }
            else if (IsEqualGUID(event.GetGuid(), PageFaultGuid))
            {
                //ProcessPageFaultEvent(event);
            }
            else if (IsEqualGUID(event.GetGuid(), PerfInfoGuid))
            {
                //ProcessPerfInfoEvent(event);
            }
            else if (IsEqualGUID(event.GetGuid(), ThreadGuid))
            {
                //ProcessThreadEvent(event);
            }
            else if (IsEqualGUID(event.GetGuid(), ProcessGuid))
            {
                //ProcessProcessEvent(event);
            }
        }
                
        return;
    }

    VOID WINAPI KernelConsumer::ProcessFileIoEvent(Event event)
    {
        int type = event.GetType();
        // EventTypeName{"Create"}
        if (type == FileIoEventType::kCreate)
        {
            FileIoCreateEvent file_create_event(event);
            /*
            ulti::WriteDebugA("[+] Create event");
			ulti::WriteDebugW(L"    - Open Path: " + std::wstring(file_create_event.open_path));
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)file_create_event.file_object));
            ulti::WriteDebugA("\n");
            */
        }
        // EventTypeName{ "DirEnum", "DirNotify" }]
        else if (type == FileIoEventType::kDirEnum)
        {
            // FileIoDirEnumEvent dir_enum_event(event);
        }
        else if (type == FileIoEventType::kDirNotify)
        {
            // FileIoDirNotifyEvent dir_noti_event(event);
        }
        // EventTypeName{ "SetInfo", "Delete", "Rename", "QueryInfo", "FSControl" }
        else if (type == FileIoEventType::kSetInfo)
        {
            FileIoSetInfoEvent set_info_event(event);
            /*
            ulti::WriteDebugA("[+] Set info event");
            ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)set_info_event.file_object));
            ulti::WriteDebugA("\n");
            */
        }
        else if (type == FileIoEventType::kDelete)
        {
			FileIoDeleteEvent delete_event(event);
            /*
			ulti::WriteDebugA("[+] Delete event");
            ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)delete_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kRename)
		{
			FileIoRenameEvent rename_event(event);
            /*
			ulti::WriteDebugA("[+] Rename event");
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)rename_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kQueryInfo)
		{
            /*
			FileIoQueryInfoEvent query_info_event(event);
			ulti::WriteDebugA("[+] Query info event");
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)query_info_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kFSControl)
		{
            /*
			FileIoFSControlEvent fs_control_event(event);
			ulti::WriteDebugA("[+] FS control event");
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)fs_control_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		// EventTypeName{"Name", "FileCreate", "FileDelete", "FileRundown"}
		else if (type == FileIoEventType::kName)
		{
            /*
			FileIoNameEvent name_event(event);
			ulti::WriteDebugA("[+] File name event");
            ulti::WriteDebugW(L"    - File Name: " + std::wstring(name_event.file_name));
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)name_event.file_object));
            ulti::WriteDebugA("\n");
            */

		}
		else if (type == FileIoEventType::kFileCreate)
		{
            /*
			FileIoFileCreateEvent file_create_event(event);
			ulti::WriteDebugA("[+] File create event");
			ulti::WriteDebugW(L"    - File Name: " + std::wstring(file_create_event.file_name));
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)file_create_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kFileDelete)
		{
            /*
			FileIoFileDeleteEvent file_delete_event(event);
			ulti::WriteDebugA("[+] File delete event");
			ulti::WriteDebugW(L"    - File Name: " + std::wstring(file_delete_event.file_name));
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)file_delete_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kFileRundown)
		{
            /*
			FileIoFileRundownEvent file_rundown_event(event);
			ulti::WriteDebugA("[+] File rundown event");
			ulti::WriteDebugW(L"    - File Name: " + std::wstring(file_rundown_event.file_name));
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)file_rundown_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
        // EventTypeName{ "OperationEnd" }
		else if (type == FileIoEventType::kOperationEnd)
		{
			// FileIoOperationEndEvent operation_end_event(event);
		}
        // EventTypeName{"Read", "Write"}
		else if (type == FileIoEventType::kRead)
		{
			FileIoReadEvent read_event(event);
		}
		else if (type == FileIoEventType::kWrite)
		{
			FileIoWriteEvent write_event(event);
		}
        // EventTypeName{ "Cleanup", "Close", "Flush" }
        else if (type == FileIoEventType::kCleanup)
        {
            /*
            FileIoSimpleOpCleanupEvent cleanup_event(event);
			ulti::WriteDebugA("[+] Cleanup event");
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)cleanup_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kClose)
		{
            FileIoSimpleOpCloseEvent close_event(event);
            /*
			ulti::WriteDebugA("[+] Close event");
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)close_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}
		else if (type == FileIoEventType::kFlush)
		{
            /*
            FileIoSimpleOpFlushEvent flush_event(event);
			ulti::WriteDebugA("[+] Flush event");
			ulti::WriteDebugW(L"    - File Object: " + std::to_wstring((size_t)flush_event.file_object));
            ulti::WriteDebugA("\n");
            */
		}

        return VOID();
    }

    VOID __stdcall KernelConsumer::ProcessProcessEvent(Event event)
    {
        int type = event.GetType();
        if (type == ProcessEventType::kProcessStart)
        {
            ProcessStartEvent process_start_event(event);
        }
        if (type == ProcessEventType::kProcessEnd)
        {
            ProcessEndEvent process_end_event(event);
        }
        return VOID();
    }

    VOID __stdcall KernelConsumer::ProcessThreadEvent(Event event)
    {
        int type = event.GetType();
        if (type == ThreadEventType::kThreadStart)
        {
            ThreadStartEvent thread_start_event(event);
            size_t issuing_pid = event.GetProcessId();
            size_t allocated_pid = thread_start_event.pid;
        }
        else if (type == ThreadEventType::kThreadEnd)
        {
            ThreadStartEvent thread_end_event(event);
        }
        return VOID();
    }

    VOID __stdcall KernelConsumer::ProcessPageFaultEvent(Event event)
    {
        int type = event.GetType();
        if (type == PageFaultEventType::VirtualAlloc)
        {
            PageFaultVirtualAllocEvent alloc_event(event);
            size_t issuing_pid = event.GetProcessId();
            size_t allocated_pid = alloc_event.process_id;
        }
        else if (type == ThreadEventType::kThreadEnd)
        {
            PageFaultVirtualFreeEvent free_event(event);
        }

        return VOID();
    }

    VOID __stdcall KernelConsumer::ProcessPerfInfoEvent(Event event)
    {
        int type = event.GetType();
        if (type == PerfInfoEventType::SysClEnter)
        {
            SysCallEnterEvent sys_call_enter_event(event);
            ulti::WriteDebugA("[+] SysCallEnterEvent");
            ulti::WriteDebugA("    - PID:     " + std::format("{:#x}", event.GetProcessId()));
            ulti::WriteDebugA("    - TID:     " + std::format("{:#x}", event.GetThreadId()));
            ulti::WriteDebugA("    - Time:     " + std::format("{:#x}", event.GetTimeInMs()));
            ulti::WriteDebugA("    - Address: " + std::format("{:#x}",(size_t)sys_call_enter_event.sys_call_address));
            ulti::WriteDebugA("\n");
        }
        return VOID();
    }

    
    
    ULONG KernelConsumer::Close()
    {
        ULONG status = ERROR_SUCCESS;

        ulti::WriteDebugA("[+] Total event count: " + std::to_string(event_count_));

        if ((TRACEHANDLE)INVALID_HANDLE_VALUE != handle_trace_)
        {
            status = CloseTrace(handle_trace_);
            handle_trace_ = NULL;
        }
        return status;
    }

    KernelConsumer::~KernelConsumer()
    {
        if (Close() != ERROR_SUCCESS){
            // throw some exception here;
        }
        return;
    }
}

#endif