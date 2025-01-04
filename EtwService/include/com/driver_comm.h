﻿#ifndef DRIVERCOMM_H
#define DRIVERCOMM_H

#include <Windows.h>
#include <string>
#include <vector>

enum IOCTL_CMD_CLASS
{
	kGetImageFromPid = 0x80002000,
	kEnableSelfDefense = 0x80002001,
	kDisableSelfDefense = 0x80002002,
};

struct IOCTL_CMD
{
	IOCTL_CMD_CLASS cmd_class;
	size_t data_len;
	char data[1];
};

#define IOCTL_HIEU CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2204, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HIEU_DEVICE_LINK L"\\\\.\\HieuDeviceLink"

class DriverComm {
public:
	DriverComm();
	DriverComm(const std::wstring& device_link);

	bool Initialize(const std::wstring& deviceLink);

	void Cleanup();

	std::wstring GetProcessImageFromPid(int pid);

private:
	HANDLE device;
	BOOL status;

	std::vector<UCHAR> SendCommand(IOCTL_CMD_CLASS cmdClass, const size_t number);
};

inline DriverComm* kDriverComm = nullptr;

#endif // DRIVERCOMM_H
