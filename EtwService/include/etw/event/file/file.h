#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_FILEIOEVENT_H_
#define ETWSERVICE_ETW_FILEIOEVENT_H_

#include "etw/event.h"
namespace etw
{
	class FileEvent: public Event
	{
	private:

	public:
		FileEvent() = default;
		FileEvent(const PEVENT_TRACE& p_event);
		FileEvent(const Event& event);
	};
	
	class FileCreateEvent : public FileEvent
	{
	private:
	public:
		PVOID IrpPtr = NULL;
		PVOID TTID = NULL;
		PVOID FileObject = NULL;
		DWORD CreateOptions = NULL;
		DWORD FileAttributes = NULL;
		DWORD ShareAccess = NULL;
		wchar_t* OpenPath = NULL;

		FileCreateEvent() = default;
		FileCreateEvent(const Event& event);
	};

};

#endif

#endif