#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_FILE_FILE_H_
#define ETWSERVICE_ETW_ETW_EVENT_FILE_FILE_H_

#include "ulti/debug.h"
#include "etw/event.h"
#include "etw/wmieventclass.h"

namespace etw
{
	enum FileIoEventType
	{
		kCreate = 64,
		kDirEnum = 72,
		kDirNotify = 77,
		kSetInfo = 69,
		kDelete = 70,
		kRename = 71,
		kQueryInfo = 74,
		kFSControl = 75,
		kName = 0,
		kFileCreate = 32,
		kFileDelete = 35,
		kFileRundown = 36,
		kOperationEnd = 76,
		kRead = 67,
		kWrite = 68,
		kCleanup = 65,
		kClose = 66,
		kFlush = 73
	};

	/*
	[EventType{64}, EventTypeName{"Create"}]
	class FileIo_Create : FileIo
	{
	  uint32 IrpPtr;
	  uint32 TTID;
	  uint32 FileObject;
	  uint32 CreateOptions;
	  uint32 FileAttributes;
	  uint32 ShareAccess;
	  string OpenPath;
	};
	*/
	struct FileIoCreateEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD irp_ptr_offs = 0;
		DWORD ttid_offs = 0;
		DWORD file_object_offs = 0;
		DWORD create_options_offs = 0;
		DWORD file_attributes_offs = 0;
		DWORD share_access_offs = 0;
		DWORD open_path_offs = 0;

