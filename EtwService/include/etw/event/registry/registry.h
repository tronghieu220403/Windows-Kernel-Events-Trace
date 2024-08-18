#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_REGISTRYEVENT_H_
#define ETWSERVICE_ETW_REGISTRYEVENT_H_

#include "etw/event.h"

namespace etw
{
	/*
	[EventType{10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27}, EventTypeName{"Create", "Open", "Delete", "Query", "SetValue", "DeleteValue", "QueryValue", "EnumerateKey", "EnumerateValueKey", "QueryMultipleValue", "SetInformation", "Flush", "KCBCreate", "KCBDelete", "KCBRundownBegin", "KCBRundownEnd", "Virtualize", "Close"}]
	class Registry_TypeGroup1 : Registry
	{
	  sint64 InitialTime;
	  uint32 Status;
	  uint32 Index;
	  uint32 KeyHandle;
	  string KeyName;
	};
	*/
	struct RegistryTypeGroup1EventOffset
	{
		bool is_positioned_ = false;

		DWORD initial_time_offs_ = 0;
		DWORD status_offs_ = 0;
		DWORD index_offs_ = 0;
		DWORD key_handle_offs_ = 0;
		DWORD key_name_offs_ = 0;
	};

	struct RegistryTypeGroup1EventMember
	{
	public:
		int64_t InitialTime = 0;
		uint32_t Status = 0;
		uint32_t Index = 0;
		uint32_t KeyHandle = 0;
		wchar_t* KeyName = nullptr;

		RegistryTypeGroup1EventMember() = default;
		RegistryTypeGroup1EventMember(const Event& event, RegistryTypeGroup1EventOffset* offset);
	};

	struct RegistryCreateEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryCreateEvent(const Event& event);
	};

	struct RegistryOpenEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryOpenEvent(const Event& event);
	};

	struct RegistryDeleteEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryDeleteEvent(const Event& event);
	};

	struct RegistryQueryEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryQueryEvent(const Event& event);
	};

	struct RegistrySetValueEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistrySetValueEvent(const Event& event);
	};

	struct RegistryDeleteValueEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryDeleteValueEvent(const Event& event);
	};

	struct RegistryQueryValueEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryQueryValueEvent(const Event& event);
	};

	struct RegistryEnumerateKeyEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryEnumerateKeyEvent(const Event& event);
	};

	struct RegistryEnumerateValueKeyEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryEnumerateValueKeyEvent(const Event& event);
	};

	struct RegistryQueryMultipleValueEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryQueryMultipleValueEvent(const Event& event);
	};

	struct RegistrySetInformationEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistrySetInformationEvent(const Event& event);
	};

	struct RegistryFlushEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryFlushEvent(const Event& event);
	};

	struct RegistryKCBCreateEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryKCBCreateEvent(const Event& event);
	};

	struct RegistryKCBDeleteEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryKCBDeleteEvent(const Event& event);
	};

	struct RegistryKCBRundownBeginEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryKCBRundownBeginEvent(const Event& event);
	};

	struct RegistryKCBRundownEndEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryKCBRundownEndEvent(const Event& event);
	};

	struct RegistryVirtualizeEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryVirtualizeEvent(const Event& event);
	};

	struct RegistryCloseEvent : RegistryTypeGroup1EventMember
	{
	private:
		static inline RegistryTypeGroup1EventOffset offset_;
	public:
		RegistryCloseEvent(const Event& event);
	};

};

#endif

#endif