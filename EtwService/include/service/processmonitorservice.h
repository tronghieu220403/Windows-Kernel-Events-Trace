#ifdef _WIN32


#ifndef ETWSERVICE_SERVICE_PROCESSMONITORSERVICE_H_
#define ETWSERVICE_SERVICE_PROCESSMONITORSERVICE_H_

#include "ulti/support.h"
#include "ulti/collections.h"
#include "service/serviceevent.h"

namespace etw
{
    class ProcessMonitoringService
    {
    private:

        inline static SERVICE_STATUS service_status_ = {0};
        inline static SERVICE_STATUS_HANDLE status_handle_ = nullptr;
        inline static LPVOID main_worker_function_ = nullptr;

    public:
        inline static const std::wstring kName = L"Process Monitoring Service";

        static void SetMainWorkerFunction(LPVOID main_worker_function);

        static void ProcessMonitoringServiceCtrlHandler(DWORD ctrl_code);
        static void ProcessMonitoringWorkerFunction();
        static VOID WINAPI ProcessMonitoringServiceMain();

        static void ProcessMonitoringServiceStop();
        static void ProcessMonitoringServicePause();
        static void ProcessMonitoringServiceContinue();
        static void ProcessMonitoringServiceShutdown();

        static void Close();

    };
}

#endif

#endif