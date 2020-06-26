#pragma once
#include <string>
#include <vector>
#include <boost/serialization/singleton.hpp>
#include "CServer.h"

enum ServerType {
	CHAT_SERVER = 0,
	MAX_SERVERS
};

class CServerMgr : public boost::serialization::singleton<CServerMgr>
{
public:
	friend class boost::serialization::singleton<CServerMgr>;

	static CServerMgr& GetMutable() {
		return boost::serialization::singleton<CServerMgr>::get_mutable_instance();
	}

	CServerMgr();

public:
	void Connect(ServerType id, std::string ip, int port);
	void Connect(ServerType id);
	bool IsConnect(ServerType id);

	void AddHandler(ServerType id, PacketHandler* handler);
	void RemoveHandler(ServerType id, PacketHandler* handler);
	void SendPacket(ServerType id, const char* buf, int size);
	void Close(ServerType id);

private:
	std::vector<CServer> m_servers;
};

