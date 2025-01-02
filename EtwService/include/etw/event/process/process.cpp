#ifdef _WIN32

#include "process.h"

namespace etw
{
	ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(const Event& event, ProcessTypeGroup1EventOffset* event_offset)
	{
		WmiEventClass wec(EventGuid::kProcess, event.GetVersion(), event.GetType(), sizeof(PVOID));
		std::pair<int, int> p;

		if (event_offset->is_positioned == false)
		{

			p = wec.GetPropertyInfo(L"UniqueProcessKey", event);
			event_offset->unique_process_key_offs = p.first;
			event_offset->unique_process_key_size = p.second;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			event_offset->process_id_offs = p.first;
			event_offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"ParentId", event);
			event_offset->parent_id_offs = p.first;
			event_offset->parent_id_size = p.second;

			p = wec.GetPropertyInfo(L"SessionId", event);
			event_offset->session_id_offs = p.first;
			event_offset->session_id_size = p.second;

			p = wec.GetPropertyInfo(L"ExitStatus", event);
			event_offset->exit_status_offs = p.first;
			event_offset->exit_status_size = p.second;

			p = wec.GetPropertyInfo(L"DirectoryTableBase", event);
			event_offset->directory_table_base_offs = p.first;
			event_offset->directory_table_base_size = p.second;

			// The "Flags" attribute is undocumented, only tested on Windows 10 22h
			/*
			p = wec.GetPropertyInfo(L"Flags", event);
			event_offset->directory_table_base_offs = p.first;
			event_offset->directory_table_base_size = p.second;
			*/
			/*
			p = wec.GetPropertyInfo(L"UserSID", event);
			event_offset->user_sid_offs = p.first;
			event_offset->user_sid_size = p.second;
			*/
			if (0 == event_offset->unique_process_key_size ||
				0 == event_offset->process_id_size ||
				0 == event_offset->parent_id_size ||
				0 == event_offset->session_id_size ||
				0 == event_offset->exit_status_size ||
				0 == event_offset->directory_table_base_size)
			{
				PrintDebugW(L"Event type %d GetPropertyInfo failed", event.GetType());
				event_offset->is_successful = false;
				return;
			}

			PrintDebugW(L"Event type %d GetPropertyInfo completed", event.GetType());
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&process_unique_key, p_data + event_offset->unique_process_key_offs, event_offset->unique_process_key_size);
		memcpy(&pid, p_data + event_offset->process_id_offs, event_offset->process_id_size);
		memcpy(&ppid, p_data + event_offset->parent_id_offs, event_offset->parent_id_size);
		memcpy(&session_id, p_data + event_offset->session_id_offs, event_offset->session_id_size);
		memcpy(&exit_status, p_data + event_offset->exit_status_offs, event_offset->exit_status_size);
		memcpy(&directory_table_base, p_data + event_offset->directory_table_base_offs, event_offset->directory_table_base_size);

		p = wec.GetPropertyInfo(L"ImageFileName", event);
		image_file_name = (char*)(p_data + p.first);

