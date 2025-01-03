#ifdef _WIN32

#include "event.h"

namespace etw
{
    Event::Event(PEVENT_TRACE p_event)
    {
        p_event_ = p_event;
    }

    PEVENT_TRACE Event::GetPEvent() const
    {
        return PEVENT_TRACE();
    }

    GUID Event::GetGuid() const {
        return p_event_->Header.Guid;
    }

    int Event::GetVersion() const {
        return p_event_->Header.Class.Version;
    }

    int Event::GetType() const {
        return p_event_->Header.Class.Type;
    }

    FILETIME Event::GetFileTime() const
    {
        return *(FILETIME*)&p_event_->Header.TimeStamp.QuadPart;
    }

    ULONGLONG Event::GetTimeInMs() const
    {
        return p_event_->Header.TimeStamp.QuadPart / 10000;
    }

    ULONGLONG Event::GetTimeInMicroSec() const
    {
        return p_event_->Header.TimeStamp.QuadPart / 10;
    }

    ULONG Event::GetMofLength() const
    {
        return p_event_->MofLength;
    }

    PBYTE Event::GetPEventData() const {
        return (PBYTE)p_event_->MofData;
    }

    size_t Event::GetThreadId() const
    {
        return (size_t)p_event_->Header.ThreadId;
    }

    size_t Event::GetProcessId() const
    {
        return (size_t)p_event_->Header.ProcessId;
    }
}

#endif