#ifdef _WIN32

#include "provider.h"

namespace etw
{

    KernelProvider::KernelProvider()
    {
        Initialize();
    }

    KernelProvider::KernelProvider(ULONG flags)
    {
        this->KernelProvider::KernelProvider();
        SetFlags(flags);
    }

    void KernelProvider::Initialize()
    {
        if (session_properties_ == nullptr)
        {
            session_properties_ = (EVENT_TRACE_PROPERTIES*)(new char[buffer_size_]);
        }
        ZeroMemory(session_properties_, buffer_size_);
        session_properties_->EnableFlags = flags_;
        session_properties_->Wnode.BufferSize = buffer_size_;
        session_properties_->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
        session_properties_->Wnode.ClientContext = 2; //System clock resolution
        session_properties_->Wnode.Guid = SystemTraceControlGuid;
        session_properties_->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
        session_properties_->MaximumFileSize = 0;
        session_properties_->LoggerNameOffset = 0;
    }

    void KernelProvider::SetFlags(ULONG flags)
    {
        flags_ = flags;
    }


    TRACEHANDLE KernelProvider::GetSessionHandle() const {
        return session_handle_;
    }

    void KernelProvider::SetSessionHandle(TRACEHANDLE session_handle) {
        session_handle_ = session_handle;
    }

    EVENT_TRACE_PROPERTIES KernelProvider::GetSessionProperties() const {
        EVENT_TRACE_PROPERTIES session_properties;
        memcpy(&session_properties, session_properties_, sizeof(session_properties));
        return session_properties;
    }

    ULONG KernelProvider::BeginTrace()
	{
        ULONG status = ERROR_SUCCESS;

        Initialize();
        status = StartTrace((PTRACEHANDLE)0, KERNEL_LOGGER_NAME, session_properties_);

        if (status == ERROR_ALREADY_EXISTS)
        {
            ControlTrace((TRACEHANDLE)0, KERNEL_LOGGER_NAME, session_properties_, EVENT_TRACE_CONTROL_STOP);
            Initialize();
            status = StartTrace((PTRACEHANDLE)NULL, KERNEL_LOGGER_NAME, session_properties_);
        }

        return status;

	}

    ULONG KernelProvider::CloseTrace()
	{
        ULONG status = ERROR_SUCCESS;
        status = ControlTrace((TRACEHANDLE)0, KERNEL_LOGGER_NAME, session_properties_, EVENT_TRACE_CONTROL_STOP);

        return status;
	}

    KernelProvider::~KernelProvider()
    {
        CloseTrace();
        if (session_properties_ != nullptr)
        {
            free(session_properties_);
            session_properties_ = 0;
        }
    }
}

#endif
