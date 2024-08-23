#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_ETW_EVENT_NETWORK_NETWORK_H_
#define ETWSERVICE_ETW_ETW_EVENT_NETWORK_NETWORK_H_

#include "etw/event.h"

namespace etw
{
	enum NetworkEventType
	{
		// TCP IP
		kTcpIpFail = 17,
		// IPv4
		kTcpIpSendIPV4 = 10,
		kTcpIpRecvIPV4 = 11,
		kTcpIpDisconnectIPV4 = 13,
		kTcpIpRetransmitIPV4 = 14,
		kTcpIpReconnectIPV4 = 16,
		kTcpIpTCPCopyIPV4 = 18,
		KTcpIpConnectIPV4 = 12,
		KTcpIpAcceptIPV4 = 15,
		// IPv6
		kTcpIpSendIPV6 = 26,
		kTcpIpRecvIPV6 = 27,
		kTcpIpDisconnectIPV6 = 29,
		kTcpIpRetransmitIPV6 = 30,
		kTcpIpReconnectIPV6 = 32,
		kTcpIpTCPCopyIPV6 = 34,
		KTcpIpConnectIPV6 = 28,
		KTcpIpAcceptIPV6 = 31,

		// UDP IP
		kUdpIpFail = 17,
		// IPv4
		kUdpIpSendIPV4 = 10,
		kUdpIpRecvIPV4 = 11,
		// IPv6
		kUdpIpSendIPV6 = 26,
		kUdpIpRecvIPV6 = 27
	};

	/*-------------TCP IP-------------*/
	/*
	[EventType{17}, EventTypeName{"Fail"}]
	class TcpIp_Fail : TcpIp
	{
	  uint16 Proto;
	  uint16 FailureCode;
	};
	*/
	struct TcpIpFailEventOffset
	{
		bool is_positioned = false;

		DWORD proto_offs = 0;
		DWORD failure_code_offs = 0;
	};

	struct TcpIpFailEventMember
	{
	public:
		size_t Proto = 0;
		size_t FailureCode = 0;

		TcpIpFailEventMember() = default;
		TcpIpFailEventMember(const Event& event, TcpIpFailEventOffset* offset);
	};

	struct TcpIpFailEvent : TcpIpFailEventMember
	{
	private:
		static inline TcpIpFailEventOffset offset;
	public:
		TcpIpFailEvent(const Event& event);
	};

	/*
	[EventType{ 10, 26 }, EventTypeName{ "SendIPV4" , "SendIPV6" }]
	class TcpIp_SendIPV4 : TcpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint32 startime;
		uint32 endtime;
		uint32 seqnum;
		uint32 connid;
	};
	*/
	struct TcpIpSendIPV4EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD pid_offs = 0;
		DWORD size_offs = 0;
		DWORD daddr_offs = 0;
		DWORD saddr_offs = 0;
		DWORD dport_offs = 0;
		DWORD sport_offs = 0;
		DWORD startime_offs = 0;
		DWORD endtime_offs = 0;
		DWORD seqnum_offs = 0;
		DWORD connid_offs = 0;

