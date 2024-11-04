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
#include "service/processmonitorservice.h"
#include "service/servicecontrol.h"
#include "service/service.h"
#include "manager/manager.h"

bool provider_started = false;
bool comsumer_started = false;
bool provider_end = false;
bool comsumer_end = false;

void SetUpProvider()
{
    ULONG status;
    etw::KernelProvider* kp = new etw::KernelProvider(
        EVENT_TRACE_FLAG_NO_SYSCONFIG
        //| EVENT_TRACE_FLAG_DISK_IO_INIT | EVENT_TRACE_FLAG_DISK_IO 
        | EVENT_TRACE_FLAG_FILE_IO_INIT | EVENT_TRACE_FLAG_FILE_IO | EVENT_TRACE_FLAG_DISK_FILE_IO
        //| EVENT_TRACE_FLAG_IMAGE_LOAD
        //| EVENT_TRACE_FLAG_NETWORK_TCPIP
        | EVENT_TRACE_FLAG_PROCESS
		//| EVENT_TRACE_FLAG_REGISTRY
        //| EVENT_TRACE_FLAG_THREAD
        | EVENT_TRACE_FLAG_VIRTUAL_ALLOC
        );
    auto start_time = std::chrono::high_resolution_clock::now();

    status = kp->BeginTrace();
    if (status != ERROR_SUCCESS && status != ERROR_ALREADY_EXISTS)
    {
        debug::DebugLogW(L"Provider run failed\n");
        ExitProcess(0);
        return;
    }
    provider_started = true;

    debug::DebugLogW(L"Provider is running");
    Sleep(3000000);
    debug::DebugLogW(L"Provider is closing");
    kp->CloseTrace();
	provider_end = true;
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000;
    debug::DebugLogW(L"Provider is closed after " + std::to_wstring(duration) + L" seconds");
    std::cout << "Provider is closed after " << duration << " seconds" << std::endl;

	return;
}

void SetUpComsumer()
{
    while (provider_started == false)
    {
        Sleep(50);
    }
    etw::KernelConsumer* kc = new etw::KernelConsumer();
    
    if (kc->Open() != ERROR_SUCCESS)
    {
        debug::DebugLogW(L"Consummer can not be opened");
        return;
    }

    debug::DebugLogW(L"Consummer is opened");

    comsumer_started = true;
    auto start_time = std::chrono::high_resolution_clock::now();
    while (etw::Init() != S_OK)
    {
		debug::DebugLogW(L"ETW namespace is waiting for connection");
        Sleep(1000);
    }
	debug::DebugLogW(L"ETW namespace is connected");

    if (kc->StartProcessing() != ERROR_SUCCESS)
    {
        debug::DebugLogW(L"Consummer run failed\n");
    }
    else
    {
        debug::DebugLogW(L"Consummer run is running");
    }

    kc->Close();
	comsumer_end = true;
	etw::CleanUp();
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000;
    debug::DebugLogW(L"Consummer is closed after " + std::to_wstring(duration) + L" seconds");
	std::cout << "Consummer is closed after " << duration << " seconds" << std::endl;
    return;
}

void ServiceMainWorker()
{
    debug::InitDebugLog();
	debug::DebugLogW(L"ServiceMainWorker");
	kDriverComm = new DriverComm();
	kDriverComm->Initialize(HIEU_DEVICE_LINK);
    manager::Init();
    std::jthread provider_thread(&SetUpProvider);
    std::jthread comsumer_thread(&SetUpComsumer);
	std::jthread manager_thread([]() {
        while (provider_started == false || comsumer_started == false)
        {
            Sleep(100);
        }
        while (provider_end == false || comsumer_end == false)
        {
			auto start_time = std::chrono::high_resolution_clock::now();
            manager::EvaluateProcess();
			auto end_time = std::chrono::high_resolution_clock::now();
			DWORD duration = (DWORD)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
			Sleep(10000 - duration);
        }
        });
	manager_thread.join();
    comsumer_thread.join();
    provider_thread.join();
	debug::CleanupDebugLog();
}

void RunService()
{
    etw::ServiceControl service_control(L"Windows Events Trace Service");

    std::wstring w_path;
    w_path.resize(1000);
    GetModuleFileNameW(nullptr, &w_path[0], 1000);
    w_path.resize(wcslen(&w_path[0]));
    service_control.SetPath(w_path);
    if (service_control.Create() == true)
    {
        service_control.Run();
    }
    else
    {
        return;
    }

    etw::ProcessMonitoringService::SetMainWorkerFunction((LPVOID)ServiceMainWorker);
    etw::Service sc;
    sc.SetName(L"Windows Events Trace Service");
    sc.SetServiceMainFunc((LPSERVICE_MAIN_FUNCTION)etw::ProcessMonitoringService::ProcessMonitoringServiceMain);
    sc.Start();
}

void RunProgram()
{
	ServiceMainWorker();
}

int main()
{
	RunProgram();
    
    return 0;
}

#endif

