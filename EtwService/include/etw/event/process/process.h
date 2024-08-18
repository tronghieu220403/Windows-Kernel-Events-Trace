#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_DISKIOEVENT_H_
#define ETWSERVICE_ETW_DISKIOEVENT_H_

#include "etw/event.h"

namespace etw
{
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
		bool is_positioned_ = false;

		DWORD image_base_offs_ = 0;
		DWORD image_size_offs_ = 0;
		DWORD process_id_offs_ = 0;
		DWORD image_checksum_offs_ = 0;
		DWORD time_date_stamp_offs_ = 0;
		DWORD reserved0_offs_ = 0;
		DWORD default_base_offs_ = 0;
		DWORD reserved1_offs_ = 0;
		DWORD reserved2_offs_ = 0;
		DWORD reserved3_offs_ = 0;
		DWORD reserved4_offs_ = 0;
		DWORD file_name_offs_ = 0;
	};

	struct ImageLoadEventMember
	{
	public:
		uint32_t ImageBase = 0;
		uint32_t ImageSize = 0;
		uint32_t ProcessId = 0;
		uint32_t ImageCheckSum = 0;
		uint32_t TimeDateStamp = 0;
		uint32_t Reserved0 = 0;
		uint32_t DefaultBase = 0;
		uint32_t Reserved1 = 0;
		uint32_t Reserved2 = 0;
		uint32_t Reserved3 = 0;
		uint32_t Reserved4 = 0;
		wchar_t* FileName = nullptr;

		ImageLoadEventMember() = default;
		ImageLoadEventMember(const Event& event, ImageLoadEventOffset* offset);
	};

	struct ImageLoadEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset_;
	public:
		ImageLoadEvent(const Event& event);
	};

	struct ImageUnloadEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset_;
	public:
		ImageUnloadEvent(const Event& event);
	};

	struct ImageDCStartEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset_;
	public:
		ImageDCStartEvent(const Event& event);
	};

	struct ImageDCEndEvent : ImageLoadEventMember
	{
	private:
		static inline ImageLoadEventOffset offset_;
	public:
		ImageDCEndEvent(const Event& event);
	};

	/*---------------------------------*/

	

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
		bool is_positioned_ = false;

		DWORD unique_process_key_offs_ = 0;
		DWORD process_id_offs_ = 0;
		DWORD parent_id_offs_ = 0;
		DWORD session_id_offs_ = 0;
		DWORD exit_status_offs_ = 0;
		DWORD directory_table_base_offs_ = 0;
		DWORD user_sid_offs_ = 0;
		DWORD image_file_name_offs_ = 0;
		DWORD command_line_offs_ = 0;
	};

	struct ProcessTypeGroup1EventMember
	{
	public:
		uint32_t UniqueProcessKey = 0;
		uint32_t ProcessId = 0;
		uint32_t ParentId = 0;
		uint32_t SessionId = 0;
		int32_t ExitStatus = 0;
		uint32_t DirectoryTableBase = 0;
		PVOID UserSID = nullptr;  // Assuming object type is represented as a pointer
		wchar_t* ImageFileName = nullptr;
		wchar_t* CommandLine = nullptr;

		ProcessTypeGroup1EventMember() = default;
		ProcessTypeGroup1EventMember(const Event& event, ProcessTypeGroup1EventOffset* offset);
	};

	struct ProcessStartEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset_;
	public:
		ProcessStartEvent(const Event& event);
	};

	struct ProcessEndEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset_;
	public:
		ProcessEndEvent(const Event& event);
	};

	struct ProcessDCStartEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset_;
	public:
		ProcessDCStartEvent(const Event& event);
	};

	struct ProcessDCEndEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset_;
	public:
		ProcessDCEndEvent(const Event& event);
	};

	struct ProcessDefunctEvent : ProcessTypeGroup1EventMember
	{
	private:
		static inline ProcessTypeGroup1EventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD process_id_offs_ = 0;
		DWORD tthread_id_offs_ = 0;
		DWORD stack_base_offs_ = 0;
		DWORD stack_limit_offs_ = 0;
		DWORD user_stack_base_offs_ = 0;
		DWORD user_stack_limit_offs_ = 0;
		DWORD affinity_offs_ = 0;
		DWORD win32_start_addr_offs_ = 0;
		DWORD teb_base_offs_ = 0;
		DWORD subprocess_tag_offs_ = 0;
		DWORD base_priority_offs_ = 0;
		DWORD page_priority_offs_ = 0;
		DWORD io_priority_offs_ = 0;
		DWORD thread_flags_offs_ = 0;
	};

	struct ThreadTypeGroup1EventMember
	{
	public:
		uint32_t ProcessId = 0;
		uint32_t TThreadId = 0;
		uint32_t StackBase = 0;
		uint32_t StackLimit = 0;
		uint32_t UserStackBase = 0;
		uint32_t UserStackLimit = 0;
		uint32_t Affinity = 0;
		uint32_t Win32StartAddr = 0;
		uint32_t TebBase = 0;
		uint32_t SubProcessTag = 0;
		uint8_t BasePriority = 0;
		uint8_t PagePriority = 0;
		uint8_t IoPriority = 0;
		uint8_t ThreadFlags = 0;

		ThreadTypeGroup1EventMember() = default;
		ThreadTypeGroup1EventMember(const Event& event, ThreadTypeGroup1EventOffset* offset);
	};

	struct ThreadStartEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset_;
	public:
		ThreadStartEvent(const Event& event);
	};

	struct ThreadEndEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset_;
	public:
		ThreadEndEvent(const Event& event);
	};

	struct ThreadDCStartEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset_;
	public:
		ThreadDCStartEvent(const Event& event);
	};

	struct ThreadDCEndEvent : ThreadTypeGroup1EventMember
	{
	private:
		static inline ThreadTypeGroup1EventOffset offset_;
	public:
		ThreadDCEndEvent(const Event& event);
	};



};

#endif

#endif