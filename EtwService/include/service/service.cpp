#ifdef _WIN32

#include "service.h"

namespace srv
{
	DWORD Service::Create(const std::wstring& service_path, const DWORD& type, const DWORD& start_type)
	{
		if (h_services_control_manager_ == NULL) {
			debug::DebugPrintW(L"Failed to open Service Control Manager");
			return ERROR_INVALID_HANDLE;
		}
		type_ = type;
		start_type_ = start_type;

		SC_HANDLE handle_service = OpenServiceW(h_services_control_manager_, service_name_.c_str(), SERVICE_QUERY_STATUS);
		if (handle_service != NULL)
		{
			return ERROR_SERVICE_EXISTS;
		}

		service_path_ = service_path;
		handle_service = CreateServiceW(h_services_control_manager_, service_name_.c_str(), service_name_.c_str(),
			SERVICE_ALL_ACCESS, type_,
			start_type_, SERVICE_ERROR_NORMAL,
			service_path_.c_str(), NULL, NULL, NULL, NULL, NULL);

		if (handle_service == NULL)
		{
			ULONG status = GetLastError();
			if (status == ERROR_DUPLICATE_SERVICE_NAME || status == ERROR_SERVICE_EXISTS)
			{
				return status;
			}
			return status;
		}
		CloseServiceHandle(handle_service);

		return ERROR_SUCCESS;
	}

	DWORD Service::Run()
	{
		if (h_services_control_manager_ == NULL) {
			debug::DebugPrintW(L"Failed to open Service Control Manager");
			return ERROR_INVALID_HANDLE;
		}

		SC_HANDLE handle_service;
		std::wstring working_dir;
		working_dir.resize(MAX_PATH + 1);

		GetCurrentDirectoryW(MAX_PATH + 1, &working_dir[0]);
		LPCWSTR argv_start[] = { &service_name_[0], &working_dir[0] };

		handle_service = OpenServiceW(h_services_control_manager_, service_name_.data(), SERVICE_ALL_ACCESS);

		if (handle_service == NULL)
		{
			return GetLastError();
		}

		if (!StartServiceW(handle_service, 2, argv_start))
		{
			return GetLastError();
		}
		CloseServiceHandle(handle_service);
		return ERROR_SUCCESS;
	}

	DWORD Service::Stop()
	{
		if (h_services_control_manager_ == NULL) {
			debug::DebugPrintW(L"Failed to open Service Control Manager");
			return ERROR_INVALID_HANDLE;
		}

		std::string stop_srv_cmd = "sc stop " + ulti::WstrToStr(service_name_);
		std::system(stop_srv_cmd.c_str());

		// Open the service to stop and delete.
		SC_HANDLE service_handle = OpenService(h_services_control_manager_, service_name_.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
		if (service_handle == NULL) {
			debug::DebugPrintW(L"Failed to open service %ws: %d", service_name_.c_str(), GetLastError());
			return GetLastError();
		}

		// Check the status of the service.
		SERVICE_STATUS status;
		if (!QueryServiceStatus(service_handle, &status)) {
			debug::DebugPrintW(L"Failed to query service statu %ws: %d", service_name_.c_str(), GetLastError());
			CloseServiceHandle(service_handle);
			return GetLastError();
		}

		// If the service is running, stop the service.
		if (status.dwCurrentState == SERVICE_RUNNING) {
			if (!ControlService(service_handle, SERVICE_CONTROL_STOP, &status)) {
				debug::DebugPrintW(L"Failed to stop the service %ws: %d", service_name_.c_str(), GetLastError());
				CloseServiceHandle(service_handle);
				return GetLastError();
			}
		}

		CloseServiceHandle(service_handle);
		return ERROR_SUCCESS;
	}

	DWORD Service::Delete()
	{
		if (h_services_control_manager_ == NULL) {
			debug::DebugPrintW(L"Failed to open Service Control Manager");
			return ERROR_INVALID_HANDLE;
		}

		DWORD err = ERROR_SUCCESS;
		std::string delete_srv_cmd = "sc delete " + ulti::WstrToStr(service_name_);
		SC_HANDLE service = OpenService(h_services_control_manager_, service_name_.c_str(), DELETE);
		if (service)
		{
			// Delete the service
			if (DeleteService(service) == FALSE)
			{
				err = GetLastError();
				debug::DebugPrintW(L"DeleteService %ws  failed with error %d, %ws", service_name_.c_str(), err, debug::GetErrorMessage(err));
			}
			else
			{
				// Recursively delete all subkeys
				LONG result = RegDeleteTreeW(HKEY_LOCAL_MACHINE, (std::wstring(L"SYSTEM\\ControlSet001\\Services\\") + service_name_).c_str());
			}

			CloseServiceHandle(service);
		}
		else
		{
			err = GetLastError();
			debug::DebugPrintW(L"OpenService %ws failed with error %d, %ws", service_name_.c_str(), err, debug::GetErrorMessage(err));
		}
		std::system(delete_srv_cmd.c_str());
		return err;
	}

	Service::Service(const std::wstring& name)
		: service_name_(name),
		h_services_control_manager_(OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS))
	{
		if (h_services_control_manager_ == NULL) {
			auto err = GetLastError();
			debug::DebugPrintW(L"Failed to open Service Control Manager: %d, %ws", err, debug::GetErrorMessage(err));
		}
	}

	Service::~Service()
	{
		if (h_services_control_manager_ != NULL)
		{
			CloseServiceHandle(h_services_control_manager_);
		}
		h_services_control_manager_ = 0;
	}
}

#endif