#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_FILE_FILE_H_
#define ETWSERVICE_ETW_ETW_EVENT_FILE_FILE_H_

#include "ulti/debug.h"
#include "etw/event.h"
#include "etw/wmieventclass.h"

//
// Define the I/O status information return values for NtCreateFile/NtOpenFile
//
#define FILE_SUPERSEDED							0x00000000
#define FILE_OPENED								0x00000001
#define FILE_CREATED							0x00000002
#define FILE_OVERWRITTEN						0x00000003
#define FILE_EXISTS								0x00000004
#define FILE_DOES_NOT_EXIST						0x00000005


//
// Define the create/open option flags
//

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_REMOTE_INSTANCE               0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

//
// Define the file information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.
//

typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,                   // 2
	FileBothDirectoryInformation,                   // 3
	FileBasicInformation,                           // 4
	FileStandardInformation,                        // 5
	FileInternalInformation,                        // 6
	FileEaInformation,                              // 7
	FileAccessInformation,                          // 8
	FileNameInformation,                            // 9
	FileRenameInformation,                          // 10
	FileLinkInformation,                            // 11
	FileNamesInformation,                           // 12
	FileDispositionInformation,                     // 13
	FilePositionInformation,                        // 14
	FileFullEaInformation,                          // 15
	FileModeInformation,                            // 16
	FileAlignmentInformation,                       // 17
	FileAllInformation,                             // 18
	FileAllocationInformation,                      // 19
	FileEndOfFileInformation,                       // 20
	FileAlternateNameInformation,                   // 21
	FileStreamInformation,                          // 22
	FilePipeInformation,                            // 23
	FilePipeLocalInformation,                       // 24
	FilePipeRemoteInformation,                      // 25
	FileMailslotQueryInformation,                   // 26
	FileMailslotSetInformation,                     // 27
	FileCompressionInformation,                     // 28
	FileObjectIdInformation,                        // 29
	FileCompletionInformation,                      // 30
	FileMoveClusterInformation,                     // 31
	FileQuotaInformation,                           // 32
	FileReparsePointInformation,                    // 33
	FileNetworkOpenInformation,                     // 34
	FileAttributeTagInformation,                    // 35
	FileTrackingInformation,                        // 36
	FileIdBothDirectoryInformation,                 // 37
	FileIdFullDirectoryInformation,                 // 38
	FileValidDataLengthInformation,                 // 39
	FileShortNameInformation,                       // 40
	FileIoCompletionNotificationInformation,        // 41
	FileIoStatusBlockRangeInformation,              // 42
	FileIoPriorityHintInformation,                  // 43
	FileSfioReserveInformation,                     // 44
	FileSfioVolumeInformation,                      // 45
	FileHardLinkInformation,                        // 46
	FileProcessIdsUsingFileInformation,             // 47
	FileNormalizedNameInformation,                  // 48
	FileNetworkPhysicalNameInformation,             // 49
	FileIdGlobalTxDirectoryInformation,             // 50
	FileIsRemoteDeviceInformation,                  // 51
	FileUnusedInformation,                          // 52
	FileNumaNodeInformation,                        // 53
	FileStandardLinkInformation,                    // 54
	FileRemoteProtocolInformation,                  // 55

	//
	//  These are special versions of these operations (defined earlier)
	//  which can be used by kernel mode drivers only to bypass security
	//  access checks for Rename and HardLink operations.  These operations
	//  are only recognized by the IOManager, a file system should never
	//  receive these.
	//

	FileRenameInformationBypassAccessCheck,         // 56
	FileLinkInformationBypassAccessCheck,           // 57

	//
	// End of special information classes reserved for IOManager.
	//

	FileVolumeNameInformation,                      // 58
	FileIdInformation,                              // 59
	FileIdExtdDirectoryInformation,                 // 60
	FileReplaceCompletionInformation,               // 61
	FileHardLinkFullIdInformation,                  // 62
	FileIdExtdBothDirectoryInformation,             // 63
	FileDispositionInformationEx,                   // 64
	FileRenameInformationEx,                        // 65
	FileRenameInformationExBypassAccessCheck,       // 66
	FileDesiredStorageClassInformation,             // 67
	FileStatInformation,                            // 68
	FileMemoryPartitionInformation,                 // 69
	FileStatLxInformation,                          // 70
	FileCaseSensitiveInformation,                   // 71
	FileLinkInformationEx,                          // 72
	FileLinkInformationExBypassAccessCheck,         // 73
	FileStorageReserveIdInformation,                // 74
	FileCaseSensitiveInformationForceAccessCheck,   // 75
	FileKnownFolderInformation,                     // 76
	FileStatBasicInformation,                       // 77
	FileId64ExtdDirectoryInformation,               // 78
	FileId64ExtdBothDirectoryInformation,           // 79
	FileIdAllExtdDirectoryInformation,              // 80
	FileIdAllExtdBothDirectoryInformation,          // 81

	FileMaximumInformation
} FILE_INFORMATION_CLASS, * PFILE_INFORMATION_CLASS;

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