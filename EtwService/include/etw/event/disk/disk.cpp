#include "disk.h"

namespace etw
{
	DiskIoEventTypeGroup1Member::DiskIoEventTypeGroup1Member(const Event& event, DiskIoTypeGroup1EventOffset* event_offset)
	{
		if (event_offset->is_positioned == false)
		{
			WmiEventClass wec(EventGuid::kDiskIo, event.GetVersion(), event.GetType(), sizeof(PVOID));
			std::pair<int, int> p;

			p = wec.GetPropertyInfo(L"DiskNumber", event);
			event_offset->disk_number_offs = p.first;
			event_offset->disk_number_size = p.second;

            p = wec.GetPropertyInfo(L"IrpFlags", event);
            event_offset->irp_flags_offs = p.first;
            event_offset->irp_flags_size = p.second;

            p = wec.GetPropertyInfo(L"TransferSize", event);
            event_offset->transfer_size_offs = p.first;
            event_offset->transfer_size_size = p.second;

            p = wec.GetPropertyInfo(L"Reserved", event);
            event_offset->reserved_offs = p.first;
            event_offset->reserved_size = p.second;

            p = wec.GetPropertyInfo(L"ByteOffset", event);
            event_offset->byte_offset_offs = p.first;
            event_offset->byte_offset_size = p.second;

            p = wec.GetPropertyInfo(L"FileObject", event);
            event_offset->file_object_offs = p.first;
            event_offset->file_object_size = p.second;

            p = wec.GetPropertyInfo(L"Irp", event);
            event_offset->irp_offs = p.first;
            event_offset->irp_size = p.second;

            p = wec.GetPropertyInfo(L"HighResResponseTime", event);
            event_offset->high_res_response_time_offs = p.first;
            event_offset->high_res_response_time_size = p.second;

            p = wec.GetPropertyInfo(L"IssuingThreadId", event);
            event_offset->thread_id_offs = p.first;
            event_offset->thread_id_size = p.second;

			if (0 == event_offset->disk_number_size ||
                0 == event_offset->irp_flags_size ||
                0 == event_offset->transfer_size_size ||
                0 == event_offset->reserved_size ||
                0 == event_offset->byte_offset_size ||
                0 == event_offset->file_object_size ||
                0 == event_offset->irp_size ||
                0 == event_offset->high_res_response_time_size ||
                0 == event_offset->thread_id_size
                )
			{
                PrintDebugW(L"Error in GetPropertyInfo");
                event_offset->is_successful = false;
                return;
			}

            event_offset->is_successful = true;
			event_offset->is_positioned = true;
		}

		PBYTE p_data = event.GetPEventData();
        memcpy(&disk_number, p_data + event_offset->disk_number_offs, event_offset->disk_number_size);
        memcpy(&irp_flags, p_data + event_offset->irp_flags_offs, event_offset->irp_flags_size);
        memcpy(&transfer_size, p_data + event_offset->transfer_size_offs, event_offset->transfer_size_size);
        memcpy(&reserved, p_data + event_offset->reserved_offs, event_offset->reserved_size);
        memcpy(&byte_offset, p_data + event_offset->byte_offset_offs, event_offset->byte_offset_size);
        memcpy(&file_object, p_data + event_offset->file_object_offs, event_offset->file_object_size);
        memcpy(&irp, p_data + event_offset->irp_offs, event_offset->irp_size);
        memcpy(&high_res_response_time, p_data + event_offset->high_res_response_time_offs, event_offset->high_res_response_time_size);
        memcpy(&thread_id, p_data + event_offset->thread_id_offs, event_offset->thread_id_size);
	}

	DiskIoWriteEvent::DiskIoWriteEvent(const Event& event)
	{
		this->DiskIoEventTypeGroup1Member::DiskIoEventTypeGroup1Member(event, &offset);
	}

    DiskIoReadEvent::DiskIoReadEvent(const Event& event)
    {
        this->DiskIoEventTypeGroup1Member::DiskIoEventTypeGroup1Member(event, &offset);
    }
}
