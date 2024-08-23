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

bool provider_oke = false;
bool comsumer_oke = false;

void SetUpProvider()
{
    ULONG status;
    etw::KernelProvider* kp = new etw::KernelProvider(
        EVENT_TRACE_FLAG_NO_SYSCONFIG
        //| EVENT_TRACE_FLAG_DISK_IO_INIT | EVENT_TRACE_FLAG_DISK_IO | EVENT_TRACE_FLAG_DISK_FILE_IO
        //| EVENT_TRACE_FLAG_FILE_IO_INIT | EVENT_TRACE_FLAG_FILE_IO
        //| EVENT_TRACE_FLAG_IMAGE_LOAD
        //| EVENT_TRACE_FLAG_NETWORK_TCPIP
        //| EVENT_TRACE_FLAG_PROCESS
		//| EVENT_TRACE_FLAG_REGISTRY
        //| EVENT_TRACE_FLAG_THREAD
        //| EVENT_TRACE_FLAG_VIRTUAL_ALLOC
        | EVENT_TRACE_FLAG_SYSTEMCALL 
        );
    status = kp->BeginTrace();
    if (status != ERROR_SUCCESS && status != ERROR_ALREADY_EXISTS)
    {
        return;
    }
    provider_oke = true;
    
    ulti::WriteDebugA("Provider run oke");

	Sleep(1000);

    ulti::WriteDebugA("Provider close");

	kp->CloseTrace();
	return;
}

void SetUpComsumer()
{
    while (provider_oke == false)
    {
        Sleep(50);
    }
    etw::KernelConsumer* kc = new etw::KernelConsumer();
    
    if (kc->Open() != ERROR_SUCCESS)
    {
        ulti::WriteDebugA("Consummer can not be opened");
        return;
    }

    ulti::WriteDebugA("Consummer is opened");

    comsumer_oke = true;

    if (kc->StartProcessing() != ERROR_SUCCESS)
    {
        ulti::WriteDebugA("Consummer run not oke");
    }
    else
    {
        ulti::WriteDebugA("Consummer run oke");
    }

    kc->Close();

    return;
}

void ServiceMainWorker()
{
    std::jthread provider_thread(&SetUpProvider);
    std::jthread comsumer_thread(&SetUpComsumer);
    provider_thread.join();
    comsumer_thread.join();
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

