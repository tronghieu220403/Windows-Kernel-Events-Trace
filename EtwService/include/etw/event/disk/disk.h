#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_DISKIO_DISKIO_H_
#define ETWSERVICE_ETW_ETW_EVENT_DISKIO_DISKIO_H_

#include "etw/event.h"
#include "etw/wmieventclass.h"
#include "ulti/debug.h"

namespace etw
{
	inline struct _DiskIoEventType
	{
		static const int kRead = 10;
		static const int kWrite = 11;
    } DiskIoEventType;

	/*
	[EventType{10,11}, EventTypeName{"Read","Write"}]
	class DiskIo_TypeGroup1 : DiskIo
	{
		uint32 DiskNumber;
		uint32 IrpFlags;
		uint32 TransferSize;
		uint32 Reserved;
		sint64 ByteOffset;
		uint32 FileObject;
		uint32 Irp;
		uint64 HighResResponseTime;
		uint32 IssuingThreadId;
	};	
	*/
	struct DiskIoTypeGroup1EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

        DWORD disk_number_offs = 0;
        DWORD irp_flags_offs = 0;
        DWORD transfer_size_offs = 0;
        DWORD reserved_offs = 0;
        DWORD byte_offset_offs = 0;
		DWORD file_object_offs = 0;
        DWORD irp_offs = 0;
        DWORD high_res_response_time_offs = 0;
		DWORD thread_id_offs = 0;
		
        DWORD disk_number_size = 0;
        DWORD irp_flags_size = 0;
        DWORD transfer_size_size = 0;
        DWORD reserved_size = 0;
        DWORD byte_offset_size = 0;
        DWORD file_object_size = 0;
        DWORD irp_size = 0;
        DWORD high_res_response_time_size = 0;
		DWORD thread_id_size = 0;
	};

	struct DiskIoEventTypeGroup1Member
	{
	public:
        uint64_t disk_number = 0;
        uint64_t irp_flags = 0;
        uint64_t transfer_size = 0;
        uint64_t reserved = 0;
        uint64_t byte_offset = 0;
        uint64_t file_object = 0;
        uint64_t irp = 0;
        uint64_t high_res_response_time = 0;
		uint64_t thread_id = 0;

		DiskIoEventTypeGroup1Member() = default;
		DiskIoEventTypeGroup1Member(const Event& event, DiskIoTypeGroup1EventOffset* event_offset);
	};

	struct DiskIoWriteEvent : DiskIoEventTypeGroup1Member
	{
	private:
		static inline DiskIoTypeGroup1EventOffset offset;
	public:
		DiskIoWriteEvent(const Event& event);
	};

    struct DiskIoReadEvent : DiskIoEventTypeGroup1Member
    {
    private:
        static inline DiskIoTypeGroup1EventOffset offset;
    public:
        DiskIoReadEvent(const Event& event);
    };

}

#endif

#endif
