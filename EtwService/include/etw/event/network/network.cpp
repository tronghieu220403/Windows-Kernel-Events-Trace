#ifdef _WIN32

#include "network.h"

namespace etw
{
    NetworkEvent::NetworkEvent(PEVENT_TRACE p_event, int pointer_size)
    {
        this->Event::Event(p_event);
    }
};

#endif