#ifdef _WIN32

#include "registry.h"

namespace etw
{
	RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(const Event& event, RegistryTypeGroup1EventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kRegistry, event.GetVersion(), event.GetType(), sizeof(PVOID));

			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"Initial Time", event); // Different than the document ?????
			event_offset->initial_time_offs = p.first;
			event_offset->initial_time_size = p.second;

			p = wec.GetPropertyInfo(L"Status", event);
			event_offset->status_offs = p.first;
			event_offset->status_size = p.second;

			p = wec.GetPropertyInfo(L"Index", event);
			event_offset->index_offs = p.first;
			event_offset->index_size = p.second;

			p = wec.GetPropertyInfo(L"KeyHandle", event);
			event_offset->key_handle_offs = p.first;
			event_offset->key_handle_size = p.second;

			p = wec.GetPropertyInfo(L"KeyName", event);
			event_offset->key_name_offs = p.first;
			event_offset->key_name_size = p.second;

			if (0 == event_offset->initial_time_size ||
				0 == event_offset->status_size ||
				0 == event_offset->index_size ||
				0 == event_offset->key_handle_size ||
				0 == event_offset->key_name_size)
			{
				PrintDebugW(L"Event type %d GetPropertyInfo failed", event.GetType());
				event_offset->is_successful = false;
				return;
			}

			PrintDebugW(L"Event type %d GetPropertyInfo completed", event.GetType());
			event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
		memcpy(&InitialTime, p_data + event_offset->initial_time_offs, event_offset->initial_time_size);
		memcpy(&Status, p_data + event_offset->status_offs, event_offset->status_size);
		memcpy(&Index, p_data + event_offset->index_offs, event_offset->index_size);
		memcpy(&KeyHandle, p_data + event_offset->key_handle_offs, event_offset->key_handle_size);
		KeyName = (wchar_t*)(p_data + event_offset->key_name_offs);
	}

	RegistryCreateEvent::RegistryCreateEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryOpenEvent::RegistryOpenEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryDeleteEvent::RegistryDeleteEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryQueryEvent::RegistryQueryEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistrySetValueEvent::RegistrySetValueEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryDeleteValueEvent::RegistryDeleteValueEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryQueryValueEvent::RegistryQueryValueEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryEnumerateKeyEvent::RegistryEnumerateKeyEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryEnumerateValueKeyEvent::RegistryEnumerateValueKeyEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryQueryMultipleValueEvent::RegistryQueryMultipleValueEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistrySetInformationEvent::RegistrySetInformationEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryFlushEvent::RegistryFlushEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryKCBCreateEvent::RegistryKCBCreateEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryKCBDeleteEvent::RegistryKCBDeleteEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryKCBRundownBeginEvent::RegistryKCBRundownBeginEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryKCBRundownEndEvent::RegistryKCBRundownEndEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryVirtualizeEvent::RegistryVirtualizeEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}

	RegistryCloseEvent::RegistryCloseEvent(const Event& event)
	{
		this->RegistryTypeGroup1EventMember::RegistryTypeGroup1EventMember(event, &offset_);
	}
};

#endif
