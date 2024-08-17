#ifdef _WIN32

#include "processmonitorservice.h"

namespace etw
{
    void ProcessMonitoringService::ProcessMonitoringServiceStop()
    {
        WriteDebugA("Get SERVICE_CONTROL_STOP");

        if (service_status_.dwCurrentState != SERVICE_RUNNING)
            return;

        service_status_.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_STOP_PENDING;
        service_status_.dwWin32ExitCode = 1;
        service_status_.dwCheckPoint = 0x00000003;

        if (SetServiceStatus (status_handle_, &service_status_) == FALSE){
            WriteDebugA("Failed to SetServiceStatus SERVICE_STOP_PENDING");
            exit(0);
        }

        // Do something here while pending

        // End of pending

        service_status_.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_STOPPED;
        service_status_.dwWin32ExitCode = 1;
        service_status_.dwCheckPoint = 0x00000001;

        if (SetServiceStatus (status_handle_, &service_status_) == FALSE){
            WriteDebugA("Failed to SetServiceStatus SERVICE_STOPPED after SERVICE_STOP_PENDING");
            exit(0);
        }

        // This will signal the worker thread to start shutting down

    }

    void ProcessMonitoringService::ProcessMonitoringServicePause()
    {
        WriteDebugA("Get SERVICE_CONTROL_PAUSE");

        if (service_status_.dwCurrentState != SERVICE_RUNNING)
            return;

        service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_PAUSE_PENDING;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwCheckPoint = 0x00000006;

        if (SetServiceStatus (status_handle_, &service_status_) == FALSE){
            WriteDebugA("Failed to SetServiceStatus SERVICE_PAUSE_PENDING");
            exit(0);
        }
       
        // Do something here while pending

        // End of pending

        service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE |
            SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_PAUSED;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwCheckPoint = 0x00000007;

        if (SetServiceStatus (status_handle_, &service_status_) == FALSE){
            WriteDebugA("Failed to SetServiceStatus SERVICE_PAUSED after SERVICE_PAUSE_PENDING");
            exit(0);
        }

        // This will signal the worker thread to be paused

    }

    void ProcessMonitoringService::ProcessMonitoringServiceContinue()
    {
        WriteDebugA("Get SERVICE_CONTROL_CONTINUE");

        if (service_status_.dwCurrentState != SERVICE_PAUSED)
            return;
        
        service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_CONTINUE_PENDING;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwCheckPoint = 0x00000005;

        if (SetServiceStatus (status_handle_, &service_status_) == FALSE){
            WriteDebugA("Failed to SetServiceStatus SERVICE_CONTINUE_PENDING");
            exit(0);
        }

        // Do something here while pending

        // End of pending

        service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE |
            SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_RUNNING;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwCheckPoint = 0x00000004;

        if (SetServiceStatus (status_handle_, &service_status_) == FALSE){
            WriteDebugA("Failed to SetServiceStatus SERVICE_RUNNING after SERVICE_CONTINUE_PENDING");
            exit(0);
        }


    }

    void ProcessMonitoringService::ProcessMonitoringServiceShutdown()
    {
        WriteDebugA("Get SERVICE_CONTROL_SHUTDOWN");

        exit(0);
    }


    void ProcessMonitoringService::SetMainWorkerFunction(LPVOID main_worker_function)
    {
        main_worker_function_ = main_worker_function;
    }

    void ProcessMonitoringService::ProcessMonitoringServiceCtrlHandler(DWORD ctrl_code)
    {
        switch (ctrl_code)
        {
            case SERVICE_CONTROL_STOP:
                ProcessMonitoringService::ProcessMonitoringServiceStop();
                break;
        
            case SERVICE_CONTROL_PAUSE:
                ProcessMonitoringService::ProcessMonitoringServicePause();
                break;

            case SERVICE_CONTROL_CONTINUE:
                ProcessMonitoringService::ProcessMonitoringServiceContinue();
                break;

            case SERVICE_CONTROL_SHUTDOWN:
                ProcessMonitoringService::ProcessMonitoringServiceShutdown();
                break;

            default:

                WriteDebugA("Get service control code: " + ctrl_code);
                break;
        }

    }

    void ProcessMonitoringService::ProcessMonitoringWorkerFunction()
    {
        WriteDebugA("In worker");
        // Tell the services that the service is running.
        service_status_.dwCurrentState = SERVICE_RUNNING;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwCheckPoint = 4;

        if (SetServiceStatus(status_handle_, &service_status_) == FALSE)
        {
            return;
        }

        ((void(*)(void))main_worker_function_)();
    }


    VOID WINAPI ProcessMonitoringService::ProcessMonitoringServiceMain()
    {
        WriteDebugA("In main");

        status_handle_ = RegisterServiceCtrlHandler(kName.data(), (LPHANDLER_FUNCTION)ProcessMonitoringService::ProcessMonitoringServiceCtrlHandler);

        if (status_handle_ == NULL)
        {
            WriteDebugA("Can not RegisterServiceCtrlHandler: " + GetLastError());
            goto EXIT;
        }

        ZeroMemory(&service_status_, sizeof(service_status_));
        service_status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE |
            SERVICE_ACCEPT_SHUTDOWN;
        service_status_.dwCurrentState = SERVICE_START_PENDING;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwServiceSpecificExitCode = 0;
        service_status_.dwCheckPoint = 0;

        if (SetServiceStatus (status_handle_ , &service_status_) == FALSE)
        {
            WriteDebugA("Can not SetServiceStatus in line 218: " + GetLastError());
            goto EXIT;
        }

        /*
        Service worker is here
        */
        ProcessMonitoringWorkerFunction();

        // Tell the service controller we are stopped
        service_status_.dwControlsAccepted = 0;
        service_status_.dwCurrentState = SERVICE_STOPPED;
        service_status_.dwWin32ExitCode = 0;
        service_status_.dwCheckPoint = 3;

        if (SetServiceStatus(status_handle_, &service_status_) == FALSE)
        {
            WriteDebugA("Can not SetServiceStatus in line 253: " + GetLastError());
        }

        EXIT:
            Close();
            return;

    }

    void ProcessMonitoringService::Close()
    {
        //control_event_.Close();
        if (status_handle_ != INVALID_HANDLE_VALUE && status_handle_ != nullptr)
        {
            CloseHandle(status_handle_);
        }
        return;
    }


}

#endif