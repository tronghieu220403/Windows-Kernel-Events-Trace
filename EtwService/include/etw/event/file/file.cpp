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
			
			p = wec.GetPropertyInfo(L"IrpPtr");
			irp_ptr_offs = p.first;
			irp_ptr_size = p.second;

			p = wec.GetPropertyInfo(L"TTID");
			ttid_offs = p.first;
			ttid_size = p.second;

			p = wec.GetPropertyInfo(L"FileObject");
			file_object_offs = p.first;
			file_object_size = p.second;

			p = wec.GetPropertyInfo(L"CreateOptions");
			create_options_offs = p.first;
			create_options_size = p.second;

			p = wec.GetPropertyInfo(L"FileAttributes");
			file_attributes_offs = p.first;
			file_attributes_size = p.second;

			p = wec.GetPropertyInfo(L"ShareAccess");
			share_access_offs = p.first;
			share_access_size = p.second;

			p = wec.GetPropertyInfo(L"OpenPath");
			open_path_offs = p.first;

			if (open_path_offs > irp_ptr_offs &&
				open_path_offs > ttid_offs &&
				open_path_offs > file_object_offs &&
				open_path_offs > create_options_offs &&
				open_path_offs > file_attributes_offs &&
				open_path_offs > share_access_offs)
			{
				is_positioned_ = true;
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
		WriteDebugW(s);

    }
};

#endif