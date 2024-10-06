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
            
            if (manager::OverallEventFilter(event.GetProcessId()) == false)
            {
				return;
            }

            if (IsEqualGUID(event.GetGuid(), FileIoGuid))
            {
                ProcessFileIoEvent(event);
            }
            else if (IsEqualGUID(event.GetGuid(), PageFaultGuid))
            {
                ProcessPageFaultEvent(event);
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
                ProcessProcessEvent(event);
            }
        }
                
        return;
    }

    inline void PrintDebugFileObject(size_t file_object)
    {
        std::wstring file_path = manager::kFileMan->GetFilePath(file_object);
        if (file_path.empty())
        {
            debug::DebugLogW(L"    - File Object: 0x" + std::format(L"{:x}", file_object));
        }
        else
        {
            debug::DebugLogW(L"    - File Path:   " + file_path);
        }
    }

	inline void PrintDebugPid(size_t pid)
	{
        std::wstring issuing_image_path = manager::kProcMan->GetImageFileName(pid);
        if (issuing_image_path.empty())
        {
            debug::DebugLogW(L"    - PID:           " + std::to_wstring(pid));
        }
        else
        {
            debug::DebugLogW(L"    - Issuing Image: " + issuing_image_path);
        }
    }

    VOID WINAPI KernelConsumer::ProcessFileIoEvent(Event event)
    {
        int type = event.GetType();
		std::wstring file_path;
        // EventTypeName{"Create"}
        if (type == FileIoEventType::kCreate)
        {
            FileIoCreateEvent file_create_event(event);
			file_path = file_create_event.open_path;

			manager::kFileMan->AddFile((size_t)file_create_event.file_object, file_path);

            debug::DebugLogW(L"[+] File I/O: Create event");
			debug::DebugLogW(L"    - Open Path:   " + file_path);
			debug::DebugLogW(L"    - File Object: 0x" + std::format(L"{:x}", (size_t)file_create_event.file_object));
			PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");

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
            debug::DebugLogW(L"[+] File I/O: Set info event");
			PrintDebugFileObject((size_t)set_info_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
        }
        else if (type == FileIoEventType::kDelete)
        {
			FileIoDeleteEvent delete_event(event);
			debug::DebugLogW(L"[+] File I/O: Delete event");
			PrintDebugFileObject((size_t)delete_event.file_object);
			PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
		}
		else if (type == FileIoEventType::kRename)
		{
			FileIoRenameEvent rename_event(event);
			debug::DebugLogW(L"[+] File I/O: Rename event");
            PrintDebugFileObject((size_t)rename_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
		}
		else if (type == FileIoEventType::kQueryInfo)
		{
			FileIoQueryInfoEvent query_info_event(event);
            /*
			debug::DebugLogW(L"[+] File I/O: Query info event");
			debug::DebugLogW(L"    - File Object: 0x" + std::format(L"{:x}", (size_t)query_info_event.file_object));
            debug::DebugLogW(L"    - PID:         " + std::to_wstring(event.GetProcessId()));
            debug::DebugLogW(L"\n");
            */
		}
		else if (type == FileIoEventType::kFSControl)
		{
			FileIoFSControlEvent fs_control_event(event);
			debug::DebugLogW(L"[+] File I/O: FS control event");
            PrintDebugFileObject((size_t)fs_control_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
		}
		// EventTypeName{"Name", "FileCreate", "FileDelete", "FileRundown"}
		else if (type == FileIoEventType::kName)
		{
			FileIoNameEvent name_event(event);
			debug::DebugLogW(L"[+] File I/O: File name event");
            debug::DebugLogW(L"    - File Name:   " + std::wstring(name_event.file_name));
            PrintDebugFileObject((size_t)name_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
		}
		else if (type == FileIoEventType::kFileCreate)
		{
			FileIoFileCreateEvent file_create_event(event);
			debug::DebugLogW(L"[+] File I/O: File create event");
			debug::DebugLogW(L"    - File Name: " + std::wstring(file_create_event.file_name));
            PrintDebugFileObject((size_t)file_create_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
		}
		else if (type == FileIoEventType::kFileDelete)
		{
			FileIoFileDeleteEvent file_delete_event(event);
			debug::DebugLogW(L"[+] File I/O: File delete event");
			debug::DebugLogW(L"    - File Name:   " + std::wstring(file_delete_event.file_name));
            PrintDebugFileObject((size_t)file_delete_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
		}
		else if (type == FileIoEventType::kFileRundown)
		{
			FileIoFileRundownEvent file_rundown_event(event);
			debug::DebugLogW(L"[+] File I/O: File rundown event");
			debug::DebugLogW(L"    - File Name:   " + std::wstring(file_rundown_event.file_name));
            PrintDebugFileObject((size_t)file_rundown_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
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
			debug::DebugLogW(L"[+] File I/O: Cleanup event");
			debug::DebugLogW(L"    - File Object: 0x" + std::format(L"{:x}", (size_t)cleanup_event.file_object));
            debug::DebugLogW(L"\n");
            */
		}
		else if (type == FileIoEventType::kClose)
		{
            FileIoSimpleOpCloseEvent close_event(event);

			debug::DebugLogW(L"[+] File I/O: Close event");
            PrintDebugFileObject((size_t)close_event.file_object);
            PrintDebugPid(event.GetProcessId());
            debug::DebugLogW(L"\n");
            
            manager::kFileMan->RemoveFile((size_t)close_event.file_object);

		}
		else if (type == FileIoEventType::kFlush)
		{
            /*
            FileIoSimpleOpFlushEvent flush_event(event);
			debug::DebugLogW(L"[+] File I/O: Flush event");
			debug::DebugLogW(L"    - File Object: 0x" + std::format(L"{:x}", (size_t)flush_event.file_object));
            debug::DebugLogW(L"\n");
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
            debug::DebugLogW(L"[+] Process: Start event");
            debug::DebugLogW(L"    - PID:     " + std::to_wstring(process_start_event.pid));
			debug::DebugLogW(L"    - PPID:    " + std::to_wstring(event.GetProcessId()));
            debug::DebugLogW(L"    - Time:    " + std::to_wstring((size_t)event.GetTimeInMicroSec()));
			std::string image_name = std::string((PCHAR)process_start_event.image_file_name);
            debug::DebugLogW(L"    - Image:   " + std::wstring(image_name.begin(), image_name.end()));
			// TODO: Dump process name from command line
            debug::DebugLogW(L"    - Command Line: " + std::wstring((PWCHAR)process_start_event.command_line));
            debug::DebugLogW(L"\n");
            if (manager::kProcMan->GetImageFileName(process_start_event.pid).empty())
            {
                manager::kProcMan->AddProcess(process_start_event.pid, event.GetProcessId());
            }
        }
        if (type == ProcessEventType::kProcessEnd)
        {
            ProcessEndEvent process_end_event(event);
            debug::DebugLogW(L"[+] Process: Start event");
            debug::DebugLogW(L"    - PID:     " + std::to_wstring(process_end_event.pid));
            debug::DebugLogW(L"    - Time:    " + std::to_wstring((size_t)event.GetTimeInMicroSec()));
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
			if (manager::PageFaultEventFilter(issuing_pid, allocated_pid))
			{
				debug::DebugLogW(L"[+] VirtualAlloc event is accepted");
				debug::DebugLogW(L"    - Size:            0x" + std::format(L"{:x}", alloc_event.region_size));
                debug::DebugLogW(L"    - Issuing PID:     " + std::to_wstring(issuing_pid));
                debug::DebugLogW(L"    - Issuing Image:   " + manager::kProcMan->GetImageFileName(issuing_pid));
                debug::DebugLogW(L"    - Allocated PID:   " + std::to_wstring(allocated_pid));
                debug::DebugLogW(L"    - Allocated Image: " + manager::kProcMan->GetImageFileName(allocated_pid));
				debug::DebugLogW(L"    - Time:            " + std::to_wstring((size_t)event.GetTimeInMicroSec()));
                debug::DebugLogW(L"\n");
			}
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
            debug::DebugLogW(L"[+] SysCallEnterEvent");
            debug::DebugLogW(L"    - PID:     " + std::to_wstring(event.GetProcessId()));
            debug::DebugLogW(L"    - TID:     " + std::to_wstring(event.GetThreadId()));
            debug::DebugLogW(L"    - Time:     " + std::to_wstring(event.GetTimeInMs()));
            debug::DebugLogW(L"    - Address: " + std::to_wstring((size_t)sys_call_enter_event.sys_call_address));
            debug::DebugLogW(L"\n");
        }
        return VOID();
    }

    
    
    ULONG KernelConsumer::Close()
    {
        ULONG status = ERROR_SUCCESS;

        debug::DebugLogW(L"[+] Total event count: " + std::to_wstring(event_count_));

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