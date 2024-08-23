#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_PROCESS_PROCESS_H_
#define ETWSERVICE_ETW_ETW_EVENT_PROCESS_PROCESS_H_

#include "etw/event.h"
#include "etw/wmieventclass.h"

namespace etw
{

	enum ProcessEventType
	{
		kProcessStart = 1,
		kProcessEnd = 2,
		kProcessDCStart = 3,
		kProcessDCEnd = 4,
		kProcessDefunct = 39,
	};

	enum ImageEventType
	{
		kImageLoad = 10,
		kImageUnload = 2,
		kImageDCStart = 3,
		kImageDCEnd = 4,
	};

	enum ThreadEventType
	{
		kThreadStart = 1,
		kThreadEnd = 2,
		kThreadDCStart = 3,
		kThreadDCEnd = 4
	};


	/*-------------PROCESS-------------*/

	/*
	[EventType{1, 2, 3, 4, 39}, EventTypeName{"Start", "End", "DCStart", "DCEnd", "Defunct"}]
	class Process_TypeGroup1 : Process
	{
		uint32 UniqueProcessKey;
		uint32 ProcessId;
		uint32 ParentId;
		uint32 SessionId;
		sint32 ExitStatus;
		uint32 DirectoryTableBase;
		object UserSID;
		string ImageFileName;
		string CommandLine;
	};
	*/
	struct ProcessTypeGroup1EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD unique_process_key_offs = 0;
		DWORD process_id_offs = 0;
		DWORD parent_id_offs = 0;
		DWORD session_id_offs = 0;
		DWORD exit_status_offs = 0;
		DWORD directory_table_base_offs = 0;
		//DWORD user_sid_offs = 0;
		//DWORD image_file_name_offs = 0;
		//DWORD command_line_offs = 0;

