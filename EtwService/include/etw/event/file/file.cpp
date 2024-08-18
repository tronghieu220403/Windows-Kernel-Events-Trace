#ifdef _WIN32

#include "file.h"

namespace etw
{

	FileIoCreateEventMember::FileIoCreateEventMember(const Event& event, FileIoCreateEventOffset* offset)
	{
		if (offset->is_positioned_ == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"IrpPtr", event);
			offset->irp_ptr_offs_ = p.first;
			offset->irp_ptr_size_ = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			offset->ttid_offs_ = p.first;
			offset->ttid_size_ = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			offset->file_object_offs_ = p.first;
			offset->file_object_size_ = p.second;

			p = wec.GetPropertyInfo(L"CreateOptions", event);
			offset->create_options_offs_ = p.first;
			offset->create_options_size_ = p.second;

			p = wec.GetPropertyInfo(L"FileAttributes", event);
			offset->file_attributes_offs_ = p.first;
			offset->file_attributes_size_ = p.second;

			p = wec.GetPropertyInfo(L"ShareAccess", event);
			offset->share_access_offs_ = p.first;
			offset->share_access_size_ = p.second;

			p = wec.GetPropertyInfo(L"OpenPath", event);
			offset->open_path_offs_ = p.first;

			if (0 == offset->irp_ptr_size_ ||
				0 == offset->ttid_size_ ||
				0 == offset->file_object_size_ ||
				0 == offset->create_options_size_ ||
				0 == offset->file_attributes_size_ ||
				0 == offset->share_access_size_)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileCreateEvent");
				return;
			}
			if (offset->open_path_offs_ > offset->irp_ptr_offs_ &&
				offset->open_path_offs_ > offset->ttid_offs_ &&
				offset->open_path_offs_ > offset->file_object_offs_ &&
				offset->open_path_offs_ > offset->create_options_offs_ &&
				offset->open_path_offs_ > offset->file_attributes_offs_ &&
				offset->open_path_offs_ > offset->share_access_offs_)
			{
				ulti::WriteDebugA("The position for FileCreateEvent is as expected.");
				offset->is_positioned_ = true;
			}
			else
			{
				ulti::WriteDebugA("The position for FileCreateEvent is not as expected.");
			}
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&IrpPtr, p_data + offset->irp_ptr_offs_, offset->irp_ptr_size_);
		memcpy(&TTID, p_data + offset->ttid_offs_, offset->ttid_size_);
		memcpy(&FileObject, p_data + offset->file_object_offs_, offset->file_object_size_);
		memcpy(&CreateOptions, p_data + offset->create_options_offs_, offset->create_options_size_);
		memcpy(&FileAttributes, p_data + offset->file_attributes_offs_, offset->file_attributes_size_);
		memcpy(&ShareAccess, p_data + offset->share_access_offs_, offset->share_access_size_);
		OpenPath = (wchar_t*)(p_data + offset->open_path_offs_);
	}


    FileIoCreateEvent::FileIoCreateEvent(const Event& event)
    {
		
		FileIoCreateEventMember::FileIoCreateEventMember(event, &offset_);
		std::wstring s(OpenPath);
		//ulti::WriteDebugW(s);

    }
};

#endif