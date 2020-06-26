#pragma once
#include <thread>
#include <string>
#include <winsock2.h>

class CServer
{
public:
	void Connect(std::string Ip, int port);
	void SendPacket(const char *buf, int size);
	void AddHandler(PacketHandler* handler);
	void RemoveHandler(PacketHandler* handler);
	void Close();
	bool IsConnect() const;

private:
	void ErrorHandling(const char* message);
	void ReceiveThreading();
	void ConnectSocket();
	
private:
	bool m_bConnect = false;

	std::string m_ipAddress;
	int m_nPort;
	SOCKET m_nSocket;
	std::thread* m_pThread = nullptr;
	std::vector<PacketHandler*> m_handlers;

};