		DWORD unique_process_key_size = 0;
		DWORD process_id_size = 0;
		DWORD parent_id_size = 0;
		DWORD session_id_size = 0;
		DWORD exit_status_size = 0;
		DWORD directory_table_base_size = 0;
		//DWORD user_sid_size = 0;
		//DWORD image_file_name_size = 0;
		//DWORD command_line_size = 0;
	};

	struct ProcessTypeGroup1EventMember
	{
	public:
		size_t process_unique_key = 0;
		size_t pid = 0;
		size_t ppid = 0;
		size_t session_id = 0;
		size_t exit_status = 0;
		size_t directory_table_base = 0;
		//PVOID user_sid = nullptr;  // Assuming object type is represented as a pointer
		//wchar_t* image_file_name = nullptr;
		//wchar_t* command_line = nullptr;

		ProcessTypeGroup1EventMember() = default;
		ProcessTypeGroup1EventMember(const Event& event, ProcessTypeGroup1EventOffset* offset);
	};

	struct ProcessStartEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset;
	public:
		ProcessStartEvent(const Event& event);
	};

	struct ProcessEndEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset;
	public:
		ProcessEndEvent(const Event& event);
	};

	struct ProcessDCStartEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset;
	public:
		ProcessDCStartEvent(const Event& event);
	};

	struct ProcessDCEndEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset;
	public:
		ProcessDCEndEvent(const Event& event);
	};

	struct ProcessDefunctEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset;
	public:
		ProcessDefunctEvent(const Event& event);
	};

	/*--------------------------------*/



	/*-------------THREAD-------------*/

	/*
	[EventType{1, 2, 3, 4}, EventTypeName{"Start", "End", "DCStart", "DCEnd"}]
	class Thread_TypeGroup1 : Thread
	{
		uint32 ProcessId;
		uint32 TThreadId;
		uint32 StackBase;
		uint32 StackLimit;
		uint32 UserStackBase;
		uint32 UserStackLimit;
		uint32 Affinity;
		uint32 Win32StartAddr;
		uint32 TebBase;
		uint32 SubProcessTag;
		uint8  BasePriority;
		uint8  PagePriority;
		uint8  IoPriority;
		uint8  ThreadFlags;
	};
	*/
	struct ThreadTypeGroup1EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD process_id_offs = 0;
		DWORD tthread_id_offs = 0;
		DWORD stack_base_offs = 0;
		DWORD stack_limit_offs = 0;
		DWORD user_stack_base_offs = 0;
		DWORD user_stack_limit_offs = 0;
		DWORD affinity_offs = 0;
		DWORD win32_start_addr_offs = 0;
		DWORD teb_base_offs = 0;
		DWORD subprocess_tag_offs = 0;
		DWORD base_priority_offs = 0;
		DWORD page_priority_offs = 0;
		DWORD io_priority_offs = 0;
		DWORD thread_flags_offs = 0;

		DWORD process_id_size = 0;
		DWORD tthread_id_size = 0;
		DWORD stack_base_size = 0;
		DWORD stack_limit_size = 0;
		DWORD user_stack_base_size = 0;
		DWORD user_stack_limit_size = 0;
		DWORD affinity_size = 0;
		DWORD win32_start_addr_size = 0;
		DWORD teb_base_size = 0;
		DWORD subprocess_tag_size = 0;
		DWORD base_priority_size = 0;
		DWORD page_priority_size = 0;
		DWORD io_priority_size = 0;
		DWORD thread_flags_size = 0;
	};

	struct ThreadTypeGroup1EventMember
	{
	public:
		size_t pid = 0;
		size_t ttid = 0;
		size_t stack_base = 0;
		size_t stack_limit = 0;
		size_t user_stack_base = 0;
		size_t user_stack_limit = 0;
		size_t affinity = 0;
		size_t win32_start_addr = 0;
		size_t teb_base = 0;
		size_t sub_process_tag = 0;
		size_t base_priority = 0;
		size_t page_priority = 0;
		size_t io_priority = 0;
		size_t thread_flags = 0;

		ThreadTypeGroup1EventMember() = default;
		ThreadTypeGroup1EventMember(const Event& event, ThreadTypeGroup1EventOffset* offset);
	};

	struct ThreadStartEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset;
	public:
		ThreadStartEvent(const Event& event);
	};

	struct ThreadEndEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset;
	public:
		ThreadEndEvent(const Event& event);
	};

	struct ThreadDCStartEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset;
	public:
		ThreadDCStartEvent(const Event& event);
	};

	struct ThreadDCEndEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset;
	public:
		ThreadDCEndEvent(const Event& event);
	};

	/*-------------IMAGE-------------*/

	/*
	[EventType(10, 2, 3, 4), EventTypeName("Load", "Unload", "DCStart", "DCEnd")]
	class Image_Load : Image
	{
		uint32 ImageBase;
		uint32 ImageSize;
		uint32 ProcessId;
		uint32 ImageCheckSum;
		uint32 TimeDateStamp;
		uint32 Reserved0;
		uint32 DefaultBase;
		uint32 Reserved1;
		uint32 Reserved2;
		uint32 Reserved3;
		uint32 Reserved4;
		string FileName;
	};
	*/
	struct ImageLoadEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD image_base_offs = 0;
		DWORD image_size_offs = 0;
		DWORD process_id_offs = 0;
		DWORD image_checksum_offs = 0;
		DWORD time_date_stamp_offs = 0;
		DWORD reserved0_offs = 0;
		DWORD default_base_offs = 0;
		DWORD reserved1_offs = 0;
		DWORD reserved2_offs = 0;
		DWORD reserved3_offs = 0;
		DWORD reserved4_offs = 0;
		DWORD file_name_offs = 0;

		DWORD image_base_size = 0;
		DWORD image_size_size = 0;
		DWORD process_id_size = 0;
		DWORD image_checksum_size = 0;
		DWORD time_date_stamp_size = 0;
		DWORD reserved0_size = 0;
		DWORD default_base_size = 0;
		DWORD reserved1_size = 0;
		DWORD reserved2_size = 0;
		DWORD reserved3_size = 0;
		DWORD reserved4_size = 0;
		DWORD file_name_size = 0;
	};

	struct ImageLoadEventMember
	{
	public:
		size_t ImageBase = 0;
		size_t ImageSize = 0;
		size_t ProcessId = 0;
		size_t ImageCheckSum = 0;
		size_t TimeDateStamp = 0;
		size_t Reserved0 = 0;
		size_t DefaultBase = 0;
		size_t Reserved1 = 0;
		size_t Reserved2 = 0;
		size_t Reserved3 = 0;
		size_t Reserved4 = 0;
		wchar_t* file_name = nullptr;

		ImageLoadEventMember() = default;
		ImageLoadEventMember(const Event& event, ImageLoadEventOffset* offset);
	};

	struct ImageLoadEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset;
	public:
		ImageLoadEvent(const Event& event);
	};

	struct ImageUnloadEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset;
	public:
		ImageUnloadEvent(const Event& event);
	};

	struct ImageDCStartEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset;
	public:
		ImageDCStartEvent(const Event& event);
	};

	struct ImageDCEndEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset;
	public:
		ImageDCEndEvent(const Event& event);
	};

	/*---------------------------------*/




};

#endif

#endif