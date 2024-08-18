#ifdef _WIN32

#include "file.h"

namespace etw
{

	FileIoCreateEventMember::FileIoCreateEventMember(const Event& event, FileIoCreateEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs = p.first;
			offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			offset->ttid_offs = p.first;
			offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs = p.first;
			offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"CreateOptions", event);
			offset->create_options_offs = p.first;
			offset->create_options_size = p.second;

			p = wec.GetPropertyInfo(L"FileAttributes", event);
			offset->file_attributes_offs = p.first;
			offset->file_attributes_size = p.second;

			p = wec.GetPropertyInfo(L"ShareAccess", event);
			offset->share_access_offs = p.first;
			offset->share_access_size = p.second;

			p = wec.GetPropertyInfo(L"OpenPath", event);
			offset->open_path_offs = p.first;

			if (0 == offset->irp_ptr_size ||
				0 == offset->ttid_size ||
				0 == offset->file_object_size ||
				0 == offset->create_options_size ||
				0 == offset->file_attributes_size ||
				0 == offset->share_access_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileCreateEvent");
				offset->is_successful = false;
				return;
			}

			offset->is_positioned = true;
			if (offset->open_path_offs > offset->irp_ptr_offs &&
				offset->open_path_offs > offset->ttid_offs &&
				offset->open_path_offs > offset->file_object_offs &&
				offset->open_path_offs > offset->create_options_offs &&
				offset->open_path_offs > offset->file_attributes_offs &&
				offset->open_path_offs > offset->share_access_offs)
			{
				ulti::WriteDebugA("The position for FileCreateEvent is as expected.");
				offset->is_positioned = true;
			}
			else
			{
				ulti::WriteDebugA("The position for FileCreateEvent is not as expected.");
			}
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + offset->irp_ptr_offs, offset->irp_ptr_size);
		memcpy(&ttid, p_data + offset->ttid_offs, offset->ttid_size);
		memcpy(&file_object, p_data + offset->file_object_offs, offset->file_object_size);
		memcpy(&create_options, p_data + offset->create_options_offs, offset->create_options_size);
		memcpy(&file_attributes, p_data + offset->file_attributes_offs, offset->file_attributes_size);
		memcpy(&share_access, p_data + offset->share_access_offs, offset->share_access_size);
		open_path = (wchar_t*)(p_data + offset->open_path_offs);
	}


    FileIoCreateEvent::FileIoCreateEvent(const Event& event)
    {
		this->FileIoCreateEventMember::FileIoCreateEventMember(event, &offset);
    }

	FileIoDirEnumEventMember::FileIoDirEnumEventMember(const Event& event, FileIoDirEnumEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs = p.first;
			offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			offset->ttid_offs = p.first;
			offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs = p.first;
			offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			offset->file_key_offs = p.first;
			offset->file_key_size = p.second;

			p = wec.GetPropertyInfo(L"Length", event);
			offset->length_offs = p.first;
			offset->length_size = p.second;

			p = wec.GetPropertyInfo(L"InfoClass", event);
			offset->info_class_offs = p.first;
			offset->info_class_size = p.second;

			p = wec.GetPropertyInfo(L"FileIndex", event);
			offset->file_index_offs = p.first;
			offset->file_index_size = p.second;

			p = wec.GetPropertyInfo(L"FileName", event);
			offset->file_name_offs = p.first;

			if (0 == offset->irp_ptr_size ||
				0 == offset->ttid_size ||
				0 == offset->file_object_size ||
				0 == offset->file_key_size ||
				0 == offset->length_size ||
				0 == offset->info_class_size ||
				0 == offset->file_index_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileIoDirEnumEvent");
				offset->is_successful = false;
				return;
			}

			offset->is_positioned = true;
			if (offset->file_name_offs > offset->irp_ptr_offs &&
				offset->file_name_offs > offset->ttid_offs &&
				offset->file_name_offs > offset->file_object_offs &&
				offset->file_name_offs > offset->file_key_offs&&
				offset->file_name_offs > offset->length_offs&&
				offset->file_name_offs > offset->info_class_offs&&
				offset->file_name_offs > offset->file_index_offs)
			{
				ulti::WriteDebugA("The position for FileIoDirEnumEvent is as expected.");
				offset->is_positioned = true;
			}
			else
			{
				ulti::WriteDebugA("The position for FileIoDirEnumEvent is not as expected.");
			}
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + offset->irp_ptr_offs, offset->irp_ptr_size);
		memcpy(&ttid, p_data + offset->ttid_offs, offset->ttid_size);
		memcpy(&file_object, p_data + offset->file_object_offs, offset->file_object_size);
		file_name = (wchar_t*)(p_data + offset->file_name_offs);

	}

	FileIoDirEnumEvent::FileIoDirEnumEvent(const Event& event)
	{
		this->FileIoDirEnumEventMember::FileIoDirEnumEventMember(event, &offset);
	}

	FileIoDirNotifyEvent::FileIoDirNotifyEvent(const Event& event)
	{
		this->FileIoDirEnumEventMember::FileIoDirEnumEventMember(event, &offset);
	}

	FileIoInfoEventMember::FileIoInfoEventMember(const Event& event, FileIoInfoEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs = p.first;
			offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			offset->ttid_offs = p.first;
			offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs = p.first;
			offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			offset->file_key_offs = p.first;
			offset->file_key_size = p.second;

			p = wec.GetPropertyInfo(L"ExtraInfo", event);
			offset->extra_info_offs = p.first;
			offset->extra_info_size = p.second;

			p = wec.GetPropertyInfo(L"InfoClass", event);
			offset->info_class_offs = p.first;
			offset->info_class_size = p.second;

			if (0 == offset->irp_ptr_size ||
				0 == offset->ttid_size ||
				0 == offset->file_object_size ||
				0 == offset->file_key_size ||
				0 == offset->extra_info_size ||
				0 == offset->info_class_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileIoInfoEvent");
				offset->is_successful = false;
				return;
			}
			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + offset->irp_ptr_offs, offset->irp_ptr_size);
		memcpy(&ttid, p_data + offset->ttid_offs, offset->ttid_size);
		memcpy(&file_object, p_data + offset->file_object_offs, offset->file_object_size);
		memcpy(&file_key, p_data + offset->file_key_offs, offset->file_key_size);
		memcpy(&extra_info, p_data + offset->extra_info_offs, offset->extra_info_size);
		memcpy(&info_class, p_data + offset->info_class_offs, offset->info_class_size);
	}

	FileIoSetInfoEvent::FileIoSetInfoEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &offset);
	}

	FileIoDeleteEvent::FileIoDeleteEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &offset);
	}

	FileIoRenameEvent::FileIoRenameEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &offset);
	}

	FileIoQueryInfoEvent::FileIoQueryInfoEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &offset);
	}

	FileIoFSControlEvent::FileIoFSControlEvent(const Event& event)
	{
		this->FileIoInfoEventMember::FileIoInfoEventMember(event, &offset);
	}

	FileIoNameEventMember::FileIoNameEventMember(const Event& event, FileIoNameEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs = p.first;
			offset->file_name_offs = p.second;

			p = wec.GetPropertyInfo(L"FileName", event);
			offset->file_name_offs = p.first;

			if (0 == offset->file_object_offs ||
				0 == offset->file_name_offs)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileIoNameEvent");
				offset->is_successful = false;
				return;
			}
			offset->is_successful = true;

			if (offset->file_name_offs > offset->file_object_offs)
			{
				ulti::WriteDebugA("The position for FileIoNameEvent is as expected.");
				offset->is_positioned = true;
			}
			else
			{
				ulti::WriteDebugA("The position for FileIoNameEvent is not as expected.");
			}
		}
	}

	FileIoNameEvent::FileIoNameEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &offset);
	}

	FileIoFileCreateEvent::FileIoFileCreateEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &offset);
	}

	FileIoFileDeleteEvent::FileIoFileDeleteEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &offset);
	}

	FileIoFileRundownEvent::FileIoFileRundownEvent(const Event& event)
	{
		this->FileIoNameEventMember::FileIoNameEventMember(event, &offset);
	}

	FileIoOpEndEventMember::FileIoOpEndEventMember(const Event& event, FileIoOpEndEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs = p.first;
			offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"ExtraInfo", event);
			offset->extra_info_offs = p.first;
			offset->extra_info_size = p.second;

			p = wec.GetPropertyInfo(L"NtStatus", event);
			offset->nt_status_offs = p.first;
			offset->nt_status_size = p.second;

			if (0 == offset->irp_ptr_size ||
				0 == offset->extra_info_size ||
				0 == offset->nt_status_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileIoOpEndEvent");
				offset->is_successful = false;
				return;
			}
			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + offset->irp_ptr_offs, offset->irp_ptr_size);
		memcpy(&extra_info, p_data + offset->extra_info_offs, offset->extra_info_size);
		memcpy(&nt_status, p_data + offset->nt_status_offs, offset->nt_status_size);
	}

	FileIoOpEndEvent::FileIoOpEndEvent(const Event& event)
	{
		this->FileIoOpEndEventMember::FileIoOpEndEventMember(event, &offset);
	}

	FileIoReadWriteEventMember::FileIoReadWriteEventMember(const Event& event, FileIoReadWriteEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"Offset", event);
			offset->offset_offs = p.first;
			offset->offset_size = p.second;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs = p.first;
			offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			offset->ttid_offs = p.first;
			offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs = p.first;
			offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			offset->file_key_offs = p.first;
			offset->file_key_size = p.second;

			p = wec.GetPropertyInfo(L"IoSize", event);
			offset->io_size_offs = p.first;
			offset->io_size_size = p.second;

			p = wec.GetPropertyInfo(L"IoFlags", event);
			offset->io_flags_offs = p.first;
			offset->io_flags_size = p.second;

			if (0 == offset->irp_ptr_size ||
				0 == offset->ttid_size ||
				0 == offset->file_object_size ||
				0 == offset->file_key_size ||
				0 == offset->offset_size ||
				0 == offset->io_size_size ||
				0 == offset->io_flags_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileIoReadWriteEvent");
				offset->is_successful = false;
				return;
			}
			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&offset, p_data + offset->offset_offs, offset->offset_size);
		memcpy(&irp_ptr, p_data + offset->irp_ptr_offs, offset->irp_ptr_size);
		memcpy(&ttid, p_data + offset->ttid_offs, offset->ttid_size);
		memcpy(&file_object, p_data + offset->file_object_offs, offset->file_object_size);
		memcpy(&file_key, p_data + offset->file_key_offs, offset->file_key_size);
		memcpy(&io_size, p_data + offset->io_size_offs, offset->io_size_size);
		memcpy(&io_flags, p_data + offset->io_flags_offs, offset->io_flags_size);
	}

	FileIoReadEvent::FileIoReadEvent(const Event& event)
	{
		this->FileIoReadWriteEventMember::FileIoReadWriteEventMember(event, &offset);
	}

	FileIoWriteEvent::FileIoWriteEvent(const Event& event)
	{
		this->FileIoReadWriteEventMember::FileIoReadWriteEventMember(event, &offset);
	}

	FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(const Event& event, FileIoSimpleOpEventOffset* offset)
	{
		if (offset->is_positioned == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs = p.first;
			offset->irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			offset->ttid_offs = p.first;
			offset->ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs = p.first;
			offset->file_object_size = p.second;

			p = wec.GetPropertyInfo(L"FileKey", event);
			offset->file_key_offs = p.first;
			offset->file_key_size = p.second;

			if (0 == offset->irp_ptr_size ||
				0 == offset->ttid_size ||
				0 == offset->file_object_size ||
				0 == offset->file_key_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileIoSimpleOpEvent");
				offset->is_successful = false;
				return;
			}
			offset->is_successful = true;
			offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&irp_ptr, p_data + offset->irp_ptr_offs, offset->irp_ptr_size);
		memcpy(&ttid, p_data + offset->ttid_offs, offset->ttid_size);
		memcpy(&file_object, p_data + offset->file_object_offs, offset->file_object_size);
		memcpy(&file_key, p_data + offset->file_key_offs, offset->file_key_size);
	}

	FileIoSimpleOpCleanupEvent::FileIoSimpleOpCleanupEvent(const Event& event)
	{
		this->FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(event, &offset);
	}

	FileIoSimpleOpCloseEvent::FileIoSimpleOpCloseEvent(const Event& event)
	{
		this->FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(event, &offset);
	}

	FileIoSimpleOpFlushEvent::FileIoSimpleOpFlushEvent(const Event& event)
	{
		this->FileIoSimpleOpEventMember::FileIoSimpleOpEventMember(event, &offset);
	}
};

#endif