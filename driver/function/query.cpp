#include "query.h"

namespace query
{
	void DrvRegister()
	{
		kPidMapMutex = new Mutex();
		kPidToImageName = new Map<size_t, String<WCHAR>>();
		NTSTATUS status = PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&ProcessNotifyCallBack, FALSE);
		if (STATUS_SUCCESS != status)
		{
			DebugMessage("Fail to register PsSetCreateProcessNotifyRoutineEx in %s, line %d, status %x", __FILE__, __LINE__, status);
		}
	}

	void DrvUnload()
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&ProcessNotifyCallBack, TRUE);
		delete kPidToImageName;
		kPidToImageName = nullptr;
		delete kPidMapMutex;
		kPidMapMutex = nullptr;
	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		if (create_info) // Process creation
		{
			String<WCHAR> process_image_name(GetProcessImageName(pid));
			if (process_image_name.Size() > 0)
			{
				kPidMapMutex->Lock();
				kPidToImageName->Insert(pid, process_image_name);
				kPidMapMutex->Unlock();
			}
		}
		else // Process termination
		{

		}
	}

	String<WCHAR> GetProcessImageName(size_t pid)
	{
		String<WCHAR> process_image_name;
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		PEPROCESS eproc;
		status = PsLookupProcessByProcessId((HANDLE)pid, &eproc);
		if (!NT_SUCCESS(status))
		{
			return String<WCHAR>();
		}

		PUNICODE_STRING process_name = NULL;
		status = SeLocateProcessImageName(eproc, &process_name);
		if (status == STATUS_SUCCESS)
		{
			process_image_name = process_name;
			delete process_name;
			ObDereferenceObject(eproc);
			return process_image_name;
		}

		ObDereferenceObject(eproc);

		ULONG returned_length = 0;
		HANDLE h_process = NULL;
		Vector<UCHAR> buffer;

		if (ZwQueryInformationProcess == NULL)
		{
			DebugMessage("Cannot resolve ZwQueryInformationProcess\n");
			status = STATUS_UNSUCCESSFUL;
			goto cleanUp;
		}

		status = ObOpenObjectByPointer(eproc,
			0, NULL, 0, 0, KernelMode, &h_process);
		if (!NT_SUCCESS(status))
		{
			DebugMessage("ObOpenObjectByPointer Failed: %08x\n", status);
			return String<WCHAR>();
		}

		if (ZwQueryInformationProcess == NULL)
		{
			DebugMessage("Cannot resolve ZwQueryInformationProcess\n");
			status = STATUS_UNSUCCESSFUL;
			goto cleanUp;
		}

		// Query the actual size of the process path 
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			NULL, // buffer
			0,    // buffer size
			&returned_length);

		if (STATUS_INFO_LENGTH_MISMATCH != status) {
			DebugMessage("ZwQueryInformationProcess status = %x\n", status);
			goto cleanUp;
		}

		buffer.Resize(returned_length);

		if (buffer.Data() == NULL)
		{
			goto cleanUp;
		}

		// Retrieve the process path from the handle to the process
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			buffer.Data(),
			returned_length,
			&returned_length);
		process_image_name = (PUNICODE_STRING)buffer.Data();

	cleanUp:
		ZwClose(h_process);

		return process_image_name;
	}
}