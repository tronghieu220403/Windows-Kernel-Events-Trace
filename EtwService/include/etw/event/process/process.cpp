#ifdef _WIN32

#include "process.h"

namespace etw
{

    ProcessEvent::ProcessEvent(PEVENT_TRACE p_event, int pointer_size)
    {
        this->Event::Event(p_event);
    }
};

#endif