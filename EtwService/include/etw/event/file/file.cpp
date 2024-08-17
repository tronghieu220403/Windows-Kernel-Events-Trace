#ifdef _WIN32

#include "file.h"

namespace etw
{

    FileCreateEvent::FileCreateEvent(const Event& event)
    {
		if (is_positioned_ == false)
		{
			WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;
			
			p = wec.GetPropertyInfo(L"IrpPtr", event);
			irp_ptr_offs = p.first;
			irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID", event);
			ttid_offs = p.first;
			ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject", event);
			file_object_offs = p.first;
			file_object_size = p.second;

			p = wec.GetPropertyInfo(L"CreateOptions", event);
			create_options_offs = p.first;
			create_options_size = p.second;

			p = wec.GetPropertyInfo(L"FileAttributes", event);
			file_attributes_offs = p.first;
			file_attributes_size = p.second;

			p = wec.GetPropertyInfo(L"ShareAccess", event);
			share_access_offs = p.first;
			share_access_size = p.second;

			p = wec.GetPropertyInfo(L"OpenPath", event);
			open_path_offs = p.first;

			if (0 == irp_ptr_size ||
				0 == ttid_size ||
				0 == file_object_size ||
				0 == create_options_size ||
				0 == file_attributes_size ||
				0 == share_access_size)
			{
				ulti::WriteDebugA("Error in GetPropertyInfo in FileCreateEvent");
				return;
			}
			if (open_path_offs > irp_ptr_offs &&
				open_path_offs > ttid_offs &&
				open_path_offs > file_object_offs &&
				open_path_offs > create_options_offs &&
				open_path_offs > file_attributes_offs &&
				open_path_offs > share_access_offs)
			{
				ulti::WriteDebugA("The position for FileCreateEvent is  as expected.");
				is_positioned_ = true;
			}
			else
			{
				ulti::WriteDebugA("The position for FileCreateEvent is not as expected.");
			}
		}

        PBYTE p_data = event.GetPEventData();
		memcpy(&IrpPtr, p_data + irp_ptr_offs, irp_ptr_size);
		memcpy(&TTID, p_data + ttid_offs, ttid_size);
		memcpy(&FileObject, p_data + file_object_offs, file_object_size);
		memcpy(&CreateOptions, p_data + create_options_offs, create_options_size);
		memcpy(&FileAttributes, p_data + file_attributes_offs, file_attributes_size);
		memcpy(&ShareAccess, p_data + share_access_offs, share_access_size);
		OpenPath = (wchar_t*)(p_data + open_path_offs);
		
		std::wstring s(OpenPath);
		//ulti::WriteDebugW(s);

    }
};

#endif