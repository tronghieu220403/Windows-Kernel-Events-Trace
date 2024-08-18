#ifdef _WIN32

#pragma once

#ifndef ETWSERVICE_ETW_NETWORKEVENT_H_
#define ETWSERVICE_ETW_NETWORKEVENT_H_

#include "etw/event.h"

namespace etw
{

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
		bool is_positioned_ = false;

		DWORD proto_offs_ = 0;
		DWORD failure_code_offs_ = 0;
	};

	struct TcpIpFailEventMember
	{
	public:
		uint16_t Proto = 0;
		uint16_t FailureCode = 0;

		TcpIpFailEventMember() = default;
		TcpIpFailEventMember(const Event& event, TcpIpFailEventOffset* offset);
	};

	struct TcpIpFailEvent : TcpIpFailEventMember
	{
	private:
		static inline TcpIpFailEventOffset offset_;
	public:
		TcpIpFailEvent(const Event& event);
	};

	/*
	[EventType{ 10, 26 }, EventTypeName{ "SendIPV4" , "SendIPV6" }]
	class TcpIp_SendIPV4 : TcpIp
	{
		uint32 PID;
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
		bool is_positioned_ = false;

		DWORD pid_offs_ = 0;
		DWORD size_offs_ = 0;
		DWORD daddr_offs_ = 0;
		DWORD saddr_offs_ = 0;
		DWORD dport_offs_ = 0;
		DWORD sport_offs_ = 0;
		DWORD startime_offs_ = 0;
		DWORD endtime_offs_ = 0;
		DWORD seqnum_offs_ = 0;
		DWORD connid_offs_ = 0;
	};

	struct TcpIpSendIPV4EventMember
	{
	public:
		uint32_t PID = 0;
		uint32_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		uint32_t startime = 0;
		uint32_t endtime = 0;
		uint32_t seqnum = 0;
		uint32_t connid = 0;

		TcpIpSendIPV4EventMember() = default;
		TcpIpSendIPV4EventMember(const Event& event, TcpIpSendIPV4EventOffset* offset);
	};

	struct TcpIpSendIPV4Event : TcpIpSendIPV4EventMember
	{
	private:
		static inline TcpIpSendIPV4EventOffset offset_;
	public:
		TcpIpSendIPV4Event(const Event& event);
	};

	struct TcpIpSendIPV6Event : TcpIpSendIPV4EventMember
	{
	private:
		static inline TcpIpSendIPV4EventOffset offset_;
	public:
		TcpIpSendIPV6Event(const Event& event);
	};

	/*
	* TcpIp_TypeGroup1 and TcpIp_TypeGroup3 are the same so we can use the same class for both
	[EventType{11, 13, 14, 16, 18}, EventTypeName{"RecvIPV4", "DisconnectIPV4", "RetransmitIPV4", "ReconnectIPV4", "TCPCopyIPV4"}]
	class TcpIp_TypeGroup1 : TcpIp
	{
		uint32 PID;
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
		uint32 PID;
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
		bool is_positioned_ = false;

		DWORD pid_offs_ = 0;
		DWORD size_offs_ = 0;
		DWORD daddr_offs_ = 0;
		DWORD saddr_offs_ = 0;
		DWORD dport_offs_ = 0;
		DWORD sport_offs_ = 0;
		DWORD seqnum_offs_ = 0;
		DWORD connid_offs_ = 0;
	};

	struct TcpIpTypeGroup1EventMember
	{
	public:
		uint32_t PID = 0;
		uint32_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		uint32_t seqnum = 0;
		uint32_t connid = 0;

		TcpIpTypeGroup1EventMember() = default;
		TcpIpTypeGroup1EventMember(const Event& event, TcpIpTypeGroup1EventOffset* offset);
	};

	struct TcpIpRecvIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpRecvIPV4Event(const Event& event);
	};

	struct TcpIpDisconnectIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpDisconnectIPV4Event(const Event& event);
	};

	struct TcpIpRetransmitIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpRetransmitIPV4Event(const Event& event);
	};

	struct TcpIpReconnectIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpReconnectIPV4Event(const Event& event);
	};

	struct TcpIpTCPCopyIPV4Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpTCPCopyIPV4Event(const Event& event);
	};

	struct TcpIpRecvIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpRecvIPV6Event(const Event& event);
	};

	struct TcpIpDisconnectIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpDisconnectIPV6Event(const Event& event);
	};

	struct TcpIpRetransmitIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpRetransmitIPV6Event(const Event& event);
	};

	struct TcpIpReconnectIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpReconnectIPV6Event(const Event& event);
	};

	struct TcpIpTCPCopyIPV6Event : TcpIpTypeGroup1EventMember
	{
	private:
		static inline TcpIpTypeGroup1EventOffset offset_;
	public:
		TcpIpTCPCopyIPV6Event(const Event& event);
	};

	/*
	* TcpIp_TypeGroup2 and TcpIp_TypeGroup4 are the same so we can use the same class for both
	[EventType{12, 15}, EventTypeName{"ConnectIPV4", "AcceptIPV4"}]
	class TcpIp_TypeGroup2 : TcpIp
	{
		uint32 PID;
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
		uint32 PID;
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
		bool is_positioned_ = false;

		DWORD pid_offs_ = 0;
		DWORD size_offs_ = 0;
		DWORD daddr_offs_ = 0;
		DWORD saddr_offs_ = 0;
		DWORD dport_offs_ = 0;
		DWORD sport_offs_ = 0;
		DWORD mss_offs_ = 0;
		DWORD sackopt_offs_ = 0;
		DWORD tsopt_offs_ = 0;
		DWORD wsopt_offs_ = 0;
		DWORD rcvwin_offs_ = 0;
		DWORD rcvwinscale_offs_ = 0;
		DWORD sndwinscale_offs_ = 0;
		DWORD seqnum_offs_ = 0;
		DWORD connid_offs_ = 0;
	};

	struct TcpIpTypeGroup2EventMember
	{
	public:
		uint32_t PID = 0;
		uint32_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		uint16_t mss = 0;
		uint16_t sackopt = 0;
		uint16_t tsopt = 0;
		uint16_t wsopt = 0;
		uint32_t rcvwin = 0;
		int16_t rcvwinscale = 0;
		int16_t sndwinscale = 0;
		uint32_t seqnum = 0;
		uint32_t connid = 0;

		TcpIpTypeGroup2EventMember() = default;
		TcpIpTypeGroup2EventMember(const Event& event, TcpIpTypeGroup2EventOffset* offset);
	};

	struct TcpIpConnectIPV4Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset_;
	public:
		TcpIpConnectIPV4Event(const Event& event);
	};

	struct TcpIpAcceptIPV4Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset_;
	public:
		TcpIpAcceptIPV4Event(const Event& event);
	};

	struct TcpIpConnectIPV6Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset_;
	public:
		TcpIpConnectIPV6Event(const Event& event);
	};

	struct TcpIpAcceptIPV6Event : TcpIpTypeGroup2EventMember
	{
	private:
		static inline TcpIpTypeGroup2EventOffset offset_;
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
		bool is_positioned_ = false;

		DWORD proto_offs_ = 0;
		DWORD failure_code_offs_ = 0;
	};

	struct UdpIpFailEventMember
	{
	public:
		uint16_t Proto = 0;
		uint16_t FailureCode = 0;

		UdpIpFailEventMember() = default;
		UdpIpFailEventMember(const Event& event, UdpIpFailEventOffset* offset);
	};

	struct UdpIpFailEvent : UdpIpFailEventMember
	{
	private:
		static inline UdpIpFailEventOffset offset_;
	public:
		UdpIpFailEvent(const Event& event);
	};

	/*
	* UdpIp_TypeGroup1 and UdpIp_TypeGroup2 are the same so we can use the same class for both
	[EventType{10, 11}, EventTypeName{"SendIPV4", "RecvIPV4"}]
	class UdpIp_TypeGroup1 : UdpIp
	{
		uint32 PID;
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
		uint32 PID;
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
		bool is_positioned_ = false;

		DWORD pid_offs_ = 0;
		DWORD size_offs_ = 0;
		DWORD daddr_offs_ = 0;
		DWORD saddr_offs_ = 0;
		DWORD dport_offs_ = 0;
		DWORD sport_offs_ = 0;
		DWORD seqnum_offs_ = 0;
		DWORD connid_offs_ = 0;
	};

	struct UdpIpTypeGroup1EventMember
	{
	public:
		uint32_t PID = 0;
		uint32_t size = 0;
		PVOID daddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID saddr = nullptr;  // Assuming object type is represented as a pointer
		PVOID dport = nullptr;  // Assuming object type is represented as a pointer
		PVOID sport = nullptr;  // Assuming object type is represented as a pointer
		uint32_t seqnum = 0;
		uint32_t connid = 0;

		UdpIpTypeGroup1EventMember() = default;
		UdpIpTypeGroup1EventMember(const Event& event, UdpIpTypeGroup1EventOffset* offset);
	};

	struct UdpIpSendIPV4Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset_;
	public:
		UdpIpSendIPV4Event(const Event& event);
	};

	struct UdpIpRecvIPV4Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset_;
	public:
		UdpIpRecvIPV4Event(const Event& event);
	};

	struct UdpIpSendIPV6Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset_;
	public:
		UdpIpSendIPV6Event(const Event& event);
	};

	struct UdpIpRecvIPV6Event : UdpIpTypeGroup1EventMember
	{
	private:
		static inline UdpIpTypeGroup1EventOffset offset_;
	public:
		UdpIpRecvIPV6Event(const Event& event);
	};

	/*--------------------------------*/

};

#endif
#endif
