#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_REGISTRYEVENT_H_
#define ETWSERVICE_ETW_REGISTRYEVENT_H_

#include "etw/event.h"

namespace etw
{
	class RegistryEvent: public Event
	{
	private:

		size_t transfer_size_ = 0;
		int thread_id_ = -1;

	public:
		RegistryEvent() = default;
		RegistryEvent(PEVENT_TRACE p_event, int pointer_size);

	};

};

#endif

#endif