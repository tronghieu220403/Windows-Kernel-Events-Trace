#ifdef _WIN32

#include "file.h"

namespace etw
{

    FileCreateEvent::FileCreateEvent(const Event& event)
    {
        PBYTE p_data = event.GetPEventData();
		DWORD offset = 0;
		memcpy(&IrpPtr, p_data, sizeof(PVOID));
		offset += sizeof(PVOID);
		memcpy(&TTID, p_data + offset, sizeof(PVOID));
		offset += sizeof(PVOID);
		memcpy(&FileObject, p_data + offset, sizeof(PVOID));
		offset += sizeof(PVOID);
		memcpy(&CreateOptions, p_data + offset, sizeof(DWORD));
		offset += sizeof(DWORD);
		memcpy(&FileAttributes, p_data + offset, sizeof(DWORD));
		offset += sizeof(DWORD);
		memcpy(&ShareAccess, p_data + offset, sizeof(DWORD));
		offset += sizeof(DWORD);
		OpenPath = (wchar_t*)(p_data + offset - 4);
		
		WmiEventClass wec(EventType().kFileIo, event.GetVersion(), event.GetType(), sizeof(PVOID));

		int prop_offset[16];
		prop_offset[0] = wec.GetPropertyInfo(L"IrpPtr").first;
		prop_offset[1] = wec.GetPropertyInfo(L"IrpPtr").second;

		prop_offset[2] = wec.GetPropertyInfo(L"TTID").first;
		prop_offset[3] = wec.GetPropertyInfo(L"TTID").second;

		prop_offset[4] = wec.GetPropertyInfo(L"FileObject").first;
		prop_offset[5] = wec.GetPropertyInfo(L"FileObject").second;

		prop_offset[6] = wec.GetPropertyInfo(L"CreateOptions").first;
		prop_offset[7] = wec.GetPropertyInfo(L"CreateOptions").second;

		prop_offset[8] = wec.GetPropertyInfo(L"FileAttributes").first;
		prop_offset[9] = wec.GetPropertyInfo(L"FileAttributes").second;

		prop_offset[10] = wec.GetPropertyInfo(L"ShareAccess").first;
		prop_offset[11] = wec.GetPropertyInfo(L"ShareAccess").second;

		std::stringstream ss;
		for (int i = 0; i < 12; i++)
		{
			ss << prop_offset[i] << " ";
		}

		std::wstring s(OpenPath);
		WriteDebugW(s);

		WriteDebugA(ss.str());
    }
};

#endif