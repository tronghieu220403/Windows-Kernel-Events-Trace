#ifndef ETWSERVICE_ETWSERVICE_MAIN
#define ETWSERVICE_ETWSERVICE_MAIN

/*
C/C++ -> General -> Additional Include Dir -> $(ProjectDir)include
*/
#include "ulti/support.h"
#include "etw/wmieventclass.h"
#include "etw/event.h"
#include "etw/consumer.h"
#include "etw/provider.h"
#include "service/service.h"
#include "manager/manager.h"

void StartEventCollector()
{
    ULONG status;
    etw::KernelProvider* kp = new etw::KernelProvider(
        EVENT_TRACE_FLAG_NO_SYSCONFIG
        //| EVENT_TRACE_FLAG_DISK_IO_INIT | EVENT_TRACE_FLAG_DISK_IO 
        //| EVENT_TRACE_FLAG_FILE_IO_INIT | EVENT_TRACE_FLAG_FILE_IO | EVENT_TRACE_FLAG_DISK_FILE_IO
        //| EVENT_TRACE_FLAG_IMAGE_LOAD
        //| EVENT_TRACE_FLAG_NETWORK_TCPIP
        | EVENT_TRACE_FLAG_PROCESS
        //| EVENT_TRACE_FLAG_REGISTRY
        //| EVENT_TRACE_FLAG_THREAD
        //| EVENT_TRACE_FLAG_VIRTUAL_ALLOC
    );

    status = kp->BeginTrace();
    if (status != ERROR_SUCCESS && status != ERROR_ALREADY_EXISTS)
    {
        PrintDebugW(L"Provider run failed");
        return;
    }

    etw::KernelConsumer* kc = new etw::KernelConsumer();

    if (kc->Open() != ERROR_SUCCESS)
    {
        PrintDebugW(L"Failed to open Kernel Consumer");
        return;
    }

    while (etw::Init() != S_OK)
    {
        PrintDebugW(L"ETW namespace is waiting for connection");
        Sleep(1000);
    }

    if (kc->StartProcessing() != ERROR_SUCCESS)
    {
        PrintDebugW(L"Consummer run failed\n");
        return;
    }
    return;
}

void ServiceMain()
{
    PrintDebugW(L"ServiceMain");
#ifndef _DEBUG
    srv::InitServiceCtrlHandler(SERVICE_NAME);
#endif // _DEBUG

    if (ulti::CreateDir(MAIN_DIR) == false)
    {
        PrintDebugW(L"Create main dir %ws failed", MAIN_DIR);
        return;
    }

    kDriverComm = new DriverComm();
    kDriverComm->Initialize(HIEU_DEVICE_LINK);
    manager::Init();
    std::jthread collector_thread(([]() {
        while (true)
        {
            StartEventCollector();
            Sleep(50);
        }
        }));
    std::jthread manager_thread([]() {
        if (ulti::CreateDir(TEMP_DIR) == false)
        {
            return;
        }
        while (true)
        {
            auto start_time = std::chrono::high_resolution_clock::now();
            manager::EvaluateProcess();
            auto end_time = std::chrono::high_resolution_clock::now();
            DWORD duration = (DWORD)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            Sleep(duration < (DWORD)EVALUATATION_INTERVAL_MS ? EVALUATATION_INTERVAL_MS - duration : 0);
        }
        });
    manager_thread.join();
    collector_thread.join();
    debug::CleanupDebugLog();
}

void RunService()
{
    if (ulti::IsRunningAsSystem() == false)
    {
        ShowWindow(GetConsoleWindow(), SW_HIDE);
        srv::Service service(SERVICE_NAME);
        service.Stop();
        service.Delete();
        std::wstring w_srv_path;
        w_srv_path.resize(1024);
        GetModuleFileNameW(nullptr, &w_srv_path[0], 1024);
        w_srv_path.resize(wcslen(&w_srv_path[0]));
        auto status = service.Create(w_srv_path, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START);
        if (status == ERROR_SUCCESS || status == ERROR_DUPLICATE_SERVICE_NAME || status == ERROR_SERVICE_EXISTS)
        {
            auto status = service.Run();
            if (status != ERROR_SUCCESS)
            {
                PrintDebugW(L"Service run failed %d", status);
            }
        }
    }
    else
    {
        SERVICE_TABLE_ENTRYW service_table[] = {
            { (LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTIONW)ServiceMain },
            { nullptr, nullptr }
        };

        if (!StartServiceCtrlDispatcher(service_table)) {
            PrintDebugW(L"StartServiceCtrlDispatcher failed");
        }
    }

}

void RunProgram()
{
#ifdef _DEBUG
    ServiceMain();
#else
    RunService();
#endif // _DEBUG
}

int main()
{
    RunProgram();

    return 0;
}

#endif

