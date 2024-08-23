#ifdef _WIN32

#include "process.h"

namespace etw
{
	ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(const Event& event, ProcessTypeGroup1EventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kProcess, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"UniqueProcessKey", event);
			offset->unique_process_key_offs = p.first;
			offset->unique_process_key_size = p.second;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			offset->process_id_offs = p.first;
			offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"ParentId", event);
			offset->parent_id_offs = p.first;
			offset->parent_id_size = p.second;

			p = wec.GetPropertyInfo(L"SessionId", event);
			offset->session_id_offs = p.first;
			offset->session_id_size = p.second;

			p = wec.GetPropertyInfo(L"ExitStatus", event);
			offset->exit_status_offs = p.first;
			offset->exit_status_size = p.second;

			p = wec.GetPropertyInfo(L"DirectoryTableBase", event);
			offset->directory_table_base_offs = p.first;
			offset->directory_table_base_size = p.second;

			/*
			p = wec.GetPropertyInfo(L"UserSID", event);
			offset->user_sid_offs = p.first;
			offset->user_sid_size = p.second;

			p = wec.GetPropertyInfo(L"ImageFileName", event);
			offset->image_file_name_offs = p.first;
			offset->image_file_name_size = p.second;

			p = wec.GetPropertyInfo(L"CommandLine", event);
			offset->command_line_offs = p.first;
			offset->command_line_size = p.second;

			if (0 == offset->unique_process_key_size ||
				0 == offset->process_id_size ||
				0 == offset->parent_id_size ||
				0 == offset->session_id_size ||
				0 == offset->exit_status_size ||
				0 == offset->directory_table_base_size ||
				// 0 == offset->user_sid_size ||
				0 == offset->image_file_name_size ||
				0 == offset->command_line_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in ProcessTypeGroup1Event");
				offset->is_successful = false;
				return;
			}
			*/

			offset->is_successful = true;
			offset->is_positioned = true;
			/*
			if (offset->command_line_offs > offset->unique_process_key_offs &&
				offset->command_line_offs > offset->process_id_offs &&
				offset->command_line_offs > offset->parent_id_offs &&
				offset->command_line_offs > offset->session_id_offs &&
				offset->command_line_offs > offset->exit_status_offs &&
				offset->command_line_offs > offset->directory_table_base_offs &&
				// offset->command_line_offs > offset->user_sid_offs &&
				offset->image_file_name_offs > offset->unique_process_key_offs &&
				offset->image_file_name_offs > offset->process_id_offs &&
				offset->image_file_name_offs > offset->parent_id_offs &&
				offset->image_file_name_offs > offset->session_id_offs &&
				offset->image_file_name_offs > offset->exit_status_offs &&
				offset->image_file_name_offs > offset->directory_table_base_offs // &&
				// offset->image_file_name_offs > offset->user_sid_offs
				)
			{
				ulti::WriteDebugA("The position for ProcessTypeGroup1Event is as expected.");
				offset->is_positioned = true;
			}
			else
			{
				ulti::WriteDebugA("The position for ProcessTypeGroup1Event is not as expected.");
				offset->is_positioned = false;
			}
			*/
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&process_unique_key, p_data + offset->unique_process_key_offs, offset->unique_process_key_size);
		memcpy(&pid, p_data + offset->process_id_offs, offset->process_id_size);
		memcpy(&ppid, p_data + offset->parent_id_offs, offset->parent_id_size);
		memcpy(&session_id, p_data + offset->session_id_offs, offset->session_id_size);
		memcpy(&exit_status, p_data + offset->exit_status_offs, offset->exit_status_size);
		memcpy(&directory_table_base, p_data + offset->directory_table_base_offs, offset->directory_table_base_size);
		// memcpy(&user_sid, p_data + offset->user_sid_offs, offset->user_sid_size);
		// image_file_name = (wchar_t*)(p_data + offset->image_file_name_offs);
		// process_command_line = (wchar_t*)(p_data + offset->command_line_offs);
	}

	ProcessStartEvent::ProcessStartEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset);
	}
	ProcessEndEvent::ProcessEndEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset);
	}
	ProcessDCStartEvent::ProcessDCStartEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset);
	}
	ProcessDCEndEvent::ProcessDCEndEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset);
	}
	ProcessDefunctEvent::ProcessDefunctEvent(const Event& event)
	{
		this->ProcessTypeGroup1EventMember::ProcessTypeGroup1EventMember(event, &offset);
	}

	ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(const Event& event, ThreadTypeGroup1EventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kThread, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"ProcessId", event);
			offset->process_id_offs = p.first;
			offset->process_id_size = p.second;

			p = wec.GetPropertyInfo(L"TThreadId", event);
			offset->tthread_id_offs = p.first;
			offset->tthread_id_size = p.second;

			p = wec.GetPropertyInfo(L"StackBase", event);
			offset->stack_base_offs = p.first;
			offset->stack_base_size = p.second;

			p = wec.GetPropertyInfo(L"StackLimit", event);
			offset->stack_limit_offs = p.first;
			offset->stack_limit_size = p.second;

			p = wec.GetPropertyInfo(L"UserStackBase", event);
			offset->user_stack_base_offs = p.first;
			offset->user_stack_base_size = p.second;

			p = wec.GetPropertyInfo(L"UserStackLimit", event);
			offset->user_stack_limit_offs = p.first;
			offset->user_stack_limit_size = p.second;

			p = wec.GetPropertyInfo(L"Affinity", event);
			offset->affinity_offs = p.first;
			offset->affinity_size = p.second;

			p = wec.GetPropertyInfo(L"Win32StartAddr", event);
			offset->win32_start_addr_offs = p.first;
			offset->win32_start_addr_size = p.second;

			p = wec.GetPropertyInfo(L"TebBase", event);
			offset->teb_base_offs = p.first;
			offset->teb_base_size = p.second;

			p = wec.GetPropertyInfo(L"SubProcessTag", event);
			offset->subprocess_tag_offs = p.first;
			offset->subprocess_tag_size = p.second;

			p = wec.GetPropertyInfo(L"BasePriority", event);
			offset->base_priority_offs = p.first;
			offset->base_priority_size = p.second;

			p = wec.GetPropertyInfo(L"PagePriority", event);
			offset->page_priority_offs = p.first;
			offset->page_priority_size = p.second;

			p = wec.GetPropertyInfo(L"IoPriority", event);
			offset->io_priority_offs = p.first;
			offset->io_priority_size = p.second;

			p = wec.GetPropertyInfo(L"ThreadFlags", event);
			offset->thread_flags_offs = p.first;
			offset->thread_flags_size = p.second;

			if (offset->process_id_size == 0 ||
				offset->tthread_id_size == 0 ||
				offset->stack_base_size == 0 ||
				offset->stack_limit_size == 0 ||
				offset->user_stack_base_size == 0 ||
				offset->user_stack_limit_size == 0 ||
				offset->affinity_size == 0 ||
				offset->win32_start_addr_size == 0 ||
				offset->teb_base_size == 0 ||
				offset->subprocess_tag_size == 0 ||
				offset->base_priority_size == 0 ||
				offset->page_priority_size == 0 ||
				offset->io_priority_size == 0 ||
				offset->thread_flags_size == 0)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in ThreadTypeGroup1Event");
				offset->is_successful = false;
				return;
			}

			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&pid, p_data + offset->process_id_offs, offset->process_id_size);
		memcpy(&ttid, p_data + offset->tthread_id_offs, offset->tthread_id_size);
		memcpy(&stack_base, p_data + offset->stack_base_offs, offset->stack_base_size);
		memcpy(&stack_limit, p_data + offset->stack_limit_offs, offset->stack_limit_size);
		memcpy(&user_stack_base, p_data + offset->user_stack_base_offs, offset->user_stack_base_size);
		memcpy(&user_stack_limit, p_data + offset->user_stack_limit_offs, offset->user_stack_limit_size);
		memcpy(&affinity, p_data + offset->affinity_offs, offset->affinity_size);
		memcpy(&win32_start_addr, p_data + offset->win32_start_addr_offs, offset->win32_start_addr_size);
		memcpy(&teb_base, p_data + offset->teb_base_offs, offset->teb_base_size);
		memcpy(&sub_process_tag, p_data + offset->subprocess_tag_offs, offset->subprocess_tag_size);
		memcpy(&base_priority, p_data + offset->base_priority_offs, offset->base_priority_size);
		memcpy(&page_priority, p_data + offset->page_priority_offs, offset->page_priority_size);
		memcpy(&io_priority, p_data + offset->io_priority_offs, offset->io_priority_size);
		memcpy(&thread_flags, p_data + offset->thread_flags_offs, offset->thread_flags_size);
	}
	ThreadStartEvent::ThreadStartEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset);
	}
	ThreadEndEvent::ThreadEndEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset);
	}
	ThreadDCStartEvent::ThreadDCStartEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset);
	}
	ThreadDCEndEvent::ThreadDCEndEvent(const Event& event)
	{
		this->ThreadTypeGroup1EventMember::ThreadTypeGroup1EventMember(event, &offset);
	}
};

#endif