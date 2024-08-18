#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_FILEIOEVENT_H_
#define ETWSERVICE_ETW_FILEIOEVENT_H_

#include "etw/event.h"
#include "etw/wmieventclass.h"

namespace etw
{
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
		bool is_positioned_ = false;

		DWORD irp_ptr_offs_ = 0;
		DWORD ttid_offs_ = 0;
		DWORD file_object_offs_ = 0;
		DWORD create_options_offs_ = 0;
		DWORD file_attributes_offs_ = 0;
		DWORD share_access_offs_ = 0;
		DWORD open_path_offs_ = 0;

		DWORD irp_ptr_size_ = 0;
		DWORD ttid_size_ = 0;
		DWORD file_object_size_ = 0;
		DWORD create_options_size_ = 0;
		DWORD file_attributes_size_ = 0;
		DWORD share_access_size_ = 0;

	};

	struct FileIoCreateEventMember
	{
	public:
		PVOID IrpPtr = nullptr;
		size_t TTID = NULL;
		PVOID FileObject = nullptr;
		size_t CreateOptions = NULL;
		size_t FileAttributes = NULL;
		size_t ShareAccess = NULL;
		wchar_t* OpenPath = nullptr;

		FileIoCreateEventMember() = default;
		FileIoCreateEventMember(const Event& event, FileIoCreateEventOffset* offset);

	};

	struct FileIoCreateEvent: FileIoCreateEventMember
	{
	private:
		static inline FileIoCreateEventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD irp_ptr_offs_ = 0;
		DWORD ttid_offs_ = 0;
		DWORD file_object_offs_ = 0;
		DWORD file_key_offs_ = 0;
		DWORD length_offs_ = 0;
		DWORD info_class_offs_ = 0;
		DWORD file_index_offs_ = 0;
		DWORD file_name_offs_ = 0;

		DWORD irp_ptr_size_ = 0;
		DWORD ttid_size_ = 0;
		DWORD file_object_size_ = 0;
		DWORD file_key_size_ = 0;
		DWORD length_size_ = 0;
		DWORD info_class_size_ = 0;
		DWORD file_index_size_ = 0;
	};

	struct FileIoDirEnumEventMember
	{
	public:
		PVOID IrpPtr = nullptr;
		size_t TTID = NULL;
		PVOID FileObject = nullptr;
		PVOID FileKey = nullptr;
		size_t Length = NULL;
		PVOID InfoClass = nullptr;
		size_t FileIndex = NULL;
		wchar_t* FileName = nullptr;

		FileIoDirEnumEventMember() = default;
		FileIoDirEnumEventMember(const Event& event, FileIoDirEnumEventOffset* offset);
	};

	struct FileIoDirEnumEvent : FileIoDirEnumEventMember
	{
	private:
		static inline FileIoDirEnumEventOffset offset_;
	public:
		FileIoDirEnumEvent(const Event& event);
	};

	struct FileIoDirNotifyEvent : FileIoDirEnumEventMember
	{
	private:
		static inline FileIoDirEnumEventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD irp_ptr_offs_ = 0;
		DWORD ttid_offs_ = 0;
		DWORD file_object_offs_ = 0;
		DWORD file_key_offs_ = 0;
		DWORD extra_info_offs_ = 0;
		DWORD info_class_offs_ = 0;

		DWORD irp_ptr_size_ = 0;
		DWORD ttid_size_ = 0;
		DWORD file_object_size_ = 0;
		DWORD file_key_size_ = 0;
		DWORD extra_info_size_ = 0;
		DWORD info_class_size_ = 0;
	};

	struct FileIoInfoEventMember
	{
	public:
		PVOID IrpPtr = nullptr;
		size_t TTID = NULL;
		PVOID FileObject = nullptr;
		PVOID FileKey = nullptr;
		size_t ExtraInfo = NULL;
		PVOID InfoClass = nullptr;

		FileIoInfoEventMember() = default;
		FileIoInfoEventMember(const Event& event, FileIoInfoEventOffset* offset);
	};

	struct FileIoSetInfoEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset_;
	public:
		FileIoSetInfoEvent(const Event& event);
	};

	struct FileIoDeleteEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset_;
	public:
		FileIoDeleteEvent(const Event& event);
	};

	struct FileIoRenameEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset_;
	public:
		FileIoRenameEvent(const Event& event);
	};

	struct FileIoQueryInfoEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset_;
	public:
		FileIoQueryInfoEvent(const Event& event);
	};

	struct FileIoFSControlEvent : FileIoInfoEventMember
	{
	private:
		static inline FileIoInfoEventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD file_object_offs_ = 0;
		DWORD file_name_offs_ = 0;
	};

	struct FileIoNameEventMember
	{
	public:
		PVOID FileObject = nullptr;
		wchar_t* FileName = nullptr;

		FileIoNameEventMember() = default;
		FileIoNameEventMember(const Event& event, FileIoNameEventOffset* offset);
	};

	struct FileIoNameEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset_;
	public:
		FileIoNameEvent(const Event& event);
	};

	struct FileCreateEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset_;
	public:
		FileCreateEvent(const Event& event);
	};

	struct FileDeleteEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset_;
	public:
		FileDeleteEvent(const Event& event);
	};

	struct FileRundownEvent : FileIoNameEventMember
	{
	private:
		static inline FileIoNameEventOffset offset_;
	public:
		FileRundownEvent(const Event& event);
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
		bool is_positioned_ = false;

		DWORD irp_ptr_offs_ = 0;
		DWORD extra_info_offs_ = 0;
		DWORD nt_status_offs_ = 0;
	};

	struct FileIoOpEndEventMember
	{
	public:
		PVOID IrpPtr = nullptr;
		size_t ExtraInfo = NULL;
		size_t NtStatus = NULL;

		FileIoOpEndEventMember() = default;
		FileIoOpEndEventMember(const Event& event, FileIoOpEndEventOffset* offset);
	};

	struct FileIoOpEndEvent : FileIoOpEndEventMember
	{
	private:
		static inline FileIoOpEndEventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD offset_offs_ = 0;
		DWORD irp_ptr_offs_ = 0;
		DWORD ttid_offs_ = 0;
		DWORD file_object_offs_ = 0;
		DWORD file_key_offs_ = 0;
		DWORD io_size_offs_ = 0;
		DWORD io_flags_offs_ = 0;
	};

	struct FileIoReadWriteEventMember
	{
	public:
		uint64_t Offset = 0;
		PVOID IrpPtr = nullptr;
		size_t TTID = NULL;
		PVOID FileObject = nullptr;
		PVOID FileKey = nullptr;
		size_t IoSize = NULL;
		size_t IoFlags = NULL;

		FileIoReadWriteEventMember() = default;
		FileIoReadWriteEventMember(const Event& event, FileIoReadWriteEventOffset* offset);
	};

	struct FileIoReadEvent : FileIoReadWriteEventMember
	{
	private:
		static inline FileIoReadWriteEventOffset offset_;
	public:
		FileIoReadEvent(const Event& event);
	};

	struct FileIoWriteEvent : FileIoReadWriteEventMember
	{
	private:
		static inline FileIoReadWriteEventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD irp_ptr_offs_ = 0;
		DWORD ttid_offs_ = 0;
		DWORD file_object_offs_ = 0;
		DWORD file_key_offs_ = 0;
	};

	struct FileIoSimpleOpEventMember
	{
	public:
		PVOID IrpPtr = nullptr;
		size_t TTID = NULL;
		PVOID FileObject = nullptr;
		PVOID FileKey = nullptr;

		FileIoSimpleOpEventMember() = default;
		FileIoSimpleOpEventMember(const Event& event, FileIoSimpleOpEventOffset* offset);
	};

	struct FileIoCleanupEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset offset_;
	public:
		FileIoCleanupEvent(const Event& event);
	};

	struct FileIoCloseEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset offset_;
	public:
		FileIoCloseEvent(const Event& event);
	};

	struct FileIoFlushEvent : FileIoSimpleOpEventMember
	{
	private:
		static inline FileIoSimpleOpEventOffset offset_;
	public:
		FileIoFlushEvent(const Event& event);
	};

};

#endif

#endif