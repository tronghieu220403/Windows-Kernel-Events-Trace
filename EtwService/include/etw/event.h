#ifdef _WIN32


#pragma once

#ifndef ETWSERVICE_ETW_EVENT_H_
#define ETWSERVICE_ETW_EVENT_H_

#include "ulti/support.h"

#define INITGUID
DEFINE_GUID( /* 3d6fa8d4-fe05-11d0-9dda-00c04fd7ba7c */    DiskIoGuid, 0x3d6fa8d4, 0xfe05, 0x11d0, 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c);
DEFINE_GUID( /* 90cbdc39-4a3e-11d1-84f4-0000f80464e3 */    FileIoGuid, 0x90cbdc39, 0x4a3e, 0x11d1, 0x84, 0xf4, 0x00, 0x00, 0xf8, 0x04, 0x64, 0xe3);
DEFINE_GUID( /* 2cb15d1d-5fc1-11d2-abe1-00a0c911f518 */    ImageLoadGuid, 0x2cb15d1d, 0x5fc1, 0x11d2, 0xab, 0xe1, 0x00, 0xa0, 0xc9, 0x11, 0xf5, 0x18);
DEFINE_GUID( /* 3d6fa8d0-fe05-11d0-9dda-00c04fd7ba7c */    ProcessGuid, 0x3d6fa8d0, 0xfe05, 0x11d0, 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c);
DEFINE_GUID( /* AE53722E-C863-11d2-8659-00C04FA321A1 */    RegistryGuid, 0xae53722e, 0xc863, 0x11d2, 0x86, 0x59, 0x0, 0xc0, 0x4f, 0xa3, 0x21, 0xa1);
DEFINE_GUID( /* 9a280ac0-c8e0-11d1-84e2-00c04fb998a2 */    TcpIpGuid, 0x9a280ac0, 0xc8e0, 0x11d1, 0x84, 0xe2, 0x00, 0xc0, 0x4f, 0xb9, 0x98, 0xa2);
DEFINE_GUID( /* 3d6fa8d1-fe05-11d0-9dda-00c04fd7ba7c */    ThreadGuid, 0x3d6fa8d1, 0xfe05, 0x11d0, 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c);
DEFINE_GUID( /* bf3a50c5-a9c9-4988-a005-2df0b7c80f80 */    UdpIpGuid, 0xbf3a50c5, 0xa9c9, 0x4988, 0xa0, 0x05, 0x2d, 0xf0, 0xb7, 0xc8, 0x0f, 0x80);
DEFINE_GUID( /* 3d6fa8d3-fe05-11d0-9dda-00c04fd7ba7c */    PageFaultGuid, 0x3d6fa8d3, 0xfe05, 0x11d0, 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c);
DEFINE_GUID( /* ce1dbfb4-137e-4da6-87b0-3f59aa102cbc */    PerfInfoGuid, 0xce1dbfb4, 0x137e, 0x4da6, 0x87, 0xb0, 0x3f, 0x59, 0xaa, 0x10, 0x2c, 0xbc);


struct EventGuid
{
public:
	inline static const std::wstring kDiskIo = L"{3D6FA8D4-FE05-11D0-9DDA-00C04FD7BA7C}";
	inline static const std::wstring kFileIo = L"{90CBDC39-4A3E-11D1-84F4-0000F80464E3}";
	inline static const std::wstring kImageLoad = L"{2CB15D1D-5FC1-11D2-ABE1-00A0C911F518}";
	inline static const std::wstring kProcess = L"{3D6FA8D0-FE05-11D0-9DDA-00C04FD7BA7C}";
	inline static const std::wstring kRegistry = L"{AE53722E-C863-11d2-8659-00C04FA321A1}";
	inline static const std::wstring kTcpip = L"{9A280AC0-C8E0-11D1-84E2-00C04FB998A2}";
	inline static const std::wstring kThread = L"{3D6FA8D1-FE05-11D0-9DDA-00C04FD7BA7C}";
	inline static const std::wstring kUdpip = L"{BF3A50C5-A9C9-4988-A005-2DF0B7C80F80}";
	inline static const std::wstring kPageFault = L"{3D6FA8D3-FE05-11D0-9DDA-00C04FD7BA7C}";
	inline static const std::wstring kPerfInfo = L"{CE1DBFB4-137E-4DA6-87B0-3F59AA102CBC}";
};


namespace etw
{

	class Event
	{
	private:
		PEVENT_TRACE p_event_ = NULL;
	public:
		Event() = default;
		Event(PEVENT_TRACE p_event);

		PEVENT_TRACE GetPEvent() const;

		GUID GetGuid() const;
		int GetVersion() const;
		int GetType() const;

		FILETIME GetFileTime() const;
		ULONGLONG GetTimeInMs() const;
		ULONGLONG GetTimeInMicroSec() const;

		ULONG GetMofLength() const;

		PBYTE GetPEventData() const;

		size_t GetThreadId() const;

		size_t GetProcessId() const;
	};

};

#endif

#endif