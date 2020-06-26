#pragma once
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "msg.pb.h"
#include "PacketHandler.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 512
using namespace google;

class Socket
{
public:
	Socket();
	virtual ~Socket();

	bool Connect(std::string address, int nPort);
	void Close();

	void PutPacket(int packetType, ::google::protobuf::Message* message);
	void SendPacket(int packetType, ::google::protobuf::Message* message);
	bool GetPacket(PacketHandler* pPacketHandle);
	void FlushPacket();


private:
	void __SendPacket();

private:
	SOCKET m_hSocket;
	char sendbuffer[BUF_SIZE];
	char recvbuffer[BUF_SIZE];

	int m_nSendBuffUsed = 0;
	int m_nRecvBuffUsed = 0;

};

