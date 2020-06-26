#include "PacketHandler.h"
#include <string>
#include "CServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
using namespace google::protobuf::io;

#pragma comment(lib, "ws2_32.lib")
using namespace std;

void CServer::ErrorHandling(const char* message) {

}

void CServer::ReceiveThreading() {
	if (!m_bConnect)
	{
		return;
	}


	WSABUF dataBuf;
	int recvBytes = 0;
	int flags = 0;

	const int bufSize = 1024;
	char buf[bufSize] = { 0, };

	while (true)
	{
		recvBytes = recv(m_nSocket, buf, 8, 0);
		google::protobuf::uint32 size;
		google::protobuf::uint32 type;
		{
			google::protobuf::io::ArrayInputStream ais(buf, 8);
			CodedInputStream coded_input(&ais);

			coded_input.ReadVarint32(&type);
			coded_input.ReadVarint32(&size);
			if (size > bufSize) {
				//resetconnect
				continue;
			}

		}

		recvBytes = recv(m_nSocket, buf + 8, size, 0);
		{
			google::protobuf::io::ArrayInputStream ais(buf, size + 8);
			CodedInputStream coded_input(&ais);

			google::protobuf::uint32 type;
			coded_input.ReadVarint32(&type);//type

			google::protobuf::uint32 size;
			coded_input.ReadVarint32(&size);//size

			for each (PacketHandler * ptr in m_handlers)
			{
				ptr->Handle(type, &coded_input);
			}
		}
		if (!m_bConnect) {
			break;
		}
	}

}

void CServer::Connect(std::string ip, int port)
{
	m_ipAddress = ip;
	m_nPort = port;

	ConnectSocket();
}

void  CServer::AddHandler(PacketHandler* handler)
{
	for each (PacketHandler * ptr in m_handlers)
	{
		if (ptr == handler) {
			return;
		}
	}

	m_handlers.push_back(handler);
}

void CServer::RemoveHandler(PacketHandler* handler) {
	for (auto iter = begin(m_handlers); iter != end(m_handlers); ++iter) {
		if (*iter == handler) {
			m_handlers.erase(iter);
		}
	}
}

void CServer::ConnectSocket()
{
	WSADATA wsaData;
	SOCKADDR_IN recvAddr;

	char message[1024] = { 0, };
	int sendBytes = 0;
	int recvBytes = 0;
	int flags = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.2 DLL */
		ErrorHandling("WSAStartup() error!");

	m_nSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_nSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&recvAddr, 0, sizeof(recvAddr));
	recvAddr.sin_family = AF_INET;
	//recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, m_ipAddress.c_str(), &recvAddr.sin_addr);
	recvAddr.sin_port = htons(atoi("4807"));

	if (::connect(m_nSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!");
	}


	m_bConnect = true;

	if (m_pThread == nullptr)
	{
		m_pThread = new std::thread(&CServer::ReceiveThreading, this);
	}
}

void CServer::Close() {
	if (m_pThread) {
		m_pThread->join();
		m_pThread = nullptr;
	}

	closesocket(m_nSocket);
	WSACleanup();

}

void CServer::SendPacket(const char* buf, int size)
{
	if (!m_bConnect)
	{
		return;
	}

	if (send(m_nSocket, buf, size, 0) == SOCKET_ERROR)
	{
		ErrorHandling("WSASend() error");
	}
}

bool CServer::IsConnect() const {
	return m_bConnect;
}