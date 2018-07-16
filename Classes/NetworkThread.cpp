#include "NetworkThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "ServerMessage.pb.h"
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 512
using namespace google;

CNetworkThread::CNetworkThread()
{
}


CNetworkThread::~CNetworkThread()
{
	Terminate();
}

void CNetworkThread::Start(std::string strIp, int nPort, PacketHandler* pHandler)
{
	m_strIpAddress = strIp;
	m_nPort = nPort; 
	m_pPackethandler = pHandler;

	if (m_pThread == nullptr)
	{
		m_pThread = new std::thread(&CNetworkThread::Work, this);
	}
}

void CNetworkThread::Work()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		//ErrorHandling("WSAStartup() error!");
	}

	m_pSocket.Connect(m_strIpAddress, m_nPort);


	while (m_pSocket.GetPacket(m_pPackethandler))
	{

	}

	/*
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		//ErrorHandling("socket() error!");
	}

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(m_strIpAddress.c_str());
	servAdr.sin_port = htons(m_nPort);

	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		//ErrorHandling("connect() error!");

	}
	else
	{
		puts("connected.......");
	}

	int strLen = 0;
	int readLen = 0;
	int nTotalLen = 0;
	
	char message[512];
	int nUsedSeize = 0;

	while ((readLen = recv(hSocket, message + nUsedSeize, BUF_SIZE - 1 - nUsedSeize, 0)) == 0)
	{

		//cout << "From : " << message << endl;
		if (readLen < MessageHeaderSize)
		{
			nUsedSeize = readLen;
			readLen = 0;
			continue;
		}


		protobuf::io::ArrayInputStream input_array_stream(message, readLen);
		protobuf::io::CodedInputStream input_coded_stream(&input_array_stream);

		MessageHeader messageHeader;
		while (input_coded_stream.ReadRaw(&messageHeader, MessageHeaderSize))
		{
			const void* payload_ptr = NULL;
			int remainSize = 0;
			input_coded_stream.GetDirectBufferPointer(&payload_ptr, &remainSize);
			if (remainSize < (signed)messageHeader.size)
			{
				//패킷전송이 한번에 다완료되지 않은경우 버퍼를 보관했다가 다음 완료된 바이트와 붙여서 다시 파싱을 해야하함
				nUsedSeize = readLen;
				readLen = 0;
				continue;
			}  


			protobuf::io::ArrayInputStream payload_array_stream(payload_ptr, messageHeader.size);
			protobuf::io::CodedInputStream payload_input_stream(&payload_array_stream);

			input_coded_stream.Skip(messageHeader.size);
			readLen -= MessageHeaderSize + messageHeader.size;

			switch (messageHeader.type)
			{
				case ServerMessage::LOGIN:
				{
					ServerMessage::Login message;
					if (false == message.ParseFromCodedStream(&payload_input_stream))
						continue;
					m_pPackethandler->Handle(message);
				}
				break;
				case ServerMessage::CHAT:
				{
					ServerMessage::Chat message;
					if (false == message.ParseFromCodedStream(&payload_input_stream))
						continue;
					m_pPackethandler->Handle(message);
				}
				break;
				case ServerMessage::MOVE:
				{
					ServerMessage::Move message;
					if (false == message.ParseFromCodedStream(&payload_input_stream))
						continue;
					m_pPackethandler->Handle(message);
				}
				break;
			}

			if (readLen != 0)
			{
				readLen = 0;
				//오류가 발생한경우 
			}
		}
	}
	*/

	m_pSocket.Close();
	WSACleanup();
}

void CNetworkThread::Terminate()
{
	if (m_pThread != nullptr)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void CNetworkThread::SendPacket(ServerMessage::MessageType packetType, ::google::protobuf::Message* message)
{
	m_pSocket.SendPacket(packetType, message);
}
