#ifdef _WIN32

#include "file.h"

namespace etw
{

	FileIoCreateEventMember::FileIoCreateEventMember(const Event& event, FileIoCreateEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			event_offset->irp_ptr_offs = p.first;
			event_offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			event_offset->ttid_offs = p.first;
			event_offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			event_offset->file_object_offs = p.first;
			event_offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"CreateOptions", event);
			event_offset->create_options_offs = p.first;
			event_offset->create_options_size = p.second;

			p = wec.GetPropertyInfo(L"FileAttributes", event);
			event_offset->file_attributes_offs = p.first;
			event_offset->file_attributes_size = p.second;

			p = wec.GetPropertyInfo(L"ShareAccess", event);
			event_offset->share_access_offs = p.first;
			event_offset->share_access_size = p.second;

			p = wec.GetPropertyInfo(L"OpenPath", event);
			event_offset->open_path_offs = p.first;

			if (0 == event_offset->irp_ptr_size ||
				0 == event_offset->ttid_size ||
				0 == event_offset->file_object_size ||
				0 == event_offset->create_options_size ||
				0 == event_offset->file_attributes_size ||
				0 == event_offset->share_access_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo in FileCreateEvent");
				event_offset->is_successful = false;
				return;
			}

			event_offset->is_successful = true;
			if (event_offset->open_path_offs > event_offset->irp_ptr_offs &&
				event_offset->open_path_offs > event_offset->ttid_offs &&
				event_offset->open_path_offs > event_offset->file_object_offs &&
				event_offset->open_path_offs > event_offset->create_options_offs &&
				event_offset->open_path_offs > event_offset->file_attributes_offs &&
				event_offset->open_path_offs > event_offset->share_access_offs)
			{
				PrintDebugW(L"Event type %d GetPropertyInfo completed", event.GetType());
				event_offset->is_positioned = true;
			}
			else
			{
				PrintDebugW(L"Event type %d GetPropertyInfo failed", event.GetType());
				event_offset->is_positioned = false;
			}
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + event_offset->irp_ptr_offs, event_offset->irp_ptr_size);
		memcpy(&ttid, p_data + event_offset->ttid_offs, event_offset->ttid_size);
		memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
		memcpy(&create_options, p_data + event_offset->create_options_offs, event_offset->create_options_size);
		memcpy(&file_attributes, p_data + event_offset->file_attributes_offs, event_offset->file_attributes_size);
		memcpy(&share_access, p_data + event_offset->share_access_offs, event_offset->share_access_size);
		open_path = (wchar_t*)(p_data + event_offset->open_path_offs);
	}


    FileIoCreateEvent::FileIoCreateEvent(const Event& event)
    {
		this->FileIoCreateEventMember::FileIoCreateEventMember(event, &event_offset_);
    }

	FileIoDirEnumEventMember::FileIoDirEnumEventMember(const Event& event, FileIoDirEnumEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			event_offset->irp_ptr_offs = p.first;
			event_offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			event_offset->ttid_offs = p.first;
			event_offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			event_offset->file_object_offs = p.first;
			event_offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			event_offset->file_key_offs = p.first;
			event_offset->file_key_size = p.second;

			p = wec.GetPropertyInfo(L"Length", event);
			event_offset->length_offs = p.first;
			event_offset->length_size = p.second;

			p = wec.GetPropertyInfo(L"InfoClass", event);
			event_offset->info_class_offs = p.first;
			event_offset->info_class_size = p.second;

			p = wec.GetPropertyInfo(L"FileIndex", event);
			event_offset->file_index_offs = p.first;
			event_offset->file_index_size = p.second;

			p = wec.GetPropertyInfo(L"FileName", event);
			event_offset->file_name_offs = p.first;

			if (0 == event_offset->irp_ptr_size ||
				0 == event_offset->ttid_size ||
				0 == event_offset->file_object_size ||
				0 == event_offset->file_key_size ||
				0 == event_offset->length_size ||
				0 == event_offset->info_class_size ||
				0 == event_offset->file_index_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo");
				event_offset->is_successful = false;
				return;
			}
			event_offset->is_successful = true;
			event_offset->is_positioned = false;
			if (event_offset->file_name_offs > event_offset->irp_ptr_offs &&
				event_offset->file_name_offs > event_offset->ttid_offs &&
				event_offset->file_name_offs > event_offset->file_object_offs &&
				event_offset->file_name_offs > event_offset->file_key_offs&&
				event_offset->file_name_offs > event_offset->length_offs&&
				event_offset->file_name_offs > event_offset->info_class_offs&&
				event_offset->file_name_offs > event_offset->file_index_offs)
			{
				event_offset->is_positioned = true;
			}
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + event_offset->irp_ptr_offs, event_offset->irp_ptr_size);
		memcpy(&ttid, p_data + event_offset->ttid_offs, event_offset->ttid_size);
		memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
		file_name = (wchar_t*)(p_data + event_offset->file_name_offs);

	}

	FileIoDirEnumEvent::FileIoDirEnumEvent(const Event& event)
	{
		this->FileIoDirEnumEventMember::FileIoDirEnumEventMember(event, &event_offset_);
	}

	FileIoDirNotifyEvent::FileIoDirNotifyEvent(const Event& event)
	{
		this->FileIoDirEnumEventMember::FileIoDirEnumEventMember(event, &event_offset_);
	}

	FileIoInfoEventMember::FileIoInfoEventMember(const Event& event, FileIoInfoEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			event_offset->irp_ptr_offs = p.first;
			event_offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			event_offset->ttid_offs = p.first;
			event_offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			event_offset->file_object_offs = p.first;
			event_offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			event_offset->file_key_offs = p.first;
			event_offset->file_key_size = p.second;

			p = wec.GetPropertyInfo(L"ExtraInfo", event);
			event_offset->extra_info_offs = p.first;
			event_offset->extra_info_size = p.second;

			p = wec.GetPropertyInfo(L"InfoClass", event);
			event_offset->info_class_offs = p.first;
			event_offset->info_class_size = p.second;

			if (0 == event_offset->irp_ptr_size ||
				0 == event_offset->ttid_size ||
				0 == event_offset->file_object_size ||
				0 == event_offset->file_key_size ||
				0 == event_offset->extra_info_size ||
				0 == event_offset->info_class_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo");
				event_offset->is_successful = false;
				return;
			}
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + event_offset->irp_ptr_offs, event_offset->irp_ptr_size);
		memcpy(&ttid, p_data + event_offset->ttid_offs, event_offset->ttid_size);
		memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
		memcpy(&file_key, p_data + event_offset->file_key_offs, event_offset->file_key_size);
		memcpy(&extra_info, p_data + event_offset->extra_info_offs, event_offset->extra_info_size);
		memcpy(&info_class, p_data + event_offset->info_class_offs, event_offset->info_class_size);
	}

	FileIoSetInfoEvent::FileIoSetInfoEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &event_offset_);
	}

	FileIoDeleteEvent::FileIoDeleteEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &event_offset_);
	}

	FileIoRenameEvent::FileIoRenameEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &event_offset_);
	}

	FileIoQueryInfoEvent::FileIoQueryInfoEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &event_offset_);
	}

	FileIoFSControlEvent::FileIoFSControlEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &event_offset_);
	}

	FileIoNameEventMember::FileIoNameEventMember(const Event& event, FileIoNameEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"FileObject", event);
			event_offset->file_object_offs = p.first;
			event_offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileName", event);
			event_offset->file_name_offs = p.first;

			if (0 == event_offset->file_object_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo");
				event_offset->is_successful = false;
				return;
			}
			event_offset->is_successful = true;

			if (event_offset->file_name_offs > event_offset->file_object_offs)
			{
				PrintDebugW(L"The position for FileIoNameEvent is as expected.");
				event_offset->is_positioned = true;
			}
			else
			{
				PrintDebugW(L"The position for FileIoNameEvent is not as expected.");
			}
		}
		PBYTE p_data = event.GetPEventData();
		memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
		file_name = (wchar_t*)(p_data + event_offset->file_name_offs);

	}

	FileIoNameEvent::FileIoNameEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &event_offset_);
	}

	FileIoFileCreateEvent::FileIoFileCreateEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &event_offset_);
	}

	FileIoFileDeleteEvent::FileIoFileDeleteEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &event_offset_);
	}

	FileIoFileRundownEvent::FileIoFileRundownEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &event_offset_);
	}

	FileIoOpEndEventMember::FileIoOpEndEventMember(const Event& event, FileIoOpEndEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			event_offset->irp_ptr_offs = p.first;
			event_offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"ExtraInfo", event);
			event_offset->extra_info_offs = p.first;
			event_offset->extra_info_size = p.second;

			p = wec.GetPropertyInfo(L"NtStatus", event);
			event_offset->nt_status_offs = p.first;
			event_offset->nt_status_size = p.second;

			if (0 == event_offset->irp_ptr_size ||
				0 == event_offset->extra_info_size ||
				0 == event_offset->nt_status_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo");
				event_offset->is_successful = false;
				return;
			}
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + event_offset->irp_ptr_offs, event_offset->irp_ptr_size);
		memcpy(&extra_info, p_data + event_offset->extra_info_offs, event_offset->extra_info_size);
		memcpy(&nt_status, p_data + event_offset->nt_status_offs, event_offset->nt_status_size);
	}

	FileIoOpEndEvent::FileIoOpEndEvent(const Event& event)
	{
		this->FileIoOpEndEventMember::FileIoOpEndEventMember(event, &event_offset_);
	}

	FileIoReadWriteEventMember::FileIoReadWriteEventMember(const Event& event, FileIoReadWriteEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"Offset", event);
			event_offset->offset_offs = p.first;
			event_offset->offset_size = p.second;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			event_offset->irp_ptr_offs = p.first;
			event_offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			event_offset->ttid_offs = p.first;
			event_offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			event_offset->file_object_offs = p.first;
			event_offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			event_offset->file_key_offs = p.first;
			event_offset->file_key_size = p.second;

			p = wec.GetPropertyInfo(L"IoSize", event);
			event_offset->io_size_offs = p.first;
			event_offset->io_size_size = p.second;

			p = wec.GetPropertyInfo(L"IoFlags", event);
			event_offset->io_flags_offs = p.first;
			event_offset->io_flags_size = p.second;

			if (0 == event_offset->irp_ptr_size ||
				0 == event_offset->ttid_size ||
				0 == event_offset->file_object_size ||
				0 == event_offset->file_key_size ||
				0 == event_offset->offset_size ||
				0 == event_offset->io_size_size ||
				0 == event_offset->io_flags_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo");
				event_offset->is_successful = false;
				return;
			}
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&offset, p_data + event_offset->offset_offs, event_offset->offset_size);
		memcpy(&irp_ptr, p_data + event_offset->irp_ptr_offs, event_offset->irp_ptr_size);
		memcpy(&ttid, p_data + event_offset->ttid_offs, event_offset->ttid_size);
		memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
		memcpy(&file_key, p_data + event_offset->file_key_offs, event_offset->file_key_size);
		memcpy(&io_size, p_data + event_offset->io_size_offs, event_offset->io_size_size);
		memcpy(&io_flags, p_data + event_offset->io_flags_offs, event_offset->io_flags_size);
	}

	FileIoReadEvent::FileIoReadEvent(const Event& event)
	{
		this->FileIoReadWriteEventMember::FileIoReadWriteEventMember(event, &event_offset_);
	}

	FileIoWriteEvent::FileIoWriteEvent(const Event& event)
	{
		this->FileIoReadWriteEventMember::FileIoReadWriteEventMember(event, &event_offset_);
	}

	FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(const Event& event, FileIoSimpleOpEventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			event_offset->irp_ptr_offs = p.first;
			event_offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			event_offset->ttid_offs = p.first;
			event_offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			event_offset->file_object_offs = p.first;
			event_offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			event_offset->file_key_offs = p.first;
			event_offset->file_key_size = p.second;

			if (0 == event_offset->irp_ptr_size ||
				0 == event_offset->ttid_size ||
				0 == event_offset->file_object_size ||
				0 == event_offset->file_key_size)
			{
				PrintDebugW(L"Error in GetPropertyInfo");
				event_offset->is_successful = false;
				return;
			}
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + event_offset->irp_ptr_offs, event_offset->irp_ptr_size);
		memcpy(&ttid, p_data + event_offset->ttid_offs, event_offset->ttid_size);
		memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
		memcpy(&file_key, p_data + event_offset->file_key_offs, event_offset->file_key_size);
	}

	FileIoSimpleOpCleanupEvent::FileIoSimpleOpCleanupEvent(const Event& event)
	{
		this->FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(event, &event_offset_);
	}

	FileIoSimpleOpCloseEvent::FileIoSimpleOpCloseEvent(const Event& event)
	{
		this->FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(event, &event_offset_);
	}

	FileIoSimpleOpFlushEvent::FileIoSimpleOpFlushEvent(const Event& event)
	{
		this->FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(event, &event_offset_);
	}
};

#endif