
#ifndef ETWSERVICE_MUTEX_MUTEX_H_
#define ETWSERVICE_MUTEX_MUTEX_H_

#include "ulti/support.h"

namespace etw
{
    class NamedMutex
    {
    private:
        std::string mutex_name_;

        HANDLE handle_mutex_ = nullptr;
    public:
        NamedMutex() = default;
        explicit NamedMutex(const std::string& mutex_name);
        
        void SetMutex(const std::string& mutex_name);

        std::string GetMutexName() const;

        void Lock();
        void Unlock();

        void Close();

        ~NamedMutex();
    };

}

#endif