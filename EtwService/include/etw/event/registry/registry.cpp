#ifdef _WIN32

#include "registry.h"

namespace etw
{

    RegistryEvent::RegistryEvent(PEVENT_TRACE p_event, int pointer_size)
    {
        this->Event::Event(p_event);
    }
};

#endif