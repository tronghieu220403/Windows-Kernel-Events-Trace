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
        debug::DebugPrintW(L"Provider run failed");
        return;
    }
    provider_started = true;
    debug::DebugPrintW(L"Provider is running");

#ifdef _DEBUG
    Sleep(3000000);
    debug::DebugPrintW(L"Provider is closing");
    kp->CloseTrace();
    provider_end = true;
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000;
    debug::DebugPrintW(L"Provider is closed after %d seconds", duration);

#endif // _DEBUG
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
        debug::DebugPrintW(L"Consummer can not be opened");
        return;
    }

    debug::DebugPrintW(L"Consummer is opened");

    comsumer_started = true;
    auto start_time = std::chrono::high_resolution_clock::now();
    while (etw::Init() != S_OK)
    {
		debug::DebugPrintW(L"ETW namespace is waiting for connection");
        Sleep(1000);
    }
	debug::DebugPrintW(L"ETW namespace is connected");

    if (kc->StartProcessing() != ERROR_SUCCESS)
    {
        debug::DebugPrintW(L"Consummer run failed\n");
    }
    else
    {
        debug::DebugPrintW(L"Consummer run is running");
    }

#ifdef _DEBUG
    kc->Close();
    comsumer_end = true;
    etw::CleanUp();
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000;
    debug::DebugPrintW(L"Consummer is closed after %d seconds", duration);
#endif // _DEBUG

    return;
}

void ServiceMain()
{

    debug::DebugPrintW(L"ServiceMain");
    srv::InitServiceCtrlHandler(SERVICE_NAME);

    if (ulti::CreateDir(MAIN_DIR) == false)
	{
		debug::DebugPrintW(L"Create main dir %ws failed", MAIN_DIR);
		return;
	}

	kDriverComm = new DriverComm();
	kDriverComm->Initialize(HIEU_DEVICE_LINK);
    manager::Init();
    std::jthread provider_thread(&SetUpProvider);
    std::jthread comsumer_thread(&SetUpComsumer);
	std::jthread manager_thread([]() {
        if (ulti::CreateDir(TEMP_DIR) == false)
        {
            return;
        }
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
			Sleep(duration < (DWORD)EVALUATATION_INTERVAL_MS ? EVALUATATION_INTERVAL_MS - duration : 0);
        }
        });
	manager_thread.join();
    comsumer_thread.join();
    provider_thread.join();
	debug::CleanupDebugLog();
}

void RunService()
{
	if (ulti::IsRunningAsSystem() == false)
	{
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
				debug::DebugPrintW(L"Service run failed %d", status);
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
			debug::DebugPrintW(L"StartServiceCtrlDispatcher failed");
        }
    }
    
}

void RunProgram()
{
#ifdef _DEBUG
    //ServiceMainWorker();
    RunService();
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

