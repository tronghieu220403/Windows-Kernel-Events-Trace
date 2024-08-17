#ifdef _WIN32

#ifndef ETWSERVICE_SERVICE_SERVICE_H_
#define ETWSERVICE_SERVICE_SERVICE_H_

#include "ulti/support.h"
#include "ulti/collections.h"

namespace etw
{
    class Service
    {
    private:
        std::wstring name_;
        LPSERVICE_MAIN_FUNCTION service_main_func_ = nullptr;

    public:

        LPSERVICE_MAIN_FUNCTION GetServiceMainFunc() const;
        void SetServiceMainFunc(LPSERVICE_MAIN_FUNCTION main_func);

        std::wstring GetName() const;
        void SetName(std::wstring name);

        void Start();
    };
}

#endif

#endif