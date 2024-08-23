#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_FILE_FILE_H_
#define ETWSERVICE_ETW_ETW_EVENT_FILE_FILE_H_

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
		PVOID irp_ptr = nullptr;
		size_t ttid = NULL;
		PVOID file_object = nullptr;
		size_t create_options = NULL;
		size_t file_attributes = NULL;
		size_t share_access = NULL;
		wchar_t* open_path = nullptr;

		FileIoCreateEventMember() = default;
		FileIoCreateEventMember(const Event& event, FileIoCreateEventOffset* offset);

	};

	struct FileIoCreateEvent: FileIoCreateEventMember
	{
	private:
		static inline FileIoCreateEventOffset offset;
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
		PVOID irp_ptr = nullptr;
		size_t ttid = NULL;
		PVOID file_object = nullptr;
		PVOID file_key = nullptr;
		size_t length = NULL;
		PVOID info_class = nullptr;
		size_t file_index = NULL;
		wchar_t* file_name = nullptr;

		FileIoDirEnumEventMember() = default;
		FileIoDirEnumEventMember(const Event& event, FileIoDirEnumEventOffset* offset);
	};

	struct FileIoDirEnumEvent : FileIoDirEnumEventMember
	{
	private:
		static inline FileIoDirEnumEventOffset offset;
	public:
		FileIoDirEnumEvent(const Event& event);
	};

	struct FileIoDirNotifyEvent : FileIoDirEnumEventMember
	{
	private:
		static inline FileIoDirEnumEventOffset offset;
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
		PVOID irp_ptr = nullptr;
		size_t ttid = NULL;
		PVOID file_object = nullptr;
		PVOID file_key = nullptr;
		size_t extra_info = NULL;
		PVOID info_class = nullptr;

		FileIoInfoEventMember() = default;
		FileIoInfoEventMember(const Event& event, FileIoInfoEventOffset* offset);
	};

	struct FileIoSetInfoEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset;
	public:
		FileIoSetInfoEvent(const Event& event);
	};

	struct FileIoDeleteEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset;
	public:
		FileIoDeleteEvent(const Event& event);
	};

	struct FileIoRenameEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset;
	public:
		FileIoRenameEvent(const Event& event);
	};

	struct FileIoQueryInfoEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset;
	public:
		FileIoQueryInfoEvent(const Event& event);
	};

	struct FileIoFSControlEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset;
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
		PVOID file_object = nullptr;
		wchar_t* file_name = nullptr;

		FileIoNameEventMember() = default;
		FileIoNameEventMember(const Event& event, FileIoNameEventOffset* offset);
	};

	struct FileIoNameEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset;
	public:
		FileIoNameEvent(const Event& event);
	};

	struct FileIoFileCreateEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset;
	public:
		FileIoFileCreateEvent(const Event& event);
	};

	struct FileIoFileDeleteEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset;
	public:
		FileIoFileDeleteEvent(const Event& event);
	};

	struct FileIoFileRundownEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset;
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
		PVOID irp_ptr = nullptr;
		size_t extra_info = NULL;
		size_t nt_status = NULL;

		FileIoOpEndEventMember() = default;
		FileIoOpEndEventMember(const Event& event, FileIoOpEndEventOffset* offset);
	};

	struct FileIoOpEndEvent : FileIoOpEndEventMember
	{
	private:
		static inline FileIoOpEndEventOffset offset;
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
		PVOID irp_ptr = nullptr;
		size_t ttid = NULL;
		PVOID file_object = nullptr;
		PVOID file_key = nullptr;
		size_t io_size = NULL;
		size_t io_flags = NULL;

		FileIoReadWriteEventMember() = default;
		FileIoReadWriteEventMember(const Event& event, FileIoReadWriteEventOffset* offset);
	};

	struct FileIoReadEvent : FileIoReadWriteEventMember
	{
	private:
		static inline FileIoReadWriteEventOffset offset;
	public:
		FileIoReadEvent(const Event& event);
	};

	struct FileIoWriteEvent : FileIoReadWriteEventMember
	{
	private:
		static inline FileIoReadWriteEventOffset offset;
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
		PVOID irp_ptr = nullptr;
		size_t ttid = NULL;
		PVOID file_object = nullptr;
		PVOID file_key = nullptr;

		FileIoSimpleOpEventMember() = default;
		FileIoSimpleOpEventMember(const Event& event, FileIoSimpleOpEventOffset* offset);
	};

	struct FileIoSimpleOpCleanupEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset offset;
	public:
		FileIoSimpleOpCleanupEvent(const Event& event);
	};

	struct FileIoSimpleOpCloseEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset offset;
	public:
		FileIoSimpleOpCloseEvent(const Event& event);
	};

	struct FileIoSimpleOpFlushEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset offset;
	public:
		FileIoSimpleOpFlushEvent(const Event& event);
	};

};

#endif

#endif