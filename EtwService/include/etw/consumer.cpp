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
            Event event(p_event);
            
            if (manager::OverallEventFilter(event.GetProcessId()) == false)
            {
				return;
            }
            event_count_++;

            if (IsEqualGUID(event.GetGuid(), FileIoGuid))
            {
                //ProcessFileIoEvent(event);
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
            else if (IsEqualGUID(event.GetGuid(), RegistryGuid))
            {
                //ProcessRegistryEvent(event);
            }
        }
                
        return;
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
            std::wstring wstr;
            wstr.resize(2000);
			int pid = static_cast<int>(event.GetProcessId());
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, Create event, path %ws, file object 0x%p, create options 0x%llx, file attributes 0x%llx, pid %d, image path %ws\n", file_create_event.open_path, file_create_event.file_object, file_create_event.create_options, file_create_event.file_attributes, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
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
            std::wstring wstr;
            wstr.resize(2000);
			int pid = static_cast<int>(event.GetProcessId());
			file_path = manager::kFileMan->GetFilePath((size_t)set_info_event.file_object);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, SetInfo event, path %ws, file object 0x%p, IrpPtr 0x%p, file key 0x%p, extra info 0x%llx, info class 0x%p, pid %d, image path %ws\n", file_path.data(), set_info_event.file_object, set_info_event.irp_ptr, set_info_event.file_key, set_info_event.extra_info, set_info_event.info_class, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
        }
        else if (type == FileIoEventType::kDelete)
        {
		}
		else if (type == FileIoEventType::kRename)
		{
			FileIoRenameEvent rename_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath((size_t)rename_event.file_object);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, Rename event, path %ws, file object 0x%p, IrpPtr 0x%p, file key 0x%p, extra info 0x%llx, info class 0x%p, pid %d, image path %ws\n", file_path.data(), rename_event.file_object, rename_event.irp_ptr, rename_event.file_key, rename_event.extra_info, rename_event.info_class, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
        }
		else if (type == FileIoEventType::kQueryInfo)
		{
			FileIoQueryInfoEvent query_info_event(event);
		}
        
		else if (type == FileIoEventType::kFSControl)
		{
			FileIoFSControlEvent fs_control_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath((size_t)fs_control_event.file_object);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, FSControl event, path %ws, file object 0x%p, IrpPtr 0x%p, file key 0x%p, extra info 0x%llx, info class 0x%p, pid %d, image path %ws\n", file_path.data(), fs_control_event.file_object, fs_control_event.irp_ptr, fs_control_event.file_key, fs_control_event.extra_info, fs_control_event.info_class, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
        }
		// EventTypeName{"Name", "FileCreate", "FileDelete", "FileRundown"}
		else if (type == FileIoEventType::kName)
		{
			FileIoNameEvent name_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath((size_t)name_event.file_object);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, FileName event, path %ws, file object 0x%p, file name %ws, pid %d, image path %ws\n", file_path.data(), name_event.file_object, name_event.file_name, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
		}
		else if (type == FileIoEventType::kFileCreate)
		{
			FileIoFileCreateEvent file_create_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            // To determine the file name, match the value of this file_object property to the file_key property of a previous event.
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, FileCreate event, path %ws, file key 0x%p, file name %ws, pid %d, image path %ws\n", file_path.data(), file_create_event.file_object, file_create_event.file_name, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
		}
		else if (type == FileIoEventType::kFileDelete)
		{
			FileIoFileDeleteEvent file_delete_event(event);
		}
		else if (type == FileIoEventType::kFileRundown)
		{
			FileIoFileRundownEvent file_rundown_event(event);
		}
        // EventTypeName{ "OperationEnd" }
		else if (type == FileIoEventType::kOperationEnd)
		{
			FileIoOpEndEvent operation_end_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, OperationEnd event, IrpPtr 0x%p, NTSTATUS 0x%x, extra info 0x%llx, pid %d, image path %ws\n", operation_end_event.irp_ptr, (DWORD)operation_end_event.nt_status, operation_end_event.extra_info, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
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
            FileIoSimpleOpCleanupEvent cleanup_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath((size_t)cleanup_event.file_object);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, Cleanup event, path %ws, file object 0x%p, IrpPtr 0x%p, file key 0x%p, pid %d, image path %ws\n", file_path.data(), cleanup_event.file_object, cleanup_event.irp_ptr, cleanup_event.file_key, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);

		}
        else if (type == FileIoEventType::kClose)
		{
            FileIoSimpleOpCloseEvent close_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath((size_t)close_event.file_object);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"File I/O, Close event, path %ws, file object 0x%p, IrpPtr 0x%p, file key 0x%p, pid %d, image path %ws\n", file_path.data(), close_event.file_object, close_event.irp_ptr, close_event.file_key, pid, manager::kProcMan->GetImageFileName(pid).data()));
            debug::DebugLogW(wstr);
            manager::kFileMan->RemoveFile((size_t)close_event.file_object);

		}
		else if (type == FileIoEventType::kFlush)
		{
		}
        return VOID();
    }

    VOID __stdcall KernelConsumer::ProcessProcessEvent(Event event)
    {
        int type = event.GetType();
        if (type == ProcessEventType::kProcessStart)
        {
            ProcessStartEvent process_start_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            int pid = static_cast<int>(event.GetProcessId());
			std::string image = process_start_event.image_file_name;
			std::wstring w_image(image.begin(), image.end());
            const std::wstring image_path = manager::kProcMan->GetImageFileName(pid);
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"Process Operation, Create event, pid %lld, ppid %lld, image %s, image path %ws, commandline %ws", process_start_event.pid, event.GetProcessId(), w_image.data(), image_path.data(), process_start_event.command_line));
            debug::DebugLogW(wstr);
            if (manager::kProcMan->GetImageFileName(process_start_event.pid).empty())
            {
                manager::kProcMan->AddProcess(process_start_event.pid, event.GetProcessId());
            }
        }
        if (type == ProcessEventType::kProcessEnd)
        {
            ProcessEndEvent process_end_event(event);
            std::wstring wstr;
            wstr.resize(2000);
            const std::wstring image_path = manager::kProcMan->GetImageFileName(process_end_event.pid);
            const std::wstring issued_image_path = manager::kProcMan->GetImageFileName(event.GetProcessId());
            std::string image = process_end_event.image_file_name;
            std::wstring w_image(image.begin(), image.end());
            wstr.resize(swprintf(wstr.data(), wstr.size(), L"Process Operation, End event, pid %lld, issued pid %lld, image %s, image path %ws, issued image path %ws, commandline %ws", process_end_event.pid, event.GetProcessId(), image_path.data(), image_path.data(), issued_image_path.data(), process_end_event.command_line));
            debug::DebugLogW(wstr);
            manager::kProcMan->RemoveProcess(process_end_event.pid);
        } 
        else if (type == ProcessEventType::kProcessDCStart)
        {

        }
        else if (type == ProcessEventType::kProcessDCEnd)
        {

        }
        else if (type == ProcessEventType::kProcessDefunct)
        {

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
        if (type == PageFaultEventType::kVirtualAlloc)
        {
            PageFaultVirtualAllocEvent alloc_event(event);
            size_t issued_pid = event.GetProcessId();
            size_t allocated_pid = alloc_event.process_id;
			if (manager::PageFaultEventFilter(issued_pid, allocated_pid))
			{
                std::wstring wstr;
                wstr.resize(2000);
                const std::wstring issued_image_path = manager::kProcMan->GetImageFileName(issued_pid);
                const std::wstring allocated_image_path = manager::kProcMan->GetImageFileName(allocated_pid);
                wstr.resize(swprintf(wstr.data(), wstr.size(), L"Page Fault, VirtualAlloc event, allocated pid %lld, issued pid %lld, size 0x%llx, allocated image path %ws, issued image path %ws", issued_pid, allocated_pid, alloc_event.region_size, issued_image_path.data(), allocated_image_path.data()));
                debug::DebugLogW(wstr);
			}
        }
        else if (type == ThreadEventType::kThreadEnd)
        {
            //PageFaultVirtualFreeEvent free_event(event);
        }

        return VOID();
    }

    inline void PrintDebugRegistryEvent(const std::wstring& name, const RegistryTypeGroup1EventMember& event, size_t pid)
    {
        /*
        debug::DebugLogW(L"[+] Registry: " + name + L" event");
        debug::DebugLogW(L"    - Key Name:    " + std::wstring(event.KeyName));
        debug::DebugLogW(L"    - Key Handle:  0x" + std::format(L"{:x}", event.KeyHandle));
        debug::DebugLogW(L"    - Status:      0x" + std::format(L"{:x}", event.Status));
        debug::DebugLogW(L"    - Index:       0x" + std::to_wstring(event.Index));
        PrintDebugPid(pid);
        debug::DebugLogW(L"\n");
        */
    }

    VOID __stdcall KernelConsumer::ProcessRegistryEvent(Event event)
    {
        int type = event.GetType();
        size_t pid = event.GetProcessId();
        
        if (type < RegistryEventType::kRegistryCreate || type > RegistryEventType::kRegistryClose)
        {
            return VOID();
        }

        RegistryOverallEvent registry_event(event);

        if (type == RegistryEventType::kRegistryCreate)
        {
            //PrintDebugRegistryEvent(L"Create", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryOpen)
        {
            //PrintDebugRegistryEvent(L"Open", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryDelete)
        {
            //PrintDebugRegistryEvent(L"Delete", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryQuery)
        {
            //PrintDebugRegistryEvent(L"Query", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistrySetValue)
        {
            //PrintDebugRegistryEvent(L"SetValue", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryDeleteValue)
        {
            //PrintDebugRegistryEvent(L"DeleteValue", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryQueryValue)
        {
            //PrintDebugRegistryEvent(L"QueryValue", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryEnumerateKey)
        {
            //PrintDebugRegistryEvent(L"EnumerateKey", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryEnumerateValueKey)
        {
            //PrintDebugRegistryEvent(L"EnumerateValueKey", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryQueryMultipleValue)
        {
            //PrintDebugRegistryEvent(L"QueryMultipleValue", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistrySetInformation)
        {
            //PrintDebugRegistryEvent(L"SetInformation", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryFlush)
        {
            //PrintDebugRegistryEvent(L"Flush", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryKCBCreate)
        {
            //PrintDebugRegistryEvent(L"KCBCreate", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryKCBDelete)
        {
            //PrintDebugRegistryEvent(L"KCBDelete", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryKCBRundownBegin)
        {
            //PrintDebugRegistryEvent(L"KCBRundownBegin", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryKCBRundownEnd)
        {
            //PrintDebugRegistryEvent(L"KCBRundownEnd", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryVirtualize)
        {
            //PrintDebugRegistryEvent(L"Virtualize", registry_event, pid);
        }
        else if (type == RegistryEventType::kRegistryClose)
        {
            //PrintDebugRegistryEvent(L"Close", registry_event, pid);
        }

        return VOID();
    }


    VOID __stdcall KernelConsumer::ProcessPerfInfoEvent(Event event)
    {
        int type = event.GetType();
        if (type == PerfInfoEventType::SysClEnter)
        {
            /*
            SysCallEnterEvent sys_call_enter_event(event);
            debug::DebugLogW(L"[+] SysCallEnterEvent");
            debug::DebugLogW(L"    - PID:     " + std::to_wstring(event.GetProcessId()));
            debug::DebugLogW(L"    - TID:     " + std::to_wstring(event.GetThreadId()));
            debug::DebugLogW(L"    - Time:     " + std::to_wstring(event.GetTimeInMs()));
            debug::DebugLogW(L"    - Address: " + std::to_wstring((size_t)sys_call_enter_event.sys_call_address));
            debug::DebugLogW(L"\n");
			*/
        }
        return VOID();
    }
    
    ULONG KernelConsumer::Close()
    {
        ULONG status = ERROR_SUCCESS;

        debug::DebugLogW(L"Total event count: " + std::to_wstring(event_count_));

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