#ifdef _WIN32

#include "file.h"

namespace etw
{

    FileEvent::FileEvent(const PEVENT_TRACE& p_event)
    {
        this->Event::Event(p_event);
    }

    FileEvent::FileEvent(const Event& event)
	{
		this->Event::Event(event);
    }

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
		
		std::wstring s(OpenPath);
		WriteDebugW(s);
    }
};

#endif