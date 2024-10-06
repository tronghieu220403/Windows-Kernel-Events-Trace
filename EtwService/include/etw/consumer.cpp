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
            else if (IsEqualGUID(event.GetGuid(), RegistryGuid))
            {
                //ProcessRegistryEvent(event);
            }
        }
                
        return;
    }

	std::unordered_set<size_t> file_object_read;
	std::unordered_set<size_t> file_object_write;
	std::unordered_map<size_t, std::pair<size_t, size_t>> irp_ptr_to_file_object_map_; // irp_ptr -> <pid, file_object>
	std::unordered_map<size_t, size_t> file_key_to_pid_map_; // file_key -> <pid>
    std::unordered_map<size_t, std::pair<std::wstring, std::wstring>> file_rename_map_; // file_key -> <old_name, new_name>
	std::unordered_set<size_t> file_delete_on_close_set_; // file_object

    VOID WINAPI KernelConsumer::ProcessFileIoEvent(Event event)
    {
        int type = event.GetType();
		std::wstring file_path;
        const std::lock_guard<std::mutex> pm_lock(manager::kProcMan->process_map_mutex_);

        // EventTypeName{"Create"}
        if (type == FileIoEventType::kCreate)
        {
            FileIoCreateEvent file_create_event(event);
			file_path = file_create_event.open_path;

			manager::kFileMan->AddFileObject(file_create_event.file_object, manager::GetWin32Path(file_path));
            int pid = static_cast<int>(event.GetProcessId());

            // Handle for create event
			irp_ptr_to_file_object_map_[file_create_event.irp_ptr] = std::make_pair(pid, file_create_event.file_object);

			// Handle for delete event
            if (FlagOn(file_create_event.create_options, FILE_DELETE_ON_CLOSE))
            {
				file_delete_on_close_set_.insert(file_create_event.file_object);
            }
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
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath(set_info_event.file_object);
			if (file_path.empty())
			{
				return;
			}
            manager::kProcMan->PushSetInfoFileEventToProcess(pid, file_path);
        }
        else if (type == FileIoEventType::kDelete)
        {
            FileIoDeleteEvent delete_event(event);
            
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath(delete_event.file_object);
			if (file_path.empty())
			{
				return;
			}

			// Directly delete event
            manager::kProcMan->PushDeleteFileEventToProcess(pid, file_path);
		}
		else if (type == FileIoEventType::kRename)
		{
			FileIoRenameEvent rename_event(event);
            int pid = static_cast<int>(event.GetProcessId());
            file_path = manager::kFileMan->GetFilePath(rename_event.file_object);
            if (file_path.empty())
            {
				return;
            }

            // Rename event
			file_key_to_pid_map_[rename_event.file_key] = pid;
			file_rename_map_[rename_event.file_key] = std::make_pair(file_path, L"");
        }
		else if (type == FileIoEventType::kQueryInfo)
		{
			//FileIoQueryInfoEvent query_info_event(event);
		}
		else if (type == FileIoEventType::kFSControl)
		{
			//FileIoFSControlEvent fs_control_event(event);
        }
		// EventTypeName{"Name", "FileCreate", "FileDelete", "FileRundown"}
		else if (type == FileIoEventType::kName)
		{
			//FileIoNameEvent name_event(event);
		}
		else if (type == FileIoEventType::kFileCreate)
		{
			FileIoFileCreateEvent file_create_event(event);
            int pid = static_cast<int>(event.GetProcessId());

            // Rename event
            if (file_rename_map_.find(file_create_event.file_object) != file_rename_map_.end())
            {
                file_rename_map_[file_create_event.file_object].second = manager::GetWin32Path(file_create_event.file_name);
				const size_t issue_pid = file_key_to_pid_map_[file_create_event.file_object];
                const std::pair<std::wstring, std::wstring> pss = file_rename_map_[file_create_event.file_object];
				manager::kProcMan->PushRenameFileEventToProcess(issue_pid, pss.first, pss.second);
                file_rename_map_.erase(file_create_event.file_object);
            }
		}
		else if (type == FileIoEventType::kFileDelete)
		{
            //FileIoFileDeleteEvent file_delete_event(event);
		}
		else if (type == FileIoEventType::kFileRundown)
		{
			//FileIoFileRundownEvent file_rundown_event(event);
		}
        // EventTypeName{ "OperationEnd" }
		else if (type == FileIoEventType::kOperationEnd)
		{
			FileIoOpEndEvent operation_end_event(event);
            int pid = static_cast<int>(event.GetProcessId());

            // Create event
			auto file_create_it = irp_ptr_to_file_object_map_.find(operation_end_event.irp_ptr);
            if (file_create_it != irp_ptr_to_file_object_map_.end())
            {
                if (operation_end_event.extra_info == FILE_CREATED)
                {
                    const std::wstring create_path = manager::kFileMan->GetFilePath(file_create_it->second.second);
                    if (create_path.empty())
                    {
                        return;
                    }
					manager::kProcMan->PushCreateFileEventToProcess(file_create_it->second.first, create_path);
                }
				irp_ptr_to_file_object_map_.erase(file_create_it);
            }
		}
        // EventTypeName{"Read", "Write"}
		else if (type == FileIoEventType::kRead)
		{
			FileIoReadEvent read_event(event);
			int pid = static_cast<int>(event.GetProcessId());
			auto it = file_object_read.find(read_event.file_object);
			if (it == file_object_read.end())
			{
				file_object_read.insert(read_event.file_object);
			}
		}
		else if (type == FileIoEventType::kWrite)
		{
			FileIoWriteEvent write_event(event);
			int pid = static_cast<int>(event.GetProcessId());
			auto it = file_object_write.find(write_event.file_object);
            if (it == file_object_write.end())
            {
                file_object_write.insert(write_event.file_object);
            }
		}
        // EventTypeName{ "Cleanup", "Close", "Flush" }
        else if (type == FileIoEventType::kCleanup)
        {
            //FileIoSimpleOpCleanupEvent cleanup_event(event);
		}
        else if (type == FileIoEventType::kClose)
		{
            FileIoSimpleOpCloseEvent close_event(event);
            int pid = static_cast<int>(event.GetProcessId());

			// Handle for delete on close event
            if (file_delete_on_close_set_.find(close_event.file_object) != file_delete_on_close_set_.end())
            {
                file_path = manager::kFileMan->GetFilePath(close_event.file_object);
                if (file_path.empty())
                {
                    return;
                }
                manager::kProcMan->PushDeleteFileEventToProcess(pid, file_path);
            }

            if (file_object_read.find(close_event.file_object) != file_object_read.end())
            {
                file_path = manager::kFileMan->GetFilePath(close_event.file_object);
                if (file_path.empty())
                {
                    return;
                }
                manager::kProcMan->PushReadFileEventToProcess(pid, file_path);
				file_object_read.erase(close_event.file_object);
            }

            if (file_object_write.find(close_event.file_object) != file_object_write.end())
            {
                file_path = manager::kFileMan->GetFilePath(close_event.file_object);
                if (file_path.empty())
                {
                    return;
                }
                manager::kProcMan->PushWriteFileEventToProcess(pid, file_path);
				file_object_write.erase(close_event.file_object);
            }

            // Clean up
            manager::kFileMan->RemoveFileObject(close_event.file_object);
			file_rename_map_.erase(close_event.file_key);
            file_delete_on_close_set_.erase(close_event.file_object);
			file_key_to_pid_map_.erase(close_event.file_key);
			file_object_read.erase(close_event.file_object);
			file_object_write.erase(close_event.file_object);
		}
		else if (type == FileIoEventType::kFlush)
		{

		}
        return VOID();
    }

    VOID __stdcall KernelConsumer::ProcessProcessEvent(Event event)
    {
        int type = event.GetType();
        const std::lock_guard<std::mutex> pm_lock(manager::kProcMan->process_map_mutex_);
        if (type == ProcessEventType::kProcessStart)
        {
            ProcessStartEvent process_start_event(event);
            int pid = static_cast<int>(event.GetProcessId());
			std::string image = process_start_event.image_file_name;
			std::wstring w_image(image.begin(), image.end());
            const std::wstring image_path = manager::kProcMan->GetImageFileName(process_start_event.pid);

            debug::DebugPrintW(L"Process Operation, Start event, pid %lld, ppid %lld, image %s, image path %ws, commandline %ws", process_start_event.pid, event.GetProcessId(), w_image.data(), image_path.data(), process_start_event.command_line);

            manager::kProcMan->AddProcess(process_start_event.pid, event.GetProcessId());
			manager::kProcMan->UpdateProcessCreationTime(process_start_event.pid, event.GetTimeInMs());
        }
        if (type == ProcessEventType::kProcessEnd)
        {
            ProcessEndEvent process_end_event(event);
            const std::wstring image_path = manager::kProcMan->GetImageFileName(process_end_event.pid);
            const std::wstring issued_image_path = manager::kProcMan->GetImageFileName(event.GetProcessId());
            std::string image = process_end_event.image_file_name;
            std::wstring w_image(image.begin(), image.end());

            std::wstring wstr;
            debug::DebugPrintW(L"Process Operation, End event, pid %lld, issued pid %lld, image %s, image path %ws, issued image path %ws, commandline %ws", process_end_event.pid, event.GetProcessId(), image_path.data(), image_path.data(), issued_image_path.data(), process_end_event.command_line);

            manager::kProcMan->RemoveProcess(process_end_event.pid);
        } 
        else if (type == ProcessEventType::kProcessDCStart)
        {
            //ProcessDCStartEvent process_dc_start_event(event);

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
			if (manager::PageFaultEventFilter(issued_pid, allocated_pid, event.GetTimeInMs()))
			{
                const std::wstring issued_image_path = manager::kProcMan->GetImageFileName(issued_pid);
                const std::wstring allocated_image_path = manager::kProcMan->GetImageFileName(allocated_pid);
                debug::DebugPrintW(L"Page Fault, VirtualAlloc event, allocated pid %lld, issued pid %lld, size 0x%llx, allocated image path %ws, issued image path %ws", issued_pid, allocated_pid, alloc_event.region_size, issued_image_path.data(), allocated_image_path.data());
			}
        }
        return VOID();
    }

    inline void PrintDebugRegistryEvent(const std::wstring& name, const RegistryTypeGroup1EventMember& event, size_t pid)
    {
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
			*/
        }
        return VOID();
    }
    
    ULONG KernelConsumer::Close()
    {
        ULONG status = ERROR_SUCCESS;

#ifdef _DEBUG
        debug::DebugPrintW(L"Total event count: %ws", std::to_wstring(event_count_).c_str());
#endif // _DEBUG

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