		p = wec.GetPropertyInfo(L"CommandLine", event);
		command_line = (wchar_t*)(p_data + p.first);
	}

	ProcessStartEvent::ProcessStartEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset_);
	}
	ProcessEndEvent::ProcessEndEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset_);
	}
	ProcessDCStartEvent::ProcessDCStartEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset_);
	}
	ProcessDCEndEvent::ProcessDCEndEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset_);
	}
	ProcessDefunctEvent::ProcessDefunctEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset_);
	}

	ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(const Event& event, ThreadTypeGroup1EventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kThread, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			event_offset->process_id_offs = p.first;
			event_offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"TThreadId", event);
			event_offset->tthread_id_offs = p.first;
			event_offset->tthread_id_size = p.second;

			p = wec.GetPropertyInfo(L"StackBase", event);
			event_offset->stack_base_offs = p.first;
			event_offset->stack_base_size = p.second;

			p = wec.GetPropertyInfo(L"StackLimit", event);
			event_offset->stack_limit_offs = p.first;
			event_offset->stack_limit_size = p.second;

			p = wec.GetPropertyInfo(L"UserStackBase", event);
			event_offset->user_stack_base_offs = p.first;
			event_offset->user_stack_base_size = p.second;

			p = wec.GetPropertyInfo(L"UserStackLimit", event);
			event_offset->user_stack_limit_offs = p.first;
			event_offset->user_stack_limit_size = p.second;

			p = wec.GetPropertyInfo(L"Affinity", event);
			event_offset->affinity_offs = p.first;
			event_offset->affinity_size = p.second;

			p = wec.GetPropertyInfo(L"Win32StartAddr", event);
			event_offset->win32_start_addr_offs = p.first;
			event_offset->win32_start_addr_size = p.second;

			p = wec.GetPropertyInfo(L"TebBase", event);
			event_offset->teb_base_offs = p.first;
			event_offset->teb_base_size = p.second;

			p = wec.GetPropertyInfo(L"SubProcessTag", event);
			event_offset->subprocess_tag_offs = p.first;
			event_offset->subprocess_tag_size = p.second;

			p = wec.GetPropertyInfo(L"BasePriority", event);
			event_offset->base_priority_offs = p.first;
			event_offset->base_priority_size = p.second;

			p = wec.GetPropertyInfo(L"PagePriority", event);
			event_offset->page_priority_offs = p.first;
			event_offset->page_priority_size = p.second;

			p = wec.GetPropertyInfo(L"IoPriority", event);
			event_offset->io_priority_offs = p.first;
			event_offset->io_priority_size = p.second;

			p = wec.GetPropertyInfo(L"ThreadFlags", event);
			event_offset->thread_flags_offs = p.first;
			event_offset->thread_flags_size = p.second;

			if (event_offset->process_id_size == 0 ||
				event_offset->tthread_id_size == 0 ||
				event_offset->stack_base_size == 0 ||
				event_offset->stack_limit_size == 0 ||
				event_offset->user_stack_base_size == 0 ||
				event_offset->user_stack_limit_size == 0 ||
				event_offset->affinity_size == 0 ||
				event_offset->win32_start_addr_size == 0 ||
				event_offset->teb_base_size == 0 ||
				event_offset->subprocess_tag_size == 0 ||
				event_offset->base_priority_size == 0 ||
				event_offset->page_priority_size == 0 ||
				event_offset->io_priority_size == 0 ||
				event_offset->thread_flags_size == 0)
			{
				PrintDebugW(L"Event type %d GetPropertyInfo failed", event.GetType());
				event_offset->is_successful = false;
				return;
			}

			PrintDebugW(L"Event type %d GetPropertyInfo completed", event.GetType());
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&pid, p_data + event_offset->process_id_offs, event_offset->process_id_size);
		memcpy(&ttid, p_data + event_offset->tthread_id_offs, event_offset->tthread_id_size);
		memcpy(&stack_base, p_data + event_offset->stack_base_offs, event_offset->stack_base_size);
		memcpy(&stack_limit, p_data + event_offset->stack_limit_offs, event_offset->stack_limit_size);
		memcpy(&user_stack_base, p_data + event_offset->user_stack_base_offs, event_offset->user_stack_base_size);
		memcpy(&user_stack_limit, p_data + event_offset->user_stack_limit_offs, event_offset->user_stack_limit_size);
		memcpy(&affinity, p_data + event_offset->affinity_offs, event_offset->affinity_size);
		memcpy(&win32_start_addr, p_data + event_offset->win32_start_addr_offs, event_offset->win32_start_addr_size);
		memcpy(&teb_base, p_data + event_offset->teb_base_offs, event_offset->teb_base_size);
		memcpy(&sub_process_tag, p_data + event_offset->subprocess_tag_offs, event_offset->subprocess_tag_size);
		memcpy(&base_priority, p_data + event_offset->base_priority_offs, event_offset->base_priority_size);
		memcpy(&page_priority, p_data + event_offset->page_priority_offs, event_offset->page_priority_size);
		memcpy(&io_priority, p_data + event_offset->io_priority_offs, event_offset->io_priority_size);
		memcpy(&thread_flags, p_data + event_offset->thread_flags_offs, event_offset->thread_flags_size);
	}
	ThreadStartEvent::ThreadStartEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset_);
	}
	ThreadEndEvent::ThreadEndEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset_);
	}
	ThreadDCStartEvent::ThreadDCStartEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset_);
	}
	ThreadDCEndEvent::ThreadDCEndEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset_);
	}


	ImageLoadEventMember::ImageLoadEventMember(const Event& event, ImageLoadEventOffset* event_offset)
	{

		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kImageLoad, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"ImageBase", event);
			event_offset->image_base_offs = p.first;
			event_offset->image_base_size = p.second;

			p = wec.GetPropertyInfo(L"ImageSize", event);
			event_offset->image_size_offs = p.first;
			event_offset->image_size_size = p.second;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			event_offset->process_id_offs = p.first;
			event_offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"ImageCheckSum", event);
			event_offset->image_checksum_offs = p.first;
			event_offset->image_checksum_size = p.second;

			p = wec.GetPropertyInfo(L"TimeDateStamp", event);
			event_offset->time_date_stamp_offs = p.first;
			event_offset->time_date_stamp_size = p.second;

			// Get offsets for reserved fields and file name
			p = wec.GetPropertyInfo(L"Reserved0", event);
			event_offset->reserved0_offs = p.first;
			event_offset->reserved0_size = p.second;

			p = wec.GetPropertyInfo(L"DefaultBase", event);
			event_offset->default_base_offs = p.first;
			event_offset->default_base_size = p.second;

			p = wec.GetPropertyInfo(L"FileName", event);
			event_offset->file_name_offs = p.first;
			event_offset->file_name_size = p.second;

			if (0 == event_offset->image_base_size ||
				0 == event_offset->image_size_size ||
				0 == event_offset->process_id_size ||
				0 == event_offset->image_checksum_size ||
				0 == event_offset->time_date_stamp_size ||
				0 == event_offset->file_name_size)
			{
				PrintDebugW(L"Event type %d GetPropertyInfo failed", event.GetType());
				event_offset->is_successful = false;
				return;
			}
			PrintDebugW(L"Event type %d GetPropertyInfo completed", event.GetType());
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&ImageBase, p_data + event_offset->image_base_offs, event_offset->image_base_size);
		memcpy(&ImageSize, p_data + event_offset->image_size_offs, event_offset->image_size_size);
		memcpy(&ProcessId, p_data + event_offset->process_id_offs, event_offset->process_id_size);
		memcpy(&ImageCheckSum, p_data + event_offset->image_checksum_offs, event_offset->image_checksum_size);
		memcpy(&TimeDateStamp, p_data + event_offset->time_date_stamp_offs, event_offset->time_date_stamp_size);

		file_name = (wchar_t*)(p_data + event_offset->file_name_offs);
	}

	ImageLoadEvent::ImageLoadEvent(const Event& event)
	{
		this->ImageLoadEventMember::ImageLoadEventMember(event, &offset_);
	}

	ImageUnloadEvent::ImageUnloadEvent(const Event& event)
	{
		this->ImageLoadEventMember::ImageLoadEventMember(event, &offset_);
	}

	ImageDCStartEvent::ImageDCStartEvent(const Event& event)
	{
		this->ImageLoadEventMember::ImageLoadEventMember(event, &offset_);
	}

	ImageDCEndEvent::ImageDCEndEvent(const Event& event)
	{
		this->ImageLoadEventMember::ImageLoadEventMember(event, &offset_);
	}

};

#endif