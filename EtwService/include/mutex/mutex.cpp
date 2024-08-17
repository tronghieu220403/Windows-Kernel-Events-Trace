#include "mutex.h"

namespace etw
{
    NamedMutex::NamedMutex(const std::string& mutex_name):
        mutex_name_(mutex_name)
    {
        SetMutex(mutex_name);
    }

    void NamedMutex::SetMutex(const std::string& mutex_name)
    {
        Close();
        mutex_name_ = mutex_name;
        _SECURITY_ATTRIBUTES sec;
        sec.nLength = sizeof(_SECURITY_ATTRIBUTES);
        sec.bInheritHandle = TRUE;
        sec.lpSecurityDescriptor = nullptr;
        handle_mutex_ = CreateMutexA(
                            &sec,
                            FALSE, 
                            mutex_name.size() != 0 ? &mutex_name[0] : nullptr
                        );
    }

    std::string NamedMutex::GetMutexName() const
    {
        return mutex_name_;
    }

    void NamedMutex::Lock()
    {
        WaitForSingleObject(handle_mutex_, INT_MAX);
    }

    void NamedMutex::Unlock()
    {
        ReleaseMutex(handle_mutex_);
    }

    void NamedMutex::Close()
    {
        if (handle_mutex_ != nullptr)
        {
            CloseHandle(handle_mutex_);
            handle_mutex_ = 0;
        }
    }

    NamedMutex::~NamedMutex()
    {
        Close();
    }
}