		DWORD pid_size = 0;
		DWORD size_size = 0;
		DWORD daddr_size = 0;
		DWORD saddr_size = 0;
		DWORD dport_size = 0;
		DWORD sport_size = 0;
		DWORD startime_size = 0;
		DWORD endtime_size = 0;
		DWORD seqnum_size = 0;
		DWORD connid_size = 0;
	};

	struct TcpIpSendIPV4EventMember
	{
	public:
		size_t pid = 0;
		size_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		size_t startime = 0;
		size_t endtime = 0;
		size_t seqnum = 0;
		size_t connid = 0;

		TcpIpSendIPV4EventMember() = default;
		TcpIpSendIPV4EventMember(const Event& event, TcpIpSendIPV4EventOffset* offset);
	};

	struct TcpIpSendIPV4Event : TcpIpSendIPV4EventMember
	{
	private:
		static inline TcpIpSendIPV4EventOffset offset;
	public:
		TcpIpSendIPV4Event(const Event& event);
	};

	struct TcpIpSendIPV6Event : TcpIpSendIPV4EventMember
	{
	private:
		static inline TcpIpSendIPV4EventOffset offset;
	public:
		TcpIpSendIPV6Event(const Event& event);
	};

	/*
	* TcpIp_TypeGroup1 and TcpIp_TypeGroup3 are the same so we can use the same class for both
	[EventType{11, 13, 14, 16, 18}, EventTypeName{"RecvIPV4", "DisconnectIPV4", "RetransmitIPV4", "ReconnectIPV4", "TCPCopyIPV4"}]
	class TcpIp_TypeGroup1 : TcpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint32 seqnum;
		uint32 connid;
	};
	[EventType{27, 29, 30, 32, 34}, EventTypeName{"RecvIPV6", "DisconnectIPV6", "RetransmitIPV6", "ReconnectIPV6", "TCPCopyIPV6"}]
	class TcpIp_TypeGroup3 : TcpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint32 seqnum;
		uint32 connid;
	};
	*/
	struct TcpIpTypeGroup1EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD pid_offs = 0;
		DWORD size_offs = 0;
		DWORD daddr_offs = 0;
		DWORD saddr_offs = 0;
		DWORD dport_offs = 0;
		DWORD sport_offs = 0;
		DWORD seqnum_offs = 0;
		DWORD connid_offs = 0;

		DWORD pid_size = 0;
		DWORD size_size = 0;
		DWORD daddr_size = 0;
		DWORD saddr_size = 0;
		DWORD dport_size = 0;
		DWORD sport_size = 0;
		DWORD seqnum_size = 0;
		DWORD connid_size = 0;
	};

	struct TcpIpTypeGroup1EventMember
	{
	public:
		size_t pid = 0;
		size_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		size_t seqnum = 0;
		size_t connid = 0;

		TcpIpTypeGroup1EventMember() = default;
		TcpIpTypeGroup1EventMember(const Event& event, TcpIpTypeGroup1EventOffset* offset);
	};

	struct TcpIpRecvIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpRecvIPV4Event(const Event& event);
	};

	struct TcpIpDisconnectIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpDisconnectIPV4Event(const Event& event);
	};

	struct TcpIpRetransmitIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpRetransmitIPV4Event(const Event& event);
	};

	struct TcpIpReconnectIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpReconnectIPV4Event(const Event& event);
	};

	struct TcpIpTCPCopyIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpTCPCopyIPV4Event(const Event& event);
	};

	struct TcpIpRecvIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpRecvIPV6Event(const Event& event);
	};

	struct TcpIpDisconnectIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpDisconnectIPV6Event(const Event& event);
	};

	struct TcpIpRetransmitIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpRetransmitIPV6Event(const Event& event);
	};

	struct TcpIpReconnectIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpReconnectIPV6Event(const Event& event);
	};

	struct TcpIpTCPCopyIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset;
	public:
		TcpIpTCPCopyIPV6Event(const Event& event);
	};

	/*
	* TcpIp_TypeGroup2 and TcpIp_TypeGroup4 are the same so we can use the same class for both
	[EventType{12, 15}, EventTypeName{"ConnectIPV4", "AcceptIPV4"}]
	class TcpIp_TypeGroup2 : TcpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint16 mss;
		uint16 sackopt;
		uint16 tsopt;
		uint16 wsopt;
		uint32 rcvwin;
		sint16 rcvwinscale;
		sint16 sndwinscale;
		uint32 seqnum;
		uint32 connid;
	};
	[EventType{28, 31}, EventTypeName{"ConnectIPV6", "AcceptIPV6"}]
	class TcpIp_TypeGroup4 : TcpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint16 mss;
		uint16 sackopt;
		uint16 tsopt;
		uint16 wsopt;
		uint32 rcvwin;
		sint16 rcvwinscale;
		sint16 sndwinscale;
		uint32 seqnum;
		uint32 connid;
	};
	*/
	struct TcpIpTypeGroup2EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD pid_offs = 0;
		DWORD size_offs = 0;
		DWORD daddr_offs = 0;
		DWORD saddr_offs = 0;
		DWORD dport_offs = 0;
		DWORD sport_offs = 0;
		DWORD mss_offs = 0;
		DWORD sackopt_offs = 0;
		DWORD tsopt_offs = 0;
		DWORD wsopt_offs = 0;
		DWORD rcvwin_offs = 0;
		DWORD rcvwinscale_offs = 0;
		DWORD sndwinscale_offs = 0;
		DWORD seqnum_offs = 0;
		DWORD connid_offs = 0;

		DWORD pid_size = 0;
		DWORD size_size = 0;
		DWORD daddr_size = 0;
		DWORD saddr_size = 0;
		DWORD dport_size = 0;
		DWORD sport_size = 0;
		DWORD mss_size = 0;
		DWORD sackopt_size = 0;
		DWORD tsopt_size = 0;
		DWORD wsopt_size = 0;
		DWORD rcvwin_size = 0;
		DWORD rcvwinscale_size = 0;
		DWORD sndwinscale_size = 0;
		DWORD seqnum_size = 0;
		DWORD connid_size = 0;
	};

	struct TcpIpTypeGroup2EventMember
	{
	public:
		size_t pid = 0;
		size_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		size_t mss = 0;
		size_t sackopt = 0;
		size_t tsopt = 0;
		size_t wsopt = 0;
		size_t rcvwin = 0;
		size_t rcvwinscale = 0;
		size_t sndwinscale = 0;
		size_t seqnum = 0;
		size_t connid = 0;

		TcpIpTypeGroup2EventMember() = default;
		TcpIpTypeGroup2EventMember(const Event& event, TcpIpTypeGroup2EventOffset* offset);
	};

	struct TcpIpConnectIPV4Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset;
	public:
		TcpIpConnectIPV4Event(const Event& event);
	};

	struct TcpIpAcceptIPV4Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset;
	public:
		TcpIpAcceptIPV4Event(const Event& event);
	};

	struct TcpIpConnectIPV6Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset;
	public:
		TcpIpConnectIPV6Event(const Event& event);
	};

	struct TcpIpAcceptIPV6Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset;
	public:
		TcpIpAcceptIPV6Event(const Event& event);
	};

	/*--------------------------------*/



	/*-------------UDP IP-------------*/
	
	/*
	[EventType{17}, EventTypeName{"Fail"}]
	class UdpIp_Fail : UdpIp
	{
		uint16 Proto;
		uint16 FailureCode;
	};
	*/
	struct UdpIpFailEventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD proto_offs = 0;
		DWORD failure_code_offs = 0;

		DWORD proto_size = 0;
		DWORD failure_code_size = 0;
	};

	struct UdpIpFailEventMember
	{
	public:
		size_t Proto = 0;
		size_t FailureCode = 0;

		UdpIpFailEventMember() = default;
		UdpIpFailEventMember(const Event& event, UdpIpFailEventOffset* offset);
	};

	struct UdpIpFailEvent : UdpIpFailEventMember
	{
	private:
		static inline UdpIpFailEventOffset offset;
	public:
		UdpIpFailEvent(const Event& event);
	};

	/*
	* UdpIp_TypeGroup1 and UdpIp_TypeGroup2 are the same so we can use the same class for both
	[EventType{10, 11}, EventTypeName{"SendIPV4", "RecvIPV4"}]
	class UdpIp_TypeGroup1 : UdpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint32 seqnum;
		uint32 connid;
	};
	[EventType{26, 27}, EventTypeName{"SendIPV6", "RecvIPV6"}]
	class UdpIp_TypeGroup2 : UdpIp
	{
		uint32 pid;
		uint32 size;
		object daddr;
		object saddr;
		object dport;
		object sport;
		uint32 seqnum;
		uint32 connid;
	};
	*/
	struct UdpIpTypeGroup1EventOffset
	{
		bool is_positioned = false;
		bool is_successful = false;

		DWORD pid_offs = 0;
		DWORD size_offs = 0;
		DWORD daddr_offs = 0;
		DWORD saddr_offs = 0;
		DWORD dport_offs = 0;
		DWORD sport_offs = 0;
		DWORD seqnum_offs = 0;
		DWORD connid_offs = 0;

		DWORD pid_size = 0;
		DWORD size_size = 0;
		DWORD daddr_size = 0;
		DWORD saddr_size = 0;
		DWORD dport_size = 0;
		DWORD sport_size = 0;
		DWORD seqnum_size = 0;
		DWORD connid_size = 0;
	};

	struct UdpIpTypeGroup1EventMember
	{
	public:
		size_t pid = 0;
		size_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		size_t seqnum = 0;
		size_t connid = 0;

		UdpIpTypeGroup1EventMember() = default;
		UdpIpTypeGroup1EventMember(const Event& event, UdpIpTypeGroup1EventOffset* offset);
	};

	struct UdpIpSendIPV4Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset;
	public:
		UdpIpSendIPV4Event(const Event& event);
	};

	struct UdpIpRecvIPV4Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset;
	public:
		UdpIpRecvIPV4Event(const Event& event);
	};

	struct UdpIpSendIPV6Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset;
	public:
		UdpIpSendIPV6Event(const Event& event);
	};

	struct UdpIpRecvIPV6Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset;
	public:
		UdpIpRecvIPV6Event(const Event& event);
	};

	/*--------------------------------*/

};

#endif
#endif
