#include "Socket.h"
#include <stdio.h>


Socket::Socket()
	: m_hSocket(INVALID_SOCKET)
{
}


Socket::~Socket()
{
}

bool Socket::Connect(std::string address, int nPort)
{
	printf("Connect With %s, %d", address.c_str(), nPort);

	m_hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		printf("socket() error!");
		return false;
	}

	SOCKADDR_IN servAdr;
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(address.c_str());
	servAdr.sin_port = htons(nPort);

	if (connect(m_hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		printf("connect() error!");
		return false;
	}

	return true;
}

void Socket::Close()
{
	closesocket(m_hSocket);
}

void Socket::PutPacket(int packetType, ::google::protobuf::Message* message)
{
	if (message == nullptr)
	{
		return;
	}

	if (m_hSocket == INVALID_SOCKET)
	{
		return;
	}

	int nPacketSize = message->ByteSize() + MessageHeaderSize;
	if (m_nSendBuffUsed + nPacketSize > BUF_SIZE)
	{
		//User Reserve
		return;
	}

	protobuf::io::ArrayOutputStream  output_array_stream(sendbuffer + m_nSendBuffUsed, nPacketSize);
	protobuf::io::CodedOutputStream output_coded_stream(&output_array_stream);


	MessageHeader messageHeader;
	messageHeader.size = message->ByteSize();
	messageHeader.type = packetType;
	output_coded_stream.WriteRaw(&messageHeader, sizeof(MessageHeader));

	message->SerializeToCodedStream(&output_coded_stream);
	m_nSendBuffUsed += nPacketSize;

}

void Socket::SendPacket(int packetType, ::google::protobuf::Message* message)
{
	if (m_hSocket == INVALID_SOCKET)
	{
		return;
	}

	PutPacket(packetType, message);
	__SendPacket();
}

void Socket::FlushPacket()
{
	if (m_nSendBuffUsed <= 0)
	{
		return;
	}

	__SendPacket();
}

void Socket::__SendPacket()
{
	int nRet = send(m_hSocket, sendbuffer, m_nSendBuffUsed, 0);
	if (nRet == ERROR_CURRENT_DIRECTORY)
	{
		printf("ERROR_CURRENT_DIRECTORY");
	}

	m_nSendBuffUsed = 0;
}

bool Socket::GetPacket(PacketHandler* pPacketHandle)
{
	int readLen = recv(m_hSocket, recvbuffer, BUF_SIZE - 1 - m_nRecvBuffUsed, 0);

	if (readLen < MessageHeaderSize)
	{
		m_nRecvBuffUsed += readLen;
		readLen = 0;
		return true;
	}


	protobuf::io::ArrayInputStream input_array_stream(recvbuffer, m_nRecvBuffUsed + readLen);
	protobuf::io::CodedInputStream input_coded_stream(&input_array_stream);

	MessageHeader messageHeader;
	while (input_coded_stream.ReadRaw(&messageHeader, MessageHeaderSize))
	{
		const void* payload_ptr = NULL;
		int remainSize = 0;
		input_coded_stream.GetDirectBufferPointer(&payload_ptr, &remainSize);
		if (remainSize < (signed)messageHeader.size)
		{
			m_nRecvBuffUsed += readLen;
			return true;
		}

		protobuf::io::ArrayInputStream payload_array_stream(payload_ptr, messageHeader.size);
		protobuf::io::CodedInputStream payload_input_stream(&payload_array_stream);

		input_coded_stream.Skip(messageHeader.size);
		if (m_nRecvBuffUsed > 0)
		{
			m_nRecvBuffUsed -= MessageHeaderSize + messageHeader.size;
		}

		switch (messageHeader.type)
		{
			case ServerMessage::CHAT:
			{
				ServerMessage::Chat recvMessage;
				if (false == recvMessage.ParseFromCodedStream(&payload_input_stream))
					break;

				printf("From : %s", recvMessage.message().c_str());
				pPacketHandle->Handle(recvMessage);
			}
			break;
			default :
				return false;
		}

	}
	return true;
}