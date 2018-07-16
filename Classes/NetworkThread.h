#pragma once
#include <thread>
#include "../proj.win32/Socket.h"
#include "../proj.win32/PacketHandler.h"

#include <boost/serialization/singleton.hpp>


//performFunctionInCocosThread example
//http://codelala.net/cocos2d-x-2-05-multi-threading-2-mutex/
class CNetworkThread : public boost::serialization::singleton<CNetworkThread>
{
public:
	friend class boost::serialization::singleton<CNetworkThread>;


	CNetworkThread();
	virtual ~CNetworkThread();

	void Start(std::string strIp, int nPort, PacketHandler* pHandler);
	void Work();
	void Terminate();

	void SendPacket(ServerMessage::MessageType packetType, ::google::protobuf::Message* message);

	static CNetworkThread& GetMutable() {

		return boost::serialization::singleton<CNetworkThread>::get_mutable_instance();

	}

private:
	std::string m_strIpAddress;
	int m_nPort;
	PacketHandler* m_pPackethandler;
	std::thread* m_pThread = nullptr;
	Socket m_pSocket;
};

