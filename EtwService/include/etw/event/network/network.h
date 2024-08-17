#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_NETWORKEVENT_H_
#define ETWSERVICE_ETW_NETWORKEVENT_H_

#include "etw/event.h"

namespace etw
{
	class NetworkEvent: public Event
	{
	private:
	

	public:

		NetworkEvent(PEVENT_TRACE p_event, int pointer_size);

	};

};

#endif
#endif
