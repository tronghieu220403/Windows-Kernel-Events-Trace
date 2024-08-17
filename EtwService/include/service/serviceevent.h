#ifdef _WIN32

#ifndef ETWSERVICE_SERVICE_SERVICEEVENT_H_
#define ETWSERVICE_SERVICE_SERVICEEVENT_H_

#include "ulti/support.h"
#include "ulti/collections.h"

namespace etw
{
    class ServiceEvent
    {
        public: 
            HANDLE stop_event_ = nullptr;
            HANDLE pause_event_ = nullptr;
            
            HANDLE pause_handled_ = nullptr;
            HANDLE stop_handled_ = nullptr;

            void Close();
    };
}
#endif

#endif
