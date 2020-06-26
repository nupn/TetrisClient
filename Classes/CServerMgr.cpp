#include "PacketHandler.h"
#include "CServer.h"
#include "CServerMgr.h"

CServerMgr::CServerMgr() {
	m_servers.resize(MAX_SERVERS);
}

void CServerMgr::Connect(ServerType id) {
	if (ServerType::CHAT_SERVER == id && !m_servers.at(ServerType::CHAT_SERVER).IsConnect()) {
		m_servers.at(ServerType::CHAT_SERVER).Connect("13.125.197.13", 4807);
	}
}

void CServerMgr::Connect(ServerType id, std::string ip, int port) {
	if (ServerType::CHAT_SERVER == id && !m_servers.at(ServerType::CHAT_SERVER).IsConnect()) {
		m_servers.at(ServerType::CHAT_SERVER).Connect(ip, port);
	}
}

void CServerMgr::Close(ServerType id) {
	m_servers.at(id).Close();
}

void CServerMgr::AddHandler(ServerType id, PacketHandler* handler) {
	
	m_servers.at(id).AddHandler(handler);
}

void CServerMgr::RemoveHandler(ServerType id, PacketHandler* handler) {

	m_servers.at(id).RemoveHandler(handler);
}

void CServerMgr::SendPacket(ServerType id, const char* buf, int size) {
	m_servers.at(id).SendPacket(buf, size);
}

bool CServerMgr::IsConnect(ServerType id){
	return m_servers.at(id).IsConnect();
}