		DWORD irp_ptr_size = 0;
		DWORD ttid_size = 0;
		DWORD file_object_size = 0;
		DWORD create_options_size = 0;
		DWORD file_attributes_size = 0;
		DWORD share_access_size = 0;
	};

	struct FileIoCreateEventMember
	{
	public:
		size_t irp_ptr = 0;
		size_t ttid = NULL;
		size_t file_object = 0;
		size_t create_options = NULL;
		size_t file_attributes = NULL;
		size_t share_access = NULL;
		wchar_t* open_path = nullptr;

		FileIoCreateEventMember() = default;
		FileIoCreateEventMember(const Event& event, FileIoCreateEventOffset* event_offset);

	};

	struct FileIoCreateEvent: FileIoCreateEventMember
	{
	private:
		static inline FileIoCreateEventOffset event_offset_;
	public:
		FileIoCreateEvent(const Event& event);
	};

	/*
	[EventType{ 72, 77 }, EventTypeName{ "DirEnum", "DirNotify" }]
		class FileIo_DirEnum : FileIo
	{
		uint32 IrpPtr;
		uint32 TTID;
		uint32 FileObject;
		uint32 FileKey;
		uint32 Length;
		uint32 InfoClass;
		uint32 FileIndex;
		string FileName;
	};
	*/
	struct FileIoDirEnumEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD irp_ptr_offs = 0;
		DWORD ttid_offs = 0;
		DWORD file_object_offs = 0;
		DWORD file_key_offs = 0;
		DWORD length_offs = 0;
		DWORD info_class_offs = 0;
		DWORD file_index_offs = 0;
		DWORD file_name_offs = 0;

		DWORD irp_ptr_size = 0;
		DWORD ttid_size = 0;
		DWORD file_object_size = 0;
		DWORD file_key_size = 0;
		DWORD length_size = 0;
		DWORD info_class_size = 0;
		DWORD file_index_size = 0;
	};

	struct FileIoDirEnumEventMember
	{
	public:
		size_t irp_ptr = 0;
		size_t ttid = NULL;
		size_t file_object = 0;
		size_t file_key = 0;
		size_t length = NULL;
		PVOID info_class = nullptr;
		size_t file_index = NULL;
		wchar_t* file_name = nullptr;

		FileIoDirEnumEventMember() = default;
		FileIoDirEnumEventMember(const Event& event, FileIoDirEnumEventOffset* event_offset);
	};

	struct FileIoDirEnumEvent : FileIoDirEnumEventMember
	{
	private:
		static inline FileIoDirEnumEventOffset event_offset_;
	public:
		FileIoDirEnumEvent(const Event& event);
	};

	struct FileIoDirNotifyEvent : FileIoDirEnumEventMember
	{
	private:
		static inline FileIoDirEnumEventOffset event_offset_;
	public:
		FileIoDirNotifyEvent(const Event& event);
	};

	/*
	[EventType{69, 70, 71, 74, 75}, EventTypeName{"SetInfo", "Delete", "Rename", "QueryInfo", "FSControl"}]
	class FileIo_Info : FileIo
	{
		uint32 IrpPtr;
		uint32 TTID;
		uint32 FileObject;
		uint32 FileKey;
		uint32 ExtraInfo;
		uint32 InfoClass;
	};
	*/
	struct FileIoInfoEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD irp_ptr_offs = 0;
		DWORD ttid_offs = 0;
		DWORD file_object_offs = 0;
		DWORD file_key_offs = 0;
		DWORD extra_info_offs = 0;
		DWORD info_class_offs = 0;

		DWORD irp_ptr_size = 0;
		DWORD ttid_size = 0;
		DWORD file_object_size = 0;
		DWORD file_key_size = 0;
		DWORD extra_info_size = 0;
		DWORD info_class_size = 0;
	};

	struct FileIoInfoEventMember
	{
	public:
		size_t irp_ptr = 0;
		size_t ttid = NULL;
		size_t file_object = 0;
		size_t file_key = 0;
		size_t extra_info = NULL;
		PVOID info_class = nullptr;

		FileIoInfoEventMember() = default;
		FileIoInfoEventMember(const Event& event, FileIoInfoEventOffset* event_offset);
	};

	struct FileIoSetInfoEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset event_offset_;
	public:
		FileIoSetInfoEvent(const Event& event);
	};

	struct FileIoDeleteEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset event_offset_;
	public:
		FileIoDeleteEvent(const Event& event);
	};

	struct FileIoRenameEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset event_offset_;
	public:
		FileIoRenameEvent(const Event& event);
	};

	struct FileIoQueryInfoEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset event_offset_;
	public:
		FileIoQueryInfoEvent(const Event& event);
	};

	struct FileIoFSControlEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset event_offset_;
	public:
		FileIoFSControlEvent(const Event& event);
	};

	/*
	[EventType{0, 32, 35, 36}, EventTypeName{"Name", "FileCreate", "FileDelete", "FileRundown"}]
	class FileIo_Name : FileIo
	{
		uint32 FileObject;
		string FileName;
	};
	*/
	struct FileIoNameEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD file_object_offs = 0;
		DWORD file_name_offs = 0;

		DWORD file_object_size = 0;
		DWORD file_name_size = 0;
	};

	struct FileIoNameEventMember
	{
	public:
		size_t file_object = 0;
		wchar_t* file_name = nullptr;

		FileIoNameEventMember() = default;
		FileIoNameEventMember(const Event& event, FileIoNameEventOffset* event_offset);
	};

	struct FileIoNameEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset event_offset_;
	public:
		FileIoNameEvent(const Event& event);
	};

	struct FileIoFileCreateEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset event_offset_;
	public:
		FileIoFileCreateEvent(const Event& event);
	};

	struct FileIoFileDeleteEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset event_offset_;
	public:
		FileIoFileDeleteEvent(const Event& event);
	};

	struct FileIoFileRundownEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset event_offset_;
	public:
		FileIoFileRundownEvent(const Event& event);
	};

	/*
	[EventType{76}, EventTypeName{"OperationEnd"}]
	class FileIo_OpEnd : FileIo
	{
		uint32 IrpPtr;
		uint32 ExtraInfo;
		uint32 NtStatus;
	};
	*/
	struct FileIoOpEndEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD irp_ptr_offs = 0;
		DWORD extra_info_offs = 0;
		DWORD nt_status_offs = 0;

		DWORD irp_ptr_size = 0;
		DWORD extra_info_size = 0;
		DWORD nt_status_size = 0;
	};

	struct FileIoOpEndEventMember
	{
	public:
		size_t irp_ptr = 0;
		size_t extra_info = NULL;
		size_t nt_status = NULL;

		FileIoOpEndEventMember() = default;
		FileIoOpEndEventMember(const Event& event, FileIoOpEndEventOffset* event_offset);
	};

	struct FileIoOpEndEvent : FileIoOpEndEventMember
	{
	private:
		static inline FileIoOpEndEventOffset event_offset_;
	public:
		FileIoOpEndEvent(const Event& event);
	};

	/*
	[EventType{67, 68}, EventTypeName{"Read", "Write"}]
	class FileIo_ReadWrite : FileIo
	{
		uint64 Offset;
		uint32 IrpPtr;
		uint32 TTID;
		uint32 FileObject;
		uint32 FileKey;
		uint32 IoSize;
		uint32 IoFlags;
	};
	*/
	struct FileIoReadWriteEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD offset_offs = 0;
		DWORD irp_ptr_offs = 0;
		DWORD ttid_offs = 0;
		DWORD file_object_offs = 0;
		DWORD file_key_offs = 0;
		DWORD io_size_offs = 0;
		DWORD io_flags_offs = 0;

		DWORD offset_size = 0;
		DWORD irp_ptr_size = 0;
		DWORD ttid_size = 0;
		DWORD file_object_size = 0;
		DWORD file_key_size = 0;
		DWORD io_size_size = 0;
		DWORD io_flags_size = 0;
	};

	struct FileIoReadWriteEventMember
	{
	public:
		uint64_t offset = 0;
		size_t irp_ptr = 0;
		size_t ttid = NULL;
		size_t file_object = 0;
		size_t file_key = 0;
		size_t io_size = NULL;
		size_t io_flags = NULL;

		FileIoReadWriteEventMember() = default;
		FileIoReadWriteEventMember(const Event& event, FileIoReadWriteEventOffset* event_offset);
	};

	struct FileIoReadEvent : FileIoReadWriteEventMember
	{
	private:
		static inline FileIoReadWriteEventOffset event_offset_;
	public:
		FileIoReadEvent(const Event& event);
	};

	struct FileIoWriteEvent : FileIoReadWriteEventMember
	{
	private:
		static inline FileIoReadWriteEventOffset event_offset_;
	public:
		FileIoWriteEvent(const Event& event);
	};

	/*
	[EventType{65, 66, 73}, EventTypeName{"Cleanup", "Close", "Flush"}]
	class FileIo_SimpleOp : FileIo
	{
		uint32 IrpPtr;
		uint32 TTID;
		uint32 FileObject;
		uint32 FileKey;
	};
	*/
	struct FileIoSimpleOpEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD irp_ptr_offs = 0;
		DWORD ttid_offs = 0;
		DWORD file_object_offs = 0;
		DWORD file_key_offs = 0;

		DWORD irp_ptr_size = 0;
		DWORD ttid_size = 0;
		DWORD file_object_size = 0;
		DWORD file_key_size = 0;
	};

	struct FileIoSimpleOpEventMember
	{
	public:
		size_t irp_ptr = 0;
		size_t ttid = NULL;
		size_t file_object = 0;
		size_t file_key = 0;

		FileIoSimpleOpEventMember() = default;
		FileIoSimpleOpEventMember(const Event& event, FileIoSimpleOpEventOffset* event_offset);
	};

	struct FileIoSimpleOpCleanupEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset event_offset_;
	public:
		FileIoSimpleOpCleanupEvent(const Event& event);
	};

	struct FileIoSimpleOpCloseEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset event_offset_;
	public:
		FileIoSimpleOpCloseEvent(const Event& event);
	};

	struct FileIoSimpleOpFlushEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset event_offset_;
	public:
		FileIoSimpleOpFlushEvent(const Event& event);
	};

};

#endif

#endif